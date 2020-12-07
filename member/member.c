
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "member.h"

struct Member_t
{
    int id;
    char *name;
};

Member createMember(int id, char *new_name){
    assert(new_name != NULL);
    Member member = malloc(sizeof(*member));
    if (member == NULL)
    {
        return NULL;
    }
    member->id = id;
    member->name = new_name;
    
    return member;
}

void memberDestroy(Member member){
    free(member);
}

Member memberCopy(Member member){
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

char *memberGetName(Member member){
    if (member == NULL){
        return NULL;
    }
    return member->name;
}

bool memberCompare(Member member1, Member member2){
 
    if (member1 == NULL || member2 == NULL || member1->id != member2->id ||
        strcmp(member1->name, member2->name) != 0)
    {
        return false;
    }
    return true; 
}