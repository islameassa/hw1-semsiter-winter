#ifndef MEMBER_H_
#define MEMBER_H_

#include <stdbool.h>
#include <stdio.h>

typedef struct Member_t *Member;

Member memberCreate(int id, char *name);

void memberDestroy(Member member);

Member memberCopy(Member member);

char *memberGetName(Member member);

int memberGetId(Member member);

int memberGetEventNumber(Member member);

void memberChangeEventNumber(Member member, int new_event_number);

bool memberCompare(Member member1, Member member2);

void memberPrint(Member member, FILE *file);

#endif /* MEMBER_H_ */