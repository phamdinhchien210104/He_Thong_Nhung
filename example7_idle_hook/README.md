# FreeRTOS Example 7: Defining an Idle Task Hook Function

Dự án này là bản chuyển đổi chuẩn hóa của **Example 7: Defining an Idle Task Hook Function** sang nền tảng **ESP-IDF** (ESP32 / ESP32-C3).

---

## 1. Cấu trúc thư mục dự án

```text
example7_idle_hook/
├── CMakeLists.txt              # CMake root cho project ESP-IDF
├── sdkconfig.defaults          # Cấu hình UNICORE và LEGACY IDLE HOOK
├── README.md                   # Tài liệu hướng dẫn
└── main/
    ├── CMakeLists.txt          # Khai báo component main
    └── main.c                  # Mã nguồn chính
```

---

## 2. Mục tiêu ví dụ
- Minh họa cách viết và kích hoạt hàm Idle Hook: `vApplicationIdleHook()`.
- Hàm Idle Hook được gọi tự động mỗi khi Idle Task (độ ưu tiên 0) được thực thi (tức là khi tất cả các task khác đều đang ở trạng thái Blocked).
- Trong ví dụ này, `ulIdleCycleCount` được tăng liên tục trong idle hook, đo đạc mức độ rảnh rỗi của CPU giữa các chu kỳ đánh thức của Task 1 và Task 2.

> **Lưu ý ESP-IDF**: Dự án đã cấu hình sẵn trong `sdkconfig.defaults`:
> ```ini
> CONFIG_FREERTOS_LEGACY_HOOKS=y
> CONFIG_FREERTOS_LEGACY_IDLE_HOOK=y
> ```

---

## 3. Cách biên dịch và nạp

```bash
cd c:/he_thong_nhung/example7_idle_hook
idf.py build
idf.py -p COM10 flash monitor
```
