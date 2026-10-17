# FreeRTOS Example 10: Queue Basics

Dự án này là bản chuyển đổi chuẩn hóa của **Example 10: Blocking When Receiving from a Queue** sang nền tảng **ESP-IDF** (ESP32 / ESP32-C3).

---

## 1. Cấu trúc thư mục dự án

```text
example10_queue/
├── CMakeLists.txt              # CMake root cho project ESP-IDF
├── sdkconfig.defaults          # Cấu hình UNICORE
├── README.md                   # Tài liệu hướng dẫn
└── main/
    ├── CMakeLists.txt          # Khai báo component main
    └── main.c                  # Mã nguồn chính
```

---

## 2. Mục tiêu ví dụ
- Khởi tạo Queue có sức chứa 5 phần tử kiểu `long` bằng `xQueueCreate()`.
- Hai Sender Task (Priority 1) gửi dữ liệu vào Queue bằng `xQueueSendToBack()`.
- Receiver Task (Priority 2 - cao hơn) đọc dữ liệu ra khỏi Queue bằng `xQueueReceive()`.
- Vì Receiver có priority cao hơn, ngay khi có dữ liệu trong Queue, Receiver sẽ được kích hoạt để đọc dữ liệu ra ngay lập tức, giữ cho Queue gần như luôn rỗng.

---

## 3. Cách biên dịch và nạp

```bash
cd c:/he_thong_nhung/example10_queue
idf.py build
idf.py -p COM10 flash monitor
```
