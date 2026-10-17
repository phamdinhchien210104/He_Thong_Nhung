# FreeRTOS Example 15: Resetting a Software Timer

Dự án này là bản chuyển đổi chuẩn hóa của **Example 15: Resetting a Software Timer** sang nền tảng **ESP-IDF** (ESP32 / ESP32-C3).

---

## 1. Cấu trúc thư mục dự án

```text
example15_Resetting_software_timer/
├── CMakeLists.txt              # CMake root cho project ESP-IDF
├── sdkconfig.defaults          # Cấu hình UNICORE
├── README.md                   # Tài liệu hướng dẫn
└── main/
    ├── CMakeLists.txt          # Khai báo component main
    └── main.c                  # Mã nguồn chính
```

---

## 2. Mục tiêu ví dụ
- Minh họa ứng dụng thực tế điển hình của Software Timer: **mô phỏng đèn nền màn hình (LCD backlight timeout)**.
- Khi người dùng nhấn phím (hoặc có sự kiện tương tác trong task `vKeyHitTask`), đèn nền bật lên (`xSimulatedBacklightOn = pdTRUE`).
- Hàm `xTimerReset()` được gọi mỗi khi có tương tác mới, làm mới lại khoảng đếm ngược 5000ms (`mainBACKLIGHT_TIMER_PERIOD`).
- Nếu trong suốt 5000ms không có phím nào được bấm, timer sẽ hết hạn và kích hoạt hàm callback `prvBacklightTimerCallback()` để tự động tắt đèn nền (`xSimulatedBacklightOn = pdFALSE`).
- Minh họa cách dùng `xTimerReset()` để xây dựng các cơ chế watchdog mềm, timeout màn hình hoặc tự động ngắt kết nối khi không có hoạt động.

---

## 3. Cách biên dịch và nạp

```bash
cd c:/he_thong_nhung/example15_Resetting_software_timer
idf.py set-target esp32c3
idf.py build
idf.py -p COM6 flash monitor
```
