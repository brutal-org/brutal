
#pragma once
#include <brutal-ds>

// this is used because this will be extracted and put in an other library
// and the library don't know if the child is a value or a node, making dumping way more complicated than it need to be
// There is not really a difference in performance, because the parser of a language already know depending on
// the node if it is a value or not. This is only for the AST lib, and for error cheking (you don't want to read
// a data node as a ast node).

typedef struct
{
    bool is_data : 1; // FALSE = node TRUE = data
    int index : 31;
} UdAstPtr;

#define ud_ast_ptr_node$(idx) \
    ((UdAstPtr){.is_data = false, .index = (idx)})

#define ud_ast_ptr_data$(idx) \
    ((UdAstPtr){.is_data = true, .index = (idx)})

#define ud_ast_ptr_null$() \
    ((UdAstPtr){.is_data = false, .index = 0})

#define is_ud_ast_null$(ptr) \
    ((ptr).index == 0 && (ptr).is_data == false)

#define ud_ast_root$() \
    (ud_ast_ptr_node$(0))

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

typedef enum
{
    UD_AST_DATA_NULL,
    UD_AST_DATA_BOOL,
    UD_AST_DATA_CHAR,
    UD_AST_DATA_STR,
    UD_AST_DATA_INTEGER,
    UD_AST_DATA_FLOATING,
} UdAstDataTypes;

typedef struct
{
    char data_type;
    union
    {
        bool boolean;
        Str string;
        int integer;
        float floating;
    };
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
#define UD_AST_ROOT (ud_ast_ptr_node$(0))

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
