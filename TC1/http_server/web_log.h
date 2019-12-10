
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


#define os_log(format, ...)                            \
    LOG_TMP = (char*)malloc(sizeof(char)*LOG_LEN);     \
    snprintf(LOG_TMP, LOG_LEN, format, ##__VA_ARGS__); \
    SetLogRecord(&log_record, LOG_TMP);                \
    custom_log("WIFI", format, ##__VA_ARGS__)          \

