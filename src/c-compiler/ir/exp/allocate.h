/** Handling for allocate expression nodes
 * @file
 *
 * This source file is part of the Cone Programming Language C compiler
 * See Copyright Notice in conec.h
*/

#ifndef allocate_h
#define allocate_h

// Uses RefNode from reference.h

RefNode *newAllocateNode();

// Clone allocate
INode *cloneAllocateNode(CloneState *cstate, RefNode *node);

void allocatePrint(RefNode *node);

// Name resolution of allocate node
void allocateNameRes(NameResState *pstate, RefNode **nodep);

// Type check allocate node
void allocateTypeCheck(TypeCheckState *pstate, RefNode **node);

// Perform data flow analysis on addr node
void allocateFlow(FlowState *fstate, RefNode **nodep);

#endif