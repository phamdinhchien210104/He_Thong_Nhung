# FreeRTOS Example 12: Using a Queue Set

Dự án này là bản chuyển đổi chuẩn hóa của **Example 12: Using a Queue Set** sang nền tảng **ESP-IDF** (ESP32 / ESP32-C3).

---

## 1. Cấu trúc thư mục dự án

```text
example12_queue_/
├── CMakeLists.txt              # CMake root cho project ESP-IDF
├── sdkconfig.defaults          # Cấu hình UNICORE
├── README.md                   # Tài liệu hướng dẫn
└── main/
    ├── CMakeLists.txt          # Khai báo component main
    └── main.c                  # Mã nguồn chính
```

---

## 2. Mục tiêu ví dụ
- Minh họa cách gom nhiều Queue và/hoặc Semaphore vào một **Queue Set** bằng `xQueueCreateSet()` và `xQueueAddToSet()`.
- Hai Sender Task độc lập (`vSenderTask1`, `vSenderTask2`) lần lượt gửi chuỗi thông điệp vào hai queue riêng biệt (`xQueue1`, `xQueue2`).
- Một Receiver Task duy nhất (`vReceiverTask`) có Priority cao hơn chỉ cần chờ trên Queue Set thông qua `xQueueSelectFromSet()`.
- Khi bất kỳ queue nào trong set có dữ liệu, `xQueueSelectFromSet()` sẽ trả về handle của chính queue đó để Receiver tiến hành `xQueueReceive()`.
- Kỹ thuật này giúp một task có thể đồng thời lắng nghe nhiều nguồn sự kiện/dữ liệu mà không cần phải thăm dò (polling) gây lãng phí CPU.

---

## 3. Cách biên dịch và nạp

```bash
cd c:/he_thong_nhung/example12_queue_
idf.py set-target esp32c3
idf.py build
idf.py -p COM6 flash monitor
```
