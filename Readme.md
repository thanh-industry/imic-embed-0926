# STM32 EMBEDDED SYSTEMS

# ĐỀ TÀI: HỆ THỐNG GIÁM SÁT MÔI TRƯỜNG (ENVIRONMENTAL MONITORING SYSTEM)

**Vi điều khiển:** STM32F411VETx  
**Hệ điều hành thời gian thực:** FreeRTOS  
**Ngôn ngữ lập trình:** Embedded C  
**Môi trường phát triển:** STM32CubeIDE

---

## 1. Tổng quan dự án (Project Overview)

Dự án xây dựng hệ thống giám sát môi trường sử dụng vi điều khiển STM32F411VETx và hệ điều hành thời gian thực FreeRTOS.

FreeRTOS được sử dụng để quản lý và thực thi 5 tác vụ (Task) độc lập, bao gồm SPI, I2C, UART, ADC và RTC. Mỗi tác vụ đảm nhiệm một chức năng riêng, giúp hệ thống có thể thực hiện đồng thời nhiều công việc như thu thập dữ liệu cảm biến, xử lý dữ liệu, hiển thị thông tin và giao tiếp với máy tính.

### Các chức năng chính

- **Task SPI:** Giao tiếp với cảm biến BMP280 để thu thập dữ liệu nhiệt độ và áp suất khí quyển.
- **Task I2C:** Nhận dữ liệu từ các Task khác và hiển thị thông tin lên màn hình LCD 16x2.
- **Task UART:** Theo dõi trạng thái hoạt động của các Task và xử lý lệnh được gửi từ phần mềm Hercules.
- **Task ADC:** Đọc và xử lý giá trị điện áp của nguồn cấp cho board.
- **Task RTC:** Đọc dữ liệu ngày, giờ được thiết lập trong bộ RTC của vi điều khiển.

## 2. Phần cứng sử dụng (Hardware)

### 2.1. Các thành phần chính

- Board vi điều khiển STM32F411VETx.
- Cảm biến nhiệt độ và áp suất BMP280.
- Màn hình LCD 16x2 sử dụng giao tiếp I2C.
- Máy tính kết nối với STM32 thông qua UART.
- Nguồn cấp cho hệ thống.

### 2.2. Cấu hình chân kết nối

#### A. Cảm biến BMP280 – Giao tiếp SPI1

Cảm biến BMP280 sử dụng giao tiếp SPI1 để truyền và nhận dữ liệu với vi điều khiển.

| Chân STM32 | Tín hiệu SPI | Chức năng |
|---|---|---|
| PA4 | CS | Chọn cảm biến |
| PA5 | SCK | Xung đồng hồ |
| PA6 | MISO | Nhận dữ liệu từ cảm biến |
| PA7 | MOSI | Gửi dữ liệu đến cảm biến |

#### B. Màn hình LCD 16x2 – Giao tiếp I2C

Màn hình LCD 16x2 sử dụng giao tiếp I2C để nhận dữ liệu hiển thị.

| Chân STM32 | Tín hiệu I2C | Chức năng |
|---|---|---|
| [Chân SDA] | SDA | Truyền dữ liệu |
| [Chân SCL] | SCL | Tín hiệu xung đồng hồ |

#### C. UART2 – Giao tiếp với máy tính

UART2 được sử dụng để giao tiếp giữa STM32 và phần mềm Hercules Serial Port.

| Chân STM32 | Tín hiệu UART | Chức năng |
|---|---|---|
| PA2 | TX | Truyền dữ liệu |
| PA3 | RX | Nhận dữ liệu |

#### D. ADC – Đo điện áp nguồn

ADC được sử dụng để đọc giá trị điện áp đầu vào và xử lý dữ liệu nhằm xác định điện áp nguồn cấp cho board.

| Chân STM32 | Chức năng |
|---|---|
| [Chân ADC] | Nhận tín hiệu điện áp analog |

#### E. RTC – Đồng hồ thời gian thực

Sử dụng bộ RTC tích hợp bên trong vi điều khiển để lưu trữ và đọc thông tin ngày, giờ của hệ thống.

## 3. Kiến trúc phần mềm (Software Architecture)

Firmware được xây dựng theo mô hình đa nhiệm sử dụng FreeRTOS. Hệ thống được chia thành 5 Task, mỗi Task đảm nhiệm một chức năng riêng biệt.

### 3.1. Mô tả chức năng các Task

| Task | Chức năng |
|---|---|
| Task_SPI | Đọc dữ liệu nhiệt độ và áp suất từ cảm biến BMP280. |
| Task_I2C | Nhận dữ liệu từ các Task khác và cập nhật thông tin lên LCD 16x2. |
| Task_UART | Hiển thị trạng thái hoạt động của các Task và xử lý lệnh từ Hercules. |
| Task_ADC | Đọc và chuyển đổi giá trị ADC thành điện áp. |
| Task_RTC | Đọc dữ liệu ngày, giờ từ bộ RTC. |

### 3.2. Cơ chế trao đổi dữ liệu

Các Task trao đổi dữ liệu để phối hợp quá trình thu thập, xử lý và hiển thị thông tin.

[Mô tả cơ chế thực tế đang sử dụng, ví dụ: Queue, Mutex hoặc Semaphore.]

### 3.3. Luồng hoạt động của hệ thống

1. Khởi tạo hệ thống và các ngoại vi cần thiết.
2. Khởi tạo FreeRTOS và tạo 5 Task.
3. Task_SPI đọc dữ liệu từ cảm biến BMP280.
4. Task_ADC thu thập và xử lý giá trị điện áp.
5. Task_RTC cập nhật thông tin ngày và giờ.
6. Các Task gửi dữ liệu đến Task_I2C để hiển thị lên LCD.
7. Task_UART tiếp nhận lệnh từ Hercules và cung cấp thông tin trạng thái hệ thống.

## 4. Môi trường phát triển (Development Environment)

| Thành phần | Công cụ / Công nghệ |
|---|---|
| Vi điều khiển | STM32F411VETx |
| IDE | STM32CubeIDE |
| Ngôn ngữ | Embedded C |
| Thư viện | STM32 HAL / Truy cập thanh ghi trực tiếp |
| Hệ điều hành | FreeRTOS |
| Giao tiếp | SPI, I2C, UART |
| Đo lường | ADC |
| Thời gian thực | RTC |
| Phần mềm giao tiếp UART | Hercules Serial Port |

## 5. Hướng dẫn biên dịch và sử dụng (Build and Run)

1. Tải hoặc sao chép mã nguồn dự án về máy tính.
2. Mở STM32CubeIDE và import project.
3. Kiểm tra cấu hình Clock, GPIO và các ngoại vi.
4. Kiểm tra kết nối phần cứng theo bảng cấu hình chân.
5. Build project để biên dịch chương trình.
6. Kết nối board STM32 với máy tính thông qua ST-Link.
7. Nạp chương trình xuống vi điều khiển.
8. Mở phần mềm Hercules và cấu hình kết nối UART2.
9. Theo dõi dữ liệu cảm biến, trạng thái Task và kết quả hoạt động của hệ thống.

## 6. Kiểm thử và kết quả (Testing and Results)

Các chức năng cần được kiểm tra:

- Kiểm tra giao tiếp SPI giữa STM32 và BMP280.
- Kiểm tra khả năng đọc nhiệt độ và áp suất khí quyển.
- Kiểm tra khả năng hiển thị dữ liệu trên LCD thông qua I2C.
- Kiểm tra truyền nhận dữ liệu UART và xử lý lệnh từ Hercules.
- Kiểm tra kết quả đo điện áp thông qua ADC.
- Kiểm tra khả năng đọc ngày, giờ từ RTC.
- Kiểm tra hoạt động và trao đổi dữ liệu giữa 5 Task trong FreeRTOS.

[Bổ sung hình ảnh thực tế, kết quả đo và log UART sau khi kiểm thử.]

## 7. Hướng phát triển (Future Improvements)

- Cải thiện khả năng phát hiện và xử lý lỗi trong giao tiếp SPI và I2C.
- Xây dựng hệ thống Logging để theo dõi và chẩn đoán lỗi.
- Tối ưu việc quản lý tài nguyên và lập lịch các Task trong FreeRTOS.
- Cải thiện độ chính xác trong quá trình xử lý dữ liệu cảm biến.
- Bổ sung các chức năng giám sát và cảnh báo khi hệ thống gặp sự cố.


   

https://github.com/user-attachments/assets/09893729-3adb-4e09-9821-b97c6c620159

