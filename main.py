import sensor, time, math, display
from pyb import UART, LED

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time=1000)
sensor.set_vflip(True)
sensor.set_hmirror(True)
clock = time.clock()
lcd = display.SPIDisplay()

uart = UART(3, 115200)
red_led, green_led, blue_led = LED(1), LED(2), LED(3)

# 通信协�??
PACKET_SYNC = 0xAA

MODE_FORWARD = 0x00
MODE_COLOR = 0x01
MODE_TURN_ASSIST = 0x02

FLAG_NONE = 0x00
CROSS_DETECTED = 0x01
TURN_DETECTED = 0x02

COLOR_NONE = 0x00
COLOR_RED = 0x01
COLOR_BLACK = 0x02
COLOR_WHITE = 0x03
COLOR_BLUE = 0x04
COLOR_GREEN = 0x05

class LineFollower:
    def __init__(self):
        self.forward_kp, self.forward_ki, self.forward_kd = 1.5, 0.0, 0.3
        self.forward_last_error = self.forward_integral = 0

        self.cross_count = self.cross_confirm_count = 0
        self.last_cross_time = 0
        self.cross_threshold = 900
        self.cross_confirm_threshold = 1

        self.img_width, self.img_height = sensor.width(), sensor.height()
        roi_w, roi_h = self.img_width // 2, self.img_height // 3
        self.roi = ((self.img_width - roi_w) // 2, (self.img_height - roi_h) // 2, roi_w, roi_h)
        self.threshold = [(0, 40, -20, 20, -20, 20)]

        self.color_mode = 0
        self.color_thresholds = {
            'red': [(20, 100, 15, 127, 15, 127)],
            'green': [(30, 100, -64, -8, -32, 32)],
            'blue': [(0, 30, -10, 40, -80, -20)],
            'white': [(80, 100, -20, 20, -20, 20)],
            'black': [(0, 30, -20, 20, -20, 20)]
        }
        self.detected_color = None
        self.color_detection_confidence = 0
        self.min_color_pixels = 50

        self.turn_assist_mode = False
        self.last_vertical_time = 0

        # �?向�?�测状�?
        self.turn_state = 'idle'

    def detect_color(self, img):
        best_color = None
        best_pixels = 0
        detection_results = {}

        center_x, center_y = self.img_width // 2, self.img_height // 2
        detect_size = min(self.img_width, self.img_height) // 3
        y_offset = self.img_height // 3
        color_roi = (center_x - detect_size//2, center_y - detect_size//2 - y_offset, detect_size, detect_size)

        img.draw_rectangle(color_roi, color=(255, 255, 0))

        for color_name, threshold in self.color_thresholds.items():
            blobs = img.find_blobs(threshold, roi=color_roi, merge=True, pixels_threshold=self.min_color_pixels)

            if blobs:
                total_pixels = sum(blob.pixels() for blob in blobs)
                detection_results[color_name] = total_pixels

                if total_pixels > best_pixels:
                    best_pixels = total_pixels
                    best_color = color_name

                for blob in blobs:
                    img.draw_rectangle(blob.rect(), color=(0, 255, 0))
                    img.draw_cross(blob.cx(), blob.cy(), size=5, color=(255, 0, 0))

        if best_color and best_pixels > self.min_color_pixels:
            self.detected_color = best_color
            self.color_detection_confidence = best_pixels
        else:
            self.detected_color = None
            self.color_detection_confidence = 0

        return self.detected_color, detection_results

    def detect_vertical_line(self, img):
        center_x, center_y = self.img_width // 2, self.img_height // 2
        roi_w = self.img_width // 10
        roi_h = self.img_height // 5
        gap = int(self.img_width // 2.5)

        left_roi = (center_x - gap - roi_w//2, center_y, roi_w, roi_h)
        center_roi = (center_x - roi_w//2, center_y, roi_w, roi_h)
        right_roi = (center_x + gap - roi_w//2, center_y, roi_w, roi_h)

        # 检测黑色区�?
        left_line = len(img.find_blobs(self.threshold, roi=left_roi, pixels_threshold=30)) > 0
        center_line = len(img.find_blobs(self.threshold, roi=center_roi, pixels_threshold=3)) > 0
        right_line = len(img.find_blobs(self.threshold, roi=right_roi, pixels_threshold=3)) > 0

        # �?掠�?�测状态机
        turn_detected = False

        if self.turn_state == 'idle':
            if (left_line or right_line) and not center_line:
                self.turn_state = 'side_ready'
                img.draw_string(10, 80, "SIDE READY", color=(255, 255, 0))

        elif self.turn_state == 'side_ready':
            if center_line:
                self.turn_state = 'triggered'
                turn_detected = True
                img.draw_cross(center_x, center_y, size=15, color=(255, 255, 0))
                img.draw_string(10, 80, "TRIGGERED!", color=(0, 255, 0))

        elif self.turn_state == 'triggered':
            if not left_line and not center_line and not right_line:
                self.turn_state = 'idle'

        # 绘制ROI
        left_color = (0, 255, 0) if left_line else (255, 0, 0)
        center_color = (0, 255, 0) if center_line else (255, 0, 0)
        right_color = (0, 255, 0) if right_line else (255, 0, 0)

        img.draw_rectangle(left_roi, color=left_color)
        img.draw_rectangle(center_roi, color=center_color)
        img.draw_rectangle(right_roi, color=right_color)

        return turn_detected

    def find_line_multipoint(self, img):
        roi_x, roi_y, roi_w, roi_h = self.roi
        detection_points = []

        segments = 5
        for i in range(segments):
            y_offset = int(roi_h * (i + 1) / (segments + 1))
            y_pos = roi_y + y_offset

            line_roi = (roi_x, y_pos - 2, roi_w, 4)
            blobs = img.find_blobs(self.threshold, roi=line_roi, merge=True, pixels_threshold=5)

            if blobs:
                line_x = max(blobs, key=lambda b: b.pixels()).cx()
                detection_points.append(line_x)
                img.draw_circle(line_x, y_pos, 2, color=(255, 0, 0))

        if detection_points:
            if len(detection_points) >= 3:
                detection_points.sort()
                avg_x = sum(detection_points[1:-1]) / (len(detection_points) - 2)
            else:
                avg_x = sum(detection_points) / len(detection_points)
            return int(avg_x)
        return None

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
            blobs = img.find_blobs(self.threshold, roi=roi, merge=True, pixels_threshold=20)
            if blobs:
                density = sum(blob.pixels() for blob in blobs) / (roi[2] * roi[3])
                if density > 0.15:
                    directions_with_lines += 1
            img.draw_rectangle(roi, color=(0, 255, 0) if blobs else (255, 0, 0))

        if directions_with_lines == 2:
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

    def calculate_pid(self, error, kp, ki, kd, last_error_attr, integral_attr):
        integral = getattr(self, integral_attr)
        last_error = getattr(self, last_error_attr)

        # 更新�?�?
        integral += error
        setattr(self, integral_attr, integral)

        # 计算�?�?
        derivative = error - last_error

        # PID输出
        output = kp * error + ki * integral + kd * derivative

        # 更新上�?��??�?
        setattr(self, last_error_attr, error)

        return output

    def motor_control(self, steering_value, is_cross=False, is_turn=False, detected_color=None):
        steering_value = max(-100, min(100, int(steering_value)))

        packet = [PACKET_SYNC]

        if self.color_mode:
            packet.append(MODE_COLOR)
        elif self.turn_assist_mode:
            packet.append(MODE_TURN_ASSIST)
        else:
            packet.append(MODE_FORWARD)

        if steering_value < 0:
            pid_byte = (256 + steering_value) & 0xFF
        else:
            pid_byte = steering_value & 0xFF
        packet.append(pid_byte)

        if is_cross:
            packet.append(CROSS_DETECTED)
        elif is_turn:
            packet.append(TURN_DETECTED)
        else:
            packet.append(FLAG_NONE)
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

        uart.write(bytes(packet))

        # 打印串口发送信�?
        flag_text = "CROSS" if is_cross else ("TURN" if is_turn else "NONE")
        color_text = detected_color or "NONE"
        mode_text = "TURN_ASSIST" if self.turn_assist_mode else ("COLOR" if self.color_mode else "FORWARD")
        print(f"UART发�?: {[hex(b) for b in packet]} - 模式:{mode_text}, PID:{steering_value}, 标志:{flag_text}, 颜色:{color_text}")
    def process_uart_commands(self):
        if uart.any():
            cmd_data = uart.read(1)
            if len(cmd_data) == 1:
                cmd = cmd_data[0]

                if cmd == MODE_FORWARD:
                    self.color_mode = False
                    self.turn_assist_mode = False
                elif cmd == MODE_COLOR:
                    self.color_mode = True
                    self.turn_assist_mode = False
                elif cmd == MODE_TURN_ASSIST:
                    self.turn_assist_mode = True
                    self.color_mode = False

    def run(self):
        green_led.on()
        while True:
            clock.tick()

            # 处理串口命令
            self.process_uart_commands()

            img = sensor.snapshot()
            center_x, center_y = self.img_width // 2, self.img_height // 2
            img.draw_cross(center_x, center_y, size=5, color=(255, 255, 255))

            if self.turn_assist_mode:
                if self.detect_vertical_line(img):
                    self.motor_control(0, is_turn=True)
                else:
                    self.motor_control(0)

            elif self.color_mode:
                detected_color, color_results = self.detect_color(img)

                if detected_color:
                    self.motor_control(0, detected_color=detected_color)
                    img.draw_string(10, 115, f"{detected_color.upper()}", color=(255, 255, 0))
                else:
                    self.motor_control(0)
            else:
                img.draw_rectangle(self.roi, color=255)

                if self.detect_intersection(img):
                    red_led.on()
                    self.motor_control(0, is_cross=True)
                    time.sleep_ms(200)
                    red_led.off()

                else:
                    red_led.off()

                    line_x = self.find_line_multipoint(img)
                    if line_x is not None:
                        error = line_x - (self.roi[0] + self.roi[2] // 2)
                        pid_output = self.calculate_pid(
                            error,
                            self.forward_kp, self.forward_ki, self.forward_kd,
                            'forward_last_error', 'forward_integral'
                        )
                        self.motor_control(pid_output)
                        img.draw_circle(line_x, 60, 5, color=255)
                    else:
                        self.motor_control(0)
            if self.turn_assist_mode:
                mode_text = "TURN"
                mode_color = (0, 255, 255)
            elif self.color_mode:
                mode_text = "COLOR"
                mode_color = (255, 0, 255)
            else:
                mode_text = "LINE"
                mode_color = (255, 255, 0)
            img.draw_string(10, 10, mode_text, color=mode_color)
            if self.turn_assist_mode:
                img.draw_string(10, 25, f"Turn Assist", color=255)
            else:
                img.draw_string(10, 25, f"Cross: {self.cross_count}", color=255)
            lcd.write(img)

if __name__ == "__main__":
    line_follower = LineFollower()
    line_follower.run()