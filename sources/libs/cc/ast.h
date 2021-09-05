#pragma once

#include <brutal/base.h>
#include <brutal/ds.h>

typedef struct cg_initializer CGInitializer;
typedef struct cg_declarator CGDeclarator;
typedef struct cg_constant_expression CGConstExpr;
typedef struct cg_expression CGExpression;
typedef struct cg_declaration_specifier CGDeclarationSpecifier;
typedef struct cg_type_specifier_qualifier CGTypeSpecifierQualifier;

typedef struct cg_type_name CGTypeName;
typedef struct cg_abstract_declarator CGAbstractDeclarator;

// expression
typedef struct cg_assignment_expression CGAssignExpression;
typedef struct cg_unary_expression CGUnaryExpression;
typedef struct cg_postfix_expression CGPostfixExpression;
typedef struct cg_primary_expression CGPrimaryExpression;
typedef struct cg_cast_expression CGCastExpression;
typedef struct cg_multiplicative_expression CGMultiplicativeExpresssion;
typedef struct cg_additive_expression CGAddExpression;
typedef struct cg_shift_expression CGShiftExpression;
typedef struct cg_relational_expression CGRelationalExpression;
typedef struct cg_equality_expression CGEqualityExpression;
typedef struct cg_and_expression CGAndExpression;
typedef struct cg_exclusive_or_expression CGExclusiveOrExpression;
typedef struct cg_inclusive_or_expression CGInclusiveOrExpression;
typedef struct cg_logical_and_expression CGLogicalAndExpression;
typedef struct cg_logical_or_expression CGLogicalOrExpression;
typedef struct cg_conditional_expression CGConditionalExpression;

// 6.4.2 identifier

typedef Str CGIdentifier;

// 6.4.4 constant

typedef enum
{
    CG_CONST_INTEGER,
    CG_CONST_FLOATING,
    CG_CONST_ENUMERATION,
    CG_CONST_CHAR
} CGConstantType;

typedef struct
{
    CGConstantType type;
    union
    {
        long integer;
        float floating;
        Str enumeration;
        char character;
    };
} CGConstant;

// 6.7.3 type-qualifier 
// + 6.7.4 function-specifier 
// + 6.7.5 alignment-specifier
// i combined some qualifier because this simplify some part of the code

typedef enum
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
} CGQualifier;

typedef enum
{
    CG_DECL_ARRAY,     // int vvvv[...]
    CG_DECL_DIRECT,    // int vvvv = 0;
    CG_DECL_FUNC,      // int vvvv(...)
    CG_DECL_FUNC_TYPE, // int (*vvv) (...)
} CGDeclarationType;

// 6.7.2 type specifiers

typedef enum
{
    CG_T_VOID,
    CG_T_CHAR,
    CG_T_SHORT,
    CG_T_INT,
    CG_T_LONG,
    CG_T_DOUBLE,
    CG_T_SIGNED,
    CG_T_UNSIGNED,
    CG_T_BOOL,
    CG_T_STRUCT,
    CG_T_UNION,
    CG_T_ENUM,
    CG_T_TYPEDEF_NAME,
    /* todo: 
    support complexe numbers */
} CGBuiltinTypeSpecifier;

// 6.7.2.1 member-declarator

typedef struct
{
    CGDeclarator *declarator;
    CGConstExpr *bit_size;
} CGMemberDeclarator;

// 6.7.2.1 member-declaration

typedef struct
{
    Vec(CGTypeSpecifierQualifier) type_info;
    Vec(CGMemberDeclarator) members;
} CGMemberDeclaration;

// 6.7.2.1 struct-or-union-specifier

typedef struct
{
    bool is_struct; // or union if false
    CGIdentifier identifier;
    Vec(CGMemberDeclaration) members;
} CGTypeStructOrUnion;

// 6.7.2.2 enumerator

typedef struct
{
    CGIdentifier enumeration_constant;
    CGConstExpr *constant_expression;
} CGEnumerator;

// 6.7.2.2 enum-specifier

typedef struct
{
    CGIdentifier identifier;
    Vec(CGEnumerator) enumerator_list;
} CGEnumSpecifier;

// 6.7.2 type specifier qualifier

struct cg_type_specifier_qualifier
{
    // type-specifier
    CGBuiltinTypeSpecifier specifier;
    union
    {
        CGTypeStructOrUnion struct_or_union;
        CGEnumSpecifier enumeration;
        CGIdentifier typedef_name;
    };
    // type-qualifier
};

// 6.7 declaration
typedef struct
{
    bool has_initializer;
    CGInitializer *initializer;
    CGDeclarator *declarator;
} CGInitDeclarator;

struct cg_declaration_specifier
{
    CGTypeSpecifierQualifier type_specifier;
    CGQualifier qualifier;
};

typedef struct
{
    Vec(CGDeclarationSpecifier) specifier;
    Vec(CGInitDeclarator) init_declarator_list;
} CGDeclaration;

// 6.7.6 declarator 
typedef struct
{
    CGDeclarator *declarator;
    CGConstExpr *array_size;
} CGArrayDeclarator;

typedef struct
{
    CGDeclarationSpecifier attributes;

    bool is_triple_dot; // ...
} CGParameterDeclaration;

typedef struct
{
    Vec(CGParameterDeclaration) parameters;
} CGParameterTypeList;

typedef struct
{
    CGDeclarator *declarator; // for *funcname*(....)
    CGParameterTypeList parameter_list;
} CGFuncDeclarator;

typedef struct
{
    CGDeclarationType type;
    CGDeclarationSpecifier specifier;
    union
    {
        CGDeclarator *func_type_decl;      // like void (*hello);
        CGFuncDeclarator func_declaration; // void hello()
        CGArrayDeclarator array_declarator;
        CGIdentifier direct;
    } declaration;
} CGDirectDeclarator;

typedef struct cg_pointer
{
    CGDeclarationSpecifier specifier;
} CGPointer;

struct cg_declarator
{
    bool is_pointer;

    Vec(CGPointer) pointer;
    CGDirectDeclarator *direct_declarator;
};


// 6.7.7 type-name

typedef struct
{
    CGAbstractDeclarator *declarator;
    CGConstExpr *expression;
} CGAbstractArrayDeclarator;

typedef struct
{
    CGAbstractDeclarator *declator;
    Vec(CGParameterTypeList) parameters;
} CGFunctionAbstractDeclarator;

struct cg_abstract_declarator
{
    Vec(CGPointer) pointer;
    CGAbstractDeclarator *abstract_declarator;
    CGFunctionAbstractDeclarator *func;
    CGAbstractArrayDeclarator *array;
};

struct cg_type_name
{
    Vec(CGTypeSpecifierQualifier) qualifier;
    CGAbstractDeclarator *abstract_declarator;
};
// 6.7.9 Initialization

// [xx] = 10;
// .xx = 10
typedef struct
{
    CGConstExpr *constant_expression;
    CGIdentifier identifier;
} CGDesignator;

// x =
typedef struct
{
    Vec(CGDesignator) designators;
} CGDesignation;

// { [10] = 1, .x = 10,}
typedef struct
{
    CGDesignation designation;
    CGInitializer *initializer;
} CGInitializerEntry;

struct cg_initializer
{
    Vec(CGInitializerEntry) initializer_list;
    CGAssignExpression *expression;
};

// 6.5.1 primary expression

typedef enum
{
    CG_PEXP_IDENTIFIER,
    CG_PEXP_CONSTANT,
    CG_STRING_LITERAL,
    CG_EXPR,
    CG_GENRIC_SELECTION, // not supported
} CGPrimaryExpressionType;

struct cg_primary_expression
{
    CGPrimaryExpressionType type;

    union
    {
        CGIdentifier identifier;
        CGConstant constant;
        Str string_literal;
        CGExpression *expression;
        // generic expression
    };
};

// 6.5.2 postfix expression

typedef enum
{
    CG_PFIX_PRIMARY,        // (primary expression)
    CG_PFIX_ARRAY_GET,      // v[]
    CG_PFIX_CALL,           // v()
    CG_PFIX_MEMBER,         // v.
    CG_PFIX_MEMBER_POINTER, // v->
    CG_PFIX_DPLUS,          // v++;
    CG_PFIX_DMINUS,         // v--;
    CG_PFIX_INIT_STRUCT,    // (type) { ..., ...}
} CGPostFixExpressionType;

struct cg_postfix_expression
{
    CGPostFixExpressionType type;
    union
    {
        CGPrimaryExpression *primary_expression;
        struct
        {

            CGPostfixExpression *next;
            CGIdentifier identifier;
        } member_get;

        struct
        {
            CGPostfixExpression *next;
            CGExpression *array_indice;
        } array_get;

        struct
        {
            CGPostfixExpression *next;
            Vec(CGAssignExpression) args;
        } func_get;

        struct
        {
            CGTypeName *type;
            Vec(CGInitializerEntry) initializer_list;
        } structure;
    };
};
// 6.5.3 unary expression

typedef enum
{
    CG_UNARY_OP_AND,     // &
    CG_UNARY_OP_STAR,    // *
    CG_UNARY_OP_PLUS,    // +
    CG_UNARY_OP_MINUS,   // -
    CG_UNARY_OP_NOT,     // !
    CG_UNARy_OP_NOT_BIT, // ~
} CGUnaryOperator;
typedef enum
{
    CG_UNARY_POSTFIX,
    CG_DPLUS_UNARY,  // ++(unary)
    CG_DMINUS_UNARY, // --(unary)
    CG_UNARY_CAST,   // !(unary)  -(unary)   ~(unary)
    CG_UNARY_SIZEOF,
    CG_UNARY_ALIGNOF,
} CGUnaryExpressionType;

struct cg_unary_expression
{
    CGUnaryExpressionType type;
    union
    {
        CGUnaryExpression *unary_expr;
        CGPostfixExpression *postfix_expr;
        CGTypeName type_name;
        struct
        {
            CGUnaryOperator operator;
            CGCastExpression *cast_expr;
        } cast_expression;
    };
};

// 6.5.4 cast-expression

struct cg_cast_expression
{
    CGUnaryExpression *expression;
    CGCastExpression *cast_expr;
    CGTypeName *type_name;
};

// 6.5.5 multiplicative expression

typedef enum
{
    CG_MUL_MUL, // * //  yes it's a multiplication of multiplication
    CG_MUL_DIV, // /
    CG_MUL_MOD, // %
} CGMultiplicativeExpresssionType;

struct cg_multiplicative_expression
{
    CGMultiplicativeExpresssionType type;
    CGMultiplicativeExpresssion *left;
    CGCastExpression *right;
};

// 6.5.6 additive expression

struct cg_additive_expression
{
    CGMultiplicativeExpresssion *logical_right;
    CGAddExpression *logical_left;
    bool is_add; // + or -
};

// 6.5.7 shift expression

struct cg_shift_expression
{
    CGAddExpression *logical_right;
    CGShiftExpression *logical_left;
    bool shift_left; // << or >>
};

// 6.5.8 relational expression

typedef enum
{
    CG_REL_LOWER_THAN,
    CG_REL_HIGHER_THAN,
    CG_REL_LOWER_EQUAL_THAN,
    CG_REL_HIGHER_EQUAL_THAN
} CGRelationalExpressionType;

struct cg_relational_expression
{
    CGShiftExpression *logical_right;
    CGRelationalExpressionType type;
    CGRelationalExpression *logical_left;
};
// 6.5.9  equality expression

struct cg_equality_expression
{
    CGRelationalExpression *logical_right;
    bool not_equal; // == or !=
    CGEqualityExpression *logical_left;
};
// 6.5.10 and expression

struct cg_and_expression
{
    CGEqualityExpression *logical_right;
    CGAndExpression *logical_left;
};

// 6.5.11 exclusive or expression

struct cg_exclusive_or_expression
{
    CGAndExpression *logical_right;
    CGExclusiveOrExpression *logical_left;
};

// 6.5.12 inclusive or expression

struct cg_inclusive_or_expression
{
    CGExclusiveOrExpression *logical_right;
    CGInclusiveOrExpression *logical_left;
};

// 6.5.13 logical and expression

struct cg_logical_and_expression
{
    CGInclusiveOrExpression *logical_right;
    CGLogicalAndExpression *logical_left;
};

// 6.5.14 logical or expression

struct cg_logical_or_expression
{
    CGLogicalAndExpression *logical_right;
    CGLogicalOrExpression *logical_left;
};

// 6.5.15 conditional expression

struct cg_conditional_expression
{

    CGExpression *logical_left;
    CGConditionalExpression *logical_right;
};
// 6.5.16 Assignement operators

typedef enum
{
    CG_OP_EQUAL,        // =
    CG_OP_MUL_EQUAL,    // *=
    CG_OP_DIV_EQUAL,    // /=
    CG_OP_MOD_EQUAL,    // %=
    CG_OP_MIN_EQUAL,    // -=
    CG_OP_SHIFTL_EQUAL, // <<=
    CG_OP_SHIFTR_EQUAL, // >>=
    CG_OP_AND_EQUAL,    // &=
    CG_OP_XOR_EQUAL,    // ^=
    CG_OP_OR_EQUAL,     // |=
} CGAssignementOperator;

struct cg_assignment_expression
{
    bool is_assignement;
    union
    {

        CGConditionalExpression *cond_expression;
        struct
        {
            CGUnaryExpression *unary_expr;
            CGAssignementOperator assign_op;
            CGAssignExpression *expression;
        } assignement;
    };

};

// 6.5.17 Expression

struct cg_expression
{
    Vec(CGAssignExpression) assign_expression;
};

// 6.6 constant-expression

struct cg_constant_expression
{
    CGConditionalExpression *expression;
};