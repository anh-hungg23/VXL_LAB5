#include "cmd_parser.h"
#include "string.h"
#include "stdio.h"

uint8_t command_flag = 0; // 1 = RST, 2 = OK



static uint8_t parser_state = 0;
static char parsing_buffer[10];
static uint8_t parsing_index = 0;

void command_parser_fsm(uint8_t c)
{
    switch(parser_state)
    {
    case 0:
        if(c == '!') {
            parsing_index = 0;
            parsing_buffer[parsing_index++] = c;
            parser_state = 1;
        }
        break;

    case 1:
        parsing_buffer[parsing_index++] = c;
        if(c == '#') {
            parsing_buffer[parsing_index] = 0;

            if(strcmp(parsing_buffer, "!RST#") == 0) {
                command_flag = CMD_RST;
            }
            else if(strcmp(parsing_buffer, "!OK#") == 0) {
                command_flag = CMD_OK;
            }

            parser_state = 0;
        }
        if(parsing_index >= 9) parser_state = 0;

        break;
    }
}
