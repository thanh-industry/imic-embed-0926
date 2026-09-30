\# STM32 Embedded Systems – Group Assignment



\## Đề tài: Environmental Monitoring System



Mỗi nhóm xây dựng một ứng dụng nhúng hoàn chỉnh sử dụng \*\*STM32 + FreeRTOS\*\* để thu thập dữ liệu cảm biến, giám sát điện áp và giao tiếp với máy tính.



\## 1. Yêu cầu chức năng



Hệ thống phải có:



\- \*\*I2C:\*\* giao tiếp với một cảm biến, ví dụ BME280, MPU6050, SHT30...

\- \*\*SPI:\*\* giao tiếp với một sensor/device, ví dụ W25Qxx Flash, ADXL345...

\- \*\*ADC:\*\* đo một điện áp analog và chuyển đổi sang đơn vị Volt.

\- \*\*UART:\*\* giao tiếp hai chiều với máy tính ở `115200 8N1`.

\- \*\*Logging:\*\* hiển thị thông tin hệ thống, sensor và lỗi qua UART.

\- \*\*FreeRTOS:\*\* tổ chức chương trình thành nhiều task.



Ví dụ log:



```text

\[INFO] System started

\[I2C] Temperature = 28.2 C

\[SPI] Device ID = 0xE5

\[ADC] Voltage = 3.25 V

```



\## 2. UART Command



STM32 phải nhận và xử lý command từ máy tính.



Tối thiểu hỗ trợ:



```text

help

status

sensor

voltage

version

```



Ví dụ:



```text

> status



System Status

Temperature : 28.2 C

Voltage     : 3.25 V

I2C Sensor  : OK

SPI Device  : OK

```



\## 3. FreeRTOS



Ứng dụng phải có tối thiểu các task:



```text

SensorTask

SPITask

ADCTask

UARTTask

CommandTask

```



Khuyến khích sử dụng:



\- Queue

\- Mutex

\- Semaphore



để giao tiếp và đồng bộ giữa các task.



Không được viết toàn bộ application trong `while(1)` của `main.c`.



\## 4. Repository Structure



Tất cả các nhóm làm chung một repository:



```text

stm32-embedded-project/

├── README.md

├── group01/

├── group02/

├── group03/

├── group04/

└── group05/

```



Mỗi nhóm chỉ làm việc trong folder của nhóm mình.



\## 5. Git Workflow



Mỗi nhóm phải tạo branch riêng.



Ví dụ Group 1:



```bash

git checkout main

git pull

git checkout -b group01-dev

```



Trong quá trình phát triển phải tạo nhiều commit có ý nghĩa:



```bash

git add group01/

git commit -m "feat: add I2C sensor driver"



git add group01/

git commit -m "feat: implement ADC voltage measurement"



git add group01/

git commit -m "feat: add UART command interface"



git add group01/

git commit -m "fix: handle sensor timeout"

```



Push branch:



```bash

git push origin group01-dev

```



Sau khi hoàn thành, tạo \*\*Pull Request\*\*:



```text

group01-dev -> main

```



Pull Request phải được review trước khi merge.



\### Quy định Git



Không được:



\- Code trực tiếp trên branch `main`.

\- Chỉ tạo một commit cho toàn bộ project.

\- Sửa source code trong folder của nhóm khác.

\- Merge vào `main` mà không có Pull Request.



\## 6. Deliverables



Mỗi nhóm cần submit:



1\. Source code chạy được trên STM32.

2\. File STM32CubeMX `.ioc`.

3\. `README.md` trong folder của nhóm.

4\. Mô tả hardware và pin configuration.

5\. Git history rõ ràng.

6\. Pull Request vào `main`.

7\. Demo hệ thống thực tế.



\## 7. Đánh giá



Bài tập được đánh giá dựa trên:



\- \*\*Firmware functionality\*\*

\- \*\*FreeRTOS design\*\*

\- \*\*Code structure \& quality\*\*

\- \*\*Git workflow\*\*

\- \*\*Teamwork\*\*

\- \*\*Documentation\*\*

\- \*\*Final demo\*\*



Mục tiêu của bài tập không chỉ là làm cho từng peripheral hoạt động riêng lẻ, mà là xây dựng một \*\*embedded application hoàn chỉnh\*\* theo quy trình gần với một dự án firmware thực tế.

