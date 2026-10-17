# FreeRTOS Example 5: Converting Tasks to Use vTaskDelayUntil()

Dự án này là bản chuyển đổi chuẩn hóa của **Example 5: Converting the Example Tasks to Use vTaskDelayUntil()** sang nền tảng **ESP-IDF** (ESP32 / ESP32-C3).

---

## 1. Cấu trúc thư mục dự án

```text
example5_delay_until/
├── CMakeLists.txt              # CMake root cho project ESP-IDF
├── sdkconfig.defaults          # Cấu hình UNICORE
├── README.md                   # Tài liệu hướng dẫn
└── main/
    ├── CMakeLists.txt          # Khai báo component main
    └── main.c                  # Mã nguồn chính
```

---

## 2. Mục tiêu ví dụ
- So sánh `vTaskDelay()` và `vTaskDelayUntil()`.
- `vTaskDelay()` chỉ tính thời gian trễ tương đối từ thời điểm gọi hàm.
- `vTaskDelayUntil()` đảm bảo chu kỳ thực thi cố định (fixed frequency execution), không bị trôi thời gian (drift) dù phần xử lý bên trong vòng lặp mất bao lâu.

---

## 3. Cách biên dịch và nạp

```bash
cd c:/he_thong_nhung/example5_delay_until
idf.py build
idf.py -p COM10 flash monitor
```
