//Include
#include "cli_parser.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

//Value
extern UART_HandleTypeDef huart2;
static System_Data_t current_sys_data = {0};

static void cmd_help(void);
static void cmd_status(void);
static void cmd_sensor(void);
static void cmd_voltage(void);
static void cmd_version(void);

static const cli_command_t cmd_table[] = {
    {"help",    "Hien thi tro giup",           cmd_help},
    {"status",  "Xem trang thai he thong",     cmd_status},
    {"sensor",  "Xem thong so BME280",         cmd_sensor},
    {"voltage", "Xem dien ap ADC",             cmd_voltage},
    {"version", "Xem phien ban phan mem",      cmd_version},
};
#define CMD_COUNT (sizeof(cmd_table) / sizeof(cmd_table[0]))

//functions

void cli_parser_update_data(const System_Data_t *data)
{
    if (data != NULL)
    {
        current_sys_data = *data;
    }
}

static void cli_printf(const char *format, ...)
{
    char buf[128];
    va_list args;                      // 1. Tạo biến con trỏ quản lý danh sách đối số

    va_start(args, format);            // 2. Trỏ "args" vào vị trí ngay sau tham số "format"

    // 3. Giao "args" cho vsnprintf. Hàm này tự bốc tách các biến để ráp vào buf
    int len = vsnprintf(buf, sizeof(buf), format, args);

    va_end(args);                      // 4. Đóng con trỏ, giải phóng an toàn

    // 5. Kiểm tra và truyền chuỗi ra UART
    if (len > 0 && len < (int)sizeof(buf))
    {
        HAL_UART_Transmit(&huart2, (uint8_t *)buf, (uint16_t)len, 100);
    }
}

static void cmd_help(void)
{
	cli_printf("\r\nSupported Commands:\r\n");

	for(int i = 0; i < CMD_COUNT; i++)
	{
		cli_printf("%-10s : %s\r\n", cmd_table[i].name, cmd_table[i].help);
	}
}

static void cmd_status(void)
{
	cli_printf("\r\nSystem Status:\r\n");
	cli_printf("Temperature: %.1f C\r\n", current_sys_data.temperature);
	cli_printf("Voltage: %.2f V\r\n", current_sys_data.voltage);
	cli_printf("I2C Sensor: %s\r\n", (current_sys_data.bme280_ok && current_sys_data.oled_ok) ? "OK" : "ERROR");
	cli_printf("SPI Device: %s\r\n", current_sys_data.flash_ok ? "OK" : "ERROR");
	cli_printf("ADC Sensor: %s\r\n", current_sys_data.adc_ok ? "OK" : "ERROR");
}

static void cmd_sensor(void)
{
	cli_printf("\r\nBME Sensor:\r\n");
	cli_printf("Temperature	: %.1f C\r\n", current_sys_data.temperature);
	cli_printf("Pressure	: %.1f hPa\r\n", current_sys_data.pressure);
	cli_printf("Humidity	: %.1f %%\r\n", current_sys_data.humidity);
}

static void cmd_voltage(void)
{
	cli_printf("\r\nADC Sensor:\r\n");
	cli_printf("Status	: %s\r\n", current_sys_data.adc_ok ? "OK" : "ERROR");
	cli_printf("Voltage	: %.2f V\r\n", current_sys_data.voltage);
}

static void cmd_version(void)
{
	cli_printf("\r\nFirmware Information:\r\n");
	cli_printf("Project : STM32F411 Environment Monitoring\r\n");
	cli_printf("Version : 1.0.0\r\n");
	cli_printf("Build   : FreeRTOS 5-Tasks Architecture\r\n");
}

void cli_parser_process(char *cmd_line)
{
	if(cmd_line == NULL)
	{
		return;
	}

	char *cmd_name = strtok(cmd_line, " \t\r\n"); //Lệnh strtok là để để lấy chuỗi trước \r\n bỏ vào cmd_name
	if(cmd_name == NULL)
	{
		cli_printf("\r\n> ");
		return;
	}

	uint8_t found = 0;
	for(int i = 0; i < CMD_COUNT; i++)
	{
		//strcmp(str1, str2) -> string compare dùng để so sánh 2 chuỗi
		//TH1: = 0 -> 2 chuỗi giống y hệt nhau VD: "status" = "status"
		//TH2: < 0 -> str1 ngắn hơn str2 VD: "help" < "status"
		//TH3: > 0 -> str1 dài hơn str2
		// Hàm này có giá trị trả về dựa trên:
		// Giá trị trả về = mã ASCII của ký tự ở str1 - mã ASCII của ký tự ở str2
		if(strcmp(cmd_name, cmd_table[i].name) == 0)
		{
			cmd_table[i].function();
			found = 1;
			break;
		}
	}
	if(found != 1)
	{
		cli_printf("\r\nUnknown command: '%s'. Type 'help' for options.\r\n", cmd_name);
	}

	cli_printf("\r\n> ");

}
