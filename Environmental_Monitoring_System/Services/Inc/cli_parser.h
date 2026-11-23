#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

//Include
#include "stm32f4xx.h"
#include "app_main.h"

//Struct
typedef void (*cli_func_t)(void);

typedef struct {
    const char *name;     // Tên lệnh (chuỗi string)
    const char *help;     // Câu mô tả (chuỗi string)
    cli_func_t function;  // Con trỏ trỏ tới hàm xử lý
} cli_command_t;

//Functions Prototype

	/**
	 * @brief Hàm để các Task đo đạc đẩy dữ liệu mới nhất vào cho CLI
	 */
	void cli_parser_update_data(const System_Data_t *data);

	/**
	 * @brief Hàm nhận và bóc tách chuỗi lệnh
	 */
	void cli_parser_process(char *cmd_line);

#ifdef __cplusplus
}
#endif

#endif /* _CLI_PARSER_H */
