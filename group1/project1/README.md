# Group 1

\# Group 1 – Environmental Monitoring System



\## 1. Hardware



| Thành phần | Mô tả |

|---|---|

| MCU | STM32L433RC (NUCLEO-L433RC-P) |

| Nạp / Debug | ST-Link on-board (SWD) |

| USB-UART | Cáp PL2303 (4 dây) |

| Cảm biến I2C | TODO |

| Thiết bị SPI | TODO |

| ADC | TODO |



\## 2. Pin Configuration



| Chức năng | Peripheral | Chân STM32 | Ghi chú |

|---|---|---|---|

| UART TX | USART1 | PA9 | Nối dây trắng (RX) của PL2303 |

| UART RX | USART1 | PA10 | Nối dây xanh lá (TX) của PL2303 |

| GND | – | GND | Nối dây đen của PL2303 |



Dây đỏ (5V) của PL2303 \*\*không nối\*\* vì board đã được cấp nguồn qua USB ST-Link.



\## 3. Cấu hình UART



\- USART1, chế độ Asynchronous

\- `115200 8N1`, không dùng flow control

\- `printf` được chuyển hướng ra UART qua hàm `\_\_io\_putchar()`



\## 4. Cách chạy



1\. Mở project bằng STM32CubeIDE, Build (Ctrl+B) rồi Run.

2\. Mở PuTTY: Serial, COM của PL2303, tốc độ 115200.

3\. Nhấn RESET trên board, log sẽ hiện ra:



```text

\[INFO] System started

\[INFO] Alive

\[INFO] Alive

```



\## 5. Tiến độ



\- \[x] UART logging

\- \[ ] FreeRTOS tasks

\- \[ ] UART command (help, status, sensor, voltage, version)

\- \[ ] ADC

\- \[ ] I2C sensor

\- \[ ] SPI device

