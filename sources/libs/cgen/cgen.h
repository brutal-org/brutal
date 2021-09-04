#pragma once
#include <brutal/base.h>
#include <brutal/ds.h>




typedef struct cg_initializer CGInitializer;
typedef struct cg_declarator CGDeclarator;
typedef struct cg_constant_expression CGConstExpr;
typedef Str CGIdentifier;

enum DeclarationSpecifiers
{
    CG_DS_TYPEDEF = 1 << 1,
    CG_DS_EXTERN = 1 << 2,
    CG_DS_STATIC = 1 << 3,
    CG_DS_AUTO = 1 << 4,
    CG_DS_REGISTER = 1 << 5,
    CG_DS_STRUCT = 1 << 6, // in the case of: struct value v;
    CG_DS_UNION = 1 << 7,
    CG_DS_INLINE = 1 << 8,
    CG_DS_CONST = 1 << 9,
    CG_DS_RESTRICT = 1 << 10,
    CG_DS_VOLATILE = 1 << 11,
};

enum DeclarationType 
{
    CG_DECL_ARRAY,      // int vvvv[...]
    CG_DECL_DIRECT,     // int vvvv = 0;
    CG_DECL_FUNC,       // int vvvv(...)
    CG_DECL_FUNC_TYPE,  // int (*vvv) (...)
};

typedef struct
{
    
    CGIdentifier identifier;
} CGType;

typedef struct 
{
    bool has_initializer ;
    CGInitializer* initializer;
    CGDeclarator* declarator;
} CGInitDeclarator;

typedef struct cg_pointer
{
    enum DeclarationSpecifiers specifier;
    struct cg_pointer* pointer;
    bool is_leaf;
} CGPointer;

typedef struct 
{
    CGDeclarator* declarator;
    CGConstExpr* array_size; 
} CGArrayDeclarator;

typedef struct 
{
    enum DeclarationSpecifiers attributes;
    CGDeclarator* declaration;
    bool is_triple_dot; // ... 
} CGParameterDeclaration;

typedef struct 
{
    Vec(CGParameterDeclaration) parameters;
} CGParameterTypeList;

typedef struct 
{
    CGDeclarator* declarator; // for *funcname*(....)
    CGParameterTypeList parameter_list;
} CGFuncDeclarator;

struct cg_declarator 
{
    CGPointer* pointer;
    enum DeclarationType type;
    enum DeclarationSpecifiers specifier;
    union 
    {
        CGInitDeclarator direct_variable;
        CGDeclarator* func_type_decl; 
        CGFuncDeclarator func_declaration;
        CGArrayDeclarator array_declarator;
    } declaration;
};
