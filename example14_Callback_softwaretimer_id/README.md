# FreeRTOS Example 14: Using the Callback Function Parameter and Software Timer ID

Dự án này là bản chuyển đổi chuẩn hóa của **Example 14: Using the Callback Function Parameter and the Software Timer ID** sang nền tảng **ESP-IDF** (ESP32 / ESP32-C3).

---

## 1. Cấu trúc thư mục dự án

```text
example14_Callback_softwaretimer_id/
├── CMakeLists.txt              # CMake root cho project ESP-IDF
├── sdkconfig.defaults          # Cấu hình UNICORE
├── README.md                   # Tài liệu hướng dẫn
└── main/
    ├── CMakeLists.txt          # Khai báo component main
    └── main.c                  # Mã nguồn chính
```

---

## 2. Mục tiêu ví dụ
- Tái sử dụng một hàm callback duy nhất (`prvTimerCallback`) cho nhiều Software Timer khác nhau (`xOneShotTimer` và `xAutoReloadTimer`).
- Sử dụng tham số `xTimer` truyền vào callback để phân biệt timer nào vừa hết hạn thông qua hàm `xTimer == xAutoReloadTimer` hoặc `pvTimerGetTimerID(xTimer)`.
- Sử dụng trường **Timer ID** như một biến lưu trữ dữ liệu riêng cho từng timer (trong ví dụ này dùng để đếm số lần kích hoạt `ulExecutionCount`).
- Minh họa cách cập nhật Timer ID bằng `vTimerSetTimerID()` và đọc lại bằng `pvTimerGetTimerID()`. Khi số lần chạy đạt giới hạn (5 lần), tự động dừng timer bằng `xTimerStop()`.

---

## 3. Cách biên dịch và nạp

```bash
cd c:/he_thong_nhung/example14_Callback_softwaretimer_id
idf.py set-target esp32c3
idf.py build
idf.py -p COM6 flash monitor
```
