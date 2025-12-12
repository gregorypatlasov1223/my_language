#ifndef DUMP_H_
#define DUMP_H_

#include <time.h>
#include <stdio.h>

#include "tree_common.h"
#include "tree_error_types.h"


tree_error_type generate_dot_file(tree_t* tree, const char* filename);
void create_node_recursive(node_t* node, tree_t* tree, FILE* dot_file);
void create_dot_nodes(tree_t* tree, FILE* dot_file);
void create_tree_connections(node_t* node, FILE* dot_file);
const char* get_node_color(node_t* node, tree_t* tree);
tree_error_type write_tree_common_picture(tree_t* tree, FILE* htm_file, const char* folder_path, const char* folder_name);
void write_dump_header(FILE* htm_file, time_t now, const char* comment);
void write_dump_footer(FILE* htm_file);
tree_error_type tree_dump_to_htm(tree_t* tree, FILE* htm_file, const char* folder_path, const char* folder_name, const char* comment);
tree_error_type tree_dump(tree_t* tree, const char* filename);
tree_error_type init_tree_log(const char* filename);
tree_error_type close_tree_log(const char* filename);
tree_verify_result verify_tree(tree_t* tree);
const char* tree_verify_result_to_string(tree_verify_result result);

#endif // DUMP_H_
