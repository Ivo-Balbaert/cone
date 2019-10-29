/** Handling for structs
 * @file
 *
 * This source file is part of the Cone Programming Language C compiler
 * See Copyright Notice in conec.h
*/

#include "../ir.h"

// Create a new struct type whose info will be filled in afterwards
StructNode *newStructNode(Name *namesym) {
    StructNode *snode;
    newNode(snode, StructNode, StructTag);
    snode->namesym = namesym;
    snode->llvmtype = NULL;
    snode->subtypes = newNodes(0);
    iNsTypeInit((INsTypeNode*)snode, 8);
    nodelistInit(&snode->fields, 8);
    return snode;
}

// Add a field
void structAddField(StructNode *type, VarDclNode *varnode) {
    INode *foundnode = namespaceAdd(&type->namespace, varnode->namesym, (INode*)varnode);
    if (foundnode) {
        errorMsgNode((INode*)varnode, ErrorDupName, "Duplicate name %s: Only methods can be overloaded.", &varnode->namesym->namestr);
        return;
    }
    nodelistAdd(&type->fields, (INode*)varnode);
}

// Serialize a struct type
void structPrint(StructNode *node) {
    inodeFprint(node->tag == StructTag? "struct %s {}" : "alloc %s {}", &node->namesym->namestr);
}

// Name resolution of a struct type
void structNameRes(NameResState *pstate, StructNode *node) {
    INode *svtypenode = pstate->typenode;
    pstate->typenode = (INode*)node;
    nametblHookPush();
    nametblHookNamespace(&node->namespace);
    INode **nodesp;
    uint32_t cnt;
    for (nodelistFor(&node->fields, cnt, nodesp)) {
        inodeNameRes(pstate, (INode**)nodesp);
    }
    for (nodelistFor(&node->nodelist, cnt, nodesp)) {
        inodeNameRes(pstate, (INode**)nodesp);
    }
    nametblHookPop();
    pstate->typenode = svtypenode;
}

void structTypePass(TypePass *pstate, StructNode *node) {

}

// Type check a struct type
void structTypeCheck(TypeCheckState *pstate, StructNode *node) {
    INode **nodesp;
    uint32_t cnt;
    for (nodelistFor(&node->fields, cnt, nodesp)) {
        inodeTypeCheck(pstate, (INode**)nodesp);
    }
    for (nodelistFor(&node->nodelist, cnt, nodesp)) {
        inodeTypeCheck(pstate, (INode**)nodesp);
    }
}

// Compare two struct signatures to see if they are equivalent
int structEqual(StructNode *node1, StructNode *node2) {
    // inodes must match exactly in order
    return 1;
}

// Will from struct coerce to a to struct (we know they are not the same)
int structCoerces(StructNode *to, StructNode *from) {
    return 1;
}