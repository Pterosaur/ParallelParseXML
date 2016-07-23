//
// Created by hgz92_000 on 2015/12/27.
//

#include <stdio.h>
#include <assert.h>
#include "parseBlock.h"


inline void collectKnownTag(EventList *pList, char **workingp, char *endp, Delimiter name);

inline void StartTagOrEmptyHelper(EventList *pList, char **workingp, char *endp);

inline void eatXMLName(char **workingp);

inline void eatXMLSpaces(char **pString);

inline EventNode *eatandgenXMLAttr(char **workingp);

inline void eatXMLAttrValue(char **pString);

void printDelimiterName(Delimiter d) {
    switch (d) {
        case StartTagOrEmptyTag:
            printf("StartTagOrEmptyTag\n");
            break;
        case EndTag:
            printf("EndTag\n");
            break;
        case StartPi:
            printf("StartPi\n");
            break;
        case EndPi:
            printf("EndPi\n");
            break;
        case StartComment:
            printf("StartComment\n");
            break;
        case EndComment:
            printf("EndComment\n");
            break;
        case StartCDATA:
            printf("StartCDATA\n");
            break;
        case EndCDATA:
            printf("EndCDATA\n");
            break;
        default:
            break;
    }
}


/*
 * Collect Events from starteventcount to next event
 */
 /*
void collectEvents(EventList *pList, EventBlock *p, int starteventcount) {
    char *workingp = p->content + p->map[starteventcount].position;
    char *endp = p->content + p->map[starteventcount + 1].position - 1;
    collectKnownTag(pList,
                    &workingp, endp,
                    p->map[starteventcount].type);
    collectRestafterKnownTag(pList, workingp, endp, p->map[starteventcount].type);
}

inline void collectKnownTag(EventList *pList, char **workingp, char *endp, Delimiter name) {
    switch (name) {
        case StartTagOrEmptyTag:
            StartTagOrEmptyHelper(pList, workingp, endp);
            break;
        case StartPi:
            StartPiHelper(pList, workingp, endp);
            break;
        case StartComment:
        case StartCDATA:
            CommentorCDATAHelper(pList, workingp, endp);
            break;
        default:
            break;
    }
}

inline void StartTagOrEmptyHelper(EventList *pList, char **workingp, char *endp) {
    //Resolve name info,type will get known in the end.the first event
    //shall insert in the end.
    assert(**workingp == '<');
    char *content = *workingp;
    struct EventNode* tailposforname =  pList->Tail;
    short namestart = 1;
    ++*workingp;
    eatXMLName(workingp);
    short nameend = (*workingp - content);
    eatXMLSpaces(workingp);

    while(**workingp != '/' && **workingp != '>') {
        addEventNode(pList, eatandgenXMLAttr(workingp));
        eatXMLSpaces(workingp);
    }

    //deal with nameNode
    EventNode *nameNode = InitEventNode();
    if(**workingp == '/'){
        nameNode->type = EmptyElement;
        nameNode->EventInfo.EMPTY.NameStart = namestart;
        nameNode->EventInfo.EMPTY.NameEnd = nameend;
        ++*workingp;
        if(**workingp!='>'){
            printf("Unexpected %c after reading / of emptytag",**workingp);
            exit(1);
        }else{
            ++*workingp;
        }
    }else{  //'>' case
        nameNode->type = StartofElement;
        nameNode->EventInfo.SE.NameStart = namestart;
        nameNode->EventInfo.SE.NameEnd = nameend;
        ++*workingp;
    }
    assert(*workingp < endp);
    nameNode->next = tailposforname->next;
    tailposforname->next = nameNode;
}
*/
void eatXMLSpaces(char **workingp) {
    while(isXMLSpace(**workingp)){
        ++*workingp;
    }
}

void eatXMLName(char **workingp) {
    if (!isXMLNameStartChar(**workingp)) {
        printf("eatXMLName:Illegal XMLNameStartChar readding %c", **workingp);
    } else {
        ++*workingp;
        while (isXMLNameChar(**workingp)) {
            ++*workingp;
        }
    }
}

inline void eatXMLEq(char **workingp){
    eatXMLSpaces(workingp);
    if(**workingp!='='){
        printf("eatXMLEq:Didn't find = after spaces ,reading %c", **workingp);
    }
    ++*workingp;
    eatXMLSpaces(workingp);
}

inline EventNode *eatandgenXMLAttr(char **workingp) {
    EventNode *ret = InitEventNode();
    ret->type = Attribute;
    ret->startpos = *workingp;
    ret->EventInfo.ATTR.NameStart = 0;
    eatXMLName(workingp);
    ret->EventInfo.ATTR.NameEnd = (*workingp - ret->startpos) - 1;
    eatXMLEq(workingp);
    ret->EventInfo.ATTR.AttrStart = (*workingp - ret->startpos) - 1;
    eatXMLAttrValue(workingp);
    ret->EventInfo.ATTR.AttrEnd = (*workingp - ret->startpos) - 1;
}

/*
//still buggy
inline void eatXMLAttrValue(char **workingp) {
    if(**workingp=='"'){
        while(*(++*workingp)!='"');
        ++*workingp;
    }else if(**workingp=='\''){
        while(*(++*workingp)!='\'');
        ++*workingp;
    }else{
        printf("eatXMLAttrValue:Unexpected AttrValue start,expecting \" or \'");
        exit(1);
    }
}
*/
/*
 * Collect one Event from p,
 */

EventList *InitEventList() {
    EventList *ret = (EventList *) malloc(sizeof(*ret));
    ret->Head = InitEventNode();
    ret->Tail = ret->Head;
    return ret;
}

inline EventNode *InitEventNode() {
    EventNode *ret = (EventNode *) malloc(sizeof(*ret));
    ret->next = NULL;
    ret->startpos = NULL;
    return ret;
}

inline void addEventNode(EventList *el, EventNode *en) {
    el->Tail->next = en;
    el->Tail = en;
}


//[14]   NameStartChar
inline int isXMLNameStartChar(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c == ':') || (c == '_');
}


//[15] NameChar
inline int isXMLNameChar(char c) {
    return isXMLNameStartChar(c) ||
           (c == '-') ||
           (c == '.') ||
           (c >= '0' && c <= '9');
}

//[24]Char in XML-SPEC
inline int isXMLChar(char c) {
    return (c >= 0x20 && c <= 0x7f) ||
           (c == 0x9) ||
           (c == 0xA) ||
           (c == 0xD);
}

//[23]S in XML-SPEC
inline int isXMLSpace(char c) {
    return (c == 0x20) ||
           (c == 0x9) ||
           (c == 0xd) ||
           (c == 0xc) || (c == '\n');
}
