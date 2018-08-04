/** Permission types
 * @file
 *
 * This source file is part of the Cone Programming Language C compiler
 * See Copyright Notice in conec.h
*/

#ifndef permission_h
#define permission_h

// Permission type info
typedef struct PermAstNode {
	MethodTypeAstHdr;
	INode *locker;
	uint8_t ptype;
} PermAstNode;

// Permission types
enum Perm {
	UniPerm,
	MutPerm,
	ImmPerm,
	ConstPerm,
	MutxPerm,
	IdPerm,
	LockPerm
};

// Permission type flags
// - Each true flag indicates a helpful capability the permission makes possible
// - All false flags essentially deny those capabilities to the permission
#define MayRead 0x01		// If on, the contents may be read
#define MayWrite 0x02		// If on, the contents may be mutated
// If on, another live alias may be created able to read or mutate the contents. 
// If off, any new alias turns off use of the old alias for the lifetime of the new alias.
#define MayAlias 0x04		
#define MayAliasWrite 0x08	// If on, another live alias may be created able to write the contents.
#define RaceSafe 0x10		// If on, a reference may be shared with or sent to another thread.
// If on, interior references may be made within a sum type
#define MayIntRefSum 0x20
// If on, no locks are needed to read or mutate the contents. 
// If off, the permission's designated locking mechanism must be wrapped around all content access.
#define IsLockless 0x40

PermAstNode *newPermNode(Name *namesym, char ptyp, uint16_t flags);
void permPrint(PermAstNode *node);
uint16_t permGetFlags(INode *node);
int permIsMutable(INode *lval);
int permIsSame(PermAstNode *node1, PermAstNode *node2);
int permMatches(PermAstNode *node1, PermAstNode *node2);

#endif