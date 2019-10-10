#ifndef __USER_POWER_H_
#define __USER_POWER_H_

#define PW_NUM 100
typedef struct
{
    int idx;
    uint32_t powers[PW_NUM];
} PowerRecord;

extern PowerRecord power_record;

char* GetPowerRecord(int idx);
void PowerInit(void);
void SetPowerRecord(PowerRecord* pr, uint32_t pw);

#endif
