# FreeRTOS Example 4: Using the Block State to Create a Delay

Dự án này là bản chuyển đổi chuẩn hóa của **Example 4: Using the Block State to Create a Delay** sang nền tảng **ESP-IDF** (ESP32 / ESP32-C3).

---

## 1. Cấu trúc thư mục dự án

```text
example4_block_delay/
├── CMakeLists.txt              # CMake root cho project ESP-IDF
├── sdkconfig.defaults          # Cấu hình UNICORE
├── README.md                   # Tài liệu hướng dẫn
└── main/
    ├── CMakeLists.txt          # Khai báo component main
    └── main.c                  # Mã nguồn chính
```

---

## 2. Mục tiêu ví dụ
- Khắc phục nhược điểm của busy-wait bằng cách sử dụng `vTaskDelay()`.
- Khi gọi `vTaskDelay(pdMS_TO_TICKS(250))`, task chuyển sang trạng thái Blocked.
- Trong thời gian task bị Blocked, CPU được nhường cho task khác thực thi (kể cả task có độ ưu tiên thấp hơn).

---

## 3. Cách biên dịch và nạp

```bash
cd c:/he_thong_nhung/example4_block_delay
idf.py build
idf.py -p COM10 flash monitor
```
