#ifndef OPERATIONS
#define OPERATIONS

#include <stdio.h>

#include "tree_common.h"
#include "variable_parse.h"
#include "tree_error_types.h"

void  free_subtree(node_t* node);
size_t count_tree_nodes(node_t* node);
tree_error_type evaluate_tree(tree_t* tree, variable_table* var_table, double* result);
tree_error_type execute_assignment(node_t* assignment, variable_table* var_table, double* result);
node_t* create_node(node_type type, value_of_tree_element data, node_t* left, node_t* right);


#endif // OPERATIONS
