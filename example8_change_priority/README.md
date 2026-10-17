# FreeRTOS Example 8: Changing Task Priorities

Dự án này là bản chuyển đổi chuẩn hóa của **Example 8: Changing Task Priorities** sang nền tảng **ESP-IDF** (ESP32 / ESP32-C3).

---

## 1. Cấu trúc thư mục dự án

```text
example8_change_priority/
├── CMakeLists.txt              # CMake root cho project ESP-IDF
├── sdkconfig.defaults          # Cấu hình UNICORE
├── README.md                   # Tài liệu hướng dẫn
└── main/
    ├── CMakeLists.txt          # Khai báo component main
    └── main.c                  # Mã nguồn chính
```

---

## 2. Mục tiêu ví dụ
- Tìm hiểu cách truy vấn và thay đổi độ ưu tiên của một Task trong thời gian chạy (runtime).
- Sử dụng hàm `uxTaskPriorityGet()` để lấy priority hiện tại.
- Sử dụng hàm `vTaskPrioritySet()` để thay đổi priority của task khác hoặc chính mình (truyền `NULL` làm handle).
- Quan sát cơ chế preemption ngay lập tức khi độ ưu tiên thay đổi.

---

## 3. Cách biên dịch và nạp

```bash
cd c:/he_thong_nhung/example8_change_priority
idf.py build
idf.py -p COM10 flash monitor
```
