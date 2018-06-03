/** AST handling for variable declaration nodes
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

#include <string.h>
#include <assert.h>

// Create a new name declaraction node
VarDclAstNode *newNameDclNode(Name *namesym, uint16_t asttype, AstNode *type, PermAstNode *perm, AstNode *val) {
	VarDclAstNode *name;
	newAstNode(name, VarDclAstNode, asttype);
	name->vtype = type;
	name->owner = NULL;
	name->hooklinks = NULL;
	name->namesym = namesym;
	name->hooklink = NULL;
	name->prevname = NULL;
	name->perm = perm;
	name->value = val;
	name->scope = 0;
	name->index = 0;
	name->llvmvar = NULL;
	return name;
}

// Serialize the AST for a variable/function
void varDclPrint(VarDclAstNode *name) {
	astPrintNode((AstNode*)name->perm);
	astFprint("%s ", &name->namesym->namestr);
	astPrintNode(name->vtype);
	if (name->value) {
		astFprint(" = ");
		if (name->value->asttype == BlockNode)
			astPrintNL();
		astPrintNode(name->value);
	}
}

// Syntactic sugar: Turn last statement implicit returns into explicit returns
void fnImplicitReturn(AstNode *rettype, BlockAstNode *blk) {
	AstNode *laststmt;
	laststmt = nodesLast(blk->stmts);
	if (rettype == voidType) {
		if (laststmt->asttype != ReturnNode)
			nodesAdd(&blk->stmts, (AstNode*) newReturnNode());
	}
	else {
		// Inject return in front of expression
		if (isExpNode(laststmt)) {
			ReturnAstNode *retnode = newReturnNode();
			retnode->exp = laststmt;
			nodesLast(blk->stmts) = (AstNode*)retnode;
		}
		else if (laststmt->asttype != ReturnNode)
			errorMsgNode(laststmt, ErrorNoRet, "A return value is expected but this statement cannot give one.");
	}
}

// Enable resolution of fn parameter references to parameters
void varDclFnNameResolve(PassState *pstate, VarDclAstNode *name) {
	int16_t oldscope = pstate->scope;
	pstate->scope = 1;
	FnSigAstNode *fnsig = (FnSigAstNode*)name->vtype;
	inodesHook((OwnerAstNode*)name, fnsig->parms);		// Load into global name table
	astPass(pstate, name->value);
	nameUnhook((OwnerAstNode*)name);		// Unhook from name table
	pstate->scope = oldscope;
}

// Enable name resolution of local variables
void varDclNameResolve(PassState *pstate, VarDclAstNode *name) {

	// Variable declaration within a block is a local variable
	if (pstate->scope > 1) {
		if (name->namesym->node && pstate->scope == ((VarDclAstNode*)name->namesym->node)->scope) {
			errorMsgNode((AstNode *)name, ErrorDupName, "Name is already defined. Only one allowed.");
			errorMsgNode((AstNode*)name->namesym->node, ErrorDupName, "This is the conflicting definition for that name.");
		}
		else {
			name->scope = pstate->scope;
			nameHook((OwnerAstNode *)pstate->blk, (NamedAstNode*)name, name->namesym);
		}
	}

	if (name->value)
		astPass(pstate, name->value);
}

// Provide parameter and return type context for type checking function's logic
void varDclFnTypeCheck(PassState *pstate, VarDclAstNode *varnode) {
	FnSigAstNode *oldfnsig = pstate->fnsig;
	pstate->fnsig = (FnSigAstNode*)varnode->vtype;
	astPass(pstate, varnode->value);
	pstate->fnsig = oldfnsig;
}

// Type check variable against its initial value
void varDclTypeCheck(PassState *pstate, VarDclAstNode *name) {
	astPass(pstate, name->value);
	// Global variables and function parameters require literal initializers
	if (name->scope <= 1 && !litIsLiteral(name->value))
		errorMsgNode(name->value, ErrorNotLit, "Variable may only be initialized with a literal.");
	// Infer the var's vtype from its value, if not provided
	if (name->vtype == voidType)
		name->vtype = ((TypedAstNode *)name->value)->vtype;
	// Otherwise, verify that declared type and initial value type matches
	else if (!typeCoerces(name->vtype, &name->value))
		errorMsgNode(name->value, ErrorInvType, "Initialization value's type does not match variable's declared type");
}

// Check the function or variable declaration's AST
void varDclPass(PassState *pstate, VarDclAstNode *name) {
	astPass(pstate, (AstNode*)name->perm);
	astPass(pstate, name->vtype);
	AstNode *vtype = typeGetVtype(name->vtype);

	// Process nodes in name's initial value/code block
	switch (pstate->pass) {
	case NameResolution:
		// Hook into global name table if not a global owner by module
		// (because those have already been hooked by module for forward references)
		/*if (name->owner->asttype != ModuleNode)
			namespaceHook((NamedAstNode*)name, name->namesym);*/
		if (vtype->asttype == FnSig) {
			if (name->value)
				varDclFnNameResolve(pstate, name);
		}
		else
			varDclNameResolve(pstate, name);
		break;

	case TypeCheck:
		if (name->value) {
			if (vtype->asttype == FnSig) {
				// Syntactic sugar: Turn implicit returns into explicit returns
				fnImplicitReturn(((FnSigAstNode*)name->vtype)->rettype, (BlockAstNode *)name->value);
				// Do type checking of function (with fnsig as context)
				varDclFnTypeCheck(pstate, name);
			}
			else
				varDclTypeCheck(pstate, name);
		}
		else if (vtype == voidType)
			errorMsgNode((AstNode*)name, ErrorNoType, "Name must specify a type");
		break;
	}
}
