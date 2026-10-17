# FreeRTOS Example 2: Using the Task Parameter

Dự án này là bản chuyển đổi chuẩn hóa của **Example 2: Using the Task Parameter** sang nền tảng **ESP-IDF** (ESP32 / ESP32-C3).

---

## 1. Cấu trúc thư mục dự án

```text
example2_task_parameter/
├── CMakeLists.txt              # CMake root cho project ESP-IDF
├── sdkconfig.defaults          # Cấu hình UNICORE
├── README.md                   # Tài liệu hướng dẫn
└── main/
    ├── CMakeLists.txt          # Khai báo component main
    └── main.c                  # Mã nguồn chính
```

---

## 2. Mục tiêu ví dụ
- Tái sử dụng một hàm task duy nhất (`vTaskFunction`) cho nhiều task instance.
- Truyền tham số khác nhau (chuỗi tên task) qua con trỏ `pvParameters` trong hàm `xTaskCreate()`.
- Hiểu cách dữ liệu được truyền vào task khi khởi tạo.

---

## 3. Cách biên dịch và nạp

```bash
cd c:/he_thong_nhung/example2_task_parameter
idf.py build
idf.py -p COM10 flash monitor
```
