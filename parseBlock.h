//
// Created by hgz92_000 on 2015/12/27.
//

#ifndef XML_PARSE_PARSEBLOCK_H
#define XML_PARSE_PARSEBLOCK_H

#include "xml.h"

typedef struct EventNode
{
    EventType type;
    char *startpos;
    struct EventNode *next;
    union {
        //StartofElement
        struct {
            short NameStart;
            short NameEnd;
        }SE;

        //EndofElement
        struct {
            short NameStart;
            short NameEnd;
        }EE;

        //EmptyElement
        struct {
            short NameStart;
            short NameEnd;
        }EMPTY;

        //Attribute
        struct {
            short NameStart;
            short NameEnd;

            short AttrStart;
            short AttrEnd;
        }ATTR;

        //CharData
        struct {
            short CDStart;
            short CDEnd;
        }CD;

        //Comment
        struct {
            short ComStart;
            short ComEnd;
        }COMMENT;

        //CDATA
        struct {
            short CDATAStart;
            short CDATAEnd;
        }CDATA;

        //PI
        struct {
            short TargetStart;
            short TargetEnd;

            short InsStart;
            short InsEnd;
        }PI;
    }EventInfo;
}EventNode;

typedef struct EventList{
    struct EventNode *Head;
    struct EventNode *Tail;
}EventList;

void printDelimiterName(Delimiter d);

EventList* InitEventList();
inline EventNode* InitEventNode();
inline void addEventNode(EventList* el,EventNode *en);



void collectEvents(EventList *pList, EventBlock *p, int starteventcount);



inline int isXMLChar(char c);
inline int isXMLNameStartChar(char c);
inline int isXMLNameChar(char c);
inline int isXMLSpace(char c);

#endif //XML_PARSE_PARSEBLOCK_H
