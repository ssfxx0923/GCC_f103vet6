
import sensor, time, display
from pyb import UART, LED

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.set_auto_gain(0)
sensor.set_auto_whitebal(0)
sensor.skip_frames(time=1000)
sensor.set_vflip(True)
sensor.set_hmirror(True)
clock = time.clock()

# �?选开�?
USE_LCD = False  # False时不�?用LCD
DEBUG = False    # False时不打印调试信息且不绘制状态文�?

lcd = display.SPIDisplay() if USE_LCD else None

uart = UART(3, 115200)
red_led, green_led, blue_led = LED(1), LED(2), LED(3)

# 协�??常量
PACKET_SYNC = 0xAA
MODES = [0x00, 0x01, 0x02]  # FORWARD, COLOR, TURN_ASSIST
FLAGS = [0x00, 0x01, 0x02]  # NONE, CROSS, TURN
CMDS = [0x20, 0x21, 0x22]   # RECORDED, DETECT, RESULT

# 颜色配置 - 索引对应: green, white, red, black, blue
COLOR_NAMES = ['green', 'white', 'red', 'black', 'blue']
COLOR_NUMBERS = [1, 2, 3, 4, 5]
COLOR_DISPLAYS = [(255,0,0), (255,255,255), (255,0,0), (128,128,128), (0,255,0)]


class LineFollower:
    def __init__(self):
        # PID控制参数
        self.forward_kp, self.forward_ki, self.forward_kd = 1.5, 0.0, 0.3
        self.forward_last_error = self.forward_integral = 0

        # 图像处理参数
        self.img_width, self.img_height = sensor.width(), sensor.height()
        roi_w, roi_h = self.img_width // 2, self.img_height // 3
        self.roi = ((self.img_width - roi_w) // 2, (self.img_height - roi_h) // 2, roi_w, roi_h)
        self.threshold = [(0, 40, -20, 20, -20, 20)]

        # 交叉�?口�?�测（仅发送标志，不做计数�?
        self.cross_confirm_count = 0
        self.last_cross_time = 0

        # 模式控制
        self.color_mode = 1
        self.turn_assist_mode = 0
        self.turn_state = 'idle'

        # 颜色检测参�?
        self.color_thresholds = [
            [(0, 100, 16, 127, -5, 127)],    # red
            [(43, 19, -128, -17, 34, 7)], # green
            [(0, 57, -128, 127, -12, -58)], # blue
            [(23, 0, -128, 127, -128, 72)]  # black
        ]
        self.min_color_pixels = 60

        # 颜色�?认和记录
        self.color_confirm_history = []
        self.color_confirm_start_time = 0
        self.confirmed_color = None
        self.confirmed_color_array = [None] * 5
        self.confirmed_color_count = 0
        self.detect_black_white_cache = None

        # 颜色检测�?�求
        self.color_detect_request_index = None
        self.color_detect_mode_active = False

    def detect_color(self, img):
        center_x, center_y = self.img_width // 2, self.img_height // 2
        roi_w, roi_h = self.img_width // 2, self.img_height // 3
        color_roi = (center_x - roi_w//2, center_y - roi_h//2, roi_w, roi_h)

        img.draw_rectangle(color_roi, color=(255, 255, 0))

        best_color = None
        best_pixels = 0

        # 检测红绿蓝
        for i, color_name in enumerate(['red', 'green', 'blue']):
            blobs = img.find_blobs(self.color_thresholds[i], roi=color_roi, merge=True, pixels_threshold=self.min_color_pixels)
            if blobs:
                total_pixels = sum(blob.pixels() for blob in blobs)
                if total_pixels > best_pixels:
                    best_pixels = total_pixels
                    best_color = color_name
                for blob in blobs:
                    img.draw_rectangle(blob.rect(), color=COLOR_DISPLAYS[i])

        # 如果没有检测到RGB，�?�测黑�?
        if not best_color:
            best_color = self._detect_black_white(img, color_roi)

        return best_color, {}

    def _detect_black_white(self, img, color_roi, black_threshold=720):
        # 检测黑�?
        black_blobs = img.find_blobs(self.color_thresholds[3], roi=color_roi, merge=True)

        # 统�?�黑色像素数�?
        black_pixel_count = 0
        if black_blobs:
            black_pixel_count = sum(blob.pixels() for blob in black_blobs)
            for blob in black_blobs:
                img.draw_rectangle(blob.rect(), color=(0, 0, 0))

        # 根据传入的阈值判�?颜色
        if black_pixel_count > black_threshold:
            return 'black'
        else:
            img.draw_rectangle(color_roi, color=(255, 255, 255))
            return 'white'

    def detect_vertical_line(self, img):
        center_x, center_y = self.img_width // 2, self.img_height // 2
        roi_w, roi_h = self.img_width // 10, self.img_height // 5
        gap = self.img_width // 3
        gap = self.img_width // 5

        rois = [
            (center_x - gap - roi_w//2, center_y, roi_w, roi_h),  # left
            (center_x - roi_w//2, center_y, roi_w, roi_h),       # center
            (center_x + gap - roi_w//2, center_y, roi_w, roi_h)  # right
        ]

        lines = [len(img.find_blobs(self.threshold, roi=roi, pixels_threshold=15)) > 0 for roi in rois]

        turn_detected = False
        if self.turn_state == 'idle' and (lines[0] or lines[2]) and not lines[1]:
            self.turn_state = 'ready'
        elif self.turn_state == 'ready' and lines[1]:
            self.turn_state = 'triggered'
            turn_detected = True
            green_led.on()  # 转弯检测时绿灯亮起
            if DEBUG:
                print("检测到�?�?!")
        elif self.turn_state == 'triggered' and not any(lines):
            self.turn_state = 'idle'

        for i, roi in enumerate(rois):
            img.draw_rectangle(roi, color=(0, 255, 0) if lines[i] else (255, 0, 0))

        if not turn_detected:
            green_led.off()  # 未检测到转弯时绿灯熄灭

        return turn_detected

    def find_line_multipoint(self, img):
        roi_x, roi_y, roi_w, roi_h = self.roi
        points = []

        for i in range(5):
            y_pos = roi_y + roi_h * (i + 1) // 6
            line_roi = (roi_x, y_pos - 2, roi_w, 4)
            blobs = img.find_blobs(self.threshold, roi=line_roi, merge=True, pixels_threshold=5)

            if blobs:
                line_x = max(blobs, key=lambda b: b.pixels()).cx()
                points.append(line_x)
                img.draw_circle(line_x, y_pos, 2, color=(255, 0, 0))

        if points:
            points.sort()
            return int(sum(points[1:-1]) / (len(points) - 2)) if len(points) >= 3 else int(sum(points) / len(points))
        return None

    def detect_intersection(self, img):
        current_time = time.ticks_ms()
        center_x, center_y = self.img_width // 2, self.img_height // 2
        roi_size = min(self.img_width, self.img_height) // 8

        rois = [
            (center_x - self.img_width//4 - roi_size//2, center_y - roi_size//2, roi_size, roi_size),
            (center_x + self.img_width//4 - roi_size//2, center_y - roi_size//2, roi_size, roi_size)
        ]

        lines_count = 0
        for roi in rois:
            blobs = img.find_blobs(self.threshold, roi=roi, merge=True, pixels_threshold=20)
            if blobs and sum(blob.pixels() for blob in blobs) / (roi[2] * roi[3]) > 0.15:
                lines_count += 1
            img.draw_rectangle(roi, color=(0, 255, 0) if blobs else (255, 0, 0))

        if lines_count == 2:
            self.cross_confirm_count += 1
            if self.cross_confirm_count >= 1 and current_time - self.last_cross_time > 900:
                self.last_cross_time = current_time
                self.cross_confirm_count = 0
                blue_led.on()
                if DEBUG:
                    print("检测到交叉�?�?!")
                return True
        else:
            self.cross_confirm_count = 0

        blue_led.off()
        return False

    def calculate_pid(self, error):
        self.forward_integral += error
        derivative = error - self.forward_last_error
        output = self.forward_kp * error + self.forward_ki * self.forward_integral + self.forward_kd * derivative
        self.forward_last_error = error
        return output

    def motor_control(self, steering_value, is_cross=False, is_turn=False, detected_color=None):
        steering_value = max(-100, min(100, int(steering_value)))

        mode = MODES[1] if self.color_mode else (MODES[2] if self.turn_assist_mode else MODES[0])
        flag = FLAGS[1] if is_cross else (FLAGS[2] if is_turn else FLAGS[0])
        color_num = COLOR_NUMBERS[COLOR_NAMES.index(detected_color)] if detected_color in COLOR_NAMES else 0

        pid_byte = (256 + steering_value) & 0xFF if steering_value < 0 else steering_value & 0xFF
        packet = [PACKET_SYNC, mode, pid_byte, flag, color_num]
        uart.write(bytes(packet))
        if DEBUG:
            mode_names = ["FORWARD", "COLOR", "TURN_ASSIST"]
            flag_names = ["NONE", "CROSS", "TURN"]
            color_name = detected_color if detected_color else "NONE"
            mode_name = mode_names[mode] if mode < len(mode_names) else "UNKNOWN"
            flag_name = flag_names[flag] if flag < len(flag_names) else "UNKNOWN"
            print(f"发�?: 模式={mode_name}, PID={steering_value}, 标志={flag_name}, 颜色={color_name}")
            print(f"数据�?: {[hex(b) for b in packet]}")

    def process_uart_commands(self):
        if uart.any():
            cmd_data = uart.read()
            if len(cmd_data) >= 1:
                cmd = cmd_data[0]

                if DEBUG:
                    print(f"接收命令: {[hex(b) for b in cmd_data]}")

                if cmd in MODES:
                    mode_names = ["FORWARD", "COLOR", "TURN_ASSIST"]
                    mode_name = mode_names[cmd] if cmd < len(mode_names) else "UNKNOWN"
                    if DEBUG:
                        print(f"模式切换: {mode_name}")

                    # 根据模式闪烁不同颜色的LED
                    if cmd == MODES[0]:  # FORWARD模式 - 红色LED
                        red_led.on()
                        time.sleep_ms(200)
                        red_led.off()
                    elif cmd == MODES[1]:  # COLOR模式 - 绿色LED
                        green_led.on()
                        time.sleep_ms(200)
                        green_led.off()
                    elif cmd == MODES[2]:  # TURN_ASSIST模式 - 蓝色LED
                        blue_led.on()
                        time.sleep_ms(200)
                        blue_led.off()

                    self.color_mode = (cmd == MODES[1])
                    self.turn_assist_mode = (cmd == MODES[2])
                    self.color_detect_mode_active = False

                elif len(cmd_data) >= 2:
                    if cmd == CMDS[1]:  # COLOR_DETECT
                        self.color_detect_request_index = cmd_data[1]
                        if DEBUG:
                            print(f"收到颜色检测�?�求: 索引={self.color_detect_request_index}")
                        self.color_detect_mode_active = True
                        self.color_mode = True
                        self._reset_color_confirmation()

                    elif cmd == CMDS[0]:  # COLOR_RECORDED
                        color_index = cmd_data[1]
                        if DEBUG:
                            print(f"主控�?认�?�录颜色: 索引={color_index}")
                        if color_index < 5 and self.confirmed_color:
                            self.confirmed_color_array[color_index] = self.confirmed_color
                            self.confirmed_color_count = max(self.confirmed_color_count, color_index + 1)
                            if DEBUG:
                                print(f"OpenMV记录颜色: 索引={color_index}, 颜色={self.confirmed_color}")
                        self.color_detect_mode_active = False
                        self.color_detect_request_index = None
                        self.confirmed_color = None

    def confirm_color_detection(self, detected_color):
        current_time = time.ticks_ms()

        if not detected_color:
            self._reset_color_confirmation()
            return None

        if not self.color_confirm_history:
            self.color_confirm_start_time = current_time

        if current_time - self.color_confirm_start_time > 3000:
            self.color_confirm_history = []
            self.color_confirm_start_time = current_time

        self.color_confirm_history.append((detected_color, current_time))

        # 统�?�最�?3秒内的�?�测�?�数
        recent_detections = {}
        for color, timestamp in self.color_confirm_history:
            if current_time - timestamp <= 3000:
                recent_detections[color] = recent_detections.get(color, 0) + 1

        # 检查是否有颜色�?检测到足�?��?�数
        for color, count in recent_detections.items():
            if count >= 8:
                self.confirmed_color = color
                if DEBUG:
                    print(f"颜色�?�?: {color} (检测�?�数: {count})")
                return self.process_confirmed_color(color)

        return None

    def _reset_color_confirmation(self):
        self.color_confirm_history = []
        self.color_confirm_start_time = 0
        self.confirmed_color = None



    def determine_color_by_index(self, index, detected_color):
        if index == 1:
            return self._determine_fourth_color(detected_color)
        elif index == 3:
            return self._determine_fifth_color()
        else:
            return detected_color

    def _determine_fourth_color(self, c4_detected):
        recorded_colors = set(filter(None, self.confirmed_color_array[:3]))

        if c4_detected in {'red', 'green', 'blue'}:
            if c4_detected not in recorded_colors:
                return c4_detected
            else:
                return self.detect_black_white_cache or 'white'
        elif c4_detected in {'black', 'white'}:
            if 'black' in recorded_colors:
                return 'white'
            elif 'white' in recorded_colors:
                return 'black'
            else:
                return c4_detected
        else:
            return 'white'

    def _determine_fifth_color(self):
        all_colors = {'red', 'green', 'blue', 'white', 'black'}
        recorded_colors = set(filter(None, self.confirmed_color_array[:4]))
        remaining = all_colors - recorded_colors
        return list(remaining)[0] if remaining else 'white'

    def process_confirmed_color(self, confirmed_color):
        if self.color_detect_request_index is None:
            return confirmed_color
        return self.determine_color_by_index(self.color_detect_request_index, confirmed_color)

    def send_color_result(self, index, color):
        color_number = COLOR_NUMBERS[COLOR_NAMES.index(color)] if color in COLOR_NAMES else 0
        packet = [CMDS[2], index, color_number]
        uart.write(bytes(packet))
        if DEBUG:
            print(f"发送�?�色结果: 索引={index}, 颜色={color}, 编号={color_number}")
            print(f"颜色结果�?: {[hex(b) for b in packet]}")

    def run(self):
        green_led.on()
        while True:
            clock.tick()
            self.process_uart_commands()
            img = sensor.snapshot()
            center_x, center_y = self.img_width // 2, self.img_height // 2
            img.draw_cross(center_x, center_y, size=5, color=(255, 255, 255))

            if self.turn_assist_mode:
                self.motor_control(0, is_turn=self.detect_vertical_line(img))

            elif self.color_mode:
                detected_color, _ = self.detect_color(img)

                # 更新黑白检测缓存用于�??4�?颜色判断
                center_x, center_y = self.img_width // 2, self.img_height // 2
                roi_w, roi_h = self.img_width // 2, self.img_height // 3
                color_roi = (center_x - roi_w//2, center_y - roi_h//2, roi_w, roi_h)
                self.detect_black_white_cache = self._detect_black_white(img, color_roi, 350)

                confirmed_color = self.confirm_color_detection(detected_color)

                if confirmed_color:
                    if self.color_detect_mode_active and self.color_detect_request_index is not None:
                        print(f"颜色�?认完�?: 索引={self.color_detect_request_index}, 颜色={confirmed_color}")
                        self.send_color_result(self.color_detect_request_index, confirmed_color)
                    else:
                        print(f"检测到颜色: {confirmed_color}")
                        self.motor_control(0, detected_color=confirmed_color)
                    self.color_confirm_history = []
                else:
                    self.motor_control(0)

            else:
                img.draw_rectangle(self.roi, color=255)

                if self.detect_intersection(img):
                    red_led.on()
                    self.motor_control(0, is_cross=True)
                    time.sleep_ms(500)
                    red_led.off()
                else:
                    red_led.off()
                    line_x = self.find_line_multipoint(img)
                    if line_x is not None:
                        error = line_x - (self.roi[0] + self.roi[2] // 2)
                        pid_output = self.calculate_pid(error)
                        self.motor_control(pid_output)
                        img.draw_circle(line_x, 60, 5, color=255)
                    else:
                        self.motor_control(0)
            if lcd:
                if DEBUG:
                    modes = ["LINE", "RGB COLOR", "TURN"]
                    colors = [(255, 255, 0), (255, 0, 255), (0, 255, 255)]
                    mode_idx = 1 if self.color_mode else (2 if self.turn_assist_mode else 0)
                    img.draw_string(10, 10, modes[mode_idx], color=colors[mode_idx])
                lcd.write(img)



if __name__ == "__main__":
    line_follower = LineFollower()
    line_follower.run()

