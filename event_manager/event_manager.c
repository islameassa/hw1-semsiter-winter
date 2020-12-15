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

static Event getEventByNameAndDate(PriorityQueue pq, char *event_name, Date date)
{
    PQ_FOREACH(Event, iterator, pq)
    {
        if (strcmp(eventGetName(iterator), event_name) == 0)
        {
            if (dateCompare(date, eventGetDate(iterator)) == 0)
            {
                return iterator;
            }
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

    Event tmp = getEventByNameAndDate(em->events, event_name, date);
    if (tmp != NULL)
    {
        return EM_EVENT_ALREADY_EXISTS;
    }

    tmp = getEventById(em->events, event_id);
    if (tmp != NULL)
    {
        return EM_EVENT_ID_ALREADY_EXISTS;
    }

    Event new_event = eventCreate(event_id, event_name, date);
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
    EventManagerResult result = emAddEventByDate(em, event_name, new_date, event_id);
    dateDestroy(new_date);
    return result;
}


// we didnt use EM_OUT_OF_MEMORY
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
        EVENT_FOREACH(iterator, tmp)
        {
            PQ_FOREACH(Member, iterator2, em->members)
            {
                if (memberCompare(iterator, iterator2) == 0)
                {
                    memberChangeEventNumber(iterator2, memberGetEventNumber(iterator2) - 1);
                    int tmp1 = memberGetEventNumber(iterator2) + 1;
                    int *tmp_pointer = &tmp1;
                    int tmp2 = memberGetEventNumber(iterator2);
                    int *tmp2_pointer = &tmp2;
                    pqChangePriority(em->members, iterator2, tmp_pointer, tmp2_pointer);
                }
            }
        }
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
    if (tmp == NULL)
    {
        return EM_EVENT_ID_NOT_EXISTS;
    }

    Event tmp2 = getEventByNameAndDate(em->events, eventGetName(tmp), new_date);
    if (tmp2 != NULL)
    {
        return EM_EVENT_ALREADY_EXISTS;
    }

    if (pqChangePriority(em->events, tmp, eventGetDate(tmp), new_date) == PQ_OUT_OF_MEMORY ||
        eventChangeDate(tmp, new_date) == EVENT_OUT_OF_MEMORY)
    {
        return EM_OUT_OF_MEMORY;
    }

    return EM_SUCCESS;
}

EventManagerResult emAddMember(EventManager em, char *member_name, int member_id)
{
    if (em == NULL || member_name == NULL)
    {
        return EM_NULL_ARGUMENT;
    }

    if (member_id < 0)
    {
        return EM_INVALID_MEMBER_ID;
    }

    Member tmp = getMemberById(em->members, member_id);
    if (tmp != NULL)
    {
        return EM_MEMBER_ID_ALREADY_EXISTS;
    }

    Member new_member = memberCreate(member_id, member_name);
    if (new_member == NULL)
    {
        return EM_OUT_OF_MEMORY;
    }

    int events = 0;
    int *events_pointer = events;
    if (pqInsert(em->members, new_member, events_pointer) == PQ_OUT_OF_MEMORY)
    {
        return EM_OUT_OF_MEMORY;
    }

    return EM_SUCCESS;
}

EventManagerResult emAddMemberToEvent(EventManager em, int member_id, int event_id)
{
    if(em == NULL)
    {
        return EM_NULL_ARGUMENT;
    }

    if(event_id < 0)
    {
        return EM_INVALID_EVENT_ID;
    }

    if(member_id < 0)
    {
        return EM_INVALID_MEMBER_ID;
    }

    Event ev_tmp = getEventById(em->events,event_id);
    if(ev_tmp == NULL){
        return EM_EVENT_ID_NOT_EXISTS;        
    }
    Member member_tmp = getMemberById(em->members,member_id);
    if(member_tmp == NULL){
        return EM_MEMBER_ID_NOT_EXISTS;
    }
    EVENT_FOREACH(iterator, ev_tmp){
        if(memberGetId(iterator) == member_id){
            return EM_EVENT_AND_MEMBER_ALREADY_LINKED;
        }
    }
    // they order to destroy everything and to finish program
    if(eventAddMember(ev_tmp,member_tmp) == EVENT_OUT_OF_MEMORY){
        return EM_OUT_OF_MEMORY;
    }
    return EM_SUCCESS;
}

EventManagerResult emRemoveMemberFromEvent (EventManager em, int member_id, int event_id){
    if(em == NULL)
    {
        return EM_NULL_ARGUMENT;
    }

    if(event_id < 0)
    {
        return EM_INVALID_EVENT_ID;
    }

    if(member_id < 0)
    {
        return EM_INVALID_MEMBER_ID;
    }

    Event ev_tmp = getEventById(em->events,event_id);
    if(ev_tmp == NULL){
        return EM_EVENT_ID_NOT_EXISTS;        
    }
    Member member_tmp = getMemberById(em->members,member_id);
    if(member_tmp == NULL){
        return EM_MEMBER_ID_NOT_EXISTS;
    }
    if(eventRemoveMember(ev_tmp,member_tmp) == EVENT_MEMBER_DOES_NOT_EXIST){
        return EM_EVENT_AND_MEMBER_NOT_LINKED;
    }

            PQ_FOREACH(Member, iterator2, em->members)
            {
                if (memberCompare(member_tmp, iterator2) == 0)
                {
                    memberChangeEventNumber(iterator2, memberGetEventNumber(iterator2) - 1);
                    int tmp1 = memberGetEventNumber(iterator2) + 1;
                    int *tmp_pointer = &tmp1;
                    int tmp2 = memberGetEventNumber(iterator2);
                    int *tmp2_pointer = &tmp2;     
                    if(pqChangePriority(em->members, iterator2, tmp_pointer, tmp2_pointer) == PQ_OUT_OF_MEMORY){
                        return EM_OUT_OF_MEMORY;
                    }
                    break;
                }
            }

    return EM_SUCCESS;
}

// we didnt use EM_OUT_OF_MEMORY
EventManagerResult emTick(EventManager em, int days){
    if(em == NULL){
        return EM_NULL_ARGUMENT;
    }
    if(days <= 0){
        return EM_INVALID_DATE;
    }

    for(int i = 0; i < days ; i++){
        Event ev_tmp  = (Event)pqGetFirst(em->events);
        
        while(dateCompare(eventGetDate(ev_tmp) , em->date) == 0){
            emRemoveEvent(em,eventGetId(ev_tmp));
            ev_tmp = (Event)pqGetNext(em->events);
            if(ev_tmp == NULL){
                break;
            }
        }
        dateTick(em->date);
    }
    return EM_SUCCESS;
}

int emGetEventsAmount(EventManager em){
    if(em == NULL){
        return -1;
    }    
    return pqGetSize(em->events);
}

char* emGetNextEvent(EventManager em){
    if(em == NULL){
        return NULL;
    }
    Event tmp = (Event)pqGetFirst(em->events);
    return eventGetName(tmp);
}