import sensor, time, math, display
from pyb import UART, LED

# 初始化
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
clock = time.clock()

# 初始化LCD显示屏
lcd = display.SPIDisplay()

uart = UART(3, 115200)
red_led, green_led, blue_led = LED(1), LED(2), LED(3)

# 通信协议定义 - 5字节协议：[0xAA, 模式位, PID位, 交叉线标志位, 颜色位]
PACKET_SYNC = 0xAA  # 同步字节

# 模式位定义
MODE_FORWARD = 0x00   # 向前巡线模式
MODE_BACKWARD = 0x01  # 向后巡线模式
MODE_COLOR = 0x02     # 颜色识别模式
MODE_TURN_ASSIST = 0x03  # 转弯辅助模式

# 标志位定义
FLAG_NONE = 0x00      # 无标志
CROSS_DETECTED = 0x01 # 检测到交叉线
TURN_DETECTED = 0x02  # 检测到转弯

# 颜色位定义
COLOR_NONE = 0x00     # 无颜色检测
COLOR_RED = 0x01      # 红色
COLOR_BLACK = 0x02    # 黑色
COLOR_WHITE = 0x03    # 白色
COLOR_BLUE = 0x04     # 蓝色
COLOR_GREEN = 0x05    # 绿色

class LineFollower:
    def __init__(self):
        # PID参数
        self.kp, self.ki, self.kd = 1.5, 0.0, 0.3
        self.last_error = self.integral = 0

        # 交叉线检测
        self.cross_count = self.cross_confirm_count = 0
        self.last_cross_time = 0
        self.cross_threshold = 800
        self.cross_confirm_threshold = 1

        # 图像参数
        self.img_width, self.img_height = sensor.width(), sensor.height()
        roi_w, roi_h = self.img_width // 2, self.img_height // 3
        self.roi = ((self.img_width - roi_w) // 2, (self.img_height - roi_h) // 2, roi_w, roi_h)
        self.threshold = [(0, 40, -20, 20, -20, 20)]#黑线阈值

        # 向后巡线参数
        self.backward_mode = 1  # 向后巡线模式开关
        self.rear_distance = 140  # 图像中心到车尾的距离

        # 颜色识别参数

        self.color_mode = 0  # 颜色识别模式开关
        self.color_thresholds = {
            'red': [(20, 100, 15, 127, 15, 127)],      # 红色阈值
            'green': [(30, 100, -64, -8, -32, 32)],   # 绿色阈值
            'blue': [(0, 30, -10, 40, -80, -20)],     # 蓝色阈值
            'white': [(80, 100, -20, 20, -20, 20)],   # 白色阈值
            'black': [(0, 30, -20, 20, -20, 20)]      # 黑色阈值
        }
        self.detected_color = None
        self.color_detection_confidence = 0
        self.min_color_pixels = 50  # 最小像素数阈值

        # 转弯辅助参数
        self.turn_assist_mode = False  # 转弯辅助模式开关
        self.last_vertical_time = 0    # 上次检测到竖线的时间

    def detect_color(self, img):
        """颜色识别功能"""
        best_color = None
        best_pixels = 0
        detection_results = {}

        # 定义颜色检测区域（中心区域）
        center_x, center_y = self.img_width // 2, self.img_height // 2
        detect_size = min(self.img_width, self.img_height) // 3
        color_roi = (center_x - detect_size//2, center_y - detect_size//2, detect_size, detect_size)

        # 绘制检测区域
        img.draw_rectangle(color_roi, color=(255, 255, 0))

        # 遍历所有颜色进行检测
        for color_name, threshold in self.color_thresholds.items():
            blobs = img.find_blobs(threshold, roi=color_roi, merge=True, pixels_threshold=self.min_color_pixels)

            if blobs:
                # 计算该颜色的总像素数
                total_pixels = sum(blob.pixels() for blob in blobs)
                detection_results[color_name] = total_pixels

                # 找到像素数最多的颜色
                if total_pixels > best_pixels:
                    best_pixels = total_pixels
                    best_color = color_name

                # 绘制检测到的颜色区域
                for blob in blobs:
                    img.draw_rectangle(blob.rect(), color=(0, 255, 0))
                    img.draw_cross(blob.cx(), blob.cy(), size=5, color=(255, 0, 0))

        # 更新检测结果
        if best_color and best_pixels > self.min_color_pixels:
            self.detected_color = best_color
            self.color_detection_confidence = best_pixels
        else:
            self.detected_color = None
            self.color_detection_confidence = 0

        return self.detected_color, detection_results

    def detect_vertical_line(self, img):
        """检测竖线（转弯辅助用）"""
        current_time = time.ticks_ms()
        center_x, center_y = self.img_width // 2, self.img_height // 2

        # 竖线检测ROI（垂直方向）
        roi_w, roi_h = self.img_width // 3, self.img_height // 2
        vertical_roi = (center_x - roi_w//2, center_y, roi_w, roi_h)

        blobs = img.find_blobs(self.threshold, roi=vertical_roi, merge=True, pixels_threshold=50)
        img.draw_rectangle(vertical_roi, color=(255, 255, 0))

        if blobs:
            # 检查是否为竖线（高度大于宽度）
            for blob in blobs:
                if blob.h() > blob.w() * 1.5:  # 竖线条件：高度是宽度的1.5倍以上
                    # 防重复检测
                    if current_time - self.last_vertical_time > 600:
                        self.last_vertical_time = current_time
                        img.draw_rectangle(blob.rect(), color=(0, 255, 255))
                        img.draw_cross(center_x, center_y, size=15, color=(0, 255, 255))
                        return True

        return False

    def find_line_with_slope(self, img):
        """找到线的中心点并计算斜率"""
        blobs = img.find_blobs(self.threshold, roi=self.roi, merge=True, pixels_threshold=30)
        if not blobs:
            return None, None, None

        # 获取最大的blob
        main_blob = max(blobs, key=lambda b: b.pixels())

        # 在ROI内各个y位置检测线的x位置
        detection_points = []
        roi_x, roi_y, roi_w, roi_h = self.roi

        # 在ROI内分3段检测
        for i in range(3):
            y_offset = int(roi_h * (i + 1) / 4)
            y_pos = roi_y + y_offset

            # 在该y位置附近检测线
            line_roi = (roi_x, y_pos - 5, roi_w, 10)
            line_blobs = img.find_blobs(self.threshold, roi=line_roi, merge=True, pixels_threshold=10)

            if line_blobs:
                line_x = max(line_blobs, key=lambda b: b.pixels()).cx()
                detection_points.append((line_x, y_pos))
                # 绘制检测点
                img.draw_circle(line_x, y_pos, 2, color=(255, 0, 0))

        if len(detection_points) < 2:
            return main_blob.cx(), None, detection_points

        # 计算斜率（使用最远的两个点）
        if len(detection_points) >= 2:
            p1, p2 = detection_points[0], detection_points[-1]
            dx = p2[0] - p1[0]
            dy = p2[1] - p1[1]

            slope = dx / dy if dy != 0 else float('inf')  # 处理垂直线
            return main_blob.cx(), slope, detection_points

        return main_blob.cx(), None, detection_points

    def calculate_rear_error(self, current_error, slope, detection_y):
        """计算车尾的真正偏差"""
        if slope is None:
            return current_error  # 如果无法计算斜率，使用当前误差

        center_y = self.img_height // 2
        rear_y = center_y + self.rear_distance  # 车尾y坐标

        # 处理斜率不存在的情况（垂直线）
        if slope == float('inf') or abs(slope) > 1000:  # 近似垂直线
            return current_error

        # 使用精确的几何关系计算车尾偏差
        # 黑线方程: x = k * (y - y检测) + x检测
        # 在车尾位置的偏差 = k * (y车尾 - y检测) + 当前误差
        y_distance = rear_y - detection_y
        rear_error = slope * y_distance + current_error

        return rear_error



    def detect_intersection(self, img):
        current_time = time.ticks_ms()
        center_x, center_y = self.img_width // 2, self.img_height // 2
        roi_size = min(self.img_width, self.img_height) // 8
        h_dist = self.img_width // 4

        rois = [
            (center_x - h_dist - roi_size//2, center_y - roi_size//2, roi_size, roi_size),  # left
            (center_x + h_dist - roi_size//2, center_y - roi_size//2, roi_size, roi_size)   # right
        ]

        directions_with_lines = 0
        for roi in rois:
            blobs = img.find_blobs(self.threshold, roi=roi, merge=True, pixels_threshold=20)#pixels_threshold:最小像素数
            if blobs:
                density = sum(blob.pixels() for blob in blobs) / (roi[2] * roi[3])
                if density > 0.15:#交叉线检测阈值
                    directions_with_lines += 1
            img.draw_rectangle(roi, color=(0, 255, 0) if blobs else (255, 0, 0))

        if directions_with_lines == 2:#2个方向都有线（左、右）
            self.cross_confirm_count += 1
            if self.cross_confirm_count >= self.cross_confirm_threshold:
                if current_time - self.last_cross_time > self.cross_threshold:
                    self.cross_count += 1
                    self.last_cross_time = current_time
                    self.cross_confirm_count = 0
                    blue_led.on()
                    img.draw_cross(center_x, center_y, size=20, color=(255, 255, 0))
                    return True
        else:
            self.cross_confirm_count = 0

        blue_led.off()
        return False

    def calculate_pid(self, error):
        self.integral += error
        derivative = error - self.last_error
        output = self.kp * error + self.ki * self.integral + self.kd * derivative
        self.last_error = error
        return output

    def motor_control(self, steering_value, is_cross=False, is_turn=False, is_backward=False, detected_color=None):
        steering_value = max(-100, min(100, int(steering_value)))

        # 5字节数据包格式: [0xAA, 模式位, PID位, 交叉线标志位, 颜色位]
        packet = [PACKET_SYNC]

        # 模式位
        if self.color_mode:
            packet.append(MODE_COLOR)
        elif self.turn_assist_mode:
            packet.append(MODE_TURN_ASSIST)
        elif self.backward_mode:
            packet.append(MODE_BACKWARD)
        else:
            packet.append(MODE_FORWARD)

        # PID位：转换为有符号字节（-128到127）
        if steering_value < 0:
            pid_byte = (256 + steering_value) & 0xFF  # 补码表示
        else:
            pid_byte = steering_value & 0xFF
        packet.append(pid_byte)

        # 交叉线标志位
        if is_cross:
            packet.append(CROSS_DETECTED)
        elif is_turn:
            packet.append(TURN_DETECTED)
        else:
            packet.append(FLAG_NONE)

        # 颜色位
        if detected_color:
            color_map = {
                'red': COLOR_RED,
                'black': COLOR_BLACK,
                'white': COLOR_WHITE,
                'blue': COLOR_BLUE,
                'green': COLOR_GREEN
            }
            packet.append(color_map.get(detected_color, COLOR_NONE))
        else:
            packet.append(COLOR_NONE)

        # 发送数据包
        uart.write(bytes(packet))

        # 状态显示
        if detected_color:
            status_text = f"COLOR_{detected_color.upper()}"
        elif is_cross:
            status_text = "CROSS"
        elif is_turn:
            status_text = "TURN"
        elif is_backward:
            status_text = "BACK"
        else:
            status_text = "NORMAL"

        mode_text = "TURN" if self.turn_assist_mode else ("COLOR" if self.color_mode else ("BACK" if self.backward_mode else "FORWARD"))
        flag_text = "交叉线" if is_cross else ("转弯" if is_turn else "无")
        print(f"发送: 模式={mode_text}, PID={steering_value}, 标志={flag_text}, 颜色={detected_color or 'None'}")
        print(f"字节=[0x{packet[0]:02X}, 0x{packet[1]:02X}, 0x{packet[2]:02X}, 0x{packet[3]:02X}, 0x{packet[4]:02X}]")

    def process_uart_commands(self):
        """处理串口接收到的命令"""
        if uart.any():
            cmd_data = uart.read(1)  # 读取一个字节
            if len(cmd_data) == 1:
                cmd = cmd_data[0]  # 获取字节值

                if cmd == MODE_FORWARD:  # 0x00
                    self.backward_mode = False
                    self.color_mode = False
                    self.turn_assist_mode = False
                    print("切换到向前巡线模式")
                elif cmd == MODE_BACKWARD:  # 0x01
                    self.backward_mode = True
                    self.color_mode = False
                    self.turn_assist_mode = False
                    print("切换到向后巡线模式")
                elif cmd == MODE_COLOR:  # 0x02
                    self.color_mode = True
                    self.backward_mode = False
                    self.turn_assist_mode = False
                    print("切换到颜色识别模式")
                elif cmd == MODE_TURN_ASSIST:  # 0x03
                    self.turn_assist_mode = True
                    self.color_mode = False
                    self.backward_mode = False
                    print("切换到转弯辅助模式")

    def run(self):
        green_led.on()
        while True:
            clock.tick()

            # 处理串口命令
            self.process_uart_commands()

            img = sensor.snapshot()
            center_x, center_y = self.img_width // 2, self.img_height // 2
            img.draw_cross(center_x, center_y, size=5, color=(255, 255, 255))

            # 模式控制
            if self.turn_assist_mode:
                # 转弯辅助模式：只检测竖线
                if self.detect_vertical_line(img):
                    self.motor_control(0, is_turn=True)  # 使用专用转弯标志位
                    img.draw_string(10, 55, "VLine Detected", color=(0, 255, 255), scale=1)
                else:
                    self.motor_control(0)
                    img.draw_string(10, 55, "Searching VLine", color=(255, 255, 0), scale=1)

            elif self.color_mode:
                # 颜色识别模式
                detected_color, color_results = self.detect_color(img)

                if detected_color:
                    self.motor_control(0, detected_color=detected_color)

                    # 在图像上显示检测结果
                    img.draw_string(10, 115, f"Detected: {detected_color.upper()}", color=(255, 255, 0))
                    img.draw_string(10, 130, f"Confidence: {self.color_detection_confidence}", color=(255, 255, 0))
                else:
                    self.motor_control(0)
                    img.draw_string(10, 115, "No Color Detected", color=(255, 0, 0))

                # 显示各颜色检测结果（调试信息）
                y_offset = 145
                for color_name, pixels in color_results.items():
                    img.draw_string(10, y_offset, f"{color_name}: {pixels}", color=(200, 200, 200))
                    y_offset += 12
            else:
                # 巡线模式
                img.draw_rectangle(self.roi, color=255)

                # 检测交叉线
                if self.detect_intersection(img):
                    red_led.on()
                    self.motor_control(0, is_cross=True)
                    time.sleep_ms(200)
                    red_led.off()

                else:
                    red_led.off()

                    if self.backward_mode:
                        # 向后巡线模式
                        line_x, slope, detection_points = self.find_line_with_slope(img)
                        if line_x is not None:
                            # 计算当前误差
                            current_error = line_x - (self.roi[0] + self.roi[2] // 2)

                            detection_y = self.roi[1] + self.roi[3] // 2  # ROI中心y坐标
                            rear_error = self.calculate_rear_error(current_error, slope, detection_y)

                            # PID控制
                            pid_output = self.calculate_pid(rear_error)
                            self.motor_control(pid_output, is_backward=True)

                            # 绘制检测结果
                            img.draw_circle(line_x, 60, 5, color=255)

                            # 绘制车尾位置和计算的偏差
                            rear_x = center_x + int(rear_error)
                            img.draw_cross(rear_x, center_y + self.rear_distance, size=8, color=(0, 255, 255))

                            # 显示调试信息
                            img.draw_string(10, 40, f"Slope: {slope:.2f}" if slope is not None else "Slope: None", color=255)
                            img.draw_string(10, 55, f"CurErr: {current_error:.1f}", color=255)
                            img.draw_string(10, 70, f"RearErr: {rear_error:.1f}", color=255)
                        else:
                            self.motor_control(0, is_backward=True)
                    else:
                        # 向前巡线模式
                        line_x, slope, detection_points = self.find_line_with_slope(img)
                        if line_x is not None:
                            error = line_x - (self.roi[0] + self.roi[2] // 2)
                            pid_output = self.calculate_pid(error)
                            self.motor_control(pid_output)
                            img.draw_circle(line_x, 60, 5, color=255)

                            # 显示检测点数量（调试信息）
                            img.draw_string(10, 100, f"Points: {len(detection_points)}", color=255)
                        else:
                            self.motor_control(0)

            # 状态显示
            img.draw_string(10, 10, f"Cross: {self.cross_count}", color=255)
            img.draw_string(10, 25, f"FPS: {clock.fps():.1f}", color=255)

            # 显示当前模式
            if self.turn_assist_mode:
                mode_text = "TURN"
                mode_color = (0, 255, 255)
            elif self.color_mode:
                mode_text = "COLOR"
                mode_color = (255, 0, 255)
            elif self.backward_mode:
                mode_text = "BACK"
                mode_color = (255, 255, 0)
            else:
                mode_text = "FORWARD"
                mode_color = (255, 255, 0)
            img.draw_string(10, 85, f"Mode: {mode_text}", color=mode_color)

            # 将处理后的图像显示到LCD屏幕上
            lcd.write(img)

# 主程序
if __name__ == "__main__":
    line_follower = LineFollower()

    line_follower.run()
