
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "member.h"

#define MEMBER_NULL_ARGUMENT -1
#define MEMBERS_NOT_EQUAL 1
#define MEMBERS_EQUAL 0

struct Member_t
{
    int id;
    char *name;
    int event_number;
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

Member createMember(int id, char *name)
{
    if (name == NULL)
    {
        return NULL;
    }
    Member member = malloc(sizeof(*member));
    if (member == NULL)
    {
        return NULL;
    }
    char *new_name = copyString(name);
    if(new_name == NULL)
    {
        free(member);
        return NULL;
    }
    member->id = id;
    member->name = new_name;
    member->event_number = 0;

    return member;
}

void memberDestroy(Member member)
{
    if(member == NULL)
    {
        return;
    }
    free(member->name);
    free(member);
}

Member memberCopy(Member member)
{
    if (member == NULL)
    {
        return NULL;
    }

    Member new_member = memberCreate(member->id, member->name);
    if (new_member == NULL)
    {
        return NULL;
    }
    new_member->event_number = member->event_number;

    return new_member;
}

char *memberGetName(Member member)
{
    if (member == NULL)
    {
        return NULL;
    }
    return member->name;
}

int memberGetId(Member member)
{
    if (member == NULL)
    {
        return -1;
    }
    return member->id;
}

int memberGetEventNumber(Member member)
{
    if (member == NULL)
    {
        return -1;
    }
    return member->event_number;
}

void memberChangeEventNumber(Member member, int new_event_number)
{
    if(member == NULL)
    {
        return;
    }
    member->event_number = new_event_number;
}

int memberCompare(Member member1, Member member2)
{
    assert(member1 != NULL && member2 != NULL);
    return member1->id - member2->id;
}