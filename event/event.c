#include "event.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define INITIAL_SIZE 10
#define EXPAND_FACTOR 2
#define MEMBER_NOT_FOUND -1

struct Event_t
{
    int id;
    char *name;
    Member *members;
    int members_size;
    int members_max_size;
    int iterator;
};

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

static EventResult eventRemoveMemberByIndex(Event event, int index)
{
    assert(event != NULL && index >= 0);

    memberDestroy(event->members[index]);

    for (int i = index; i < event->members_size - 1; i++)
    {
        event->members[i] = event->members[i + 1];
    }

    event->members_size --;
    event->iterator = -1;

    return EVENT_SUCCESS;
}

static EventResult addOrDestroy(Event event, Member member)
{
    EventResult result = eventAddMember(event, member);
    if (result == EVENT_OUT_OF_MEMORY)
    {
        eventDestroy(event);
    }
    return result;
}

static EventResult addAllOrDestroy(Event event, Event event_toAdd)
{
    for (int i = 0; i < event_toAdd->members_size; ++i)
    {
        if (addOrDestroy(event, event_toAdd->members[i]) == EVENT_OUT_OF_MEMORY)
        {
            return EVENT_OUT_OF_MEMORY;
        }
    }
    return EVENT_SUCCESS;
}

static EventResult expand(Event event)
{
    assert(event != NULL);
    int new_size = EXPAND_FACTOR * event->members_max_size;
    Member *new_members = realloc(event->members, new_size * sizeof(Member));
    if (new_members == NULL)
    {
        return EVENT_OUT_OF_MEMORY;
    }
    event->members = new_members;
    event->members_max_size = new_size;
    return EVENT_SUCCESS;
}

static int find(Event event, Member member)
{
    assert(event != NULL && member != NULL);
    for (int i = 0; i < event->members_size; i++)
    {
        if (memberCompare(event->members[i], member) == 0)
        {
            return i;
        }
    }
    return MEMBER_NOT_FOUND;
}

Event eventCreate(int id, char *name)
{
    if(name == NULL)
    {
        return NULL;
    }

    Event event = malloc(sizeof(*event));
    if (event == NULL)
    {
        return NULL;
    }

    event->members = malloc(INITIAL_SIZE * sizeof(Member));
    if (event->members == NULL)
    {
        free(event);
        return NULL;
    }

    char *new_name = copyString(name);
    if (new_name == NULL)
    {
        free(event->members);
        free(event);
        return NULL;
    }

    event->id = id;
    event->name = new_name;
    event->members_size = 0;
    event->members_max_size = INITIAL_SIZE;
    event->iterator = -1;

    return event;
}

void eventDestroy(Event event)
{
    if (event == NULL)
    {
        return;
    }
    for (int i = 0; i < event->members_size; i++)
    {
        eventRemoveMemberByIndex(event, 0);
        event->members_size++;
    }

    free(event->members);
    free(event->name);
    free(event);
}

Event eventCopy(Event event)
{
    if (event == NULL)
    {
        return NULL;
    }

    Event new_event = eventCreate(event->id, event->name);

    event->iterator = -1;
    if (new_event == NULL)
    {
        return NULL;
    }
    if (addAllOrDestroy(new_event, event) == EVENT_OUT_OF_MEMORY)
    {
        return NULL;
    }
    new_event->iterator = -1;
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

bool eventEquals(Event event1, Event event2)
{
    if (event1 == NULL || event2 == NULL || event1->id != event2->id ||
        strcmp(event1->name, event2->name) != 0 || event1->members_size != event2->members_size)
    {
        return false;
    }
    for (int i = 0; i < event1->members_size; i++)
    {
        if (memberCompare(event1->members[i], event2->members[i]))
        {
            return false;
        }
    }
    return true;
}

EventResult eventAddMember(Event event, Member member)
{
    if (event == NULL || member == NULL)
    {
        if (event != NULL)
        {
            event->iterator = -1;
        }
        return EVENT_NULL_ARGUMENT;
    }
    event->iterator = -1;
    if (event->members_size == event->members_max_size && expand(event) == EVENT_OUT_OF_MEMORY)
    {
        return EVENT_OUT_OF_MEMORY;
    }

    Member new_member = memberCopy(member);
    if (new_member == NULL)
    {
        return EVENT_OUT_OF_MEMORY;
    }

    event->members[event->members_size++] = new_member;
    return EVENT_SUCCESS;
}

EventResult eventRemoveMember(Event event, Member member)
{
    if (event == NULL || member == NULL)
    {
        if (event != NULL)
        {
            event->iterator = -1;
        }
        return EVENT_NULL_ARGUMENT;
    }
    event->iterator = -1;
    int index = find(event, member);
    if(index == MEMBER_NOT_FOUND)
    {
        return EVENT_MEMBER_DOES_NOT_EXIST;
    }
    return eventRemoveMemberByIndex(event, index);
}

Member eventGetFirst(Event event)
{
    if (event == NULL)
    {
        return NULL;
    }
    event->iterator = 0;
    return eventGetNext(event);
}

Member eventGetNext(Event event)
{
    if (event == NULL)
    {
        return NULL;
    }
    if (event->iterator >= event->members_size || event->iterator < 0)
    {
        return NULL;
    }
    return event->members[event->iterator++];
}