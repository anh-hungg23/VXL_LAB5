#ifndef __CMD_PARSER_H__
#define __CMD_PARSER_H__

#include "stdint.h"
#define CMD_RST   1
#define CMD_OK    2
extern uint8_t command_flag;

void command_parser_fsm(uint8_t c);

#endif
