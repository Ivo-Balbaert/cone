/** AST handling for function signature
 * @file
 *
 * This source file is part of the Cone Programming Language C compiler
 * See Copyright Notice in conec.h
*/

#ifndef fnsig_h
#define fnsig_h

typedef struct FnCallAstNode FnCallAstNode;

// For function signatures
typedef struct FnSigAstNode {
	TypeAstHdr;
	AstNode *rettype;		// void, one or tuple
	Inodes *parms;			// list of parms
} FnSigAstNode;

FnSigAstNode *newFnSigNode();
void fnSigPrint(FnSigAstNode *node);
void fnSigPass(PassState *pstate, FnSigAstNode *name);
int fnSigEqual(FnSigAstNode *node1, FnSigAstNode *node2);
int fnSigMatchesCall(FnSigAstNode *to, FnCallAstNode *from);

#endif