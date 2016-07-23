#include "task.h"
#include <stdio.h>
#include <math.h>
#include <assert.h>

//原子操作定义如下
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//++X
#define AAF(x) __sync_add_and_fetch(&(x),1)
//--X
#define SAF(x) __sync_sub_and_fetch(&(x),1)
//X++
#define FAA(x) __sync_fetch_and_add(&(x),1)
//X--
#define FAS(x) __sync_fetch_and_sub(&(x),1)
//Compare Then Swap
#define CAS(position,oldValue,newValue) __sync_bool_compare_and_swap(&(position),(oldValue),(newValue))
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//定位容器中的位置
#define Location(x,capacity) ((x)&((capacity) - 1))

//错误信息
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#define MEMORYAPPLYERROR "内存申请错误\n"
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv


TaskQueue* CreateTaskQueue(size_t capacityOf2Index)
{
	size_t capacity = pow(2,capacityOf2Index);
	if(!capacity)
	{
		perror(MEMORYAPPLYERROR);
		return NULL;
	}
	TaskQueue *tmp = (TaskQueue *)malloc(sizeof(TaskQueue));
	assert(tmp);
	tmp->tasksNumber = 0;
	tmp->maxCapacity = capacity;
	tmp->head = 0;
	tmp->tail = 0;
	tmp->container = (TaskNode *)calloc(capacity,sizeof(TaskNode));
	assert(tmp->container);
	return tmp;
}

void PushTask(TaskQueue *tq,void* (*fun)(void *), void *arg, size_t taskID)
{
	Task *tmp = (Task *)malloc(sizeof(Task));
	assert(tmp);
	tmp->id = taskID;
	tmp->fun = fun;
	tmp->arg = arg;
	//申请位置
	size_t apply;
	//下一个位置
	size_t nextPosition;
	 do
	 {
	 	apply = tq->tail;
	 	nextPosition = apply+1;
	 	//队列满了就一直等待
	 }while(!CAS(tq->tail,apply,nextPosition));
	//放到申请到的位置
	while(!CAS((tq->container + Location(apply,tq->maxCapacity) )->value,NULL,tmp));
}

Task* PullTask(TaskQueue *tq)
{
	//申请位置
	size_t apply;
	//下一个位置
	size_t nextPosition;
	 do
	 {
	 	apply = tq->head;
	 	nextPosition = apply+1;
	 }while(!CAS(tq->head,apply,nextPosition));
	
	//申请任务的临时指针
	Task* tmp;
	do
	{
		while(!(tmp = ( tq->container + Location(apply,tq->maxCapacity) )->value));		
	}while(!CAS(( tq->container + Location(apply,tq->maxCapacity) )->value,tmp,0));
		
	return tmp;
}

