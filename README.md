# Hệ Thống Nhúng & FreeRTOS Thực Hành Trên ESP32-C3 / ESP-IDF

[![ESP-IDF](https://img.shields.io/badge/ESP--IDF-v5.x%20%7C%20v6.x-blue.svg)](https://docs.espressif.com/projects/esp-idf/en/latest/)
[![FreeRTOS](https://img.shields.io/badge/RTOS-FreeRTOS-orange.svg)](https://www.freertos.org/)
[![Target MCU](https://img.shields.io/badge/Target-ESP32--C3%20(RISC--V)-red.svg)](https://www.espressif.com/en/products/socs/esp32-c3)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

Dự án này là bộ tài liệu thực hành và mã nguồn mẫu toàn diện về **Hệ thống nhúng (Embedded Systems)** và **Hệ điều hành thời gian thực (FreeRTOS)**, được xây dựng và chuẩn hóa trên vi điều khiển kiến trúc RISC-V **ESP32-C3** thông qua framework **ESP-IDF**.

Tất cả các ví dụ được phát triển dựa trên cuốn sách kinh điển *"Mastering the FreeRTOS Real Time Kernel - A Hands-On Tutorial Guide"* của tác giả Richard Barry, được chuyển đổi và tối ưu hóa từ môi trường vi điều khiển 8-bit/16-bit truyền thống sang dòng vi điều khiển 32-bit RISC-V hiện đại.

---

## 📑 Mục Lục
1. [Tổng Quan Các Ví Dụ](#-tổng-quan-các-ví-dụ)
2. [Cấu Trúc Thư Mục](#-cấu-trúc-thư-mục)
3. [Chi Tiết 15 Bài Thực Hành](#-chi-tiết-15-bài-thực-hành)
   - [Phần I: Quản Lý Tác Vụ (Task Management)](#phần-i-quản-lý-tác-vụ-task-management)
   - [Phần II: Hàng Đợi (Queue Management)](#phần-ii-hàng-đợi-queue-management)
   - [Phần III: Định Thời Phần Mềm (Software Timers)](#phần-iii-định-thời-phần-mềm-software-timers)
4. [Yêu Cầu Phần Cứng & Công Cụ](#-yêu-cầu-phần-cứng--công-cụ)
5. [Hướng Dẫn Biên Dịch & Nạp Chương Trình](#-hướng-dẫn-biên-dịch--nạp-chương-trình)
6. [Tác Giả](#-tác-giả)

---

## 🎯 Tổng Quan Các Ví Dụ

| STT | Tên Thư Mục | Trọng Tâm Kiến Thức | Các API FreeRTOS Chính |
| :---: | :--- | :--- | :--- |
| **01** | `example1_creating_tasks` | Khởi tạo tác vụ cơ bản, busy-waiting loop | `xTaskCreate()` |
| **02** | `example2_task_parameter` | Truyền tham số `pvParameters` vào task instance | `xTaskCreate()`, type casting |
| **03** | `example3_priorities` | Độ ưu tiên tác vụ & hiện tượng Task Starvation | Preemptive Scheduling |
| **04** | `example4_block_delay` | Trạng thái Blocked và nhường CPU với delay tương đối | `vTaskDelay()`, `pdMS_TO_TICKS()` |
| **05** | `example5_delay_until` | Định thời tác vụ chu kỳ cố định (Fixed-frequency) | `vTaskDelayUntil()`, `xTaskGetTickCount()` |
| **06** | `example6_combine_tasks` | Kết hợp tác vụ tuần hoàn (Periodic) và liên tục | `vTaskDelayUntil()`, Continuous tasks |
| **07** | `example7_idle_hook` | Idle Hook callback và đo lường thời gian rỗi CPU | `vApplicationIdleHook()` |
| **08** | `example8_change_priority` | Thay đổi độ ưu tiên tác vụ động lúc thực thi | `uxTaskPriorityGet()`, `vTaskPrioritySet()` |
| **09** | `example9_delete_task` | Xóa tác vụ và cơ chế giải phóng bộ nhớ của Idle task | `vTaskDelete()` |
| **10** | `example10_queue` | Hàng đợi cơ bản: Sender (thấp) và Receiver (cao) | `xQueueCreate()`, `xQueueSendToBack()`, `xQueueReceive()` |
| **11** | `example11_queue_compound` | Hàng đợi phức hợp (Struct), đa nguồn gửi, kiểm tra tràn hàng đợi | `xQueueCreate()`, `xQueueSendToBack()`, `xQueueReceive()`, `taskYIELD()` |
| **12** | `example12_queue_` | Đồng bộ đa hàng đợi với Queue Set | `xQueueCreateSet()`, `xQueueAddToSet()`, `xQueueSelectFromSet()` |
| **13** | `example13_Creating one-shot...` | Phân biệt One-Shot Timer và Auto-Reload Timer | `xTimerCreate()`, `xTimerStart()`, Timer Callbacks |
| **14** | `example14_Callback_softwaretimer_id` | Tái sử dụng callback và lưu trữ trạng thái qua Timer ID | `pvTimerGetTimerID()`, `vTimerSetTimerID()`, `xTimerStop()` |
| **15** | `example15_Resetting_software_timer` | Reset Timer mô phỏng Backlight Timeout và Watchdog | `xTimerReset()`, Event-driven timers |

---

## 📂 Cấu Trúc Thư Mục

```text
He_Thong_Nhung/
├── .gitignore
├── README.md                                             # Tài liệu tổng hợp toàn bộ repository
├── example1_creating_tasks/                              # Ex 01: Tạo 2 task cơ bản
├── example2_task_parameter/                              # Ex 02: Dùng task parameter
├── example3_priorities/                                  # Ex 03: Độ ưu tiên tác vụ
├── example4_block_delay/                                 # Ex 04: Block state & vTaskDelay
├── example5_delay_until/                                 # Ex 05: vTaskDelayUntil định kỳ
├── example6_combine_tasks/                               # Ex 06: Kết hợp Periodic & Continuous
├── example7_idle_hook/                                   # Ex 07: Idle task hook
├── example8_change_priority/                             # Ex 08: Đổi priority động
├── example9_delete_task/                                 # Ex 09: Xóa task & dọn dẹp heap
├── example10_queue/                                      # Ex 10: Queue nhận/gửi cơ bản
├── example11_queue_compound/                             # Ex 11: Queue truyền Struct
├── example12_queue_/                                     # Ex 12: Quản lý nhiều Queue với Queue Set
├── example13_Creating one-shot and auto-reload timers/   # Ex 13: One-shot & Auto-reload Timer
├── example14_Callback_softwaretimer_id/                  # Ex 14: Callback & Timer ID
└── example15_Resetting_software_timer/                   # Ex 15: Reset Timer (Backlight Demo)
```

Mỗi thư mục ví dụ đều là một **project ESP-IDF độc lập**, có sẵn:
- `CMakeLists.txt`: Khai báo cấu hình dự án CMake.
- `sdkconfig.defaults`: Cấu hình tối ưu cho vi điều khiển (đơn nhân UNICORE).
- `main/main.c`: Mã nguồn minh họa có chú thích chi tiết bằng tiếng Việt.
- `README.md`: Hướng dẫn nguyên lý, kịch bản hoạt động và lệnh chạy riêng biệt cho từng bài.

---

## 🔬 Chi Tiết 15 Bài Thực Hành

### Phần I: Quản Lý Tác Vụ (Task Management)
1. **Example 1 (`example1_creating_tasks`)**: Làm quen với hàm tạo task `xTaskCreate()`. Hai task chia sẻ thời gian thông qua cơ chế Time-Slicing khi có cùng độ ưu tiên.
2. **Example 2 (`example2_task_parameter`)**: Tái sử dụng cùng một hàm thực thi task bằng cách truyền các chuỗi thông điệp khác nhau qua tham số `pvParameters`.
3. **Example 3 (`example3_priorities`)**: Khảo sát Scheduler ưu tiên chiếm quyền (Preemptive Priority Scheduling). Task có độ ưu tiên cao hơn sẽ chiếm giữ hoàn toàn CPU nếu không bao giờ nhường, dẫn đến hiện tượng bỏ đói (Starvation) của task ưu tiên thấp.
4. **Example 4 (`example4_block_delay`)**: Khắc phục lỗi chiếm dụng CPU ở bài 3 bằng cách đưa task vào trạng thái **Blocked** qua hàm `vTaskDelay()`.
5. **Example 5 (`example5_delay_until`)**: Ứng dụng `vTaskDelayUntil()` để duy trì tần số thực thi chu kỳ chính xác tuyệt đối, loại bỏ sai lệch thời gian do thời gian xử lý nội tại của task.
6. **Example 6 (`example6_combine_tasks`)**: Mô hình hệ thống thực tế kết hợp giữa tác vụ chạy nền liên tục (Background continuous task) và tác vụ ngắt quãng chu kỳ chính xác (Periodic task).
7. **Example 7 (`example7_idle_hook`)**: Khởi tạo hàm Hook trong Idle Task (`vApplicationIdleHook()`) nhằm tận dụng thời gian rảnh rỗi của CPU để đếm chu kỳ hoặc đưa vi điều khiển vào chế độ tiết kiệm năng lượng (Low-power sleep).
8. **Example 8 (`example8_change_priority`)**: Sử dụng API `vTaskPrioritySet()` và `uxTaskPriorityGet()` để tự nâng hoặc hạ độ ưu tiên của chính task hoặc task khác trong quá trình vận hành.
9. **Example 9 (`example9_delete_task`)**: Hủy tác vụ bằng `vTaskDelete()` khi tác vụ đã hoàn thành nhiệm vụ, giải phóng TCB (Task Control Block) và Stack nhờ Idle Task.

### Phần II: Hàng Đợi (Queue Management)
10. **Example 10 (`example10_queue`)**: Giao tiếp liên tác vụ an toàn luồng (Thread-safe Inter-task Communication) bằng Queue. Minh họa cơ chế Receiver bị block cho tới khi Sender đẩy dữ liệu vào hàng đợi.
11. **Example 11 (`example11_queue_compound`)**: Đóng gói các kiểu dữ liệu phức hợp (Structure) chứa mã định danh thiết bị gửi và giá trị cảm biến qua hàng đợi; xử lý tình huống hàng đợi bị đầy (Queue Full).
12. **Example 12 (`example12_queue_`)**: Kỹ thuật `QueueSet` cho phép một task duy nhất có thể đồng thời lắng nghe nhiều hàng đợi khác nhau mà không cần bận tâm polling vòng lặp.

### Phần III: Định Thời Phần Mềm (Software Timers)
13. **Example 13 (`example13_Creating one-shot...`)**: Khởi tạo Timer One-Shot (đếm 1 lần rồi ngừng) và Auto-Reload (tự nạp lại liên tục) vận hành bởi FreeRTOS Daemon Task.
14. **Example 14 (`example14_Callback_softwaretimer_id`)**: Tối ưu hóa bộ nhớ RAM bằng cách gắn một hàm callback duy nhất cho nhiều timer, sử dụng `TimerID` làm biến đếm số lần kích hoạt độc lập.
15. **Example 15 (`example15_Resetting_software_timer`)**: Minh họa tính năng `xTimerReset()` trong bài toán bật/tắt đèn nền màn hình (Backlight) tự động khi người dùng thao tác phím.

---

## 🛠 Yêu Cầu Phần Cứng & Công Cụ

### Phần cứng
- **Vi điều khiển:** ESP32-C3 (hoặc các dòng ESP32 / ESP32-S3 / ESP32-C6).
- Cáp kết nối USB sang UART/Type-C.

### Phần mềm & Công cụ phát triển
- **ESP-IDF:** Phiên bản `v5.x` hoặc `v6.x`.
- **Trình biên dịch:** `riscv32-esp-elf-gcc` (cho ESP32-C3) hoặc `xtensa-esp32-elf-gcc` (cho ESP32).
- **Build System:** CMake `>= 3.16` & Ninja.
- **IDE đề xuất:** Visual Studio Code với ESP-IDF Extension hoặc Clangd.

---

## 🚀 Hướng Dẫn Biên Dịch & Nạp Chương Trình

Mở terminal ESP-IDF (hoặc Command Prompt đã active môi trường `export.bat` / `. ./export.ps1`) và thực hiện theo các bước:

```bash
# 1. Di chuyển vào thư mục ví dụ bạn muốn chạy, ví dụ:
cd example1_creating_tasks

# 2. Thiết lập chip đích là ESP32-C3 (chỉ cần làm 1 lần đầu tiên)
idf.py set-target esp32c3

# 3. Biên dịch dự án
idf.py build

# 4. Nạp code và mở Serial Monitor (thay COMx bằng cổng COM thực tế)
idf.py -p COM6 flash monitor
```

> **Phím tắt hữu ích trong Serial Monitor:**
> - Thoát khỏi monitor: Nhấn tổ hợp phím `Ctrl + ]`.
> - Khởi động lại chip: Nhấn `Ctrl + T` rồi nhấn `Ctrl + R`.

---

## 👨‍💻 Tác Giả
- **Phạm Đình Chiến**
- GitHub: [@phamdinhchien210104](https://github.com/phamdinhchien210104)
- Email: `phamc4717@gmail.com`
- Đơn vị: Khoa Công Nghệ Thông Tin / Kỹ Thuật Máy Tính - Trường Đại học Bách Khoa, Đại học Đà Nẵng (DUT).
