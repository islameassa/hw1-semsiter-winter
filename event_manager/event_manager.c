#include "event_manager.h"
#include "event.h"
#include "member.h"
#include "priority_queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

struct EventManager_t
{
    Date date;
    PriorityQueue events;
    PriorityQueue members;
};

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
    return (*(int *)n1 - *(int *)n2);
}

static Event getEventByName(PriorityQueue pq, char *event_name)
{
    PQ_FOREACH(Event, iterator, pq)
    {
        if (strcmp(eventGetName(iterator), event_name) == 0)
        {
            return iterator;
        }
    }
    return NULL;
}

static Event getEventById(PriorityQueue pq, int event_id)
{
    PQ_FOREACH(Event, iterator, pq)
    {
        if (eventGetId(iterator) == event_id)
        {
            return iterator;
        }
    }
    return NULL;
}

static Member getMemberById(PriorityQueue pq, int member_id)
{
    PQ_FOREACH(Member, iterator, pq)
    {
        if (memberGetId(iterator) == member_id)
        {
            return iterator;
        }
    }
    return NULL;
}

EventManager createEventManager(Date date)
{
    if (date == NULL)
    {
        return NULL;
    }

    EventManager em = malloc(sizeof(*em));
    if (em == NULL)
    {
        return NULL;
    }

    em->events = pqCreate(eventCopy, eventDestroy, eventEquals, dateCopy, dateDestroy, dateCompare);
    if (em->events == NULL)
    {
        free(em);
        return NULL;
    }

    em->members = pqCreate(memberCopy, memberDestroy, memberCompare, copyIntGeneric, freeIntGeneric, compareIntsGeneric);
    if (em->members == NULL)
    {
        free(em);
        free(em->events);
        return NULL;
    }

    Date new_date = dateCopy(date);
    if (new_date == NULL)
    {
        free(em);
        free(em->events);
        free(em->members);
        return NULL;
    }

    em->date = new_date;
    return em;
}

void destroyEventManager(EventManager em)
{
    if (em == NULL)
    {
        return;
    }

    pqDestroy(em->events);
    pqDestroy(em->members);
    dateDestroy(em->date);
    free(em);
}

EventManagerResult emAddEventByDate(EventManager em, char *event_name, Date date, int event_id)
{
    if (em == NULL || event_name == NULL || date == NULL || event_id == NULL)
    {
        return EM_NULL_ARGUMENT;
    }

    if (dateCompare(em->date, date) > 0)
    {
        return EM_INVALID_DATE;
    }

    if (event_id < 0)
    {
        return EM_INVALID_EVENT_ID;
    }

    // EM_EVENT_ALREADY_EXISTS?????

    Event tmp = getEventByName(em->events, event_name);
    if (tmp != NULL)
    {
        return EM_EVENT_ALREADY_EXISTS;
    }

    Event new_event = eventCreate(event_id, event_name);
    if (new_event == NULL)
    {
        return EM_OUT_OF_MEMORY;
    }

    if (pqInsert(em->events, new_event, date) == PQ_OUT_OF_MEMORY)
    {
        eventDestroy(new_event);
        return EM_OUT_OF_MEMORY;
    }

    eventDestroy(new_event);
    return EM_SUCCESS;
}

EventManagerResult emAddEventByDiff(EventManager em, char *event_name, int days, int event_id)
{
    if (em == NULL)
    {
        return EM_NULL_ARGUMENT;
    }

    Date new_date = dateCopy(em->date);
    if (new_date == NULL)
    {
        return EM_OUT_OF_MEMORY;
    }

    for (int i = 0; i < days; i++)
    {
        dateTick(new_date);
    }
    return emAddEventByDate(em, event_name, new_date, event_id);
}

EventManagerResult emRemoveEvent(EventManager em, int event_id)
{
    if (em == NULL)
    {
        return EM_NULL_ARGUMENT;
    }

    if (event_id < 0)
    {
        return EM_INVALID_EVENT_ID;
    }

    Event tmp = getEventById(em->events, event_id);
    if (tmp != NULL)
    {
        pqRemoveElement(em->events, tmp);
        return EM_SUCCESS;
    }

    return EM_EVENT_NOT_EXISTS;
}

EventManagerResult emChangeEventDate(EventManager em, int event_id, Date new_date)
{
    if (em == NULL || new_date == NULL)
    {
        return EM_NULL_ARGUMENT;
    }

    if (dateCompare(em->date, new_date) > 0)
    {
        return EM_INVALID_DATE;
    }

    if (event_id < 0)
    {
        return EM_INVALID_EVENT_ID;
    }

    Event tmp = getEventById(em->events, event_id);
    if(tmp == NULL)
    {
        return EM_EVENT_ID_NOT_EXISTS;
    }

    // EM_EVENT_ALREADY_EXISTS?????

    if(pqChangePriority(em->events, tmp, , new_date) == PQ_OUT_OF_MEMORY)
    {
        return EM_OUT_OF_MEMORY;
    }

    return EM_SUCCESS;
}

EventManagerResult emAddMember(EventManager em, char* member_name, int member_id)
{
    if(em == NULL || member_name == NULL)
    {
        return EM_NULL_ARGUMENT;
    }

    if(member_id < 0)
    {
        return EM_INVALID_MEMBER_ID;
    }

    Member tmp = getMemberById(em->members, member_id);
    if(tmp != NULL)
    {
        return EM_MEMBER_ID_ALREADY_EXISTS;
    }

    Member new_member = memberCreate(member_id, member_name);
    if(new_member == NULL)
    {
        return EM_OUT_OF_MEMORY;
    }
    
    int events = 0;
    int *events_pointer = events;
    if(pqInsert(em->members, new_member, events) == PQ_OUT_OF_MEMORY)
    {
        return EM_OUT_OF_MEMORY;
    }

    return EM_SUCCESS;
}