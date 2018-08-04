/** AST handling for literals
 * @file
 *
 * This source file is part of the Cone Programming Language C compiler
 * See Copyright Notice in conec.h
*/

#include "../ir.h"
#include "../../shared/memory.h"
#include "../../parser/lexer.h"
#include "../nametbl.h"
#include "../../shared/error.h"

// Create a new unsigned literal node
ULitAstNode *newULitNode(uint64_t nbr, AstNode *type) {
	ULitAstNode *lit;
	newAstNode(lit, ULitAstNode, ULitNode);
	lit->uintlit = nbr;
	lit->vtype = type;
	return lit;
}

// Serialize the AST for a Unsigned literal
void ulitPrint(ULitAstNode *lit) {
	if (((NbrAstNode*)lit->vtype)->bits == 1)
		astFprint(lit->uintlit == 1 ? "true" : "false");
	else {
		astFprint("%ld", lit->uintlit);
		astPrintNode(lit->vtype);
	}
}

// Create a new unsigned literal node
FLitAstNode *newFLitNode(double nbr, AstNode *type) {
	FLitAstNode *lit;
	newAstNode(lit, FLitAstNode, FLitNode);
	lit->floatlit = nbr;
	lit->vtype = type;
	return lit;
}

// Serialize the AST for a Unsigned literal
void flitPrint(FLitAstNode *lit) {
	astFprint("%g", lit->floatlit);
	astPrintNode(lit->vtype);
}

// Create a new string literal node
SLitAstNode *newSLitNode(char *str, AstNode *type) {
	SLitAstNode *lit;
	newAstNode(lit, SLitAstNode, SLitNode);
	lit->strlit = str;
	lit->vtype = type;
	return lit;
}

// Serialize the AST for a string
void slitPrint(SLitAstNode *lit) {
	astFprint("\"%s\"", lit->strlit);
}

int litIsLiteral(AstNode* node) {
	return (node->asttype == FLitNode || node->asttype == ULitNode);
}