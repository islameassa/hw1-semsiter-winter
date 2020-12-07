#ifndef MEMBER_H
#define MEMBER_H

#include <stdbool.h>

typedef struct Member_t *Member;

/*typedef enum MemberResult_t
{
    EVENT_SUCCESS,
    EVENT_OUT_OF_MEMORY,
    EVENT_NULL_ARGUMENT,
    EVENT_INVALID_ID,
    EVENT_MEMBER_ALREADY_EXISTS,
    EVENT_MEMBER_DOES_NOT_EXIST,
    EM_ERROR
} MemberResult;

*/

Member memberCreate(int id, char *name);

void memberDestroy(Member member);

Member memberCopy(Member member);

char *memberGetName(Member member);

bool memberCompare(Member member1, Member member2);

/*#define EVENT_FOREACH(iterator, event)           \
    for (Member iterator = eventGetFirst(event); \
         iterator;                               \
         iterator = eventGetNext(event))
*/
#endif