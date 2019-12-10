#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include"http_server/web_log.h"

LogRecord log_record = { 1,{ 0 } };
char log_record_str[LOG_NUM*LOG_LEN] = { 0 };
char* LOG_TMP;

void SetLogRecord(LogRecord* lr, char* log)
{
    if (strlen(log) > LOG_LEN)
    {
        log[LOG_LEN-1] = 0;
    }
    char** p_log = &lr->logs[(++lr->idx)% LOG_NUM];
    if (*p_log)
    {
        free(*p_log);
    }
    *p_log = log;
}

char* GetLogRecord(int idx)
{
    if (idx > log_record.idx) return "";

    int i = idx > 0 ? idx : (log_record.idx - LOG_NUM + 1);
    i = i < 0 ? 0 : i;
    char* tmp = log_record_str;
    for (; i <= log_record.idx; i++)
    {
        if (!log_record.logs[i%LOG_NUM]) continue;
        sprintf(tmp, "%s\n", log_record.logs[i%LOG_NUM]);
        tmp += strlen(tmp);
    }
    return log_record_str;
}

