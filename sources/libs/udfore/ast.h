
#pragma once
#include <brutal-ds>

typedef int UdAstPtr;

typedef struct
{
    int type;
    UdAstPtr left;
    UdAstPtr right;
} UdAstNode;

typedef Vec(UdAstNode) UdAstNodes;

#define UD_AST_INFO_USE_CHILD (int)(-1)

typedef struct
{
    int sema_type;
    int start;
    int end;
} UdAstInfo;

typedef Vec(UdAstInfo) UdAstInfos;

typedef union
{
    // int data_type;
    Str string;
    int integer;
    float floating;
} UdAstData;

typedef Vec(UdAstData) UdAstDatas;

typedef struct
{
    UdAstNodes nodes;
    UdAstInfos infos;
    // ^^ they need to match each other
    // what this mean is that informations[i] will contain information for the nodes[i] node.
    UdAstDatas datas;
    // when a node contain data, (such as a string constant node)
    //  his 'left' or 'right' index will contain the index to the data table
} UdAst;

// note: the root must not have parent.
#define UD_AST_ROOT ((UdAstPtr)0)

void ud_ast_init(UdAst *self, Alloc *alloc);

void ud_ast_deinit(UdAst *self);

UdAstPtr ud_ast_push(UdAst *self, UdAstNode node, UdAstInfo info);

UdAstPtr ud_ast_push_no_info(UdAst *self, UdAstNode node);

UdAstPtr ud_ast_push_data(UdAst *self, UdAstData data);

UdAstPtr ud_ast_push_list(UdAst *self, UdAstPtr list_node, int list_type, UdAstPtr pushed_index);

// note: avoid storing this pointer, because after an ud_ast_push the vector may reallocate.
UdAstNode *ud_ast_node(UdAst *self, UdAstPtr ptr);

UdAstInfo *ud_ast_info(UdAst *self, UdAstPtr ptr);

UdAstData *ud_ast_data(UdAst *self, UdAstPtr ptr);
