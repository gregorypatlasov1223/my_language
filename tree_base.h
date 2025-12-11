#ifndef DIFFERENTIATOR_H_
#define DIFFERENTIATOR_H_

#include <stddef.h>
#include <stdio.h>
#include <time.h>

#include "tree_common.h"
#include "tree_error_types.h"

void free_node(node_t* node);
tree_error_type tree_constructor(tree_t* tree);
tree_error_type tree_destructor(tree_t* tree);
size_t compute_hash(const char* string);
void clear_input_buffer();




#endif // DIFFERENTIATOR_H_
