# Group 02 - Environmental Monitoring System
Board: NUCLEO-L073RZ

Ứng dụng nhúng STM32 + FreeRTOS thu thập dữ liệu cảm biến, giám sát điện áp và giao tiếp với máy tính qua UART.

## Thành viên

| Họ tên | Vai trò |
| --- | --- |
| ĐẬU VĂN HÀO | CODE |
| TRƯƠNG MINH TIẾN | CODE |
| PHẠM QUANG THÀNH | HARDWARE |
## 1. Phần cứng

| Thành phần | Chi tiết |
| --- | --- |
| Board | NUCLEO-L073RZ (STM32L073RZT6, Cortex-M0+, 32 MHz, 192 KB Flash, 20 KB RAM) |
| Cảm biến I2C | BME280 / BMP280, đọc nhiệt độ, địa chỉ 0x76 |
| Thiết bị SPI | W25Qxx|
| Cảm biến analog | Cảm biến âm thanh, ngõ ra analog vào ADC |
| Giao tiếp PC | USART2 qua ST-LINK Virtual COM, 115200 8N1 |

Toàn bộ cấp nguồn 3.3 V. flash SPI và cảm biến âm thanh.

## 2. Sơ đồ chân

| Chức năng | Chân STM32 | Header Arduino | Nối tới |
| --- | --- | --- | --- |
| UART TX (USART2) | PA2 | – | ST-LINK Virtual COM |
| UART RX (USART2) | PA3 | – | ST-LINK Virtual COM |
| I2C1 SCL | PB8 | D15 | BME280 SCL |
| I2C1 SDA | PB9 | D14 | BME280 SDA |
| SPI1 SCK | PA5 | D13 | Flash CLK |
| SPI1 MISO | PA6 | D12 | Flash DO |
| SPI1 MOSI | PA7 | D11 | Flash DI |
| SPI1 CS (GPIO) | PB6 | D10 | Flash CS |
| ADC1 IN0 | PA0 | A0 | Cảm biến âm thanh OUT |
| Nguồn | 3V3, GND | – | VCC, GND của các module |



## 3. Kiến trúc FreeRTOS

Dùng CMSIS-RTOS v2. Dữ liệu dùng chung giữa các task nằm trong `g_sys` (`app_system.h`), được bảo vệ bằng mutex `SystemDataMutex`.

| Task | Nhiệm vụ |
| --- | --- |
| SensorTask | Khởi tạo và đọc nhiệt độ từ BME280 qua I2C mỗi 1 s, ghi vào `g_sys` |
| SPITask | Đọc JEDEC ID, chạy kiểm tra xóa/ghi/đọc flash, ghi trạng thái vào `g_sys` |
| ADCTask | Lấy mẫu ADC, tính điện áp trung bình và biên độ đỉnh-đỉnh (mV) |
| UARTTask | Task duy nhất ghi ra UART, nhận log từ queue |
| CommandTask | Nhận ký tự từ UART RX qua queue, xử lý lệnh |

Giao tiếp giữa các task:

- **Log queue:** các task đẩy bản tin `LogMsg_t` vào queue, UARTTask định dạng và gửi ra UART.
- **Command RX queue:** ngắt UART RX đưa ký tự vào queue cho CommandTask.
- **Mutex:** bảo vệ struct `g_sys` khi nhiều task đọc/ghi.

## 4. Lệnh UART

Cổng COM của ST-LINK, 115200 8N1 (PuTTY )

| Lệnh | Chức năng |
| --- | --- |
| `help` | Danh sách lệnh |
| `status` | Trạng thái hệ thống (nhiệt độ, điện áp, I2C, SPI) |
| `sensor` | Nhiệt độ và mức âm thanh |
| `voltage` | Điện áp ADC và mức âm thanh |
| `version` | Phiên bản firmware |
| `log on` / `log off` | Bật/tắt log định kỳ |

Ví dụ `status`:

```
System Status
Temperature : 27.8 C
Voltage     : 1.63 V
I2C Sensor  : OK
SPI Device  : OK
```

Ví dụ log khi khởi động:

```
[INFO] System started
[SPI] Device ID = 0xC84018
[INFO] SPI flash write/read test OK
[INFO] I2C sensor init OK
[I2C] Temperature = 28.1 C
[ADC] Voltage = 1.64 V
[ADC] Sound level = 87 mV (p-p)
```

## 5. Build và nạp

1. Mở project `group2/NgheAnGroup` bằng STM32CubeIDE.
2. Mở `NgheAnGroup.ioc` nếu cần xem hoặc chỉnh cấu hình ngoại vi.
3. Build (Debug), nạp lên board NUCLEO-L073RZ.
4. Mở terminal tại cổng COM của ST-LINK, 115200 8N1.

## 6. Cấu trúc thư mục

```
group2/
├── README.md
└── NgheAnGroup/
    ├── NgheAnGroup.ioc
    └── Core/
        ├── Inc/   (app_*.h, bme280.h, w25qxx.h, ...)
        └── Src/   (app_*.c, bme280.c, w25qxx.c, freertos.c, ...)
```

## 7. Git workflow

- Làm việc trên branch `group2-dev`, không code trực tiếp trên `main`.
- Commit nhỏ, có ý nghĩa (`feat:`, `fix:`).
- Hoàn thành thì tạo Pull Request `group2-dev` → `main`, review xong mới merge.

## 8. Hạn chế đã biết

- Driver BME280 hiện chỉ đọc nhiệt độ, chưa đọc áp suất và độ ẩm.
