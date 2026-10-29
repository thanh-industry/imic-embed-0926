# STM32 Embedded Systems
## Đề tài: Environmental Monitoring System (STM32F411Vet)
1. Project Overview
   -Sử dụng hệ điều hành FreeRTOS để quản lý 5 task: SPI, I2C, UART, ADC, RTC
   -Task SPI: đọc nhiệt độ và áp suất khí quyển bằng cảm biến BMP280
   -Task I2C: nhận data từ các ták khác và khiển thị lên man hinh 16x2
   -Task UART: hiện thị trạng thasi các task hoạt động và hiển thị 1 số thông tin khi nhập command vào hercules
   -Task ADC: đọc điện áp của Pin nuôi board
   -Task RTC: dọc dữ liệu ngày vầ giờ được cài đặt trong board
3. Hardware
   -Board mạch chính dùng để giao tiếp với các ngoại vi STM32F411VET
   -Cảm biến nhiệt độ và áp suất BMP280 dược cấu hình PA4 -> CS (chip select), PA5 -> SCK (xung clock), PA6 -> MISO(Master in slave out),
    PA7 -> MOSI (Master out slave in)
   -Màn hình LCD 16x2 được cấu hình PA7 -> SDA, PA6 -> SCL
   -Giao tiếp UART2 với app Hercules PA2 -> TX, PA3 -> RX     
5. Software Architecture
   
6. Build and Run
  
7. Results and Limitations
   

https://github.com/user-attachments/assets/09893729-3adb-4e09-9821-b97c6c620159

