#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "DSL.h"
#include "tree_base.h"
#include "operations.h"
#include "tree_common.h"
#include "variable_parse.h"
#include "logic_functions.h"
#include "tree_error_types.h"


void free_subtree(node_t* node)
{
    if (node == NULL)
        return;

    free_subtree(node -> left);
    free_subtree(node -> right);

    if (node -> type == NODE_VAR && node -> data.var_definition.name != NULL)
        free(node -> data.var_definition.name);

    free(node);
}


static tree_error_type evaluate_tree_recursive(node_t* node, variable_table* var_table, double* result, int depth)
{
    if (node == NULL)
        return TREE_ERROR_NULL_PTR;

    if (result == NULL)
        return TREE_ERROR_NULL_PTR;

    switch (node -> type)
    {
        case NODE_NUM:
            *result = node -> data.num_value;
            return TREE_ERROR_NO;

        case NODE_OP:
            {
                double left_result = 0, right_result = 0;
                tree_error_type error = TREE_ERROR_NO;

                if (node -> right == NULL)
                    return TREE_ERROR_NULL_PTR;

                if (is_binary(node -> data.op_value))
                {
                    if (node -> left == NULL)
                        return TREE_ERROR_NULL_PTR;

                    error = evaluate_tree_recursive(node -> left, var_table, &left_result, depth + 1);
                    if (error != TREE_ERROR_NO)
                        return error;
                }

                error = evaluate_tree_recursive(node -> right, var_table, &right_result, depth + 1);
                if (error != TREE_ERROR_NO)
                    return error;

                switch (node -> data.op_value)
                {
                    case OP_ADD:
                        *result = left_result + right_result;
                        break;
                    case OP_SUB:
                        *result = left_result - right_result;
                        break;
                    case OP_MUL:
                        *result = left_result * right_result;
                        break;
                    case OP_DIV:
                        if (is_zero(right_result))
                            return TREE_ERROR_DIVISION_BY_ZERO;
                        *result = left_result / right_result;
                        break;
                    case OP_SIN:
                        *result = sin(right_result);
                        break;
                    case OP_COS:
                        *result = cos(right_result);
                        break;
                    case OP_POW:
                        *result = pow(left_result, right_result);
                        break;
                    case OP_LN:
                        if (right_result <= 0)
                            return TREE_ERROR_YCHI_MATAN;
                        *result = log(right_result);
                        break;
                    case OP_EXP:
                        *result = exp(right_result);
                        break;
                    default:
                        return TREE_ERROR_UNKNOWN_OPERATION;
                }
                return TREE_ERROR_NO;
            }

        case NODE_VAR:
            {
                if (node -> data.var_definition.name == NULL)
                    return TREE_ERROR_VARIABLE_NOT_FOUND;

                char* var_name = node -> data.var_definition.name;
                double value = 0.0;
                tree_error_type error = get_variable_value(var_table, var_name, &value);

                if (error == TREE_ERROR_VARIABLE_NOT_FOUND)
                {
                    error = add_variable(var_table, var_name);
                    if (error != TREE_ERROR_NO)
                        return error;

                    error = request_variable_value(var_table, var_name);
                    if (error != TREE_ERROR_NO) return error;

                    error = get_variable_value(var_table, var_name, &value);
                    if (error != TREE_ERROR_NO) return error;
                }
                else if (error == TREE_ERROR_VARIABLE_UNDEFINED)
                {
                    error = request_variable_value(var_table, var_name);
                    if (error != TREE_ERROR_NO) return error;

                    error = get_variable_value(var_table, var_name, &value);
                    if (error != TREE_ERROR_NO) return error;
                }
                else if (error != TREE_ERROR_NO)
                {
                    return error;
                }

                *result = value;
                return TREE_ERROR_NO;
            }

        default:
            return TREE_ERROR_UNKNOWN_OPERATION;
    }
}


tree_error_type evaluate_tree(tree_t* tree, variable_table* var_table, double* result)
{
    if (tree == NULL || var_table == NULL || result == NULL)
        return TREE_ERROR_NULL_PTR;

    if (tree -> root == NULL)
        return TREE_ERROR_NULL_PTR;

    return evaluate_tree_recursive(tree -> root, var_table, result, 0);
}


tree_error_type execute_assignment(node_t* assignment, variable_table* var_table, double* result)
{
    if (!assignment || assignment -> type != NODE_ASSIGN || !var_table || !result)
        return TREE_ERROR_NULL_PTR;

    if (assignment -> left -> type != NODE_VAR)
        return TREE_ERROR_INVALID_NODE;

    tree_t expr_tree = {};
    tree_constructor(&expr_tree);
    expr_tree.root = assignment -> right;

    tree_error_type error = evaluate_tree(&expr_tree, var_table, result);

    if (error == TREE_ERROR_NO)
    {
        char* var_name = assignment -> left -> data.var_definition.name;
        set_variable_value(var_table, var_name, *result);
    }

    expr_tree.root = NULL;
    tree_destructor(&expr_tree);

    return error;
}


node_t* create_node(node_type type, value_of_tree_element data, node_t* left, node_t* right)
{
    node_t* node = (node_t*)calloc(1, sizeof(node_t));
    if (!node)
        return NULL;

    node -> type = type;
    node -> left = left;
    node -> right = right;
    node -> parent = NULL;
    node -> data = data;

    switch (type)
    {
        case NODE_NUM:
        case NODE_VAR:
            node -> priority = 0;
            break;

        case NODE_OP:
            switch (data.op_value)
            {
                case OP_ADD:
                case OP_SUB:
                    node -> priority = 1;
                    break;

                case OP_MUL:
                case OP_DIV:
                    node -> priority = 2;
                    break;

                case OP_LN:
                case OP_EXP:
                    node -> priority = 3;
                    break;

                case OP_POW:
                    node -> priority = 4;
                    break;

                default:
                    node -> priority = 0;
                    break;
            }
            break;

        default:
            node -> priority = 0;
            break;
    }

    if (type == NODE_VAR && data.var_definition.name != NULL)
    {
        node -> data.var_definition.name = data.var_definition.name;
        node -> data.var_definition.hash = compute_hash(data.var_definition.name);
    }

    if (left)
        left -> parent = node;
    if (right)
        right -> parent = node;

    return node;
}


static node_t* create_variable_node(const char* name)
{
    if (!name)
        return NULL;

    value_of_tree_element data = {};
    data.var_definition.name = strdup(name);
    if (!data.var_definition.name)
        return NULL;

    data.var_definition.hash = compute_hash(name);
    return create_node(NODE_VAR, data, NULL, NULL);
}


size_t count_tree_nodes(node_t* node)
{
    if (node == NULL)
        return 0;

    return 1 + count_tree_nodes(node -> left) + count_tree_nodes(node -> right);
}

#include "DSL_undef.h"
