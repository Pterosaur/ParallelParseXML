#include "xml.h"
#include "file.h"
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "parseBlock.h"

//比较x,y的前一个字节是否相等
#define Equal1(x, y) !((*(uint64_t *)(x) ^ (y)) & 0x00000000000000FF)
//比较x,y的前两个字节是否相等
#define Equal2(x, y) !((*(uint64_t *)(x) ^ (y)) & 0x000000000000FFFF)
//比较x,y的前三个字节是否相等
#define Equal3(x, y) !((*(uint64_t *)(x) ^ (y)) & 0x0000000000FFFFFF)
//比较x,y的前七个字节是否相等
#define Equal7(x, y) !((*(uint64_t *)(x) ^ (y)) & 0x00FFFFFFFFFFFFFF)

//查找指定定界符位置
//参数:
//	source-源字符串
//	length-字符串长度
//	target-查找目标(uint64_t)
//	position-开始查找位置,同时也是返回值
//	compareMacro-比较宏
#define WantFind(source, length, target, position, compareMacro)\
{\
    while((position) < (length))\
    {\
        if(compareMacro((source) + (position),(target)))\
            break;\
        ++(position);\
    }\
}

EventBlock *CreateEventBlock(int fp, size_t size, EventBlock *lastB) {
    EventBlock *eb = (EventBlock *) malloc(sizeof(EventBlock));
    assert(eb);

    if (lastB) {
        --lastB->mapSize;
        DelimiterMap *lastMap = lastB->map + lastB->mapSize;
        eb->content = (char *) malloc(size \
 + (lastB->length - lastMap->position)\
 + 1 \
 + (size / 2) * sizeof(DelimiterMap));
        assert(eb->content);
        //拷贝最后一个事件
        (char *) memcpy(eb->content, \
            lastB->content + lastMap->position, \
            lastB->length - lastMap->position);
        eb->content += (lastB->length - lastMap->position);
    }
    else {
        eb->content = (char *) malloc(size \
 + 1\
 + (size / 2) * sizeof(DelimiterMap));
        assert(eb->content);
    }

    eb->length = read(fp, eb->content, size);
    eb->content[eb->length] = 0;

    eb->map = (DelimiterMap *) (eb->content + eb->length + 1);
    eb->mapSize = 0;


    if (lastB) {
        eb->map[eb->mapSize].type = lastB->map[lastB->mapSize].type;
        eb->map[eb->mapSize++].position = lastB->map[lastB->mapSize].position - lastB->length;
    }
    else {
        eb->map[eb->mapSize].type = EndTag;
        eb->map[eb->mapSize].position = 0;
    }

    return eb;
}

void DeterminateDelimiter(EventBlock *eb) {
    for (int i = eb->map[0].position; i < (int) eb->length; ++i) {
        switch (eb->map[eb->mapSize - 1].type) {
            case StartPi: {
                WantFind(eb->content, eb->length, FEndPi, i, Equal2);
                if (i < eb->length) {//说明找到了
                    eb->map[eb->mapSize].type = EndPi;
                    eb->map[eb->mapSize++].position = i;
                }
                break;
            };
            case StartCDATA: {
                WantFind(eb->content, eb->length, FEndCDATA, i, Equal3);
                if (i < eb->length) {//说明找到了
                    eb->map[eb->mapSize].type = EndCDATA;
                    eb->map[eb->mapSize++].position = i;
                }
                break;
            };
            case StartComment: {
                WantFind(eb->content, eb->length, FEndComment, i, Equal3);
                if (i < eb->length) {//说明找到了
                    eb->map[eb->mapSize].type = EndComment;
                    eb->map[eb->mapSize++].position = i;
                }
                break;
            };
            case CDATAOrComment: {
                if (Equal2(eb->content + i, FStartComment)) {
                    eb->map[eb->mapSize - 1].type = StartComment;
                }
                else if (Equal7(eb->content + i, FStartCDATA)) {
                    eb->map[eb->mapSize - 1].type = StartCDATA;
                }
                else if (i == eb->map[0].position + 2) {
                    printf("error\n");
                    exit(1);
                } else if(i >= eb->map[0].position + 2) {
                    i = (int)eb->length;
                }

                break;
            };
            default: {
                for (; i < (int) eb->length; ++i) {
                    if (Equal2((eb->content + i), FEndTag)) {
                        (eb->map + eb->mapSize)->type = EndTag;
                        (eb->map + eb->mapSize)->position = i;
                        eb->mapSize++;
                        ++i;
                        break;
                    }
                    else if (Equal2((eb->content + i), FCDATAOrComment)) {
                        (eb->map + eb->mapSize)->type = CDATAOrComment;
                        (eb->map + eb->mapSize)->position = i;
                        eb->mapSize++;
                        ++i;
                        break;
                    }
                    else if (Equal2((eb->content + i), FStartPi)) {
                        (eb->map + eb->mapSize)->type = StartPi;
                        (eb->map + eb->mapSize)->position = i;
                        eb->mapSize++;
                        ++i;
                        break;
                    }
                    else if (Equal1((eb->content + i), FStartTagOrEmptyTag)) {
                        (eb->map + eb->mapSize)->type = StartTagOrEmptyTag;
                        (eb->map + eb->mapSize)->position = i;
                        eb->mapSize++;
                    }
                }
            };
        }
    }
}

void CutXMLFileIntoQueue(const char *file, TaskQueue *tq) {
    int fp = open(file, O_RDONLY);
    size_t optimalBlocksNumber = FileBlocksNumber(fp);
    size_t optimalSize = FileBlockSize(fp);
    char *lastEnd = NULL;
    DelimiterMap lastDelimiterMap = {0, EndTag};
    EventBlock *firstEb = CreateEventBlock(fp, optimalSize, NULL);
    unsigned taskId = 0;
    do {
        DeterminateDelimiter(firstEb);
        EventBlock *secondEb = CreateEventBlock(fp, optimalSize, firstEb);
        PushTask(tq, ParseJob, firstEb, taskId++);
        firstEb = secondEb;
    } while (firstEb->length);
    PushTask(tq, ParseJob, firstEb, taskId++);
}


/*
 * ParseJob is my main.whatever wrote here will be executed on each EventBlock
 * in different threads
 */

void *ParseJob(void *eb) {
    static int runCount = 0;
    printf("Running %d th time:\n", ++runCount);

    EventBlock *p = (EventBlock *) eb;
    EventList *res = InitEventList();

    int i = 0;
    for (i = 0; i < p->mapSize; ++i) {
        printDelimiterName(p->map[i].type);
        //collectEvents(res, p, i);
    }

    /*
for (i = 0; i < p->mapSize; ++i) {
    *(p->content + p->map[i].position-1) = 'E';
    *(p->content + p->map[i].position) = 'S';

    //...
}
printf("%s", p->content + p->map[0].position);

  */
    return p;
}

