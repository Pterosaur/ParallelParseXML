#pragma once
/*
*文件名:文件操作
*作者:甘泽
*创建时间:2015年12月18日19:40:31
*最后修改时间:2015年12月18日19:40:39
*/

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//文件描述符
#define FILEPOINT int

//获取文件块数
size_t FileBlocksNumber(FILEPOINT fp);

//获取文件块大小
size_t FileBlockSize(FILEPOINT fp);

