
#ifndef __USER_FUNCTION_H_
#define __USER_FUNCTION_H_


#include "mico.h"
#include "micokit_ext.h"

void user_send(int udp_flag, char *s);
void user_function_cmd_received(int udp_flag,uint8_t *pusrdata);
unsigned char strtohex(char a, char b);


#endif
