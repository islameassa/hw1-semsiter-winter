#ifndef MEMBER_H_
#define MEMBER_H_

#include <stdbool.h>

typedef struct Member_t *Member;

Member memberCreate(int id, char *name);

void memberDestroy(Member member);

Member memberCopy(Member member);

char *memberGetName(Member member);

int memberGetId(Member member);

int memberCompare(Member member1, Member member2);

#endif /* MEMBER_H_ */