
#ifndef __USER_KEY_H_
#define __USER_KEY_H_


#include "mico.h"
#include "micokit_ext.h"

void UserLedSet(char x);
void KeyInit(void);
void UserRelaySet(unsigned char x,unsigned char y);
void UserRelaySetAll(char y);
bool RelayOut(void);
const unsigned char* GetSocketStatus();
void SetSocketStatus(char* socket_status);

#endif
