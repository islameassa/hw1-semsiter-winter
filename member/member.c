
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

int memberCompare(Member member1, Member member2)
{

    if (member1 == NULL || member2 == NULL)
    {
        return MEMBER_NULL_ARGUMENT;
    }
    if(member1->id == member2->id)
    {
        return MEMBERS_EQUAL;
    }
    return MEMBERS_NOT_EQUAL;
}