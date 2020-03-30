# include "stack.h"
# include "huffman.h"
# include <stdint.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>

//build a new stack 
stack *newStack (void)
{
	stack *s = (stack *) malloc(sizeof(stack));											//allocate memory for stack
	if(s)																				//stack isn't NULL
	{																					//assign variables
		s->size = 10;
		s->top = 0;
		s->entries = (item *) calloc(s->size, sizeof(item));							//allocate memory 
		if(s->entries)
		{
			return s;																	//return statement
		}
	}
	free(s);																			//means there was a failure, free memory
	return (stack *) 0;																	//return statement
}

//delete a stack
void delStack (stack *s)
{
	free(s->entries);																	//free memory and set it to NULL
	s->entries = NULL;
	free(s);
	s = NULL;
	return;
}

//pop and return the item on the top of the stack
item pop (stack *s)
{
	if(s->top > 0)																		//number of elements in stack is greater than 0
	{
		return s->entries[--s->top];													//return thing on top of stack
	}
	return NULL;																		//return NULL if nothing on stack
}

//push a given item onto the stack
void push (stack *s, item it)
{
	if(full(s))																			//if stack is full
	{	
		s->size *= 2;																	//double size of stack
		s->entries = (item *)realloc(s->entries, s->size * sizeof(item));				//reallocate memory
	}
	s->entries[s->top++] = it;															//push item to top of stack
	return;																				//return statement
}

//check if a stack is empty
bool empty (stack *s)
{
	return s->top == 0;																	//if number of elements on stack is 0
}

//check if a stack is full
bool full (stack *s)
{
	return s->top == s->size;															//if no room left on stack
}