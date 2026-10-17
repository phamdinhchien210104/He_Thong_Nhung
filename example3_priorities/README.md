# FreeRTOS Example 3: Experimenting with Priorities

Dự án này là bản chuyển đổi chuẩn hóa của **Example 3: Experimenting with Priorities** sang nền tảng **ESP-IDF** (ESP32 / ESP32-C3).

---

## 1. Cấu trúc thư mục dự án

```text
example3_priorities/
├── CMakeLists.txt              # CMake root cho project ESP-IDF
├── sdkconfig.defaults          # Cấu hình UNICORE
├── README.md                   # Tài liệu hướng dẫn
└── main/
    ├── CMakeLists.txt          # Khai báo component main
    └── main.c                  # Mã nguồn chính
```

---

## 2. Mục tiêu ví dụ
- Khảo sát sự khác biệt về độ ưu tiên (Priority).
- Task 1 có độ ưu tiên 1, Task 2 có độ ưu tiên 2.
- Vì các task sử dụng vòng lặp bận (busy-wait) và không tự block hay nhường quyền, Task 2 với priority cao hơn sẽ chiếm dụng CPU liên tục và khiến Task 1 bị "đói" (starve).

---

## 3. Cách biên dịch và nạp

```bash
cd c:/he_thong_nhung/example3_priorities
idf.py build
idf.py -p COM10 flash monitor
```
