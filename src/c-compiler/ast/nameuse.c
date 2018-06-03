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

// Create a new name use node
NameUseAstNode *newNameUseNode(Name *namesym) {
	NameUseAstNode *name;
	newAstNode(name, NameUseAstNode, NameUseNode);
	name->mod = NULL;
	name->dclnode = NULL;
	name->namesym = namesym;
	return name;
}

NameUseAstNode *newMemberUseNode(Name *namesym) {
	NameUseAstNode *name;
	newAstNode(name, NameUseAstNode, MemberUseNode);
	name->namesym = namesym;
	return name;
}

// Serialize the AST for a name use
void nameUsePrint(NameUseAstNode *name) {
	astFprint("%s", &name->namesym->namestr);
}

// Check the name use's AST
void nameUsePass(PassState *pstate, NameUseAstNode *name) {
	// During name resolution, point to name declaration and copy over needed fields
	switch (pstate->pass) {
	case NameResolution:
		if (name->mod==NULL || name->mod == pstate->mod)
			name->dclnode = (NamedAstNode*)name->namesym->node;
		else {
			SymNode *symnode = inodesFind(name->mod->namednodes, name->namesym);
			if (symnode)
				name->dclnode = (NamedAstNode*)symnode->node;
		}
		if (!name->dclnode)
			errorMsgNode((AstNode*)name, ErrorUnkName, "The name %s does not refer to a declared name", &name->namesym->namestr);
		break;
	case TypeCheck:
		name->vtype = name->dclnode->vtype;
		break;
	}
}

