# STM32 Environmental Monitoring System

## 1. Hardware Connection

| Peripheral | STM32 Pin | Connect To | Note |
| :--- | :--- | :--- | :--- |
| **USART2_TX** | PA2 | RX USB-UART | Log & Command |
| **USART2_RX** | PA3 | TX USB-UART | Receive command |
| **I2C1_SCL** | PB6 | SCL (BMP280 & OLED) | I2C Clock |
| **I2C1_SDA** | PB7 | SDA (BMP280 & OLED) | I2C Data |
| **SPI1_SCK** | PA5 | CLK (W25Qxx) | SPI Clock |
| **SPI1_MISO** | PA6 | DO (W25Qxx) | SPI Data In |
| **SPI1_MOSI** | PA7 | DI (W25Qxx) | SPI Data Out |
| **SPI1_CS** | PC4 | CS (W25Qxx) | Chip Select |
| **ADC1_IN0** | PA0 | Potentiometer / VCC | Analog Voltage |
| **LED_RED** | PD12 | LED RGB R | Alert |
| **LED_GREEN** | PD13 | LED RGB G | Normal |
| **LED_BLUE** | PD14 | LED RGB B | Cold |

## 2. FreeRTOS Tasks

| Task | Function | Period |
| :--- | :--- | :--- |
| **ADCTask** | Read ADC, update global voltage (Mutex). | 500ms |
| **SensorTask** | Read BMP280 via I2C, display on OLED. | 2000ms |
| **SPITask** | Read W25Qxx JEDEC ID via SPI. | 5000ms |
| **CommandTask** | Receive commands from UART Queue, process and reply. | Event-driven |

## 3. UART Commands (115200 8N1)

| Command | Description |
| :--- | :--- |
| `help` | Show available commands |
| `status` | Show full system status |
| `voltage` | Show ADC voltage only |
| `sensor` | Show BMP280 temperature only |
| `version` | Show firmware version |

## 4. Project Structure

```text
final_btap/
├── Core/
│   ├── Inc/
│   └── Src/
├── Drivers/
├── Middlewares/
├── final_btap.ioc
└── README.md