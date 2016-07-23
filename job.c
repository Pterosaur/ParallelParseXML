#include "job.h"
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>


ResultBoard* CreateBoard(size_t size)
{
	ResultBoard *rb = (ResultBoard *)malloc(sizeof(ResultBoard));
	assert(rb);
	rb->haveDone = 0;
	rb->needDone = size;
	rb->results = (void **)calloc(size,sizeof(void *));
	assert(rb->results);
	return rb;
}

void UpdateBoard(ResultBoard *rb,int position,void *r)
{
	rb->results[position] = r;
}

void *StartWorker(void *w)
{
	while(1)
	{
		Task* t = PullTask(((Worker *)w)->tq);
		if(t->id != ENDTASK)
		{
			UpdateBoard(((Worker *)w)->rb,t->id,t->fun(t->arg));
			free(t);
		}
		else
		{
			pthread_exit(NULL);
			free(t);
		}
	}
}

void* EndWorker(void *t)
{
	//pthread_exit(NULL);
	return NULL;
}

Master* CreateMaster(int workerNumber)
{
	Master *m = (Master *)calloc(1,sizeof(Master));
	assert(m);
	m->onJobNumber = workerNumber;
	return m;
}

TaskQueue* StartJob(Master* m,ResultBoard *rb)
{
	TaskQueue *tq = CreateTaskQueue(10);
	for(int i = 0;i < m->onJobNumber;++i)
	{
		m->workers[i].tq = tq;
		m->workers[i].rb = rb;
		pthread_create(&((m->workers[i]).id),NULL,(void *(*)(void *))StartWorker,&(m->workers[i]));
	}
	return tq;
}

void EndJob(Master *m,TaskQueue *tq)
{
	for(int i = 0;i < m->onJobNumber;++i)
	{
		PushTask(tq,EndWorker,NULL,ENDTASK);
	}
	for(int i = 0;i < m->onJobNumber;++i)
	{
		pthread_join(m->workers[i].id,NULL);
	}
	free(tq);
}
