#include "priority_queue.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#define EXPAND_RATE 2
#define EXPAND_FACTOR 2
#define INITIAL_SIZE 10
#define ELEMENT_NOT_FOUND -1

/** Struct representing a Priority Queue implemented as an array */
struct PriorityQueue_t
{
    PQElement *elements;
    PQElementPriority *priorities;
    int size;
    int maxSize;
    int iterator;

    CopyPQElement copy_element;
    FreePQElement free_element;
    EqualPQElements equal_elements;

    CopyPQElementPriority copy_priority;
    FreePQElementPriority free_priority;
    ComparePQElementPriorities compare_priority;
};

static PriorityQueueResult pqRemoveElementByIndex(PriorityQueue queue, int index);

static PriorityQueueResult addOrDestroy(PriorityQueue pq, PQElement pq_element, PQElementPriority pq_priority);

static PriorityQueueResult addAllOrDestroy(PriorityQueue pq, PriorityQueue pq_toAdd);

static int find(PriorityQueue pq, PQElement element_target);

static int superFind(PriorityQueue pq, PQElement element_target, PQElementPriority priority_target);

static PriorityQueueResult expand(PriorityQueue queue);

static PriorityQueueResult insertToQueueByIndex(PriorityQueue queue,
                                                int index, PQElement element, PQElementPriority priority);

static PriorityQueueResult pqRemoveElementByIndex(PriorityQueue queue, int index);

PriorityQueue pqCreate(CopyPQElement copy_element, FreePQElement free_element,
                       EqualPQElements equal_elements, CopyPQElementPriority copy_priority,
                       FreePQElementPriority free_priority, ComparePQElementPriorities compare_priority)
{

    assert(copy_element != NULL && free_element != NULL && equal_elements != NULL &&
           copy_priority != NULL && free_priority != NULL && compare_priority != NULL);

    PriorityQueue pq = malloc(sizeof(*pq));

    if (pq == NULL)
    {
        return NULL;
    }

    pq->elements = malloc(INITIAL_SIZE * sizeof(PQElement));
    pq->priorities = malloc(INITIAL_SIZE * sizeof(PQElementPriority));

    if (pq->elements == NULL)
    {
        free(pq);
        return NULL;
    }
    if (pq->priorities == NULL)
    {
        free(pq);
        free(pq->elements);
        return NULL;
    }

    pq->size = 0;
    pq->iterator = 0;
    pq->maxSize = INITIAL_SIZE;

    pq->copy_element = copy_element;
    pq->free_element = free_element;
    pq->equal_elements = equal_elements;

    pq->copy_priority = copy_priority;
    pq->free_priority = free_priority;
    pq->compare_priority = compare_priority;

    return pq;
}

void pqDestroy(PriorityQueue queue)
{
    if (queue == NULL)
    {
        return;
    }
    for (int i = 0; i < queue->size; i++)
    {
        pqRemoveElementByIndex(queue, 0);
    }

    free(queue->elements);
    free(queue->priorities);
    free(queue);
}

static PriorityQueueResult addOrDestroy(PriorityQueue pq, PQElement pq_element, PQElementPriority pq_priority)
{
    PriorityQueueResult result = pqInsert(pq, pq_element, pq_priority);
    if (result == PQ_OUT_OF_MEMORY)
    {
        pqDestroy(pq);
    }
    return result;
}

static PriorityQueueResult addAllOrDestroy(PriorityQueue pq, PriorityQueue pq_toAdd)
{
    for (int i = 0; i < pq_toAdd->size; ++i)
    {
        if (addOrDestroy(pq, pq_toAdd->elements[i], pq_toAdd->priorities[i]) == PQ_OUT_OF_MEMORY)
        {
            return PQ_OUT_OF_MEMORY;
        }
    }
    return PQ_SUCCESS;
}

PriorityQueue pqCopy(PriorityQueue queue)
{
    if (queue == NULL)
    {
        return NULL;
    }
    PriorityQueue new_pq = pqCreate(queue->copy_element, queue->free_element,
                                    queue->equal_elements, queue->copy_priority, queue->free_priority, queue->compare_priority);

    if (new_pq == NULL)
    {
        return NULL;
    }
    if (addAllOrDestroy(new_pq, queue) == PQ_OUT_OF_MEMORY)
    {
        return NULL;
    }
    new_pq->iterator = queue->iterator;
    return new_pq;
}

int pqGetSize(PriorityQueue queue)
{
    if (queue == NULL)
    {
        return -1;
    }
    return queue->size;
}

static int find(PriorityQueue pq, PQElement element_target)
{
    assert(pq != NULL && element_target != NULL);
    for (int i = 0; i < pq->size; i++)
    {
        if (pq->equal_elements(pq->elements[i], element_target))
        {
            return i;
        }
    }
    return ELEMENT_NOT_FOUND;
}

static int superFind(PriorityQueue pq, PQElement element_target, PQElementPriority priority_target)
{
    assert(pq != NULL && element_target != NULL && priority_target != NULL);
    for (int i = 0; i < pq->size; i++)
    {
        if (pq->equal_elements(pq->elements[i], element_target))
        {
            if (pq->compare_priority(pq->priorities[i], priority_target))
            {
                return i;
            }
        }
    }
    return ELEMENT_NOT_FOUND;
}

bool pqContains(PriorityQueue queue, PQElement element)
{
    return find(queue, element) == ELEMENT_NOT_FOUND ? false : true;
}

// putting the big priority first
PriorityQueueResult pqInsert(PriorityQueue queue, PQElement element,
                             PQElementPriority priority)
{

    if (queue == NULL || element == NULL)
    {
        return PQ_NULL_ARGUMENT;
    }
    if (queue->size == queue->maxSize && expand(queue) == PQ_OUT_OF_MEMORY)
    {
        return PQ_OUT_OF_MEMORY;
    }

    PQElement new_element = queue->copy_element(element);
    if (new_element == NULL)
    {
        return PQ_OUT_OF_MEMORY;
    }
    PQElementPriority new_priority = queue->copy_priority(priority);
    if (new_priority == NULL)
    {
        free(new_element);
        return PQ_OUT_OF_MEMORY;
    }

    for (int i = 0; i < queue->size; i++)
    {
        if (queue->compare_priority(queue->priorities[i], new_priority) < 0)
        {
            return insertToQueueByIndex(queue, i, new_element, new_priority);
        }
    }

    queue->elements[queue->size] = new_element;
    queue->priorities[queue->size++] = new_priority;
    return PQ_SUCCESS;
}

static PriorityQueueResult expand(PriorityQueue queue)
{
    assert(queue != NULL);
    int newSize = EXPAND_FACTOR * queue->maxSize;
    PQElement *newElements = realloc(queue->elements, newSize * sizeof(int));
    if (newElements == NULL)
    {
        return PQ_OUT_OF_MEMORY;
    }
    PQElementPriority *newPriorities = realloc(queue->priorities, newSize * sizeof(int));
    if (newPriorities == NULL)
    {
        return PQ_OUT_OF_MEMORY;
        // in this case the user should destroy the queue so we don't free elements.
    }
    queue->priorities = newPriorities;
    queue->elements = newElements;
    queue->maxSize = newSize;
    return PQ_SUCCESS;
}

static PriorityQueueResult insertToQueueByIndex(PriorityQueue queue,
                                                int index, PQElement element, PQElementPriority priority)
{

    assert(queue != NULL && index >= 0);
    queue->size++;
    if (queue->size == queue->maxSize && expand(queue) == PQ_OUT_OF_MEMORY)
    {
        return PQ_OUT_OF_MEMORY;
    }
    for (int i = queue->size - 1; i > index; i--)
    {
        queue->elements[i] = queue->elements[i - 1];
        queue->priorities[i] = queue->priorities[i - 1];
    }
    queue->elements[index] = element;
    queue->priorities[index] = priority;
    return PQ_SUCCESS;
}

static PriorityQueueResult pqRemoveElementByIndex(PriorityQueue queue, int index)
{
    assert(queue != NULL && index >= 0);

    queue->free_element(queue->elements[index]);
    queue->free_priority(queue->priorities[index]);

    for (int i = index; i < queue->size - 1; i++)
    {
        queue->elements[i] = queue->elements[i + 1];
        queue->priorities[i] = queue->priorities[i + 1];
    }

    queue->size--;
    queue->iterator = 0;

    return PQ_SUCCESS;
}

PriorityQueueResult pqRemove(PriorityQueue queue)
{
    if (queue == NULL)
    {
        return PQ_NULL_ARGUMENT;
    }
    return pqRemoveElementByIndex(queue, 0);
}

PriorityQueueResult pqRemoveElement(PriorityQueue queue, PQElement element)
{
    if (queue == NULL || element == NULL)
    {
        return PQ_NULL_ARGUMENT;
    }
    if (!pqContains(queue, element))
    {
        return PQ_ELEMENT_DOES_NOT_EXISTS;
    }
    int index = find(queue, element); // because they are sorted - the biggest priority element exists in the first element
    return pqRemoveElementByIndex(queue, index);
}

PriorityQueueResult pqChangePriority(PriorityQueue queue, PQElement element,
                                     PQElementPriority old_priority, PQElementPriority new_priority)
{
    if (queue == NULL || element == NULL || old_priority == NULL || new_priority == NULL)
    {
        return PQ_NULL_ARGUMENT;
    }
    int index = superFind(queue, element, old_priority);
    if (index == -1)
    {
        return PQ_ELEMENT_DOES_NOT_EXISTS;
    }

    PQElementPriority new_element_copy = queue->copy_element(element);
    if (new_element_copy == NULL)
    {
        return PQ_OUT_OF_MEMORY;
    }
    PQElementPriority new_priority_copy = queue->copy_priority(new_priority);
    if (new_priority_copy == NULL)
    {
        free(new_element_copy);
        return PQ_OUT_OF_MEMORY;
    }

    pqRemoveElementByIndex(queue, index);
    pqInsert(queue, new_element_copy, new_priority_copy);
    return PQ_SUCCESS;
}

PQElement pqGetFirst(PriorityQueue queue)
{
    if(queue != NULL)
    {
        return NULL;
    }
    queue->iterator = 0;
    return pqGetNext(queue);
}

// Get next set element
PQElement pqGetNext(PriorityQueue queue)
{
    if(queue != NULL)
    {
        return NULL;
    }
    if (queue->iterator >= queue->size)
    {
        return NULL;
    }
    return queue->elements[queue->iterator++];
}