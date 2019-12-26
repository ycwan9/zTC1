#pragma once

struct TimedTask;
typedef struct TimedTask* pTimedTask;
struct TimedTask
{
    int time;        //被执行的格林尼治时间戳
    int socket_idx;  //要控制的插孔
    int on;          //开或者关
    pTimedTask next; //下一个任务(按之间排序)
};

extern pTimedTask task_top;
extern int task_count;

bool AddTask(pTimedTask task);
bool DelTask(int time);
bool DelFirstTask();
char* GetTaskStr();
