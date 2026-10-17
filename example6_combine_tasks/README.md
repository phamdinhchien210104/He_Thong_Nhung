# FreeRTOS Example 6: Combining Blocking and Non-Blocking Tasks

Dự án này là bản chuyển đổi chuẩn hóa của **Example 6: Combining Blocking and Non-Blocking Tasks** sang nền tảng **ESP-IDF** (ESP32 / ESP32-C3).

---

## 1. Cấu trúc thư mục dự án

```text
example6_combine_tasks/
├── CMakeLists.txt              # CMake root cho project ESP-IDF
├── sdkconfig.defaults          # Cấu hình UNICORE
├── README.md                   # Tài liệu hướng dẫn
└── main/
    ├── CMakeLists.txt          # Khai báo component main
    └── main.c                  # Mã nguồn chính
```

---

## 2. Mục tiêu ví dụ
- Kết hợp giữa các Continuous Processing Tasks (độ ưu tiên thấp - Priority 1) và Periodic Task (độ ưu tiên cao - Priority 2).
- Khi Periodic Task đến hạn đánh thức (bằng `vTaskDelayUntil()`), nó lập tức chiếm quyền CPU (preempt) từ Continuous Task.
- Khi Periodic Task hoàn thành công việc và đi ngủ, Continuous Task tiếp tục chạy nền.

---

## 3. Cách biên dịch và nạp

```bash
cd c:/he_thong_nhung/example6_combine_tasks
idf.py build
idf.py -p COM10 flash monitor
```
