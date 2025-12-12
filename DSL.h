#ifndef DSL_H
#define DSL_H

#include "tree_base.h"
#include "operations.h"
#include "tree_common.h"
#include "variable_parse.h"

// ==================== СОЗДАНИЕ УЗЛОВ ====================
#define NUM(val)     create_node(NODE_NUM, (value_of_tree_element){.num_value = (val)}, NULL, NULL)
#define VAR(var_name) create_node(NODE_VAR, (value_of_tree_element){.var_definition = \
                            {.name = strdup(var_name), .hash = compute_hash(var_name)}}, NULL, NULL)

// Бинарные операции
#define ADD(left, right) create_node(NODE_OP, (value_of_tree_element){.op_value = OP_ADD}, (left), (right))
#define SUB(left, right) create_node(NODE_OP, (value_of_tree_element){.op_value = OP_SUB}, (left), (right))
#define MUL(left, right) create_node(NODE_OP, (value_of_tree_element){.op_value = OP_MUL}, (left), (right))
#define DIV(left, right) create_node(NODE_OP, (value_of_tree_element){.op_value = OP_DIV}, (left), (right))
#define POW(left, right) create_node(NODE_OP, (value_of_tree_element){.op_value = OP_POW}, (left), (right))

// Унарные операции (только нужные)
#define LN(arg)     create_node(NODE_OP, (value_of_tree_element){.op_value = OP_LN},     NULL, (arg))
#define EXP(arg)    create_node(NODE_OP, (value_of_tree_element){.op_value = OP_EXP},    NULL, (arg))
#define SQRT(x)     create_node(NODE_OP, (value_of_tree_element){.op_value = OP_POW}, x, NUM(0.5))

// Присваивание (новый тип узла)
#define ASSIGN(var, expr) create_node(NODE_ASSIGN, (value_of_tree_element){0}, (var), (expr))

// ==================== УТИЛИТЫ ====================
#define FREE_NODES(count, ...) \
    do { \
        node_t* nodes[] = {__VA_ARGS__}; \
        for (size_t i = 0; i < (count) && i < sizeof(nodes)/sizeof(nodes[0]); i++) \
            if (nodes[i]) free_subtree(nodes[i]); \
    } while(0)


#define CHECK_SYMBOL(type, target_symbol) \
    do { \
        if (check_symbol(&string, tokens, &current_index, (target_symbol), (type))) \
            continue; \
    } while(0)

#endif // DSL_H
