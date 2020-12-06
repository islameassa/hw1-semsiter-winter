#ifndef EVENT_H_
#define EVENT_H_

#include <stdbool.h>
#include "member.h"

typedef struct Event_t *Event;

typedef enum EventResult_t
{
    EVENT_SUCCESS,
    EVENT_OUT_OF_MEMORY,
    EVENT_NULL_ARGUMENT,
    EVENT_INVALID_ID,
    EVENT_MEMBER_ALREADY_EXISTS,
    EVENT_MEMBER_DOES_NOT_EXIST,
    EM_ERROR
} EventResult;

Event eventCreate(int id, char *name);

void eventDestroy(Event event);

Event eventCopy(Event event);

char *eventGetName(Event event);

bool eventEquals(Event event1, Event event2);

EventResult eventAddMember(Event event, Member member);

EventResult eventRemoveMember(Event event, Member member);

Member eventGetFirst(Event event);

Member eventGetNext(Event event);

#define EVENT_FOREACH(iterator, event)           \
    for (Member iterator = eventGetFirst(event); \
         iterator;                               \
         iterator = eventGetNext(event))

#endif