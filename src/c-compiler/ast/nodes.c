/** AST node list
 * @file
 *
 * This source file is part of the Cone Programming Language C compiler
 * See Copyright Notice in conec.h
*/

#include "ast.h"
#include "../ast/nametbl.h"
#include "../parser/lexer.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// Allocate and initialize a new nodes block
Nodes *newNodes(int size) {
	Nodes *nodes;
	nodes = (Nodes*) memAllocBlk(sizeof(Nodes) + size*sizeof(AstNode*));
	nodes->avail = size;
	nodes->used = 0;
	return nodes;
}

// Add an AstNode to the end of a Nodes, growing it if full (changing its memory location)
// This assumes a nodes can only have a single parent, whose address we point at
void nodesAdd(Nodes **nodesp, AstNode *node) {
	Nodes *nodes = *nodesp;
	// If full, double its size
	if (nodes->used >= nodes->avail) {
		Nodes *oldnodes;
		AstNode **op, **np;
		oldnodes = nodes;
		nodes = newNodes(oldnodes->avail << 1);
		op = (AstNode **)(oldnodes+1);
		np = (AstNode **)(nodes+1);
		memcpy(np, op, (nodes->used = oldnodes->used) * sizeof(AstNode*));
		*nodesp = nodes;
	}
	*((AstNode**)(nodes+1)+nodes->used) = node;
	nodes->used++;
}

// Find the desired named node in an nodes list.
// Return the node, if found or NULL if not found
NamedAstNode *nodesFind(Nodes *nodes, Name *name) {
	AstNode **nodesp;
	uint32_t cnt;
	for (nodesFor(nodes, cnt, nodesp)) {
		if (isNamedNode(*nodesp)) {
			if (((NamedAstNode*)*nodesp)->namesym == name)
				return (NamedAstNode*)*nodesp;
		}
	}
	return NULL;
}

// Allocate and initialize a new Inodes block
Inodes *newInodes(int size) {
	Inodes *nodes;
	nodes = (Inodes*)memAllocBlk(sizeof(Inodes) + size * sizeof(SymNode));
	nodes->avail = size;
	nodes->used = 0;
	return nodes;
}

// Add a Name:AstNode pair to the end of a Inodes, growing it if full (changing its memory location)
// This assumes an inodes can only have a single parent, whose address we point at
void inodesAdd(Inodes **nodesp, Name *name, AstNode *node) {
	Inodes *inodes = *nodesp;
	// If full, double its size
	if (inodes->used >= inodes->avail) {
		Inodes *oldnodes;
		SymNode *op, *np;
		oldnodes = inodes;
		inodes = newInodes(oldnodes->avail << 1);
		op = (SymNode *)(oldnodes + 1);
		np = (SymNode *)(inodes + 1);
		memcpy(np, op, (inodes->used = oldnodes->used) * sizeof(SymNode));
		*nodesp = inodes; // Point to new larger block
	}
	SymNode *slotp = ((SymNode*)(inodes + 1)) + inodes->used;
	slotp->name = name;
	slotp->node = (NamedAstNode*)node;
	inodes->used++;
}

// Find a name in an inodes list, returning NULL if not found
SymNode *inodesFind(Inodes *inodes, Name *name) {
	SymNode *nodesp;
	uint32_t cnt;
	for (inodesFor(inodes, cnt, nodesp)) {
		if (nodesp->name == name)
			return nodesp;
	}
	return NULL;
}
