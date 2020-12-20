#ifndef MEMBER_H_
#define MEMBER_H_

#include <stdbool.h>
#include <stdio.h>

/**
* Member
*
* Implements a member type.
*
* The following functions are available:
*   memberCreate                - Allocates a new member.
*   memberDestroy               - Deallocates an existing member.
*   memberCopy                  - Creates a copy of target member.
*   memberGetName               - Returns the name of the member.
*   memberGetId                 - Returns the ID of the member.
*   memberGetEventNumber        - Returns the number of events that the member has.
*   memberChangeEventNumber     - Changes the number of events that the member has.
*   memberCompare               - Checks if two members are equals.
*   memberPrint                 - Prints the details of the member.
*/

/** Type for defining the member */
typedef struct Member_t *Member;

/**
* memberCreate: Allocates a new member.
*
* @param id - The ID number of the member.
* @param name - The name of the member.
*
* @return
* 	NULL - if one of the parameters is NULL or allocations failed.
* 	A new member in case of success.
*/
Member memberCreate(int id, char *name);

/**
* memberDestroy: Deallocates an existing event.
*
* @param member - Target member to be deallocated. If member is NULL nothing will be done.
*/
void memberDestroy(Member member);

/**
* memberCopy: Creates a copy of target member.
*
* @param member - Target member.
* @return
* 	NULL if a NULL was sent or a memory allocation failed.
* 	A new member containing the same elements as member otherwise.
*/
Member memberCopy(Member member);

/**
* memberGetName: Returns the name of the member.
* @param member - The member which name is requested.
* @return
* 	NULL if a NULL pointer was sent.
* 	Otherwise the name of the member.
*/
char *memberGetName(Member member);

/**
* memberGetId: Returns the ID of the member.
* @param member - The member which ID is requested.
* @return
* 	-1 if a NULL pointer was sent.
* 	Otherwise the ID of the member.
*/
int memberGetId(Member member);

/**
* memberGetEventNumber: Returns the number of events that the member has.
* @param member - The member which event number is requested.
* @return
* 	-1 if a NULL pointer was sent.
* 	Otherwise the number of events that the member has.
*/
int memberGetEventNumber(Member member);

/**
* memberChangeEventNumber: Changes the number of events that the member has.
* @param member - The member which event number should be changed.
* @param new_event_number - The new event number for the member.
*   if NULL is sent nothing happens.
*/
void memberChangeEventNumber(Member member, int new_event_number);

/**
* memberCompare: Checks if two members are equals.
*
* @param member1 - The first member to compare.
* @param member2 - The second member to compare.
*
* @return
* 	false if a NULL was sent as one of the parameters or if the members are not equal.
* 	true if the members are equal.
*/
bool memberCompare(Member member1, Member member2);

/**
* memberPrint: Prints the details of the member.
*
* @param member - The member which details should be printed.
* @param file - pointer to the output file.
*/
void memberPrint(Member member, FILE *file);

#endif /* MEMBER_H_ */