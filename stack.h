//This header file was provided by DDEL
# ifndef _STACK_H
# define _STACK_H
# include <stdint.h>
# include <stdbool.h>
# include "huffman.h"

typedef treeNode *item ; // You will need to change this

typedef struct stack
{
	uint32_t size ; // How big ?
	uint32_t top ; // Where ’s the top ?
	item * entries ; // Array to hold it ( via calloc )
} stack ;

stack * newStack () ; // Constructor
void delStack ( stack *) ; // Destructor

item pop ( stack *) ; // Returns the top item
void push ( stack *, item ) ; // Adds an item to the top

bool empty ( stack *) ; // Is it empty ?
bool full ( stack *) ; // Is it full ?

# endif
