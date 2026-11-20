# FreeRTOS Example 18: Centralized Deferred Interrupt Processing

Dự án này là bản chuyển đổi chuẩn hóa của **Example 18: Centralized Deferred Interrupt Processing** sang nền tảng **ESP-IDF** (ESP32 / ESP32-C3).

---

## 1. Cấu trúc thư mục dự án

```text
example18_centralized_deferred_interrupt/
├── CMakeLists.txt              # CMake root cho project ESP-IDF
├── sdkconfig.defaults          # Cấu hình target esp32c3, timer ISR dispatch và timer daemon task
├── README.md                   # Tài liệu hướng dẫn
└── main/
    ├── CMakeLists.txt          # Khai báo component main
    └── main.c                  # Mã nguồn chính
```

---

## 2. Mục tiêu ví dụ
- Minh họa cơ chế **Centralized Deferred Interrupt Processing** (Trì hoãn xử lý ngắt tập trung).
- Thay vì phải tạo riêng một task `vHandlerTask` và tạo `xSemaphore` để đồng bộ:
  - Sử dụng hàm `xTimerPendFunctionCallFromISR()` để gửi trực tiếp con trỏ hàm `vDeferredHandlingFunction` và tham số vào hàng đợi của **RTOS Daemon Task** (Timer Service Task).
  - Daemon Task nhận lệnh và gọi thực thi hàm `vDeferredHandlingFunction(pvParameter1, ulParameter2)` trong ngữ cảnh task thông thường.
- Ưu điểm:
  - **Tiết kiệm RAM**: Không tốn bộ nhớ cấp phát TCB và Stack cho một task handler riêng cho từng ngắt.
  - **Mô hình lập trình đơn giản**: Hàm xử lý chỉ là một hàm C thông thường có chữ ký `void (*)(void *, uint32_t)`.
- Nhược điểm:
  - Tất cả các ngắt dùng cơ chế này đều chạy chung ở mức ưu tiên của Daemon Task (`configTIMER_TASK_PRIORITY`), ít linh hoạt hơn khi cần các mức ưu tiên khác nhau cho từng loại ngắt.
  - Phải đợi các lệnh đến trước trong Timer Queue được xử lý xong.

---

## 3. Cách biên dịch và nạp

```bash
cd c:/he_thong_nhung/example18_centralized_deferred_interrupt
idf.py set-target esp32c3
idf.py build
idf.py -p COM7 flash monitor
```
