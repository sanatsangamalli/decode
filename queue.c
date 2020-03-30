// Code structured from DDEL basic queue code
# include <stdio.h>
# include <stdlib.h>
# include "queue.h"

// Creates a new queue
queue *newQueue(uint32_t size)
{
	queue *q = malloc(sizeof(queue));
	if(q)
	{
		q->size = size;
		q->head = q->tail = 0;
		q->Q = calloc(size, sizeof(item));
		if(q->Q)
		{
			return q;
		}
	}
	free(q);
	return (queue *) 0;
}

// Deletes a queue
void delQueue(queue *q)
{
	for(uint32_t i = 0; i < q->size; i++)
	{
		if(q->Q[i])
		{
			delNode(q->Q[i]);
		}
	}
	free(q->Q);
	free(q);
	return;
}

// Checks if queue is empty
bool emptyQ(queue *q)
{
	return q->head == q->tail;
}

// Checks if queue is full
bool fullQ(queue *q)
{
	return (q->head + 1) % q->size == q->tail % q->size;
}

void printQueue(queue *q)
{
	printf("H: %u T: %u\n", q->head, q->tail);
	for(uint32_t i = 0; i < q->size; i++)
	{
		printf("%u - ", i);
		if(q->Q[i])
		{
			printf("%lu", q->Q[i]->count);
		}
		printf("\n");
	}
	return;
}

// Enqueues an item to the queue in the correct (sorted) position, returning false if queue is full
bool enqueue (queue *q, item i)
{
	if (fullQ(q))
	{
		return false;
	}
	uint32_t temp = q->tail;
	while(temp!=q->head && compare(q->Q[temp], i) < 0)
	{
		temp = (temp+1) % q->size;
	}
	for(uint32_t j = q->head; j != temp; j = (j-1 + q->size) % q->size)
	{
		q->Q[j] = q->Q[(j-1+ q->size) % q->size];
	}
	q->Q[temp] = i;
	q->head = (q->head + 1) % q->size;
	return true;
}

// Dequeues an item from the queue from the tail, returning false if queue is empty
bool dequeue (queue *q, item *i)
{
	if (emptyQ(q))
	{
		return false;
	}
	else
	{
		*i = q->Q[q->tail];
		q->Q[q->tail] = NULL;
		q->tail = (q->tail + 1) % q->size;
		return true;
	}
}