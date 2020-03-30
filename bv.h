// bv.h — Bit Vector interface
// Author: Prof. Darrell Long

# ifndef _BVector
# define _BVector
# include <stdint.h>

typedef struct bitV {
	uint8_t *v;
	uint32_t l;
	uint32_t p;
} bitV;

bitV *newVec(uint32_t); // Creates new vector of given length

void delVec(bitV *); // Deletes a vector

void oneVec(bitV *); // Sets a vector to all 1s

void setBit(bitV *, uint32_t); // Sets a given bit

void clrBit(bitV *, uint32_t); // Clears a given bit

uint8_t valBit(bitV *, uint32_t); // Returns the value of a given bit

uint32_t lenVec(bitV *); // Returns length of the vector

void printVec(bitV *);
# endif
