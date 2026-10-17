# FreeRTOS Example 11: Queue Compound Types & Blocking When Sending

Dự án này là bản chuyển đổi hoàn chỉnh và chuẩn hóa của **Example 11 (Slide 31 - 42)** từ tài liệu bài giảng FreeRTOS sang nền tảng **ESP-IDF** (ESP32 / ESP32-C3).

---

## 1. Cấu trúc thư mục dự án

```text
example11_queue_compound/
├── CMakeLists.txt              # CMake root cho project ESP-IDF
├── sdkconfig.defaults          # Cấu hình UNICORE cho đúng bản chất RTOS đơn nhân
├── README.md                   # Tài liệu hướng dẫn chi tiết
└── main/
    ├── CMakeLists.txt          # Khai báo component main
    └── main.c                  # Mã nguồn chính của Example 11
```

---

## 2. Mục tiêu & Nguyên lý hoạt động (Theo Slide bài giảng)

### 2.1. Truyền kiểu dữ liệu có cấu trúc (`struct xData`)
Không chỉ truyền các kiểu số nguyên cơ bản (như trong Example 10), Queue thường được dùng để truyền các gói tin phức hợp:
```c
typedef struct {
    uint8_t ucValue;   // Giá trị đo/dữ liệu
    uint8_t ucSource;  // Định danh nguồn phát (mainSENDER_1 hoặc mainSENDER_2)
} xData;
```

### 2.2. Quan hệ ưu tiên giữa các Task
- **Sender 1 & Sender 2**: Độ ưu tiên `2` (Priority cao hơn Receiver).
- **Receiver**: Độ ưu tiên `1` (Priority thấp hơn Sender).
- **Queue**: Có độ dài là `3` phần tử.

### 2.3. Trình tự thực thi (Execution Sequence - Slide 30, 42)
1. Khi khởi động, **Sender 1** và **Sender 2** (priority 2) chạy trước.
2. Sender 1 gửi 1 phần tử rồi gọi `taskYIELD()`, nhường cho Sender 2 gửi phần tử thứ 2.
3. Sender 1 tiếp tục gửi phần tử thứ 3. Lúc này Queue **đầy** (3/3 phần tử).
4. Khi cố gắng gửi thêm, cả hai task Sender đều bị đưa vào trạng thái **Blocked** (chờ tối đa 100ms) vì Queue không còn chỗ trống.
5. Khi cả 2 task ưu tiên cao đều bị Blocked, CPU mới chuyển quyền cho **Receiver** (priority 1).
6. **Receiver** kiểm tra `uxQueueMessagesWaiting(xQueue)`: giá trị **luôn là 3** vì nó chỉ được chạy khi Queue đã đầy!
7. Ngay khi Receiver lấy ra **1 phần tử** (`xQueueReceive`), Queue lập tức có 1 vị trí trống.
8. Task Sender đang chờ lâu nhất lập tức được **Unblock** và chiếm quyền CPU (**Preempt**) ngay tức thì từ Receiver để gửi dữ liệu bù vào.
9. Queue lại đầy (3 phần tử) và chu trình lặp lại tuần tự.

---

## 3. Cách biên dịch và nạp (ESP-IDF)

### Cách A: Mở riêng thư mục này trong terminal ESP-IDF
```bash
cd c:/he_thong_nhung/example11_queue_compound
idf.py set-target esp32c3
idf.py build
idf.py -p COM8 flash monitor
```

### Cách B: Chạy trực tiếp trong project `he_thong_nhung` hiện tại
File `example11_queue_compound.c` cũng đã được tạo sẵn tại thư mục gốc `c:/he_thong_nhung`. 
Bạn chỉ cần mở `main/CMakeLists.txt` của project gốc và chọn:
```cmake
set(CURRENT_EXAMPLE "example11_queue_compound.c")
```
Sau đó nhấn build / flash từ extension ESP-IDF của VS Code.
