# FreeRTOS Example 19: Using Queues within an Interrupt Service Routine

Dự án này là bản chuyển đổi chuẩn hóa của **Example 19: Using Queues within an Interrupt Service Routine** sang nền tảng **ESP-IDF** (ESP32 / ESP32-C3).

---

## 1. Cấu trúc thư mục dự án

```text
example19_queue_in_isr/
├── CMakeLists.txt              # CMake root cho project ESP-IDF
├── sdkconfig.defaults          # Cấu hình target esp32c3 và timer ISR dispatch
├── README.md                   # Tài liệu hướng dẫn
└── main/
    ├── CMakeLists.txt          # Khai báo component main
    └── main.c                  # Mã nguồn chính
```

---

## 2. Mục tiêu ví dụ
- Khác với Semaphore (chỉ truyền tín hiệu sự kiện, không mang theo dữ liệu), **Hàng đợi (Queue)** vừa dùng để báo hiệu sự kiện vừa **truyền nhận dữ liệu** giữa Task và ISR.
- Minh họa việc sử dụng song song cả 2 hàm API:
  - `xQueueReceiveFromISR()`: Nhận dữ liệu từ hàng đợi bên trong ISR.
  - `xQueueSendToBackFromISR()`: Gửi dữ liệu vào hàng đợi từ bên trong ISR.
- Kỹ thuật tối ưu hóa: Truyền **con trỏ (pointer)** đến chuỗi thay vì copy mảng chuỗi lớn, giúp thời gian thực thi của ISR cực ngắn.
- **Tính năng ngắt phần cứng nút nhấn BOOT (GPIO 9 trên ESP32-C3)**:
  - Nút BOOT được cấu hình làm nguồn kích hoạt ngắt ngoại vi (GPIO Interrupt) với sườn âm (`GPIO_INTR_NEGEDGE`) và có pull-up nội.
  - Tích hợp bộ lọc chống rung phần mềm (Debounce) 150ms bằng `esp_timer_get_time()`.
  - Luồng hoạt động:
    1. Task `vIntegerGenerator` (ưu tiên 1) định kỳ mỗi 2 giây gửi 5 số nguyên vào `xIntegerQueue`.
    2. Khi người dùng **nhấn nút BOOT trên mạch**:
       - Ngắt phần cứng GPIO kích hoạt hàm `vButtonIsrHandler` (đặt trong IRAM với `IRAM_ATTR`).
       - ISR đọc hết toàn bộ số nguyên trong `xIntegerQueue` qua `xQueueReceiveFromISR()`.
       - Mỗi số nguyên được lấy 2 bit cuối làm chỉ mục chọn chuỗi, sau đó đẩy con trỏ chuỗi vào `xStringQueue` qua `xQueueSendToBackFromISR()`.
       - Hàm ISR gọi `portYIELD_FROM_ISR(xHigherPriorityTaskWoken)` để yêu cầu chuyển ngữ cảnh ngay.
    3. Task `vStringPrinter` (ưu tiên 2) được đánh thức lập tức và in toàn bộ chuỗi nhận được ra màn hình Terminal.

---

## 3. Cách biên dịch và nạp

```bash
cd c:/he_thong_nhung/example19_queue_in_isr
idf.py build
idf.py -p COM7 flash monitor
```

Khi chạy:
- Terminal sẽ thông báo `IntGen: Da them 5 so vao xIntegerQueue... [Hay bam nut BOOT de doc!]`.
- Hãy nhấn nút **BOOT** (gần cổng USB) trên board ESP32-C3:
- Ngắt phần cứng sẽ lập tức kích hoạt và in ra:
```text
Printer received: String 0 [Tu nut BOOT - GPIO 9]
Printer received: String 1 [Tu nut BOOT - GPIO 9]
Printer received: String 2 [Tu nut BOOT - GPIO 9]
Printer received: String 3 [Tu nut BOOT - GPIO 9]
Printer received: String 0 [Tu nut BOOT - GPIO 9]
```
