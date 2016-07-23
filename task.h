#pragma once

#include <stdlib.h>

#define ENDTASK -1

//原子计数器
typedef int AtomicCounter;

//任务结构体
typedef struct Task
{
	//任务ID
	size_t id;
	//任务执行函数
	void* (*fun)(void *);
	//任务参数
	void *arg;
}Task;

//任务节点
typedef struct TaskNode
{
	//当前节点所对应的任务
	Task* value;
}TaskNode;

//任务队列
//备注:此队列为无锁队列
//	tail 等于 head 时为空队列
//	tail + 1 % maxCapacity时为满
typedef struct TaskQueue
{
	//任务队列长度
	AtomicCounter tasksNumber;
	//队列最大容量
	size_t maxCapacity;
	//任务队列头位置
	size_t head;
	//任务队列尾位置
	size_t tail;
	//节点容器
	TaskNode* container;
}TaskQueue;

//创建任务队列
//参数:capacityOf2Index为2的指数倍,如参数10为1024的容量
TaskQueue* CreateTaskQueue(size_t capacityOf2Index);

//推送任务
void PushTask(TaskQueue *tq,void* (*fun)(void *), void *arg, size_t taskID);

//拉取任务
Task* PullTask(TaskQueue *tq);
