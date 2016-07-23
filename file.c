#include "file.h"
#include <math.h>

size_t FileBlocksNumber(FILEPOINT fp)
{
	struct stat sbuf;
	//标准输入
	if(fp == 0)
	{
		return 1;
	}
	fstat(fp,&sbuf);
	return ceil((double)sbuf.st_size/sbuf.st_blksize);
}

size_t FileBlockSize(FILEPOINT fp)
{
	struct stat sbuf;
	fstat(fp,&sbuf);
	return sbuf.st_blksize;
}