# include <stdlib.h>
# include <getopt.h>
# include <errno.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <fcntl.h>
# include <unistd.h>
# include "bv.h"
# include "queue.h"

/*
 * This function first builds the codes for each character using the huffman tree. Then, the 
 * input file is read with the code for each byte being appended to a bitVector. This bitvector is
 * written to the output file afterwards.
 *
 * @param	outputFD	File descriptor for the output file
 * @param	fileSize	Size of the original file
 * @param	tempBytes	Stores each byte of data of the original file
 * @param	root		The root of the huffman tree
 * @return	number of bits encoded
 */
uint64_t encodeFile(int32_t outputFD, uint64_t fileSize, uint8_t *tempBytes, treeNode *root)
{
	uint64_t numBits = 0;
	bitV *vec = newVec(16);
	code table[256] = {0};
	buildCode(root, newCode(), table);
	for(uint32_t i = 0; i < fileSize; i++)
	{
		if(table[tempBytes[i]].l > 0)
		{
			appendCode(vec, table[tempBytes[i]]);
			numBits+= table[tempBytes[i]].l;
		}
	}
	for(uint32_t i = 0; i < (vec->p/8)+1; i++)
	{
		if(&vec->v[i])
		{
			write(outputFD, &(vec->v[i]), sizeof(vec->v[i]));
		}
	}
	delVec(vec);
	return numBits;
}

/*
 * This main function begins by using getopt to parse the command line for specific arguments.
 * The user must enter in an input file, and they can also toggle various options such as a file
 * to write to, and whether or not to print the huffman tree and/or program statistics. Afterwards,
 * the chosen file is then read, with a histogram being created to represent the counts of each byte
 * in the file. A priority queue of huffman treeNodes is made from the histogram, and it is used to
 * continuously dequeue the smallest two nodes and build a huffman tree. Codes are built to represent
 * each character in the tree with bits, and the file along with some other information is encoded to
 * the output destination, creating an encoded file. 
 *
 * @param	argc,argv	argc is number of command line arguments, argv contains the arguments
 * @return zero
 */
int main(int argc, char **argv)
{
	struct stat fileStat;
	bool verbose = 0, treePrint = 0;
	int32_t inputFD = 0, outputFD = 1; // file descriptor for the file to encode
	int8_t c = 0; // used with getopt to determine which argument is entered
	uint32_t histogram[256] = {0};
	histogram[0] = ++histogram[255]; // default these two values in the histogram have 1
	while((c = getopt(argc, argv, "pvi:o:"))!=-1)
	{
		switch(c)
		{
			case 'i':
			{
				inputFD = open(optarg, O_RDONLY); //opens the file(with read only flag) being read and sets fd to the file descriptor
				if(inputFD == -1 || fstat(inputFD, &fileStat) == -1) //if fd is -1 then "open" failed to open the file
				{
					perror("encode");
					exit(errno); //perror prints the error message and the program is quit with errno being set accordingly
				}
				break;
			}
			case 'o':
			{
				outputFD = open(optarg, O_WRONLY | O_CREAT | O_EXCL, 0644); // creates a new file, ensuring it does not exist
				if(outputFD == -1) // File already exists
				{
					printf("Error: Output file already exists.\n");
					exit(errno);
				}
				break;
			}
			case 'p':
			{
				treePrint = 1;
				break;
			}
			case 'v':
			{
				verbose = 1;
				break;
			}
			default:
			{
				break;
			}
		}
	}
	
	if(!inputFD)
	{
		printf("No Input File\n");
	}
	else
	{
		queue *priority = newQueue(257); // max number queue needs to hold is 256, and it has 1 unused space
		uint64_t fileSize = fileStat.st_size, numBits = 0;
		uint32_t magicNumber = MAGIC;
		uint16_t treeSize = 0;
		uint8_t *tempBytes = (uint8_t *) malloc(fileSize);
		(void) read(inputFD, tempBytes, fileSize);
		for(uint32_t i = 0; i < fileSize; i++)
		{
			histogram[tempBytes[i]] +=1;
		}
		for(int i = 0; i < 256; i++)
		{
			if(histogram[i])
			{
				enqueue(priority, newNode(i,1,histogram[i]));
			}
		}
		treeSize = (priority->head *3) -1; // priority->head is currently the number of leaf nodes
		
		treeNode *smallTemp, *smallerTemp, *root; // Temporary nodes used to create the huffman tree
		while(!emptyQ(priority))
		{
			(void) dequeue(priority, &smallerTemp);
			if(emptyQ(priority))
			{
				root = smallerTemp; // found the root node of the tree
				break;
			}
			(void) dequeue(priority, &smallTemp);
			enqueue(priority, join(smallerTemp, smallTemp));
		}
		write(outputFD, &magicNumber, sizeof(magicNumber));
		write(outputFD, &fileSize, sizeof(fileSize));
		write(outputFD, &treeSize, sizeof(treeSize)); // write magic number, file size, and tree size
		dumpTree(root, outputFD);
		numBits = encodeFile(outputFD, fileSize, tempBytes, root); // Encodes file with the correct code for each character
		if(verbose)
		{
			printf("Original %lu bits: leaves %u (%u bytes) encoding %lu bits", (fileSize*8), (treeSize+1)/3, treeSize, numBits);
			printf(" (%.4f%%)\n", ((double)numBits)/(fileSize*8)*100);
		}
		if(treePrint)
		{
			printTree(root,0);
		}
		free(tempBytes);
		delQueue(priority);
		delTree(root);
	}
	if(close(inputFD) == -1 || close(outputFD) == -1) //closes the files used, exiting if close fails
	{
		perror("encode");
		exit(errno);
	}
	return 0;
}
