# include <stdio.h>
# include <stdlib.h>
# include <stdint.h>
# include <stdbool.h>
# include <getopt.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <unistd.h>
# include <errno.h>
# include "bv.h"
# include "huffman.h"
# include "stack.h"

/**
 * Build a tree from the savedTree array data, which has size treeBytes.
 * Originally in huffman tree implementation, but moved to decode.c for 
 * design purposes.
 *
 * @param uint8_t savedTree [] an array for the data of the huffman tree
 * to reconstruct
 * @param uint16_t treeBytes the size of the tree
 *
 */
treeNode *loadTree (uint8_t savedTree [] , uint16_t treeBytes)						
{
	stack *s = newStack();															//create new stack
	uint16_t index = 0;																//index
	while(index < treeBytes)														//run through the savedTree
	{
		if(savedTree[index] == 'L')													//if savedtree[index] is L, push next thing onto stack
		{
			push(s, newNode(savedTree[index + 1], 1, 0));							//create newNode with element after L, is leaf. 	
			index += 2;																//increment index by 2 to get next element
		}
			
		else																		//else create subtree with first two things on stack, savedTree[index] is I
		{
			treeNode *temp1 = pop(s); 												//right child is first thing popped off stack
			treeNode *temp2 = pop(s);												//left child is second thing popped off stack
			push(s, join(temp2, temp1));											//push newnode of joined treeNodes(subTree) onto stack	
			index++;																//increment index
		}
	}
	treeNode *result = pop(s);														//at end of method, there's only 1 thing left on the stack, so pop it
	delStack(s);																	//delete stack
	return result;																	//return node popped off stack
}

/**
 * This function reads the input file(which is compressed) descriptor, and 
 * returns the data for the huffman tree to reconstruct in conjunction
 * with loadTree(). 
 *
 * @int32_t inputFD the file descriptor for the input file.
 * @uint16_t *treeSize a pointer to the variable treeSize so it can be changed.
 * @uint64_t *outputSize a pointer to the variable outputSize so it can be changed.
 * @return a uint8_t pointer to the huffman tree data.
 *
 */
uint8_t *readFile(int32_t inputFD, uint16_t *treeSize, uint64_t *outputSize)
{
	uint32_t magicNumber = 0;										
	(void) read(inputFD, &magicNumber, 4);											//read in the first 4 bytes to the address of magicNumber
	if(magicNumber != 0xdeadd00d)													//magicNumber should be a specific value, or there's an error
	{
		perror("magic number is wrong");
		exit(errno);
	}
	(void) read(inputFD, outputSize, 8);											//read in the next 8 bytes to the get the output size
	(void) read(inputFD, treeSize, 2);												//read in the next 2 bytes to get the treeSize
	uint8_t *savedTree = calloc(*treeSize, sizeof(uint8_t));						//allocate memory for the array of huffman tree data 
	(void) read(inputFD, savedTree, *treeSize);										//read in treeSize bytes into the huffman tree data
	return savedTree;																//return array for data of huffman tree
}

/**
 * This method writes the original contents file to an ouput file. 
 *
 * @param bitV *vec which has the entire bit stream read in
 * @param treeNode *root a pointer to the root of the tree
 * @param int32_t outputFD the output file descriptor
 * @param uint64_t outputSize the output file size
 * 
 */
void writeFile(bitV *vec, treeNode *root, int32_t outputFD, uint64_t outputSize)
{					
	int32_t symbol = 0;																//symbol to write to output file
	treeNode *ptr = root;															//a seperate ptr to the tree to change
	uint32_t tempBytes = 0;
	uint32_t i = 0;
	while(i < vec->l)																//while loop to run through bit vector
	{
		do																			//do while loop to run through tree
		{
			symbol = stepTree(root, &ptr, (uint32_t)valBit(vec, i));				//call stepTree iteratively based on bit at index, root, and address of ptr
			i++;																	//increment index
		}while(i < vec->l && symbol == -1);											//if index less than length of bit vector and not at leaf node
		if(tempBytes++ < outputSize)												//write when number of bytes is less than size of output file
		{
			write(outputFD, &symbol, sizeof(uint8_t));								//write symbol to output file
		}
		ptr = root;																	//reset ptr to root of tree
	}
}

int main(int argc, char **argv)
{
	struct stat fileStat;															//structure where data of input file will be stored
	int32_t inputFD = 0, outputFD = 1;												//file descriptors for input and output files
	bool pTree = 0, verbose = 0;													//booleans to identify certain command line arguments have been passed in
	int8_t c = 0;																	//command line argument 
	while((c = getopt(argc, argv, "i:o:vp")) != -1)			
	{
		switch(c)																	//switch statement for the command line argument
		{
			case 'i' :																//case i for input file(required)
			{
				inputFD = open(optarg, O_RDONLY);									//create input file file descriptor
				if(inputFD == -1 || fstat(inputFD, &fileStat) == -1)				//means there was a failue, error handling
				{
					perror("input file descriptor not valid or fstat failure");
					exit(errno);
				}
				break;
			}
			case 'o' :																//case o for output file(optional)
			{
				outputFD = open(optarg, O_WRONLY | O_CREAT | O_EXCL, 0644);			//create output file file descriptor
				if(outputFD == -1)													//means there was a failure, error handling
				{
					perror("output file descriptor not valid");
					exit(errno);
				}
				break;
			}
			case 'v' :																//case v verbose to print out statistics of the data
			{
				verbose = 1;
				break;
			}
			case 'p':																//case p to print out tree
			{
				pTree = 1;
			}
			default :																//default case
			{
				break;
			}
		}
	}
	if(inputFD != 0)																//if no input file file descriptor, do not print anything out
	{
		uint16_t treeSize = 0;														//size of tree
		uint64_t outputSize = 0;													//size of output file
		uint8_t *savedTree = readFile(inputFD, &treeSize, &outputSize);				//data for huffman tree from function readFile()	
		treeNode *root = loadTree(savedTree, treeSize);								//root of huffman tree pointer
		uint32_t numBytes = fileStat.st_size - 14 - treeSize;						//number of bytes of the rest of the file
		bitV *vec = newVec(numBytes * 8);											//new bit vector 
		(void) read(inputFD, vec->v, numBytes);										//read in bit stream into bit vector
		if(verbose)																	//verbose case
		{
			printf("Original %lu bits: tree (%u)\n", outputSize * 8, treeSize);
		}
		writeFile(vec, root, outputFD, outputSize);									//write to output file
		if(pTree)																	//print tree case
		{
			printTree(root, 0);
		}
		delVec(vec);																//delete statements
		delTree(root);
		free(savedTree);															//free savedTree because I allocate memory for it
		savedTree = NULL;															//make savedTree NULL
		int closed1 = close(inputFD);												//close input file
		if(closed1 == -1)															//error check and handling
		{
			perror("input file did not close");
			exit(errno);
		}
		int closed2 = close(outputFD);												//close output file
		if(closed2 == -1)															//error check and handling
		{
			perror("output file did not close");
			exit(errno);
		}
	}
	return 0;																		//return statement for main
}