
#define LOG_NUM 100
#define LOG_LEN 128
typedef struct
{
    int idx;
    char* logs[LOG_NUM];
} LogRecord;

void SetLogRecord(LogRecord* lr, char* log);
char* GetLogRecord(int idx);

#define os_log(format, ...)                        \
    char* log = malloc(sizeof(char)*LOG_LEN);      \
    snprintf(log, LOG_LEN, format, ##__VA_ARGS__); \
    SetLogRecord(log_record, log);                 \
    custom_log("WIFI", format, ##__VA_ARGS__)      \

