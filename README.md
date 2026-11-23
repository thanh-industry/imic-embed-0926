# 🌍 Hệ Thống Giám Sát Môi Trường (Environmental Monitoring System)

Dự án **Environmental Monitoring System** (`final_btap`) là một hệ thống nhúng thời gian thực (RTOS) phát triển trên vi điều khiển **STM32F407VGT6** sử dụng hệ điều hành **FreeRTOS**. Hệ thống thực hiện thu thập dữ liệu nhiệt độ môi trường, đo điện áp hệ thống, kiểm tra kết nối bộ nhớ Flash ngoài, hiển thị lên màn hình **OLED SSD1306**, điều khiển **LED RGB** và hỗ trợ giao tiếp điều khiển qua **UART CLI**.

Toàn bộ tài nguyên RTOS (Task, Queue, Mutex) được cấp phát theo cơ chế **100% Static Memory Allocation** (không cấp phát động Heap trong thời gian chạy), giúp hệ thống hoạt động ổn định, không bị phân mảnh bộ nhớ và dễ dàng phân tích Stack/Memory khi bảo vệ đồ án.

---

## 🛠️ 1. Phần Cứng & Sơ Đồ Kết Nối Chân (Hardware & Pinout)

| Thiết bị / Ngoại vi    | Chân STM32               | Kết nối tới              | Giao tiếp / Thông số                          | Vai trò trong hệ thống                                |
| :--------------------- | :----------------------- | :----------------------- | :-------------------------------------------- | :---------------------------------------------------- |
| **STM32F407VGT6**      | Hệ thống                 | Core MCU                 | ARM Cortex-M4F, 168 MHz, 1 MB Flash, 192 KB RAM | Vi điều khiển trung tâm, điều phối FreeRTOS và ngoại vi. |
| **USB-to-TTL (UART)**  | `PA2` (`TX`)<br>`PA3` (`RX`) | `RX` USB-UART<br>`TX` USB-UART | `USART2` (115200 bps, 8N1, RX Interrupt)        | Xuất log (`LOG_D/I/W/E`) và giao tiếp nhận lệnh CLI.   |
| **Bosch BMP280**       | `PB6` (`SCL`)<br>`PB7` (`SDA`) | `SCL` BMP280<br>`SDA` BMP280 | `I2C1` (Địa chỉ `0x76` / `0x77`, -40..+85 °C)  | Cảm biến đo nhiệt độ môi trường kèm thuật toán calib.  |
| **OLED SSD1306**       | `PB6` (`SCL`)<br>`PB7` (`SDA`) | `SCL` OLED<br>`SDA` OLED | `I2C1` (Địa chỉ `0x3C`, màn hình 128x64)      | Hiển thị nhiệt độ theo thời gian thực (deadband 0.1°C).|
| **Winbond W25Qxx**     | `PA5` (`SCK`)<br>`PA6` (`MISO`)<br>`PA7` (`MOSI`)<br>`PC4` (`CS`) | `CLK` W25Qxx<br>`DO` W25Qxx<br>`DI` W25Qxx<br>`CS` W25Qxx | `SPI1` (Chip Select phần mềm trên `PC4`)       | Bộ nhớ Serial NOR Flash ngoài, kiểm tra qua JEDEC ID. |
| **Chiết áp / Nguồn đo** | `PA0`                    | Tín hiệu Analog          | `ADC1_IN0` (12-bit, Vref = 3.3 V, TB 10 mẫu)    | Đo điện áp hệ thống.                                  |
| **LED RGB Trạng thái** | `PD12` (`R`)<br>`PD13` (`G`)<br>`PD14` (`B`) | Kênh Red<br>Kênh Green<br>Kênh Blue | `GPIO Output`                                 | Hiển thị trạng thái hoạt động / cảnh báo.              |

---

## 📂 2. Cấu Trúc Kiến Trúc Phân Lớp 2D (Project Directory Structure)

Dự án được tổ chức theo **kiến trúc phân lớp 2 chiều** (phân tầng trừu tượng theo chiều dọc và tách biệt module chức năng theo chiều ngang) trên **STM32CubeIDE**:

```text
final_btap/
├── App/                            # TẦNG ỨNG DỤNG (Quản lý Task FreeRTOS, đồng bộ IPC & CLI)
│   ├── Inc/
│   │   └── free_rtos.h             # Khai báo các Task, hàm giao tiếp ISR và MX_FREERTOS_Init()
│   └── Src/
│       └── free_rtos.c             # Hiện thực 4 Task, Static Mutex/Queue/TCB/Stack, Hash CLI & Fault Hooks
│
├── Services/                       # TẦNG DỊCH VỤ (Ghi log đa luồng & xử lý ngắt nhận UART)
│   ├── Inc/
│   │   └── logging.h               # Macro LOG_D, LOG_I, LOG_W, LOG_E kèm timestamp và file:line
│   └── Src/
│       └── logging.c               # In log an toàn qua uartMutex, callback RX ISR & phục hồi lỗi UART
│
├── Devices/                        # TẦNG DRIVER THIẾT BỊ (Trình điều khiển ngoại vi phần cứng)
│   ├── Inc/
│   │   ├── adc.h                   # API đọc giá trị thô và điện áp lọc trung bình ADC1
│   │   ├── i2c_bmp280.h            # Thanh ghi, hệ số hiệu chuẩn và API đọc nhiệt độ BMP280
│   │   ├── i2c_oled_lcd.h          # Bộ đệm màn hình GDDRAM và font chữ 5x7 cho OLED SSD1306
│   │   ├── led.h                   # API điều khiển LED trạng thái (PD12, PD13, PD14)
│   │   └── spi_w25qxx.h            # Tập lệnh SPI (JEDEC ID, Read, Page Program, Erase) cho W25Qxx
│   └── Src/
│       ├── adc.c
│       ├── i2c_bmp280.c
│       ├── i2c_oled_lcd.c
│       ├── led.c
│       └── spi_w25qxx.c
│
├── Core/                           # TẦNG HỆ THỐNG / CUBEMX (main, ngắt, MSP, FreeRTOSConfig)
├── Drivers/                        # TẦNG ĐÁY (CMSIS & STM32F4xx HAL Driver)
├── Middlewares/                    # TẦNG TRUNG GIAN (Nhân FreeRTOS CMSIS-RTOS v2)
├── final_btap.ioc                  # File cấu hình chân và xung nhịp STM32CubeMX
└── README.md