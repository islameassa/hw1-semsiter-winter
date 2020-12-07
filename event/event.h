#ifndef EVENT_H_
#define EVENT_H_

#include <stdbool.h>
#include "member.h"

/**
* Event
*
* Implements an event type.
* The event has an internal iterator for iterating over it's members. For all functions
* where the state of the iterator after calling that function is not stated,
* it is undefined. That means that you cannot assume anything about it.
*
* The following functions are available:
*   eventCreate         - Allocates a new event.
*   eventDestroy        - Deallocates an existing event.
*   eventCopy           - Creates a copy of target event.
*                           Iterator values for both events are undefined after this operation.
*   eventGetName        - Returns the name of the event
*   eventEquals         - Checks if two events are equals.
*   eventAddMember      - add a member for the event.
*                           Iterator's value is undefined after this operation.
*   eventRemoveMember   - Removes the member from the event
*                           Iterator's value is undefined after this operation.
*   eventGetFirst       - Sets the internal iterator to the first member in the event.
*   eventGetNext        - Advances the event iterator to the next member and returns it.
* 	PQ_FOREACH	        - A macro for iterating over the event's members.
*/

/** Type for defining the event */
typedef struct Event_t *Event;

/** Type used for returning error codes from event functions */
typedef enum EventResult_t
{
    EVENT_SUCCESS,
    EVENT_OUT_OF_MEMORY,
    EVENT_NULL_ARGUMENT,
    EVENT_INVALID_ID,
    EVENT_MEMBER_ALREADY_EXISTS,
    EVENT_MEMBER_DOES_NOT_EXIST,
    Event_ERROR
} EventResult;

/**
* eventCreate: Allocates a new event.
*
* @param id - The ID number of the event
* @param name - The name of the event
*
* @return
* 	NULL - if one of the parameters is NULL or allocations failed.
* 	A new event in case of success.
*/
Event eventCreate(int id, char *name);

/**
* eventDestroy: Deallocates an existing event.
*
* @param event - Target event to be deallocated. If event is NULL nothing will be
* 		done
*/
void eventDestroy(Event event);

/**
* eventCopy: Creates a copy of target event.
* Iterator values for both events are undefined after this operation.
*
* @param event - Target event.
* @return
* 	NULL if a NULL was sent or a memory allocation failed.
* 	An event containing the same elements as event otherwise.
*/
Event eventCopy(Event event);

/**
* eventGetName: Returns the name of the event
* @param event - The event which name is requested
* @return
* 	NULL if a NULL pointer was sent.
* 	Otherwise the name of the event.
*/
char *eventGetName(Event event);

/**
*   eventEquals: Checks if two events are equals.
*
* @param event1 - The first event to compare
* @param event2 - The second event to compare.
*
* @return
* 	false if a NULL was sent as one of the parameters or if the events are not equal.
* 	true if the events are equal.
*/
bool eventEquals(Event event1, Event event2);

/**
*   eventAddMember: add a member for the event.
*   Iterator's value is undefined after this operation.
*
* @param event - The event for which to add the member
* @param member - The member which need to be added.
*      A copy of the member will be inserted.
* @return
* 	EVENT_NULL_ARGUMENT if a NULL was sent as one of the parameters
* 	EVENT_OUT_OF_MEMORY if an allocation failed
* 	EVENT_SUCCESS the member had been added successfully
*/
EventResult eventAddMember(Event event, Member member);

/**
*   eventRemoveMember: Removes the member from the event
*   Iterator's value is undefined after this operation.
*
* @param event - The event to remove the member from.
* @param member - The member to find and remove from the event.
* @return
* 	EVENT_NULL_ARGUMENT if a NULL was sent to the function.
* 	EVENT_MEMBER_DOES_NOT_EXIST if given member does not exists.
* 	event_SUCCESS if the member had been removed successfully.
*/
EventResult eventRemoveMember(Event event, Member member);

/**
*	eventGetFirst: Sets the internal iterator (also called current member) to
*	the first member in the event.
*	Use this to start iterating over the event members.
*	To continue iteration use eventGetNext.
*
* @param event - The event for which to set the iterator and return the first member.
* @return
* 	NULL if a NULL pointer was sent or the event is empty.
* 	The first member of the event otherwise
*/
Member eventGetFirst(Event event);

/**
*	eventGetNext: Advances the event iterator to the next member and returns it.
*
* @param event - The event for which to advance the iterator
* @return
* 	NULL if reached the end of the event members, or the iterator is at an invalid state
* 	or a NULL sent as argument
* 	The next member on the event in case of success
*/
Member eventGetNext(Event event);

/*!
* Macro for iterating over the event members.
* Declares a new iterator for the loop.
*/
#define EVENT_FOREACH(iterator, event)           \
    for (Member iterator = eventGetFirst(event); \
         iterator;                               \
         iterator = eventGetNext(event))

#endif /* EVENT_H_ */