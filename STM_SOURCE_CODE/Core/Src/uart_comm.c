#include "uart_comm.h"
#include "cmd_parser.h"
#include "main.h"
#include "stdio.h"
#include "string.h"

extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef hadc1;

/*
 * FSM giao tiếp UART
 * 0 = IDLE
 * 1 = WAIT_ACK
 */
static uint8_t uart_state = 0;

/* Timeout resend 3 giây */
static uint32_t last_timeout = 0;

/* Bộ đệm gửi UART */
static char tx_buffer[20];

/* Cập nhật ADC mỗi 0.5s */
static uint32_t last_adc_update = 0;
static uint32_t ADC_value = 0;

/* ------------------------------------------------------
 * CẬP NHẬT ADC MỖI 0.5s
 * ------------------------------------------------------ */
static void adc_update_500ms()
{
    if(HAL_GetTick() - last_adc_update >= 500)
    {
        last_adc_update = HAL_GetTick();

        HAL_ADC_Start(&hadc1);
        ADC_value = HAL_ADC_GetValue(&hadc1);

        // KHÔNG gửi UART, chỉ cập nhật value
    }
}

/* ------------------------------------------------------
 * FSM UART COMMUNICATION
 * ------------------------------------------------------ */
void uart_communiation_fsm()
{
    /* Luôn cập nhật ADC mỗi vòng FSM */
    adc_update_500ms();

    switch(uart_state)
    {
    /* --------------------------------------------------
     *  STATE 0 — IDLE (chờ !RST#)
     * -------------------------------------------------- */
    case 0:
        if(command_flag == CMD_RST)
        {
            command_flag = 0;

            sprintf(tx_buffer, "!ADC=%lu#", ADC_value);
            HAL_UART_Transmit(&huart2, (uint8_t*)tx_buffer, strlen(tx_buffer), 500);

            last_timeout = HAL_GetTick();
            uart_state = 1;
        }
        break;

    /* --------------------------------------------------
     *  STATE 1 — WAIT_ACK (chờ !OK#, hoặc timeout 3s)
     * -------------------------------------------------- */
    case 1:
        if(command_flag == CMD_OK)
        {
            command_flag = 0;
            uart_state = 0;          // Quay lại IDLE
        }
        else if(HAL_GetTick() - last_timeout >= 3000)   // timeout 3s
        {
            sprintf(tx_buffer, "!ADC=%lu#", ADC_value);
            HAL_UART_Transmit(&huart2, (uint8_t*)tx_buffer, strlen(tx_buffer), 500);

            last_timeout = HAL_GetTick();
        }
        break;
    }
}
