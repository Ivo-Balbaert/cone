/** AST handling for array types
 * @file
 *
 * This source file is part of the Cone Programming Language C compiler
 * See Copyright Notice in conec.h
*/

#ifndef array_h
#define array_h

// For pointers
typedef struct ArrayAstNode {
	MethodTypeAstHdr;
	uint32_t size;
	INode *elemtype;
} ArrayAstNode;

ArrayAstNode *newArrayNode();
void arrayPrint(ArrayAstNode *node);
void arrayPass(PassState *pstate, ArrayAstNode *name);
int arrayEqual(ArrayAstNode *node1, ArrayAstNode *node2);

#endif