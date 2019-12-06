#include"web_log.h"

LogRecord log_record = { 1,{ 0 } };
char log_record_str[LOG_NUM*LOG_LEN] = { 0 };

void SetLogRecord(LogRecord* lr, char* log)
{
    if (strlen(log) > LOG_LEN)
    {
        log[LOG_LEN-1] = 0;
    }
    char** log = &lr->logs[(++lr->idx)% LOG_NUM];
    if (*log)
    {
        free(*log);
    }
    *log = lr;
}

char* GetLogRecord(int idx)
{
    if (idx > log_record.idx) return "";

    int i = idx > 0 ? idx : (log_record.idx - LOG_NUM + 1);
    i = i < 0 ? 0 : i;
    char* tmp = log_record_str;
    for (; i <= log_record.idx; i++)
    {
        sprintf(tmp, "%s\n", log_record.logs[i%LOG_NUM]);
        tmp += strlen(tmp);
    }
    return log_record_str;
}

