#include "app_cmd.h"
#include "app_log.h"
#include "app_system.h"
#include "cmsis_os.h"
#include <string.h>
#include <ctype.h>

#define CMD_MAX     32
#define FW_VERSION  "v0.1.0"

extern osMessageQueueId_t CmdRxQueueHandle;
extern osMutexId_t        SystemDataMutexHandle;

static int get_snapshot(SystemData_t *s)
{
    if (osMutexAcquire(SystemDataMutexHandle, 50) != osOK) return -1;
    *s = g_sys;
    osMutexRelease(SystemDataMutexHandle);
    return 0;
}

static void cmd_help(void)
{
    LOG_PostWait(LOG_RAW, 0,
        "Commands:\r\n"
        "  help     - show this list\r\n"
        "  status   - system status\r\n"
        "  sensor   - sensor data\r\n"
        "  voltage  - ADC voltage\r\n"
        "  version  - firmware version\r\n"
        "  log on|off - periodic log\r\n");
}

static void cmd_version(void)
{
    LOG_PostWait(LOG_RAW, 0,
        "Environmental Monitoring System\r\n"
        "Firmware: " FW_VERSION "\r\n"
        "Board   : NUCLEO-L073RZ\r\n");
}

static void cmd_voltage(void)
{
    SystemData_t s;
    if (get_snapshot(&s) != 0) { LOG_PostWait(LOG_ERROR, 0, "Data busy"); return; }
    LOG_PostWait(LOG_FMT_VOLT, s.adc_mv,      "ADC Voltage : ");
    LOG_PostWait(LOG_FMT_MV,   s.sound_pp_mv, "Sound (p-p) : ");
}

static void cmd_sensor(void)
{
    SystemData_t s;
    if (get_snapshot(&s) != 0) { LOG_PostWait(LOG_ERROR, 0, "Data busy"); return; }

    if (s.i2c_ok) {
        LOG_PostWait(LOG_FMT_TEMP, s.temp_x10, "Temperature : ");
        LOG_PostWait(LOG_FMT_PA,   s.press_pa, "Pressure    : ");
    } else {
        LOG_PostWait(LOG_RAW, 0, "I2C sensor  : N/A\r\n");
    }
    LOG_PostWait(LOG_FMT_MV, s.sound_pp_mv, "Sound (p-p) : ");
}

static void cmd_status(void)
{
    SystemData_t s;
    if (get_snapshot(&s) != 0) { LOG_PostWait(LOG_ERROR, 0, "Data busy"); return; }

    LOG_PostWait(LOG_RAW, 0, "System Status\r\n");
    if (s.i2c_ok) LOG_PostWait(LOG_FMT_TEMP, s.temp_x10, "Temperature : ");
    else          LOG_PostWait(LOG_RAW, 0, "Temperature : N/A\r\n");
    LOG_PostWait(LOG_FMT_VOLT, s.adc_mv, "Voltage     : ");
    LOG_PostWait(LOG_RAW, 0, s.i2c_ok ? "I2C Sensor  : OK\r\n" : "I2C Sensor  : FAIL\r\n");
    LOG_PostWait(LOG_RAW, 0, s.spi_ok ? "SPI Device  : OK\r\n" : "SPI Device  : FAIL\r\n");
}

static void cmd_exec(const char *line)
{
    if      (strcmp(line, "help")    == 0) cmd_help();
    else if (strcmp(line, "status")  == 0) cmd_status();
    else if (strcmp(line, "sensor")  == 0) cmd_sensor();
    else if (strcmp(line, "voltage") == 0) cmd_voltage();
    else if (strcmp(line, "version") == 0) cmd_version();
    else if (strcmp(line, "log on")  == 0) { g_log_periodic = 1; LOG_PostWait(LOG_RAW, 0, "Periodic log ON\r\n"); }
    else if (strcmp(line, "log off") == 0) { g_log_periodic = 0; LOG_PostWait(LOG_RAW, 0, "Periodic log OFF\r\n"); }
    else LOG_PostWait(LOG_RAW, 0, "Unknown command. Type 'help'\r\n");
}

void APP_CMD_TaskLoop(void)
{
    char    line[CMD_MAX];
    uint8_t len = 0;
    uint8_t ch;

    for (;;) {
        if (osMessageQueueGet(CmdRxQueueHandle, &ch, NULL, osWaitForever) != osOK) continue;

        if (ch == '\r') {                               /* Enter */
            LOG_PostWait(LOG_ECHO, '\r', 0);
            line[len] = '\0';
            if (len > 0) cmd_exec(line);
            len = 0;
            LOG_PostWait(LOG_RAW, 0, "> ");
        } else if (ch == 0x08 || ch == 0x7F) {          /* Backspace */
            if (len > 0) { len--; LOG_PostWait(LOG_RAW, 0, "\b \b"); }
        } else if (ch >= 32 && ch < 127) {              /* Ky tu in duoc */
            if (len < CMD_MAX - 1) {
                line[len++] = (char)tolower(ch);
                LOG_PostWait(LOG_ECHO, ch, 0);
            }
        }
    }
}
