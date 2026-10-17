# FreeRTOS Example 1: Creating Tasks

Dự án này là bản chuyển đổi chuẩn hóa của **Example 1: Creating Tasks** sang nền tảng **ESP-IDF** (ESP32 / ESP32-C3).

---

## 1. Cấu trúc thư mục dự án

```text
example1_creating_tasks/
├── CMakeLists.txt              # CMake root cho project ESP-IDF
├── sdkconfig.defaults          # Cấu hình UNICORE
├── README.md                   # Tài liệu hướng dẫn
└── main/
    ├── CMakeLists.txt          # Khai báo component main
    └── main.c                  # Mã nguồn chính
```

---

## 2. Mục tiêu ví dụ
- Minh họa cách tạo 2 task độc lập (`vTask1` và `vTask2`) bằng hàm `xTaskCreate()`.
- Cả 2 task có cùng độ ưu tiên (Priority 1).
- Trì hoãn thời gian bằng vòng lặp busy-wait (null loop).
- Scheduler của FreeRTOS sẽ chia sẻ thời gian (time-slicing) luân phiên giữa 2 task.

---

## 3. Cách biên dịch và nạp

```bash
cd c:/he_thong_nhung/example1_creating_tasks
idf.py build
idf.py -p COM10 flash monitor
```
