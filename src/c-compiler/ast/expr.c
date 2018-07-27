/** AST handling for expression nodes that do not do value copy/move
 * @file
 *
 * This source file is part of the Cone Programming Language C compiler
 * See Copyright Notice in conec.h
*/

#include "ast.h"
#include "../shared/memory.h"
#include "../parser/lexer.h"
#include "../ast/nametbl.h"
#include "../shared/error.h"

#include <assert.h>

// Create a new sizeof node
SizeofAstNode *newSizeofAstNode() {
	SizeofAstNode *node;
	newAstNode(node, SizeofAstNode, SizeofNode);
	node->vtype = (AstNode*)usizeType;
	return node;
}

// Serialize sizeof
void sizeofPrint(SizeofAstNode *node) {
	astFprint("(sizeof, ");
	astPrintNode(node->type);
	astFprint(")");
}

// Analyze sizeof node
void sizeofPass(PassState *pstate, SizeofAstNode *node) {
	astPass(pstate, node->type);
}

// Create a new cast node
CastAstNode *newCastAstNode(AstNode *exp, AstNode *type) {
	CastAstNode *node;
	newAstNode(node, CastAstNode, CastNode);
	node->vtype = type;
	node->exp = exp;
	return node;
}

// Serialize cast
void castPrint(CastAstNode *node) {
	astFprint("(cast, ");
	astPrintNode(node->vtype);
	astFprint(", ");
	astPrintNode(node->exp);
	astFprint(")");
}

// Analyze cast node
void castPass(PassState *pstate, CastAstNode *node) {
	astPass(pstate, node->exp);
	astPass(pstate, node->vtype);
	if (pstate->pass == TypeCheck && 0 == typeMatches(node->vtype, ((TypedAstNode *)node->exp)->vtype))
		errorMsgNode(node->vtype, ErrorInvType, "expression may not be type cast to this type");
}

// Create a new deref node
DerefAstNode *newDerefAstNode() {
	DerefAstNode *node;
	newAstNode(node, DerefAstNode, DerefNode);
	node->vtype = voidType;
	return node;
}

// Serialize deref
void derefPrint(DerefAstNode *node) {
	astFprint("*");
	astPrintNode(node->exp);
}

// Analyze deref node
void derefPass(PassState *pstate, DerefAstNode *node) {
	astPass(pstate, node->exp);
	if (pstate->pass == TypeCheck) {
		PtrAstNode *ptype = (PtrAstNode*)((TypedAstNode *)node->exp)->vtype;
		if (ptype->asttype == RefType || ptype->asttype == PtrType)
			node->vtype = ptype->pvtype;
		else
			errorMsgNode((AstNode*)node, ErrorNotPtr, "Cannot de-reference a non-pointer value.");
	}
}

// Insert automatic deref, if node is a ref
void derefAuto(AstNode **node) {
	if (typeGetVtype(*node)->asttype != RefType)
		return;
	DerefAstNode *deref = newDerefAstNode();
	deref->exp = *node;
	deref->vtype = ((PtrAstNode*)((TypedAstNode *)*node)->vtype)->pvtype;
	*node = (AstNode*)deref;
}

// Create a new logic operator node
LogicAstNode *newLogicAstNode(int16_t typ) {
	LogicAstNode *node;
	newAstNode(node, LogicAstNode, typ);
	node->vtype = (AstNode*)boolType;
	return node;
}

// Serialize logic node
void logicPrint(LogicAstNode *node) {
	if (node->asttype == NotLogicNode) {
		astFprint("!");
		astPrintNode(node->lexp);
	}
	else {
		astFprint(node->asttype == AndLogicNode? "(&&, " : "(||, ");
		astPrintNode(node->lexp);
		astFprint(", ");
		astPrintNode(node->rexp);
		astFprint(")");
	}
}

// Analyze not logic node
void logicNotPass(PassState *pstate, LogicAstNode *node) {
	astPass(pstate, node->lexp);
	if (pstate->pass == TypeCheck)
		typeCoerces((AstNode*)boolType, &node->lexp);
}

// Analyze logic node
void logicPass(PassState *pstate, LogicAstNode *node) {
	astPass(pstate, node->lexp);
	astPass(pstate, node->rexp);

	if (pstate->pass == TypeCheck) {
		typeCoerces((AstNode*)boolType, &node->lexp);
		typeCoerces((AstNode*)boolType, &node->rexp);
	}
}
