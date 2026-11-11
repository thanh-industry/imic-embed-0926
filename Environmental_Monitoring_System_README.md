# 🌍 Hệ Thống Giám Sát Môi Trường (Environmental Monitoring System)

Dự án **Environmental Monitoring System** là một hệ thống nhúng thời gian thực (RTOS) được phát triển trên vi điều khiển **STM32F411VETx** sử dụng hệ điều hành **FreeRTOS**. Hệ thống có nhiệm vụ thu thập các thông số môi trường (Nhiệt độ, Độ ẩm, Áp suất khí quyển) cùng điện áp hệ thống, sau đó phân phối dữ liệu theo hai hướng: lưu trữ lịch sử (Data Logging) vào bộ nhớ Flash ngoài **W25Q32JV** và hiển thị trực quan lên màn hình **OLED SSD1306** theo 2 chế độ hoạt động linh hoạt (Liên tục hoặc điều khiển qua giao tiếp **UART CLI**).

---

## 🛠️ 1. Phần Cứng & Thông Số Kỹ Thuật (Dựa trên Datasheet)

Hệ thống được xây dựng xoay quanh các thành phần phần cứng chính sau:

| Thiết bị | Giao tiếp | Thông số nổi bật (Datasheet) | Vai trò trong hệ thống |
| :--- | :--- | :--- | :--- |
| **STM32F411VETx** | Hệ thống | ARM® Cortex®-M4 32-bit + FPU, tần số tối đa $100\text{ MHz}$ ($125\text{ DMIPS}$), $512\text{ KB}$ Flash, $128\text{ KB}$ SRAM. | Vi điều khiển trung tâm, vận hành FreeRTOS và quản lý toàn bộ ngoại vi. |
| **Bosch BME280** | $\text{I}^2\text{C}1$ | Dải đo: Nhiệt độ ($-40^\circ\text{C} \dots +85^\circ\text{C}$), Độ ẩm ($0 \dots 100\%\text{ RH}$, sai số $\pm 3\%$), Áp suất ($300 \dots 1100\text{ hPa}$). Tiêu thụ dòng cực thấp ($3.6\text{ }\mu\text{A}$ @ $1\text{ Hz}$). | Cảm biến thu thập dữ liệu nhiệt độ, độ ẩm và áp suất môi trường. |
| **Winbond W25Q32JV** | SPI | Dung lượng $32\text{ M-bit}$ ($4\text{ MB}$) Serial NOR Flash. Tổ chức gồm $16,384\text{ pages}$ ($256\text{ bytes/page}$), $1,024\text{ sectors}$ ($4\text{ KB/sector}$), $64\text{ blocks}$ ($64\text{ KB/block}$). | Lưu trữ bản ghi dữ liệu môi trường và điện áp (Data Logging) không mất dữ liệu khi mất nguồn. |
| **Solomon SSD1306** | $\text{I}^2\text{C}$ | Màn hình OLED ma trận điểm độ phân giải $128 \times 64$, tích hợp GDDRAM, điều chỉnh $256$ mức độ tương phản. | Hiển thị thông số môi trường theo thời gian thực hoặc hiển thị nội dung theo lệnh từ CLI. |
| **ADC & RTC nội** | Nội vi MCU | ADC $12\text{-bit}$ ($2.4\text{ MSPS}$), bộ đếm thời gian thực RTC độ chính xác dưới giây. | Đo điện áp hệ thống (`adc_sensor`) và cung cấp nhãn thời gian thực (`rtc_time`). |
| **UART** | UART RX ISR | Hỗ trợ tốc độ truyền lên đến $12.5\text{ Mbit/s}$, hoạt động kèm ngắt nhận (RX Interrupt). | Giao tiếp Command Line Interface (CLI) nhận lệnh điều khiển từ máy tính. |

---

## 📂 2. Cấu Trúc Cây Thư Mục (Project Directory Structure)

Dự án được tổ chức theo kiến trúc phân lớp (Layered Architecture) rõ ràng trên **STM32CubeIDE**, giúp tách biệt hoàn toàn giữa tầng ứng dụng RTOS, tầng dịch vụ xử lý, và tầng trình điều khiển thiết bị (Device Drivers):

```text
Environmental_Monitoring_System/
├── App/                                # Tầng Ứng dụng (Quản lý các Task FreeRTOS và luồng chính)
│   ├── Inc/
│   │   ├── app_main.h                  # Khai báo khởi tạo hệ thống, biến dùng chung (current_data_system)
│   │   ├── task_cli.h                  # Header cho Task xử lý giao tiếp lệnh UART CLI
│   │   ├── task_display.h              # Header cho Task hiển thị màn hình OLED SSD1306
│   │   ├── task_sensor.h               # Header cho các Task đọc cảm biến (BME280 & ADC Voltage)
│   │   └── task_storage.h              # Header cho Task lưu trữ dữ liệu vào Flash W25Q32
│   └── Src/
│       ├── app_main.c                  # Định nghĩa tạo Task, Queue, Mutex và khởi chạy FreeRTOS
│       ├── task_cli.c                  # Thực thi Task_CLI
│       ├── task_display.c              # Thực thi Task_SSD1306
│       ├── task_sensor.c               # Thực thi Task_BME280 và Task_Voltage
│       └── task_storage.c              # Thực thi Task_W25Q32
│
├── Devices/                            # Tầng Driver Thiết bị ngoại vi (Hardware Abstraction)
│   ├── Inc/
│   │   ├── adc_sensor.h                # Giao diện đọc giá trị điện áp qua ADC
│   │   ├── bme280.h                    # Thanh ghi, cấu trúc bù dữ liệu và API cho cảm biến BME280
│   │   ├── rtc_time.h                  # Giao diện quản lý thời gian thực (RTC)
│   │   ├── ssd1306.h                   # Tập lệnh điều khiển màn hình OLED SSD1306
│   │   ├── ssd1306_fonts.h             # Dữ liệu font chữ hiển thị trên OLED
│   │   └── w25q32.h                    # Tập lệnh SPI (Read, Page Program, Sector Erase...) cho W25Q32
│   └── Src/
│       ├── adc_sensor.c
│       ├── bme280.c
│       ├── rtc_time.c
│       ├── ssd1306.c
│       ├── ssd1306_fonts.c
│       └── w25q32.c
│
├── Services/                           # Tầng Dịch vụ (Xử lý chuỗi lệnh và định dạng bản ghi)
│   ├── Inc/
│   │   ├── cli_parser.h                # Bộ phân tích cú pháp lệnh nhận từ UART
│   │   └── log.h                       # Cấu trúc gói tin log và hàm đóng gói dữ liệu lưu trữ
│   └── Src/
│       ├── cli_parser.c
│       └── log.c
│
├── Test/                               # Tầng Kiểm thử phần cứng độc lập
│   ├── test_hardware.h                 # Khai báo các hàm kiểm tra kết nối I2C, SPI, UART, ADC
│   └── test_hardware.c                 # Thực thi kịch bản test phần cứng
│
├── Core/                               # Mã nguồn khởi tạo phần cứng sinh bởi STM32CubeMX (main, gpio, i2c, spi...)
├── Drivers/                            # Thư viện CMSIS và STM32F4xx HAL Driver
├── Middlewares/                        # Thư viện hệ điều hành thời gian thực FreeRTOS
├── Environmental_Monitoring_System.ioc # File cấu hình chân và clock trên STM32CubeMX
├── STM32F411VETX_FLASH.ld              # Linker Script cho bộ nhớ Flash
└── STM32F411VETX_RAM.ld                # Linker Script cho bộ nhớ RAM
```

---

## 🔄 3. Sơ Đồ Kiến Trúc & Nguyên Lý Hoạt Động

### 3.1. Sơ đồ luồng dữ liệu (Data Flow & IPC Diagram)

```text
+-----------------+    Đọc I2C1
|   Task_BME280   |---------------+
+-----------------+               |        +--------------+      +-----------------+
                                  +------->|   logQueue   |----->|   Task_W25Q32   |
+-----------------+    Đọc ADC    |        +--------------+      +-----------------+
|  Task_Voltage   |---------------+
+-----------------+               |
                                  v
                       [ current_data_system ]
                         (Biến dùng chung)
                                  |
             +--------------------+--------------------+
             |                                         |
             v                                         v
    +-----------------+                       +-----------------+
    |    Task_CLI     |                       |  Task_SSD1306   |
    +-----------------+                       +-----------------+
             |               Chiếm Mutex               |
             +-------------->+---------+<--------------+
                             |oledMutex|
                             +---------+
             ^
             |
    [    uartQueue    ]
             ^
             |
    [ Ngắt UART RX (ISR) ]
```

### 3.2. Nguyên lý hoạt động chi tiết

1. **Khối Thu Thập Dữ Liệu (Data Acquisition)**:
   * **`Task_BME280`**: Định kỳ mỗi $1000\text{ ms}$ giao tiếp qua bus $\text{I}^2\text{C}1$ để đọc dữ liệu thô từ cảm biến **BME280** và thực hiện thuật toán bù (compensation) nhằm tính ra giá trị Nhiệt độ ($^\circ\text{C}$), Độ ẩm ($\%\text{ RH}$) và Áp suất ($\text{hPa}$ / $\text{Pa}$).
   * **`Task_Voltage`**: Định kỳ mỗi $1000\text{ ms}$ đọc giá trị điện áp thông qua bộ chuyển đổi ADC.
   * Toàn bộ dữ liệu thu thập được sẽ cập nhật trực tiếp vào biến toàn cục dùng chung cho toàn dự án là **`current_data_system`** (`current_sys_data`).

2. **Hướng Dữ Liệu 1 – Lưu Trữ Vào Flash Ngoại (`W25Q32JV`)**:
   * Ngay sau khi lấy mẫu, `Task_BME280` và `Task_Voltage` đóng gói dữ liệu và đẩy vào hàng đợi **`logQueue`**.
   * **`Task_W25Q32`** ở trạng thái chờ sự kiện (`osWaitForever`) từ `logQueue`. Ngay khi có dữ liệu mới trong hàng đợi, task sẽ thức dậy, lấy gói tin ra và thực hiện ghi vào chip nhớ **W25Q32** qua giao tiếp SPI. Nhờ độ ưu tiên thấp (`osPriorityLow`), việc xóa/ghi Flash không làm gián đoạn các tác vụ thời gian thực khác.

3. **Hướng Dữ Liệu 2 – Xuất Ra Màn Hình OLED (`SSD1306`) & 2 Chế Độ Hoạt Động**:
   * Màn hình OLED là tài nguyên phần cứng dùng chung giữa `Task_SSD1306` và `Task_CLI`, do đó được bảo vệ nghiêm ngặt bởi **`oledMutex`** để tránh xung đột đường truyền $\text{I}^2\text{C}$ và lỗi hiển thị.
   * **Chế độ 1 (Hiển thị liên tục theo lập trình)**:
     * Định kỳ mỗi $1000\text{ ms}$, **`Task_SSD1306`** lấy dữ liệu mới nhất từ biến `current_data_system`, chiếm quyền `oledMutex`, cập nhật bộ đệm đồ họa GDDRAM và xuất liên tục các thông số Nhiệt độ / Độ ẩm / Áp suất / Điện áp lên màn hình OLED.
   * **Chế độ 2 (Điều khiển và xuất nội dung theo lệnh từ UART CLI)**:
     * Khi người dùng gửi lệnh từ máy tính qua cổng UART, trình phục vụ ngắt **`Ngắt UART RX (ISR)`** sẽ bắt dữ liệu và đẩy vào hàng đợi **`uartQueue`**.
     * **`Task_CLI`** đang chờ (`osWaitForever`) trên `uartQueue` sẽ lập tức được đánh thức (với mức ưu tiên cao nhất hệ thống `osPriorityAboveNormal`).
     * `Task_CLI` sử dụng bộ phân tích `cli_parser` để giải mã lệnh, trích xuất dữ liệu tương ứng từ `current_data_system` (hoặc nội dung đã thiết lập trước), chiếm quyền **`oledMutex`** để in nội dung phản hồi lên màn hình OLED cũng như phản hồi qua cổng UART.

---

## ⚙️ 4. Bảng Cấu Hình Chi Tiết Các Task FreeRTOS

Dựa trên thiết kế kiến trúc của hệ thống, các Task được phân bổ mức độ ưu tiên (Priority), chu kỳ hoạt động và kích thước vùng nhớ ngăn xếp (Stack Size) như sau:

| Tên Task | File thực thi | Độ ưu tiên (Priority) | Chu kỳ / Trạng thái chờ | IPC & Tài nguyên sử dụng | Stack Size | Mô tả nhiệm vụ chính |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **`Task_CLI`** | `task_cli.c` | `osPriorityAboveNormal` | `osWaitForever` | `uartQueue`, `oledMutex`, `current_data_system` | `512 words` | Nhận lệnh từ `uartQueue` (do ngắt UART RX đẩy lên), phân tích lệnh và chiếm `oledMutex` để in nội dung tương ứng đã thiết lập. |
| **`Task_BME280`** | `task_sensor.c` | `osPriorityNormal` | `1000 ms` | `logQueue`, `current_data_system` | `256 words` | Đọc cảm biến BME280 qua $\text{I}^2\text{C}1$, cập nhật vào `current_data_system` và gửi bản ghi vào `logQueue`. |
| **`Task_Voltage`** | `task_sensor.c` | `osPriorityNormal` | `1000 ms` | `logQueue`, `current_data_system` | `128 words` | Đọc giá trị ADC đo điện áp, cập nhật vào `current_data_system` và gửi vào `logQueue`. |
| **`Task_SSD1306`** | `task_display.c` | `osPriorityBelowNormal` | `1000 ms` | `oledMutex`, `current_data_system` | `256 words` | Đọc dữ liệu từ `current_data_system`, chiếm `oledMutex` và xuất dữ liệu liên tục lên màn hình OLED. |
| **`Task_W25Q32`** | `task_storage.c` | `osPriorityLow` | `osWaitForever` | `logQueue` | `256 words` | Chờ và lấy dữ liệu từ `logQueue`, thực thi ghi log vào chip nhớ Flash W25Q32 qua giao tiếp SPI. |

---

## 🔒 5. Cơ Chế Đồng Bộ & Chia Sẻ Tài Nguyên (IPC)

* **`current_data_system`**: Biến cấu trúc (struct) toàn cục lưu trữ trạng thái môi trường hiện tại (Nhiệt độ, Độ ẩm, Áp suất, Điện áp, Thời gian). Giúp `Task_SSD1306` và `Task_CLI` truy xuất nhanh giá trị tức thời mà không cần chờ đợi cảm biến đọc lại.
* **`logQueue`**: Hàng đợi truyền tải các gói dữ liệu đo đạc từ hai task cảm biến (`Task_BME280`, `Task_Voltage`) sang `Task_W25Q32`. Giúp tách biệt tốc độ lấy mẫu cố định ($1000\text{ ms}$) khỏi thời gian trễ khi xóa/ghi trang nhớ trên Flash W25Q32 (Page Program tốn $0.4 \dots 3\text{ ms}$, Sector Erase tốn $45 \dots 400\text{ ms}$).
* **`uartQueue`**: Hàng đợi nối tiếp giữa hàm ngắt phần cứng `UART RX ISR` và `Task_CLI`, đảm bảo hàm ngắt thực thi cực nhanh và nhường việc xử lý chuỗi phức tạp cho Task.
* **`oledMutex`**: Cơ chế khóa loại trừ tương hỗ (Mutual Exclusion) bảo vệ màn hình OLED SSD1306 khi cả `Task_SSD1306` (hiển thị định kỳ) và `Task_CLI` (hiển thị theo lệnh) cùng muốn truy cập màn hình.

---

## 🚀 6. Hướng Dẫn Biên Dịch & Sử Dụng

1. **Môi trường phát triển**:
   * Phần mềm: **STM32CubeIDE** (Workspace 2.2.0 trở lên).
   * Trình biên dịch: GNU Tools for STM32 (`arm-none-eabi-gcc`).
2. **Các bước nạp chương trình**:
   * Mở STM32CubeIDE, chọn `File` $\rightarrow$ `Import...` $\rightarrow$ `Existing Projects into Workspace` và trỏ tới thư mục `Environmental_Monitoring_System`.
   * Nhấn biểu tượng **Build** (🔨) để biên dịch toàn bộ project.
   * Kết nối mạch nạp ST-Link với board mạch STM32F411VETx và nhấn **Run / Debug** để nạp firmware.
3. **Giao tiếp qua UART CLI**:
   * Kết nối module USB-to-TTL vào chân UART của STM32 và mở phần mềm Terminal (Hercules, PuTTY, TeraTerm...) với tốc độ Baudrate đã cấu hình (mặc định thường là $115200\text{ bps}$, $8\text{N}1$).
   * Gửi các lệnh CLI đã được thiết lập trong `cli_parser.c` để chuyển đổi chế độ hiển thị hoặc yêu cầu in các thông tin mong muốn lên màn hình OLED và Terminal.