#pragma once

#include "task.h"
#include <stdlib.h>
#include <stdint.h>

//事件定界符定义转整形表
//说明:把定界符类型转换为对应的数值,因为是小端存储,所以字节顺序是反的
//	两字节特征
#define DelimiterType2(_1st,_2nd) ((((long)(_2nd))<<8) | (((long)(_1st))))
//	四字节特征
#define DelimiterType4(_1st,_2nd,_3rd,_4th) ((DelimiterType2(_3rd,_4th)<< 16) | (DelimiterType2(_1st,_2nd)))
//	八字节特征
#define DelimiterType8(_1st,_2nd,_3rd,_4th,_5th,_6th,_7th,_8th) (DelimiterType4(_5th,_6th,_7th,_8th)<<32) | (DelimiterType4(_1st,_2nd,_3rd,_4th))
//	三字节特征
#define DelimiterType3(_1st,_2nd,_3rd) DelimiterType4(_1st,_2nd,_3rd,0)
//	七字节特征
#define DelimiterType7(_1st,_2nd,_3rd,_4th,_5th,_6th,_7th) (DelimiterType8(_1st,_2nd,_3rd,_4th,_5th,_6th,_7th,0))

//定界符特征
typedef enum DelimiterFeature : uint64_t
{

	FStartTagOrEmptyTag = DelimiterType2('<',0),
	FEndTag = DelimiterType2('<','/'),
	

	FStartPi = DelimiterType2('<','?'),
	FEndPi = DelimiterType2('?','>'),
	

	FCDATAOrComment = DelimiterType2('<','!'),

	FStartComment = DelimiterType2('-','-'),
	FEndComment = DelimiterType3('-','-','>'),

	FStartCDATA = DelimiterType7('[','C','D','A','T','A','['),
	FEndCDATA = DelimiterType3(']',']','>'),


}DelimiterFeature;

//定界符
typedef enum DelimiterName
{
	StartTagOrEmptyTag,
	EndTag,

	
	StartPi,
	EndPi,
	

	CDATAOrComment,

	StartComment,
	EndComment,

	StartCDATA,
	EndCDATA,

}Delimiter;

//定界符映射表
//说明:共32bit(4byte),
//	第2位到第5位共4位设置为定界符类型,可以表示16种类型
//	第6位到第32位共27位设置定界符的位置
typedef struct DelimiterMap
{
	//定界符相对位置
	int position:28;
	//定界符名称
	Delimiter type:4;
}DelimiterMap;

//事件块
//初步判断定界符位置的块
typedef struct EventBlock
{
	//本块文本数据指针
	char* content;
	//本块长度
	size_t length;
	//需要解析的本块长度
	size_t needLength;


	//本事件块定界符映射表
	DelimiterMap* map;
	//事件数量
	size_t mapSize;

}EventBlock;

//创建事件块
//参数:	fp-文件描述符
//	size-本块事件希望包含的文本长度
//	lastB-上一块事件块
EventBlock* CreateEventBlock(int fp ,size_t size ,EventBlock *lastB);

//边界界定符判断
//参数:	rb-待解析定界符事件块
//	lastDelimiterMap-上一个块最后一个分隔符映射
void DeterminateDelimiter(EventBlock* eb);

//数据切割预处理
void CutXMLFileIntoQueue(const char* file,TaskQueue *tq);

//解析任务
void* ParseJob(void *eb);

typedef enum EventType
{
	StartofElement,
	EndofElement,
	EmptyElement,
	Attribute,
	CharData,
	Comment,
	CDATA,
	Pi,

}EventType;
