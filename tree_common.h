#ifndef TREE_COMMON_H_
#define TREE_COMMON_H_

#include <stdlib.h>

const int MAX_VARIABLE_LENGTH      = 32;
const int MAX_NUMBER_OF_VARIABLES  = 64;


#define MAX_LENGTH_OF_ADDRESS 128
#define MAX_LENGTH_OF_ANSWER 1024
#define MAX_PATH_DEPTH 512
#define MAX_LENGTH_OF_FILENAME 256
#define MAX_LENGTH_OF_SYSTEM_COMMAND 512
#define OPERATION_FAILED -1


const char* const DEFAULT_DATA_BASE_FILENAME   = "differenciator_tree.txt";
const char* const GENERAL_FOLDER_NAME_FOR_LOGS = "tree_logs";
const char* const TEX_FILENAME               = "full_analysis.tex";
const int MAX_LENGTH_OF_TEX_EXPRESSION       = 8192;
const int MAX_CUSTOM_NOTATION_LENGTH         = 2048;
const int MAX_TEX_DESCRIPTION_LENGTH         = 256;
const int MAX_FUNC_NAME_LENGTH               = 256;
const int MAX_SIZE_OF_BUFFER                 = 256;
const int INITIAL_CAPACITY                   = 64;
const int COEFFICIENT                        = 2;

enum node_type
{
    NODE_OP,
    NODE_VAR,
    NODE_NUM,
    NODE_IF,
    NODE_WHILE,
    NODE_SEQUENCE,
    NODE_ASSIGN,
    NODE_EMPTY,

    NODE_EQUAL,
    NODE_NOT_EQUAL,
    NODE_LESS,
    NODE_LESS_EQUAL,
    NODE_GREATER,
    NODE_GREATER_EQUAL,

    NODE_AND,
    NODE_OR,
    NODE_NOT,

    NODE_FUNC_DECL,
    NODE_FUNC_CALL,
    NODE_RETURN,
    NODE_PARAM,
    NODE_ARGS
};

enum operation_type
{
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_COUNT
};

struct variable_definition
{
    char* name;
    size_t hash;
};

struct operation_info
{
    unsigned int hash;
    const char* name;
    operation_type op_value;
};

struct func_call_data
{
    char* name;
    node_t* args;
};

union value_of_tree_element
{
    double              num_value;
    operation_type      op_value;
    variable_definition var_definition;
    char*               func_name;
    func_call_data      func_call;
};

struct node_t
{
    value_of_tree_element data;
    node_type             type;
    node_t*               left;
    node_t*               right;
    node_t*               parent;
    int                   priority;  // Приоритет операции (0 для чисел и переменных)
};

struct tree_t
{
    node_t* root;
    size_t  size;
    char*   file_buffer;
};

//=================================== ТАБЛИЦЫ =======================================

struct variable_t
{
    char   name[MAX_VARIABLE_LENGTH];
    size_t hash;
    double value;
    bool   is_defined;
};

struct variable_table
{
    variable_t* variables;
    int         number_of_variables;
    bool        is_sorted;
};

// TODO: для каждой функции своя таблица имен

struct function_info
{
    char* name;
    int param_count;
    char** param_names;
    node_t* body;
    struct function_info* next;
};

struct function_table
{
    function_info* functions;
    int count;
};


struct parser_context
{
    function_table func_table;
    variable_table var_table;
};

#endif // TREE_COMMON_H_
