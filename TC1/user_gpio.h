
#ifndef __USER_KEY_H_
#define __USER_KEY_H_


#include "mico.h"
#include "micokit_ext.h"

void user_led_set(char x);
void key_init(void);
void user_relay_set(unsigned char x,unsigned char y);
void user_relay_set_all(char y);
bool relay_out(void);
char* get_socket_status();

#endif
