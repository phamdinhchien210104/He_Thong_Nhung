# FreeRTOS Example 13: Creating One-Shot and Auto-Reload Software Timers

Dự án này là bản chuyển đổi chuẩn hóa của **Example 13: Creating One-Shot and Auto-Reload Timers** sang nền tảng **ESP-IDF** (ESP32 / ESP32-C3).

---

## 1. Cấu trúc thư mục dự án

```text
example13_Creating one-shot and auto-reload timers/
├── CMakeLists.txt              # CMake root cho project ESP-IDF
├── sdkconfig.defaults          # Cấu hình UNICORE
├── README.md                   # Tài liệu hướng dẫn
└── main/
    ├── CMakeLists.txt          # Khai báo component main
    └── main.c                  # Mã nguồn chính
```

---

## 2. Mục tiêu ví dụ
- Khởi tạo và quản lý **Software Timers** trong FreeRTOS thông qua API `xTimerCreate()` và `xTimerStart()`.
- So sánh sự khác biệt bản chất giữa hai loại Timer:
  - **One-Shot Timer (`pdFALSE`):** Chỉ chạy đúng 1 lần khi hết thời gian định thời (3333ms), kích hoạt callback `prvOneShotTimerCallback` rồi tự động dừng lại ở trạng thái Dormant.
  - **Auto-Reload Timer (`pdTRUE`):** Tự động nạp lại chu kỳ và kích hoạt callback `prvAutoReloadTimerCallback` liên tục theo chu kỳ cố định (500ms).
- Callback của timer được thực thi trong ngữ cảnh của Timer Service Daemon Task (RTOS Daemon Task), không chạy trực tiếp trong ngắt phần cứng ISR.

---

## 3. Cách biên dịch và nạp

```bash
cd "c:/he_thong_nhung/example13_Creating one-shot and auto-reload timers"
idf.py set-target esp32c3
idf.py build
idf.py -p COM6 flash monitor
```
