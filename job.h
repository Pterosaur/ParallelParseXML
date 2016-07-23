#pragma once

#include "task.h"
#include <stdbool.h>
#define MAXPOSITION 64

//结果告示板
typedef struct ResultBoard
{
	//已经完成的数量
	size_t haveDone;
	//需要完成的数量
	size_t needDone;
	//结果缓存集合
	void **results;
}ResultBoard;

//创建告示板
ResultBoard* CreateBoard(size_t size);

//提交结果给告示板
void UpdateBoard(ResultBoard *rb,int position,void *r);

//任务工人
//一个工人一个独立的线程
typedef struct Worker
{
	//工人的线程ID
	pthread_t id;
	//工人的任务序列
	TaskQueue *tq;
	//提交任务的告示板
	ResultBoard *rb;
}Worker;

//开始工作
//参数:工人的指针
void *StartWorker(void *w);

//结束工作
//参数:最后一个任务的指针
void *EndWorker(void *t);

//任务管理员
typedef struct Master
{
	//当前在职工人数量
	unsigned short int onJobNumber;
	//手下的工位
	Worker workers[MAXPOSITION];
}Master;

//创建管理员
//参数:工人(线程)的数量
Master* CreateMaster(int workerNumber);

//启动任务
TaskQueue* StartJob(Master* m,ResultBoard *rb);

//结束任务
void EndJob(Master *m,TaskQueue *tq);
