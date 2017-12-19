/** AST handling for block nodes: Program, FnImpl, Block, etc.
 * @file
 *
 * This source file is part of the Cone Programming Language C compiler
 * See Copyright Notice in conec.h
*/

#include "ast.h"
#include "../shared/memory.h"
#include "../parser/lexer.h"
#include "../shared/symbol.h"
#include "../shared/error.h"

// Create a new Program node
PgmAstNode *newPgmNode() {
	PgmAstNode *pgm;
	newAstNode(pgm, PgmAstNode, PgmNode);
	pgm->nodes = newNodes(8);
	return pgm;
}

// Serialize the AST for a program
void pgmPrint(PgmAstNode *pgm) {
	AstNode **nodesp;
	uint32_t cnt;

	astFprint("AST for program %s\n", pgm->lexer->url);
	astPrintIncr();
	for (nodesFor(pgm->nodes, cnt, nodesp))
		astPrintNode(*nodesp);
	astPrintDecr();
}

// Check the program's AST
void pgmPass(AstPass *pstate, PgmAstNode *pgm) {
	AstNode **nodesp;
	uint32_t cnt;

	for (nodesFor(pgm->nodes, cnt, nodesp))
		astPass(pstate, *nodesp);
}

// Create a new block node
BlockAstNode *newBlockNode() {
	BlockAstNode *blk;
	newAstNode(blk, BlockAstNode, BlockNode);
	blk->nodes = newNodes(8);
	return blk;
}

// Serialize the AST for a block
void blockPrint(BlockAstNode *blk) {
	AstNode **nodesp;
	uint32_t cnt;

	astFprint("block:\n");
	if (blk->nodes) {
		astPrintIncr();
		for (nodesFor(blk->nodes, cnt, nodesp))
			astPrintNode(*nodesp);
		astPrintDecr();
	}
}

// Check the block's AST
void blockPass(AstPass *pstate, BlockAstNode *blk) {
	AstNode **nodesp;
	uint32_t cnt;

	if (blk->nodes)
		for (nodesFor(blk->nodes, cnt, nodesp))
			astPass(pstate, *nodesp);
}

// Create a new expression statement node
StmtExpAstNode *newStmtExpNode() {
	StmtExpAstNode *node;
	newAstNode(node, StmtExpAstNode, StmtExpNode);
	return node;
}

// Serialize the AST for a statement expression
void stmtExpPrint(StmtExpAstNode *node) {
	astPrintIndent();
	astFprint("stmtexp ");
	astPrintNode(node->exp);
	astPrintNL();
}

// Check the AST for a statement expression
void stmtExpPass(AstPass *pstate, StmtExpAstNode *node) {
	astPass(pstate, node->exp);
}

// Create a new return statement node
StmtExpAstNode *newReturnNode() {
	StmtExpAstNode *node;
	newAstNode(node, StmtExpAstNode, ReturnNode);
	node->exp = voidType;
	return node;
}

// Serialize the AST for a return statement
void returnPrint(StmtExpAstNode *node) {
	astPrintIndent();
	astFprint("return ");
	astPrintNode(node->exp);
	astPrintNL();
}

// Check the AST for a return statement
void returnPass(AstPass *pstate, StmtExpAstNode *node) {
	astPass(pstate, node->exp);
	// Ensure the vtype of the expression can be coerced to the function's declared return type
	if (pstate->pass == TypeCheck) {
		if (!typeCoerces(pstate->fnsig->rettype, &node->exp)) {
			errorMsgNode(node->exp, ErrorInvType, "Return expression type does not match return type on function");
			errorMsgNode((AstNode*)pstate->fnsig->rettype, ErrorInvType, "This is the declared function's return type");
		}
	}
}
