# FreeRTOS Example 17: Using a Counting Semaphore to Synchronize a Task with an Interrupt

Dự án này là bản chuyển đổi chuẩn hóa của **Example 17: Using a Counting Semaphore to Synchronize a Task with an Interrupt** sang nền tảng **ESP-IDF** (ESP32 / ESP32-C3).

---

## 1. Cấu trúc thư mục dự án

```text
example17_counting_semaphore/
├── CMakeLists.txt              # CMake root cho project ESP-IDF
├── sdkconfig.defaults          # Cấu hình target esp32c3 và timer ISR dispatch
├── README.md                   # Tài liệu hướng dẫn
└── main/
    ├── CMakeLists.txt          # Khai báo component main
    └── main.c                  # Mã nguồn chính
```

---

## 2. Mục tiêu ví dụ
- Khắc phục nhược điểm của **Binary Semaphore** (chỉ lưu được tối đa 1 sự kiện, nếu ngắt đến dồn dập thì các sự kiện sau sẽ bị mất).
- Sử dụng **Counting Semaphore** (`xSemaphoreCreateCounting(10, 0)`):
  - Khởi tạo giá trị đếm bằng 0 (`uxInitialCount = 0`).
  - Giá trị đếm tối đa là 10 (`uxMaxCount = 10`).
- Trong ngắt mô phỏng (`vExampleInterruptHandler`), gọi `xSemaphoreGiveFromISR()` **3 lần** liên tiếp.
- `vHandlerTask` (ưu tiên 3) sẽ lấy semaphore và in log đủ **3 lần** tương ứng với 3 sự kiện được bảo lưu.

---

## 3. Cách biên dịch và nạp

```bash
cd c:/he_thong_nhung/example17_counting_semaphore
idf.py set-target esp32c3
idf.py build
idf.py -p COM7 flash monitor
```
