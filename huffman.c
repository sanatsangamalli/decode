# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <ctype.h>
# include "huffman.h"

// New node, with symbols, leaf or not, a count associated with it
treeNode *newNode (uint8_t sym, bool leaf, uint64_t count)
{
	treeNode *node = malloc(sizeof(treeNode));
	if(node)
	{
		node->symbol = sym;
		node->leaf = leaf;
		node->count = count;
		node->left = node->right = NIL;
	}
	return node;
}

// Dump a Huffman tree onto a file
void dumpTree (treeNode *t , int file)
{
	if(t)
	{
		uint8_t nodeType;
		if(t->leaf)
		{
			nodeType = 'L';
			write(file, &nodeType, sizeof(nodeType));
			write(file, &t->symbol, sizeof(t->symbol));
		}
		else
		{
			dumpTree(t->left, file);
			dumpTree(t->right, file);
			nodeType = 'I';
			write(file, &nodeType, sizeof(nodeType));
		}
	}
	return;
}

// Step through a tree following the code
int32_t stepTree (treeNode *root , treeNode **t, uint32_t code)
{
	if(root != NULL)																//check if root isn't NULL
	{
		if(code == 0)																//bit value is 0, so make pointer point to its left child
		{
			*t = (*t)->left;		
			if(*t != NULL && (*t)->leaf)											//pointer isn't NULL and node is a leaf
			{
				return (int32_t)(*t)->symbol;										//return the symbol
			}
		}
		else																		//bit value is 1, so make pointer point to its right child
		{
			*t = (*t)->right;		
			if(*t != NULL && (*t)->leaf)											//pointer isn't NULL and node is a leaf
			{
				return (int32_t)(*t)->symbol;										//return symbol
			}
		}
	}
	return -1;																		//indicates that currently at an interior node
}

// Parse a Huffman tree to build codes
void buildCode (treeNode *t, code s, code table[256])
{
	if(t)
	{
		if(t->leaf)
		{
			table[t->symbol] = s;
		}
		else
		{
			uint32_t temp = 0;
			pushCode(&s, 0);
			buildCode(t->left, s, table);
			popCode(&s, &temp);
			pushCode(&s, 1);
			buildCode(t->right, s, table);
		}
	}
	return;
}

// Delete a tree
void delTree (treeNode *t)
{
	if(t)
	{
		delTree(t->left);
		delTree(t->right);
		delNode(t);
	}
	return;
}

// Makes an internal node joining l and r treeNodes
treeNode *join(treeNode *l, treeNode *r)
{
	treeNode *node = newNode('$', 0, l->count + r->count); // No error checking for node, l, and r yet
	node->left = l;
	node->right = r;
	return node;
}

// In-order huffman tree print
void printTree(treeNode *t, int depth)
{
	if (t) 
	{
		printTree(t->left, depth + 1);

		if (t->leaf)
		{
			if (isalnum(t->symbol))
			{
				spaces(4 * depth); printf("'%c' (%lu)\n", t->symbol, t->count);
			}
			else
			{
				spaces(4 * depth); printf("0x%X (%lu)\n", t->symbol, t->count);
			}
		}
		else
		{
			spaces(4 * depth); printf("$ (%lu)\n", t->count);
		}

		printTree(t->right, depth + 1); 
	}
	return;
}
