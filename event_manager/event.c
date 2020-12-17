#include "event.h"
#include "priority_queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

struct Event_t
{
    int id;
    char *name;
    Date date;
    PriorityQueue members;
};

static PQElement copyMemberGeneric(PQElement member)
{
    Member member_copy = memberCopy((Member)member);
    return member_copy;
}

static void freeMemberGeneric(PQElement member)
{
    memberDestroy((Member)member);
}

static bool compareMembersGeneric(PQElement member1, PQElement member2)
{
    return memberCompare((Member)member1, (Member)member2);
}

static PQElementPriority copyIntGeneric(PQElementPriority n)
{
    if (!n)
    {
        return NULL;
    }
    int *copy = malloc(sizeof(*copy));
    if (!copy)
    {
        return NULL;
    }
    *copy = *(int *)n;
    return copy;
}

static void freeIntGeneric(PQElementPriority n)
{
    free(n);
}

static int compareIntsGeneric(PQElementPriority n1, PQElementPriority n2)
{
    return (*(int *)n2 - *(int *)n1);
}

static char *copyString(char *string)
{
    char *new_string = malloc(strlen(string) + 1);
    if (new_string == NULL)
    {
        return NULL;
    }
    strcpy(new_string, string);
    return new_string;
}

Event eventCreate(int id, char *name, Date date)
{
    if (name == NULL || date == NULL)
    {
        return NULL;
    }

    Event event = malloc(sizeof(*event));
    if (event == NULL)
    {
        return NULL;
    }

    event->members = pqCreate(copyMemberGeneric, freeMemberGeneric, compareMembersGeneric,
                              copyIntGeneric, freeIntGeneric, compareIntsGeneric);
    if (event->members == NULL)
    {
        return NULL;
    }

    char *new_name = copyString(name);
    if (new_name == NULL)
    {
        pqDestroy(event->members);
        free(event);
        return NULL;
    }

    Date new_date = dateCopy(date);
    if (new_date == NULL)
    {
        pqDestroy(event->members);
        free(new_name);
        free(event);
        return NULL;
    }

    event->id = id;
    event->name = new_name;
    event->date = new_date;

    return event;
}

void eventDestroy(Event event)
{
    if (event == NULL)
    {
        return;
    }

    dateDestroy(event->date);
    pqDestroy(event->members);
    free(event->name);
    free(event);
}

Event eventCopy(Event event)
{
    if (event == NULL)
    {
        return NULL;
    }

    Event new_event = eventCreate(event->id, event->name, event->date);

    if (new_event == NULL)
    {
        return NULL;
    }

    PriorityQueue new_pq = pqCopy(event->members);
    if (new_pq == NULL)
    {
        return NULL;
    }
    new_event->members = new_pq;

    return new_event;
}

char *eventGetName(Event event)
{
    if (event == NULL)
    {
        return NULL;
    }
    return event->name;
}

int eventGetId(Event event)
{
    if (event == NULL)
    {
        return -1;
    }
    return event->id;
}

Date eventGetDate(Event event)
{
    if (event == NULL)
    {
        return NULL;
    }
    return event->date;
}

bool eventEquals(Event event1, Event event2)
{
    if (event1 == NULL || event2 == NULL || event1->id != event2->id)
    {
        return false;
    }

    return true;
}

EventResult eventAddMember(Event event, Member member)
{
    if (event == NULL || member == NULL)
    {
        return EVENT_NULL_ARGUMENT;
    }

    int member_id = memberGetId(member);
    if (pqInsert(event->members, member, &member_id) == PQ_OUT_OF_MEMORY)
    {
        return EVENT_OUT_OF_MEMORY;
    }

    return EVENT_SUCCESS;
}

EventResult eventRemoveMember(Event event, Member member)
{
    if (event == NULL || member == NULL)
    {
        return EVENT_NULL_ARGUMENT;
    }

    if (pqRemoveElement(event->members, member) == PQ_ELEMENT_DOES_NOT_EXISTS)
    {
        return EVENT_MEMBER_DOES_NOT_EXIST;
    }

    return EVENT_SUCCESS;
}

EventResult eventChangeDate(Event event, Date date)
{
    if (event == NULL || date == NULL)
    {
        return EVENT_NULL_ARGUMENT;
    }

    Date new_date = dateCopy(date);
    if (new_date == NULL)
    {
        return EVENT_OUT_OF_MEMORY;
    }

    event->date = new_date;
    return EVENT_SUCCESS;
}

void eventPrint(Event event, FILE *file)
{
    int day, month, year;
    dateGet(event->date, &day, &month, &year);
    fprintf(file, "%s,%d.%d.%d", event->name, day, month, year);
    PQ_FOREACH(Member, iterator, event->members)
    {
        fprintf(file, "%s", memberGetName(iterator));
    }
}

Member eventGetFirst(Event event)
{
    if (event == NULL)
    {
        return NULL;
    }

    return pqGetFirst(event->members);
}

Member eventGetNext(Event event)
{
    if (event == NULL)
    {
        return NULL;
    }

    return pqGetNext(event->members);
}