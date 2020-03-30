// Code written mostly by DDEL
# ifndef _CODE_H
# define _CODE_H

# include <stdint.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "bv.h"

typedef struct code
{
	uint8_t bits[32];
	uint32_t l;
} code;

// Creates a new code
static inline code newCode()
{
	code t;
	for(int i = 0; i < 32; i++)
	{
		t.bits[i] = 0;
	}
	t.l = 0;
	return t;
}

// Pushes a bit to the code
static inline bool pushCode(code *c, uint32_t k)
{
	if(c->l > 256)
	{
		return false;
	}
	else if(k == 0)
	{
		c->bits[c->l / 8] &= ~(0x1 << (c->l % 8));
		c->l++;
	}
	else
	{
		c->bits[c->l / 8] |= (0x1 << (c->l % 8));
		c->l++;
	}
	return true;
}

// Pops a bit from code, storing it in k
static inline bool popCode(code *c, uint32_t *k)
{
	if(c->l == 0)
	{
		return false;
	}
	else
	{
		c->l--;
		*k = ((0x1 << (c->l % 8)) & c->bits[c->l / 8]) >> (c->l % 8);
		return true;
	}
}

// Appends a code to a bitVector
static inline void appendCode(bitV *vector, code s)
{
	if((vector->p + s.l) > vector->l)
	{
		vector->l*=2;
		vector->v = (uint8_t *) realloc(vector->v,vector->l);	// double size if there's no space
	}
	uint32_t bit, length = s.l;
	vector->p+= length;
	for(uint32_t i = 0; i < length; i++)
	{
		popCode(&s, &bit);
		if(bit)
		{
			setBit(vector, vector->p-i-1); 
		}
		else
		{
			clrBit(vector, vector->p-i-1); 
		}
	}
	return;
}

// Empties a code
static inline bool emptyCode(code *c)
{
	return c->l == 0;
}

// Makes code full
static inline bool fullCode(code *c)
{
	return c->l == 256;
}
# endif



