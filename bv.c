// Code for setBit, clrBit, and valBit from DDEL 4/20/17
# include <stdlib.h>
# include <stdio.h>
# include "bv.h"

bitV *newVec(uint32_t length) // Creates new vector of specified length
{
	uint32_t byteLength = (length/8)+1; // Calculates the given length in bytes rather than bits
	bitV * vector = (bitV *) calloc(1,sizeof(bitV)); 
	vector->v = (uint8_t *) calloc(byteLength,sizeof(uint8_t)); // byteLength is used to reduce overall memory allocated
	vector->l = length; // Sets vectors length value
	vector->p = 0; // Furthest used position
	return vector;
}

void delVec(bitV *vector) // Deletes a vector
{
	free(vector->v); // Frees the vectors uint8_t pointer
	vector->v = NULL;
	free(vector); // Frees the entire bitV pointer
	vector = NULL;
	return;
}

void oneVec(bitV *vector) // Creates a vector of all 1
{
	for(uint32_t i = 0; i < (vector->l); i+=1)
	{
		setBit(vector,i);
	} // Loop goes throught the vector, setting each bit
	return;
}

void setBit(bitV *vector, uint32_t bit)
{
	vector->v[bit>>3] |= 0x1<<(bit&0x7); // Uses bitwise OR to set the given bit
	return;
}

void clrBit(bitV *vector, uint32_t bit) 
{
	vector->v[bit>>3] &= ~(0x1<<(bit&0x7)); // Uses bitwise AND to clear the given bit
	return;
}

uint8_t valBit(bitV *vector, uint32_t bit)
{
	return (vector->v[bit>>3] & (0x1 << (bit&0x7))) >> (bit&0x7); // Returns value of given bit using AND 
} 

uint32_t lenVec(bitV *vector) // Returns length of the vector
{
	return vector->l;
}

void printVec(bitV *vec)
{
	for(uint32_t i = 0; i < vec->l; i++)
	{
		printf("%u", valBit(vec,i));
	}
	printf("\n");
}