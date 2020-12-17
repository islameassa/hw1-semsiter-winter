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

static int compareMemberPriorities(PQElementPriority member1, PQElementPriority member2)
{
    return ((memberGetEventNumber((Member)member1) - memberGetEventNumber((Member)member2)) != 0)
               ? (memberGetEventNumber((Member)member1) - memberGetEventNumber((Member)member2))
               : (memberGetId((Member)member2) - memberGetId((Member)member1));
}

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

static PQElement copyEventGeneric(PQElement event)
{
    Event event_copy = eventCopy((Event)event);
    return event_copy;
}

static void freeEventGeneric(PQElement event)
{
    eventDestroy((Event)event);
}

static bool compareEventsGeneric(PQElement event1, PQElement event2)
{
    return eventEquals((Event)event1, (Event)event2);
}

static PQElementPriority copyDateGeneric(PQElementPriority date)
{
    Date date_copy = dateCopy((Date)date);
    return date_copy;
}

static void freeDateGeneric(PQElementPriority date)
{
    dateDestroy((Date)date);
}

static int compareDatesGeneric(PQElementPriority date1, PQElementPriority date2)
{
    return (0 - dateCompare((Date)date1, (Date)date2));
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

    em->events = pqCreate(copyEventGeneric, freeEventGeneric, compareEventsGeneric, copyDateGeneric, freeDateGeneric, compareDatesGeneric);
    if (em->events == NULL)
    {
        free(em);
        return NULL;
    }

    em->members = pqCreate(copyMemberGeneric, freeMemberGeneric, compareMembersGeneric, copyMemberGeneric, freeMemberGeneric, compareMemberPriorities);
    if (em->members == NULL)
    {
        pqDestroy(em->events);
        free(em);
        return NULL;
    }

    Date new_date = dateCopy(date);
    if (new_date == NULL)
    {
        pqDestroy(em->events);
        pqDestroy(em->members);
        free(em);
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
    if (em == NULL || event_name == NULL || date == NULL)
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

    if (days < 0)
    {
        return EM_INVALID_DATE;
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
    if (tmp == NULL)
    {
        return EM_EVENT_NOT_EXISTS;
    }

    Member tmp_member2 = NULL;
    EVENT_FOREACH(iterator, tmp)
    {
        Member tmp_member = getMemberById(em->members, memberGetId(iterator));
        if (tmp_member == NULL)
        {
            return EM_ERROR;
        }

        tmp_member2 = memberCopy(tmp_member);
        if (tmp_member2 == NULL)
        {
            return EM_OUT_OF_MEMORY;
        }

        memberChangeEventNumber(tmp_member2, memberGetEventNumber(tmp_member2) - 1);
        pqRemoveElement(em->members, tmp_member);
        if (pqInsert(em->members, tmp_member2, tmp_member2) == PQ_OUT_OF_MEMORY)
        {
            memberDestroy(tmp_member2);
            return EM_OUT_OF_MEMORY;
        }
    }
    pqRemoveElement(em->events, tmp);

    memberDestroy(tmp_member2);
    return EM_SUCCESS;
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

    if (pqInsert(em->members, new_member, new_member) == PQ_OUT_OF_MEMORY)
    {
        memberDestroy(new_member);
        return EM_OUT_OF_MEMORY;
    }

    return EM_SUCCESS;
}

EventManagerResult emAddMemberToEvent(EventManager em, int member_id, int event_id)
{
    if (em == NULL)
    {
        return EM_NULL_ARGUMENT;
    }

    if (event_id < 0)
    {
        return EM_INVALID_EVENT_ID;
    }

    if (member_id < 0)
    {
        return EM_INVALID_MEMBER_ID;
    }

    Event ev_tmp = getEventById(em->events, event_id);
    if (ev_tmp == NULL)
    {
        return EM_EVENT_ID_NOT_EXISTS;
    }

    Member member_tmp = getMemberById(em->members, member_id);
    if (member_tmp == NULL)
    {
        return EM_MEMBER_ID_NOT_EXISTS;
    }

    EVENT_FOREACH(iterator, ev_tmp)
    {
        if (memberGetId(iterator) == member_id)
        {
            return EM_EVENT_AND_MEMBER_ALREADY_LINKED;
        }
    }

    if (eventAddMember(ev_tmp, member_tmp) == EVENT_OUT_OF_MEMORY)
    {
        return EM_OUT_OF_MEMORY;
    }

    memberChangeEventNumber(member_tmp, memberGetEventNumber(member_tmp) + 1);
    return EM_SUCCESS;
}

EventManagerResult emRemoveMemberFromEvent(EventManager em, int member_id, int event_id)
{
    if (em == NULL)
    {
        return EM_NULL_ARGUMENT;
    }

    if (event_id < 0)
    {
        return EM_INVALID_EVENT_ID;
    }

    if (member_id < 0)
    {
        return EM_INVALID_MEMBER_ID;
    }

    Event ev_tmp = getEventById(em->events, event_id);
    if (ev_tmp == NULL)
    {
        return EM_EVENT_ID_NOT_EXISTS;
    }

    Member member_tmp = getMemberById(em->members, member_id);
    if (member_tmp == NULL)
    {
        return EM_MEMBER_ID_NOT_EXISTS;
    }

    if (eventRemoveMember(ev_tmp, member_tmp) == EVENT_MEMBER_DOES_NOT_EXIST)
    {
        return EM_EVENT_AND_MEMBER_NOT_LINKED;
    }

    Member tmp_member2 = memberCopy(member_tmp);
    if (tmp_member2 == NULL)
    {
        return EM_OUT_OF_MEMORY;
    }

    memberChangeEventNumber(tmp_member2, memberGetEventNumber(tmp_member2) - 1);
    pqRemoveElement(em->members, member_tmp);
    if (pqInsert(em->members, tmp_member2, tmp_member2) == PQ_OUT_OF_MEMORY)
    {
        memberDestroy(tmp_member2);
        return EM_OUT_OF_MEMORY;
    }
    memberDestroy(tmp_member2);

    return EM_SUCCESS;
}

EventManagerResult emTick(EventManager em, int days)
{
    if (em == NULL)
    {
        return EM_NULL_ARGUMENT;
    }
    if (days <= 0)
    {
        return EM_INVALID_DATE;
    }

    for (int i = 0; i < days; i++)
    {
        dateTick(em->date);
    }

    PQ_FOREACH(Event, iterator, em->events)
    {
        if (dateCompare(eventGetDate(iterator), em->date) >= 0)
        {
            break;
        }
        emRemoveEvent(em, eventGetId(iterator));
    }

    return EM_SUCCESS;
}

int emGetEventsAmount(EventManager em)
{
    if (em == NULL)
    {
        return -1;
    }
    return pqGetSize(em->events);
}

char *emGetNextEvent(EventManager em)
{
    if (em == NULL)
    {
        return NULL;
    }
    Event tmp = (Event)pqGetFirst(em->events);
    return eventGetName(tmp);
}

void emPrintAllEvents(EventManager em, const char *file_name)
{
    FILE *file = fopen(file_name, "w");
    if (file == NULL)
    {
        return;
    }

    PQ_FOREACH(Event, iterator, em->events)
    {
        eventPrint(iterator, file);
        fprintf(file, "\n");
    }
    fclose(file);
}

void emPrintAllResponsibleMembers(EventManager em, const char *file_name)
{
    FILE *file = fopen(file_name, "w");
    if (file == NULL)
    {
        return;
    }

    PQ_FOREACH(Member, iterator, em->members)
    {
        memberPrint(iterator, file);
        fprintf(file, "\n");
    }
    fclose(file);
}