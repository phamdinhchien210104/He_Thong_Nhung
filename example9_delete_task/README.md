# FreeRTOS Example 9: Deleting a Task

Dự án này là bản chuyển đổi chuẩn hóa của **Example 9: Deleting a Task** sang nền tảng **ESP-IDF** (ESP32 / ESP32-C3).

---

## 1. Cấu trúc thư mục dự án

```text
example9_delete_task/
├── CMakeLists.txt              # CMake root cho project ESP-IDF
├── sdkconfig.defaults          # Cấu hình UNICORE
├── README.md                   # Tài liệu hướng dẫn
└── main/
    ├── CMakeLists.txt          # Khai báo component main
    └── main.c                  # Mã nguồn chính
```

---

## 2. Mục tiêu ví dụ
- Minh họa cách một Task tự kết thúc vòng đời bằng hàm `vTaskDelete()`.
- Task 1 tạo Task 2 với độ ưu tiên cao hơn.
- Task 2 chạy, in thông báo và tự xóa mình (`vTaskDelete(xTask2Handle)` hoặc `vTaskDelete(NULL)`).
- Task 1 dùng `vTaskDelay()` để nhường quyền cho Idle Task giải phóng tài nguyên (TCB, stack) của Task 2 đã bị xóa.

---

## 3. Cách biên dịch và nạp

```bash
cd c:/he_thong_nhung/example9_delete_task
idf.py build
idf.py -p COM10 flash monitor
```
