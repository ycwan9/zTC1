#ifndef WEB_LOG_H
#define WEB_LOG_H

#define LOG_NUM 100
#define LOG_LEN 128

typedef struct
{
    int idx;
    char* logs[LOG_NUM];
} LogRecord;

extern LogRecord log_record;
extern char* LOG_TMP;

void SetLogRecord(LogRecord* lr, char* log);
char* GetLogRecord(int idx);

#define web_log(format, ...)                           \
    LOG_TMP = (char*)malloc(sizeof(char)*LOG_LEN);     \
    snprintf(LOG_TMP, LOG_LEN, format, ##__VA_ARGS__); \
    SetLogRecord(&log_record, LOG_TMP);                \

#endif // !WEB_LOG_H
