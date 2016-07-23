#include "job.h"
#include "task.h"
#include "xml.h"
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <assert.h>

#define Equal(x,y) !((*(uint64_t *)(x) ^ (y)) & 0x00000000000000FF)

int main(int argn,char *argv[])
{
	
	ResultBoard  *rb = CreateBoard(10000);
	Master* m = CreateMaster(1);
	TaskQueue *tq = StartJob(m,rb);
	assert(argn > 1);
	CutXMLFileIntoQueue(argv[1],tq);
	EndJob(m,tq);

	return 0;
}
