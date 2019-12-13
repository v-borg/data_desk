/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Data Desk

Author  : Ryan Fleury
Updated : 5 December 2019
License : MIT, at end of file.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef DATA_DESK_H_INCLUDED_
#define DATA_DESK_H_INCLUDED_

#define DATA_DESK_VERSION_MAJOR 1
#define DATA_DESK_VERSION_MINOR 0
#define DATA_DESK_VERSION_PATCH 0
#define DATA_DESK_STRINGIFY_(a) #a
#define DATA_DESK_STRINGIFY(a) DATA_DESK_STRINGIFY_(a)
#define DATA_DESK_VERSION_STRING DATA_DESK_STRINGIFY(DATA_DESK_VERSION_MAJOR) "." DATA_DESK_STRINGIFY(DATA_DESK_VERSION_MINOR) "." DATA_DESK_STRINGIFY(DATA_DESK_VERSION_PATCH)

#ifndef DATA_DESK_NO_CRT
#include <stdio.h>
#endif

#if defined(_MSC_VER)
#define DATA_DESK_EXPORT __declspec(dllexport)
#else
#define DATA_DESK_EXPORT
#endif

#if defined(__cplusplus)
#define DATA_DESK_EXTERN_C extern "C"
#else
#define DATA_DESK_EXTERN_C
#endif

#define DATA_DESK_FUNC DATA_DESK_EXPORT DATA_DESK_EXTERN_C





/*
| /////////////////////////////////////////////////////////////////
|  Callback Structs/Functions
| /////////////////////////////////////////////////////////////////
|
| These are structures and function typedefs that correspond to
| the callbacks that a custom layer can receive. A custom layer
| doesn't need to define every possible callback. The symbol
| names that Data Desk expects for each callback are listed
| next to the corresponding function typedef.
*/

typedef struct DataDeskNode DataDeskNode;

/* DataDeskCustomInitCallback */
typedef void DataDeskInitCallback(void);

/* DataDeskCustomParseCallback */
typedef void DataDeskParseCallback(DataDeskNode *root, char *filename);

/* DataDeskCustomCleanUpCallback */
typedef void DataDeskCleanUpCallback(void);





/*
| /////////////////////////////////////////////////////////////////
|  Abstract Syntax Tree Representation
| /////////////////////////////////////////////////////////////////
|
| The following code outlines the general structure for the
| abstract syntax trees that Data Desk generates. Each sub-struct
| contained inside of the DataDeskNode struct is only safe to
| access if the "int type" variable in the struct is set to the
| corresponding type value. All of the type constants are defined
| in the following enum.
*/

typedef enum DataDeskNodeType DataDeskNodeType;
enum DataDeskNodeType
{
    DATA_DESK_NODE_TYPE_invalid,
    
    DATA_DESK_NODE_TYPE_identifier,
    DATA_DESK_NODE_TYPE_numeric_constant,
    DATA_DESK_NODE_TYPE_string_constant,
    DATA_DESK_NODE_TYPE_char_constant,
    DATA_DESK_NODE_TYPE_unary_operator,
    DATA_DESK_NODE_TYPE_binary_operator,

    DATA_DESK_NODE_TYPE_struct_declaration,
    DATA_DESK_NODE_TYPE_union_declaration,
    DATA_DESK_NODE_TYPE_enum_declaration,
    DATA_DESK_NODE_TYPE_flags_declaration,
    DATA_DESK_NODE_TYPE_declaration,
    DATA_DESK_NODE_TYPE_type_usage,
    DATA_DESK_NODE_TYPE_tag,
    DATA_DESK_NODE_TYPE_constant_definition,
    DATA_DESK_NODE_TYPE_procedure_header,
};

// NOTE(rjf): The unary operator precedence table in UnaryOperatorPrecedence
// must update to match this when this changes, and also the DataDeskGetUnaryOperatorString
// procedure in this file.
typedef enum DataDeskUnaryOperatorType DataDeskUnaryOperatorType;
enum DataDeskUnaryOperatorType
{
    DATA_DESK_UNARY_OPERATOR_TYPE_invalid,
    DATA_DESK_UNARY_OPERATOR_TYPE_negative,
    DATA_DESK_UNARY_OPERATOR_TYPE_not,
    DATA_DESK_UNARY_OPERATOR_TYPE_bitwise_negate,
    DATA_DESK_UNARY_OPERATOR_TYPE_MAX
};

// NOTE(rjf): The binary operator precedence table in BinaryOperatorPrecedence
// must update to match this when this changes, and also the DataDeskGetBinaryOperatorString
// procedure in this file.
typedef enum DataDeskBinaryOperatorType DataDeskBinaryOperatorType;
enum DataDeskBinaryOperatorType
{
    DATA_DESK_BINARY_OPERATOR_TYPE_invalid,
    DATA_DESK_BINARY_OPERATOR_TYPE_add,
    DATA_DESK_BINARY_OPERATOR_TYPE_subtract,
    DATA_DESK_BINARY_OPERATOR_TYPE_multiply,
    DATA_DESK_BINARY_OPERATOR_TYPE_divide,
    DATA_DESK_BINARY_OPERATOR_TYPE_modulus,
    DATA_DESK_BINARY_OPERATOR_TYPE_bitshift_left,
    DATA_DESK_BINARY_OPERATOR_TYPE_bitshift_right,
    DATA_DESK_BINARY_OPERATOR_TYPE_bitwise_and,
    DATA_DESK_BINARY_OPERATOR_TYPE_bitwise_or,
    DATA_DESK_BINARY_OPERATOR_TYPE_boolean_and,
    DATA_DESK_BINARY_OPERATOR_TYPE_boolean_or,
    DATA_DESK_BINARY_OPERATOR_TYPE_MAX
};

struct DataDeskNode
{
    DataDeskNodeType type;
    DataDeskNode *next;

    char *string;
    int string_length;

    DataDeskNode *first_tag;

    union
    {
        struct Identifier
        {
            DataDeskNode *declaration;
        }
        identifier;

        struct UnaryOperator
        {
            DataDeskUnaryOperatorType type;
            DataDeskNode *operand;
        }
        unary_operator;

        struct BinaryOperator
        {
            DataDeskBinaryOperatorType type;
            DataDeskNode *left;
            DataDeskNode *right;
        }
        binary_operator;

        struct StructDeclaration
        {
            DataDeskNode *first_member;
        }
        struct_declaration;

        struct UnionDeclaration
        {
            DataDeskNode *first_member;
        }
        union_declaration;

        struct EnumDeclaration
        {
            DataDeskNode *first_constant;
        }
        enum_declaration;

        struct FlagsDeclaration
        {
            DataDeskNode *first_flag;
        }
        flags_declaration;

        struct Declaration
        {
            DataDeskNode *type;
            DataDeskNode *initialization;
        }
        declaration;

        struct TypeUsage
        {
            int pointer_count;
            DataDeskNode *first_array_size_expression;
            DataDeskNode *struct_declaration;
            DataDeskNode *union_declaration;
            DataDeskNode *type_definition;
        }
        type_usage;

        struct Tag
        {
            DataDeskNode *first_tag_parameter;
        }
        tag;

        struct ConstantDefinition
        {
            DataDeskNode *expression;
        }
        constant_definition;

        struct ProcedureHeader
        {
            DataDeskNode *return_type;
            DataDeskNode *first_parameter;
        }
        procedure_header;
    };
};





/*
| /////////////////////////////////////////////////////////////////
|  Introspection Helper Functions
| /////////////////////////////////////////////////////////////////
|
| The following function prototypes are convenience utility
| functions that can be helpful when introspecting upon abstract
| syntax trees.
*/

inline int DataDeskStringHasSubString(char *string, char *substring);
inline DataDeskNode *DataDeskGetNodeTag(DataDeskNode *root, char *tag);
inline DataDeskNode *DataDeskGetTagParameter(DataDeskNode *tag, int parameter_number);
inline int DataDeskNodeHasTag(DataDeskNode *root, char *tag);
inline int DataDeskDeclarationIsType(DataDeskNode *root, char *type);
inline int DataDeskStructMemberIsType(DataDeskNode *root, char *type);
inline int DataDeskInterpretNumericExpressionAsInteger(DataDeskNode *root);
inline char *DataDeskGetBinaryOperatorString(int type);

#ifndef DATA_DESK_NO_CRT
inline void DataDeskFWriteGraphAsC(FILE *file, DataDeskNode *root, int follow_next);
inline void DataDeskFWriteStringWithSpaces(FILE *file, char *string);
inline void DataDeskFWriteStringAsLowercaseWithUnderscores(FILE *file, char *string);
inline void DataDeskFWriteStringAsUppercaseWithUnderscores(FILE *file, char *string);
inline void DataDeskFWriteStringAsUpperCamelCase(FILE *file, char *string);
inline void DataDeskFWriteStringAsLowerCamelCase(FILE *file, char *string);
inline void DataDeskFWriteStringWithSpacesN(FILE *file, char *string, int string_length);
inline void DataDeskFWriteStringAsLowercaseWithUnderscoresN(FILE *file, char *string, int string_length);
inline void DataDeskFWriteStringAsUppercaseWithUnderscoresN(FILE *file, char *string, int string_length);
inline void DataDeskFWriteStringAsUpperCamelCaseN(FILE *file, char *string, int string_length);
inline void DataDeskFWriteStringAsLowerCamelCaseN(FILE *file, char *string, int string_length);
#endif





/*
| /////////////////////////////////////////////////////////////////
|  Helper Function Implementation Code
| /////////////////////////////////////////////////////////////////
*/

inline int
DataDeskCharIsAlpha(int c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

inline int
DataDeskCharIsDigit(int c)
{
    return (c >= '0' && c <= '9');
}

inline int
DataDeskCharIsLowercaseAlpha(int c)
{
    return (c >= 'a' && c <= 'z');
}

inline int
DataDeskCharIsUppercaseAlpha(int c)
{
    return (c >= 'A' && c <= 'Z');
}

inline int
DataDeskCharToLower(int c)
{
    return (DataDeskCharIsUppercaseAlpha(c) ? c + 32 : c);
}

inline int
DataDeskCharToUpper(int c)
{
    return (DataDeskCharIsLowercaseAlpha(c) ? c - 32 : c);
}

inline int
DataDeskStringHasAlphanumericBlock(char *string, char *substring)
{
    int matches = 0;

    if(string && substring)
    {
        for(int i = 0; string[i]; ++i)
        {
            if(string[i] == substring[0])
            {
                matches = 1;

                int string_i = i+1;
                int substring_i = 1;
                for(;;)
                {
                    if(!substring[substring_i])
                    {
                        if(substring_i > 0 &&
                           (DataDeskCharIsAlpha(substring[substring_i-1]) ||
                            DataDeskCharIsDigit(substring[substring_i-1]) ||
                            substring[substring_i-1] == '_'))
                        {
                            if(DataDeskCharIsAlpha(string[string_i]) ||
                               DataDeskCharIsDigit(string[string_i]) ||
                               string[string_i] == '_')
                            {
                                matches = 0;
                            }
                        }
                        break;
                    }

                    if(string[string_i] != substring[substring_i])
                    {
                        matches = 0;
                        break;
                    }

                    ++string_i;
                    ++substring_i;
                }
            }

            if(matches)
            {
                break;
            }
        }
    }

    return matches;
}

inline int
DataDeskStringHasSubString(char *string, char *substring)
{
    return DataDeskStringHasAlphanumericBlock(string, substring);
}

inline DataDeskNode *
DataDeskGetNodeTag(DataDeskNode *root, char *tag)
{
    DataDeskNode *found_tag_node = 0;
    for(DataDeskNode *tag_node = root->first_tag;
        tag_node; tag_node = tag_node->next)
    {
        if(DataDeskStringHasAlphanumericBlock(tag_node->string, tag))
        {
            found_tag_node = tag_node;
            break;
        }
    }
    return found_tag_node;
}

inline DataDeskNode *
DataDeskGetTagParameter(DataDeskNode *tag, int parameter_number)
{
    DataDeskNode *result = 0;
    if(tag && tag->type == DATA_DESK_NODE_TYPE_tag)
    {
        int i = 0;
        for(DataDeskNode *parameter = tag->tag.first_tag_parameter;
            parameter; parameter = parameter->next)
        {
            if(i == parameter_number)
            {
                result = parameter;
                break;
            }
            ++i;
        }
    }
    return result;
}

inline int
DataDeskNodeHasTag(DataDeskNode *node, char *tag)
{
    DataDeskNode *tag_node = DataDeskGetNodeTag(node, tag);
    return tag_node != 0;
}

inline int
DataDeskDeclarationIsType(DataDeskNode *root, char *type)
{
    int matches = 0;

    if(root->type == DATA_DESK_NODE_TYPE_declaration)
    {
        int pointer_count = 0;
        char *type_name = type;

        for(int i = 0; type[i]; ++i)
        {
            if(type[i] == '*')
            {
                ++pointer_count;
            }
            else
            {
                type_name = type+i;
                break;
            }
        }

        matches = pointer_count == root->declaration.type->type_usage.pointer_count;
        if(matches)
        {
            matches = 0;
            if(root->declaration.type->string)
            {
                matches = 1;
                for(int i = 0; type_name[i] && root->declaration.type->string[i]; ++i)
                {
                    if(type_name[i] != root->declaration.type->string[i])
                    {
                        matches = 0;
                        break;
                    }
                }
            }
        }
    }

    return matches;
}

inline int
DataDeskStructMemberIsType(DataDeskNode *root, char *type)
{
    return DataDeskDeclarationIsType(root, type);
}

inline int
DataDeskCStringToInt(char *string)
{
    int value = 0;

    char value_str[64] = {0};
    int value_str_write_pos = 0;
    for(int i = 0; string[i]; ++i)
    {
        if(DataDeskCharIsDigit(string[i]))
        {
            for(int j = i; string[j]; ++j)
            {
                if(DataDeskCharIsDigit(string[j]))
                {
                    value_str[value_str_write_pos++] = string[j];
                }
                else
                {
                    break;
                }
            }

            int number_of_digits = value_str_write_pos;
            int multiplier = 1;
            for(int j = 0; j < number_of_digits; ++j)
            {
                value += (value_str[number_of_digits - j - 1] - '0') * multiplier;
                multiplier *= 10;
            }

            break;
        }
    }

    return value;
}

inline int
DataDeskInterpretNumericExpressionAsInteger(DataDeskNode *root)
{
    int result = 0;
    if(root)
    {
        switch(root->type)
        {
            case DATA_DESK_NODE_TYPE_numeric_constant:
            {
                result = DataDeskCStringToInt(root->string);
                break;
            }
            case DATA_DESK_NODE_TYPE_unary_operator:
            {
                DataDeskUnaryOperatorType unary_operator_type = root->unary_operator.type;
                int operand = DataDeskInterpretNumericExpressionAsInteger(root->unary_operator.operand);

                switch(unary_operator_type)
                {
                    case DATA_DESK_UNARY_OPERATOR_TYPE_not:
                    {
                        result = !operand;
                        break;
                    }
                    case DATA_DESK_UNARY_OPERATOR_TYPE_negative:
                    {
                        result = -operand;
                        break;
                    }
                    case DATA_DESK_UNARY_OPERATOR_TYPE_bitwise_negate:
                    {
                        result = ~operand;
                        break;
                    }
                    default: break;
                }

                break;
            }
            case DATA_DESK_NODE_TYPE_binary_operator:
            {
                DataDeskBinaryOperatorType binary_operator_type = root->binary_operator.type;
                int left_tree = DataDeskInterpretNumericExpressionAsInteger(root->binary_operator.left);
                int right_tree = DataDeskInterpretNumericExpressionAsInteger(root->binary_operator.right);

                switch(binary_operator_type)
                {
                    case DATA_DESK_BINARY_OPERATOR_TYPE_add:            { result = left_tree + right_tree; break; }
                    case DATA_DESK_BINARY_OPERATOR_TYPE_subtract:       { result = left_tree - right_tree; break; }
                    case DATA_DESK_BINARY_OPERATOR_TYPE_multiply:       { result = left_tree * right_tree; break; }
                    case DATA_DESK_BINARY_OPERATOR_TYPE_divide:         { result = (right_tree != 0) ? (left_tree / right_tree) : 0; break; }
                    case DATA_DESK_BINARY_OPERATOR_TYPE_modulus:        { result = (right_tree != 0) ? (left_tree % right_tree) : 0; break; }
                    case DATA_DESK_BINARY_OPERATOR_TYPE_bitshift_left:  { result = left_tree << right_tree; break; }
                    case DATA_DESK_BINARY_OPERATOR_TYPE_bitshift_right: { result = left_tree >> right_tree; break; }
                    default: break;
                }

                break;
            }
            default: break;
        }
    }
    return result;
}

inline char *
DataDeskGetBinaryOperatorString(int type)
{
    char *strings[] =
    {
        "(invalid)",
        "+",
        "-",
        "*",
        "/",
        "%",
        "<<",
        ">>",
        "&",
        "|",
        "&&",
        "||",
    };
    return strings[type];
}

inline char *
DataDeskGetUnaryOperatorString(int type)
{
    char *strings[] =
    {
        "(invalid)",
        "-",
        "!",
        "~",
    };
    return strings[type];
}

#ifndef DATA_DESK_NO_CRT
inline void
_DataDeskFWriteGraphAsC(FILE *file, DataDeskNode *root, int follow_next, int nest)
{
    if(root)
    {
        if(root->first_tag)
        {
            fprintf(file, "// ");
            for(DataDeskNode *tag = root->first_tag; tag; tag = tag->next)
            {
                fprintf(file, "%s", tag->string);

                if(tag->tag.first_tag_parameter)
                {
                    fprintf(file, "(");
                    for(DataDeskNode *tag_arg = tag->tag.first_tag_parameter; tag_arg; tag_arg = tag_arg->next)
                    {
                        _DataDeskFWriteGraphAsC(file, tag_arg, 0, nest+1);
                        if(tag_arg->next)
                        {
                            fprintf(file, ", ");
                        }
                    }
                    fprintf(file, ")");
                }
                fprintf(file, " ");
            }
            fprintf(file, "\n");
        }

        switch(root->type)
        {
            case DATA_DESK_NODE_TYPE_identifier:
            case DATA_DESK_NODE_TYPE_numeric_constant:
            case DATA_DESK_NODE_TYPE_string_constant:
            case DATA_DESK_NODE_TYPE_char_constant:
            {
                fprintf(file, "%s", root->string);
                break;
            }

            case DATA_DESK_NODE_TYPE_unary_operator:
            {
                fprintf(file, "(");
                char *unary_operator_string = DataDeskGetUnaryOperatorString(root->unary_operator.type);
                fprintf(file, "%s", unary_operator_string);
                fprintf(file, "(");
                _DataDeskFWriteGraphAsC(file, root->unary_operator.operand, 0, nest+1);
                fprintf(file, ")");
                fprintf(file, ")");
                break;
            }

            case DATA_DESK_NODE_TYPE_binary_operator:
            {
                fprintf(file, "(");
                _DataDeskFWriteGraphAsC(file, root->binary_operator.left, 0, nest);
                char *binary_operator_string = DataDeskGetBinaryOperatorString(root->binary_operator.type);
                fprintf(file, "%s", binary_operator_string);
                _DataDeskFWriteGraphAsC(file, root->binary_operator.right, 0, nest+1);
                fprintf(file, ")");

                break;
            }

            case DATA_DESK_NODE_TYPE_struct_declaration:
            {
                if(nest == 0)
                {
                    fprintf(file, "typedef struct %s %s;\n", root->string, root->string);
                }

                if(root->string)
                {
                    fprintf(file, "struct %s\n{\n", root->string);
                }
                else
                {
                    fprintf(file, "struct\n{\n");
                }

                for(DataDeskNode *member = root->struct_declaration.first_member;
                    member; member = member->next)
                {
                    _DataDeskFWriteGraphAsC(file, member, 0, nest+1);
                    fprintf(file, ";\n");
                }
                fprintf(file, "}");

                if(nest == 0)
                {
                    fprintf(file, ";\n\n");
                }

                break;
            }

            case DATA_DESK_NODE_TYPE_union_declaration:
            {
                if(nest == 0)
                {
                    fprintf(file, "typedef union %s %s;\n", root->string, root->string);
                }

                if(root->string)
                {
                    fprintf(file, "union %s\n{\n", root->string);
                }
                else
                {
                    fprintf(file, "union\n{\n");
                }

                for(DataDeskNode *member = root->union_declaration.first_member;
                    member; member = member->next)
                {
                    _DataDeskFWriteGraphAsC(file, member, 0, nest+1);
                    fprintf(file, ";\n");
                }
                fprintf(file, "}");

                if(nest == 0)
                {
                    fprintf(file, ";\n\n");
                }

                break;
            }

            case DATA_DESK_NODE_TYPE_enum_declaration:
            {
                if(nest == 0)
                {
                    fprintf(file, "typedef %s %s %s;\n", "enum", root->string, root->string);
                }

                if(root->string)
                {
                    fprintf(file, "enum %s\n{\n", root->string);
                }
                else
                {
                    fprintf(file, "enum\n{\n");
                }

                for(DataDeskNode *member = root->enum_declaration.first_constant;
                    member;
                    member = member->next)
                {
                    fprintf(file, "%s", member->string);
                    fprintf(file, ",\n");
                }
                fprintf(file, "}");

                fprintf(file, ";\n\n");

                break;
            }

            case DATA_DESK_NODE_TYPE_flags_declaration:
            {
                int needed_bits_for_flag_type = 32;
                int current_bit = 0;
                for(DataDeskNode *member = root->flags_declaration.first_flag;
                    member;
                    member = member->next)
                {
                    fprintf(file, "#define %s (1<<%i)\n", member->string, current_bit);
                    ++current_bit;
                }

                if(current_bit >= 31)
                {
                    needed_bits_for_flag_type = 64;
                }

                if(root->string)
                {
                    if(needed_bits_for_flag_type == 32)
                    {
                        fprintf(file, "typedef unsigned int %s;\n\n", root->string);
                    }
                    else if(needed_bits_for_flag_type > 32)
                    {
                        fprintf(file, "typedef unsigned long int %s;\n\n", root->string);
                    }
                }

                break;
            }

            case DATA_DESK_NODE_TYPE_declaration:
            {
                _DataDeskFWriteGraphAsC(file, root->declaration.type, 0, nest+1);
                fprintf(file, "%s", root->string);

                for(DataDeskNode *array = root->declaration.type->type_usage.first_array_size_expression;
                    array;
                    array = array->next)
                {
                    fprintf(file, "[");
                    _DataDeskFWriteGraphAsC(file, array, 0, nest);
                    fprintf(file, "]");
                }

                break;
            }

            case DATA_DESK_NODE_TYPE_type_usage:
            {
                if(root->type_usage.struct_declaration)
                {
                    _DataDeskFWriteGraphAsC(file, root->type_usage.struct_declaration, 0, nest+1);
                    fprintf(file, "\n");
                }
                else
                {
                    fprintf(file, "%s ", root->string);
                }

                for(int i = 0; i < root->type_usage.pointer_count; ++i)
                {
                    fprintf(file, "*");
                }

                break;
            }

            case DATA_DESK_NODE_TYPE_constant_definition:
            {
                fprintf(file, "#define %s (", root->string);
                DataDeskFWriteGraphAsC(file, root->constant_definition.expression, 0);
                fprintf(file, ")\n");
                break;
            }

            case DATA_DESK_NODE_TYPE_procedure_header:
            {
                if(root->procedure_header.return_type)
                {
                    _DataDeskFWriteGraphAsC(file, root->procedure_header.return_type, 0, nest);
                }
                else
                {
                    fprintf(file, "void");
                }
                fprintf(file, " %s(", root->string);
                if(root->procedure_header.first_parameter)
                {
                    for(DataDeskNode *parameter = root->procedure_header.first_parameter;
                        parameter; parameter = parameter->next)
                    {
                        _DataDeskFWriteGraphAsC(file, parameter, 0, nest);
                        if(parameter->next)
                        {
                            fprintf(file, ", ");
                        }
                    }
                }
                else
                {
                    fprintf(file, "void");
                }
                fprintf(file, ");\n");
                break;
            }

            default: break;
        }

        if(root->next && follow_next)
        {
            _DataDeskFWriteGraphAsC(file, root->next, follow_next, nest);
        }
    }
}

inline void
DataDeskFWriteGraphAsC(FILE *file, DataDeskNode *root, int follow_next)
{
    _DataDeskFWriteGraphAsC(file, root, follow_next, 0);
}

inline void
DataDeskFWriteStringWithSpaces(FILE *file, char *string)
{
    int string_length = 0;
    for(; string[string_length]; ++string_length);
    DataDeskFWriteStringWithSpacesN(file, string, string_length);
}

inline void
DataDeskFWriteStringAsLowercaseWithUnderscores(FILE *file, char *string)
{
    int string_length = 0;
    for(; string[string_length]; ++string_length);
    DataDeskFWriteStringAsLowercaseWithUnderscoresN(file, string, string_length);
}

inline void
DataDeskFWriteStringAsUppercaseWithUnderscores(FILE *file, char *string)
{
    int string_length = 0;
    for(; string[string_length]; ++string_length);
    DataDeskFWriteStringAsUppercaseWithUnderscoresN(file, string, string_length);
}

inline void
DataDeskFWriteStringAsUpperCamelCase(FILE *file, char *string)
{
    int string_length = 0;
    for(; string[string_length]; ++string_length);
    DataDeskFWriteStringAsUpperCamelCaseN(file, string, string_length);
}

inline void
DataDeskFWriteStringAsLowerCamelCase(FILE *file, char *string)
{
    int string_length = 0;
    for(; string[string_length]; ++string_length);
    DataDeskFWriteStringAsLowerCamelCaseN(file, string, string_length);
}

inline void
DataDeskFWriteStringWithSpacesN(FILE *file, char *string, int string_length)
{
    for(int i = 0; i < string_length && string[i]; ++i)
    {
        if(string[i] != '_')
        {
            fprintf(file, "%c", string[i]);
            if(DataDeskCharIsUppercaseAlpha(string[i+1]))
            {
                fprintf(file, " ");
            }
            else if(string[i+1] == '_')
            {
                fprintf(file, " ");
            }
        }
    }
}

inline void
DataDeskFWriteStringAsLowercaseWithUnderscoresN(FILE *file, char *string, int string_length)
{
    for(int i = 0; i < string_length && string[i]; ++i)
    {
        fprintf(file, "%c", DataDeskCharToLower(string[i]));
        if(DataDeskCharIsUppercaseAlpha(string[i+1]))
        {
            fprintf(file, "_");
        }
    }
}

inline void
DataDeskFWriteStringAsUppercaseWithUnderscoresN(FILE *file, char *string, int string_length)
{
    for(int i = 0; i < string_length && string[i]; ++i)
    {
        fprintf(file, "%c", DataDeskCharToUpper(string[i]));
        if(DataDeskCharIsUppercaseAlpha(string[i+1]))
        {
            fprintf(file, "_");
        }
    }
}

inline void
DataDeskFWriteStringAsUpperCamelCaseN(FILE *file, char *string, int string_length)
{
    int needs_uppercase = 1;
    for(int i = 0; i < string_length && string[i]; ++i)
    {
        if(string[i] != '_')
        {
            if(string[i+1] == '_')
            {
                needs_uppercase = 1;
            }
        }

        if(needs_uppercase)
        {
            fprintf(file, "%c", DataDeskCharToUpper(string[i]));
            needs_uppercase = 0;
        }
        else
        {
            fprintf(file, "%c", string[i]);
        }
    }
}

inline void
DataDeskFWriteStringAsLowerCamelCaseN(FILE *file, char *string, int string_length)
{
    int needs_uppercase = 0;
    int need_first_lowercase = 1;

    for(int i = 0; i < string_length && string[i]; ++i)
    {
        if(string[i] != '_')
        {
            if(string[i+1] == '_')
            {
                needs_uppercase = 1;
            }
        }

        if(needs_uppercase)
        {
            fprintf(file, "%c", DataDeskCharToUpper(string[i]));
            needs_uppercase = 0;
        }
        else
        {
            if(need_first_lowercase)
            {
                need_first_lowercase = 0;
                fprintf(file, "%c", DataDeskCharToLower(string[i]));
            }
            else
            {
                fprintf(file, "%c", string[i]);
            }
        }
    }
}

#endif // DATA_DESK_NO_CRT

#endif // DATA_DESK_H_INCLUDED_

/*
Copyright 2019 Ryan Fleury

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
