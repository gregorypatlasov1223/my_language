#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "dump.h"
#include "tree_base.h"
#include "tree_common.h"
#include "tree_error_types.h"


static const char* node_type_to_string(node_type type)
{
    switch (type)
    {
        case NODE_OP:            return "NODE_OP";
        case NODE_NUM:           return "NODE_NUM";
        case NODE_VAR:           return "NODE_VAR";
        case NODE_IF:            return "NODE_IF";
        case NODE_WHILE:         return "NODE_WHILE";
        case NODE_SEQUENCE:      return "NODE_SEQUENCE";
        case NODE_ASSIGN:        return "NODE_ASSIGN";
        case NODE_EMPTY:         return "NODE_EMPTY";
        case NODE_EQUAL:         return "NODE_EQUAL";
        case NODE_NOT_EQUAL:     return "NODE_NOT_EQUAL";
        case NODE_LESS:          return "NODE_LESS";
        case NODE_LESS_EQUAL:    return "NODE_LESS_EQUAL";
        case NODE_GREATER:       return "NODE_GREATER";
        case NODE_GREATER_EQUAL: return "NODE_GREATER_EQUAL";
        case NODE_AND:           return "NODE_AND";
        case NODE_OR:            return "NODE_OR";
        case NODE_NOT:           return "NODE_NOT";
        case NODE_FUNC_DECL:     return "NODE_FUNC_DECL";
        case NODE_FUNC_CALL:     return "NODE_FUNC_CALL";
        case NODE_RETURN:        return "NODE_RETURN";
        case NODE_PARAM:         return "NODE_PARAM";
        case NODE_ARGS:          return "NODE_ARGS";
        default:                 return "NODE_UNKNOWN";
    }
}


static const char* node_data_to_string(const node_t* node, char* buffer, size_t buffer_size)
{
    if (node == NULL)
        return "NULL";

    switch (node -> type)
    {
        case NODE_OP:
            switch (node -> data.op_value)
            {
                case OP_ADD: return "+";
                case OP_SUB: return "-";
                case OP_MUL: return "*";
                case OP_DIV: return "/";
                default:     return "?OP";
            }

        case NODE_NUM:
            snprintf(buffer, buffer_size, "%.2f", node -> data.num_value);
            return buffer;

        case NODE_VAR:
            snprintf(buffer, buffer_size, "var: %s", node -> data.var_definition.name);
            return buffer;

        case NODE_IF:
            return "IF";

        case NODE_WHILE:
            return "WHILE";

        case NODE_SEQUENCE:
            return "SEQ";

        case NODE_ASSIGN:
            return "=";

        case NODE_EMPTY:
            return "EMPTY";

        case NODE_EQUAL:
            return "==";

        case NODE_NOT_EQUAL:
            return "!=";

        case NODE_LESS:
            return "&lt;";
            
        case NODE_LESS_EQUAL:
            return "&lt;=";

        case NODE_GREATER:
            return "&gt;";

        case NODE_GREATER_EQUAL:
            return "&gt;=";

        case NODE_AND:
            return "&&";

        case NODE_OR:
            return "||";

        case NODE_NOT:
            return "!";

        case NODE_FUNC_DECL:
            if (node -> data.func_name)
                snprintf(buffer, buffer_size, "FUNC: %s", node -> data.func_name);
            else
                return "FUNC_DECL";
            return buffer;

        case NODE_FUNC_CALL:
            if (node -> data.func_call.name)
                snprintf(buffer, buffer_size, "CALL: %s", node -> data.func_call.name);
            else
                return "FUNC_CALL";
            return buffer;

        case NODE_RETURN:
            return "RETURN";

        case NODE_PARAM:
            return "PARAM";

        case NODE_ARGS:
            return "ARGS";

        default:
            snprintf(buffer, buffer_size, "?TYPE:%d", node -> type);
            return buffer;
    }
}


void write_dump_footer(FILE* htm_file)
{
    assert(htm_file);
    fprintf(htm_file, "</div>\n\n");
}


tree_error_type generate_dot_file(tree_t* tree, const char* filename)
{
    assert(tree);
    assert(filename);

    FILE* dot_file = fopen(filename, "w");
    if (!dot_file)
        return TREE_ERROR_OPENING_FILE;

    fprintf(dot_file, "digraph BinaryTree {\n");
    fprintf(dot_file, "    rankdir=TB;\n");
    fprintf(dot_file, "    node [shape=record, style=filled, color=black];\n\n");

    create_dot_nodes(tree, dot_file);

    if (tree -> root != NULL)
        create_tree_connections(tree -> root, dot_file);
    else
        fprintf(dot_file, "    empty [label=\"Empty Tree\", shape=plaintext];\n");

    fprintf(dot_file, "}\n");
    fclose(dot_file);

    return TREE_ERROR_NO;
}


void create_node_recursive(node_t* node, tree_t* tree, FILE* dot_file)
{
    assert(node != NULL);

    const char* color = get_node_color(node, tree);
    const char* shape = "record"; // форма по умолчанию

    char buffer[MAX_SIZE_OF_BUFFER] = {};
    const char* node_data = node_data_to_string(node, buffer, sizeof(buffer));
    const char* node_type_str = node_type_to_string(node -> type);

    fprintf(dot_file, "    node_%p [label=\"{ {type: %s\\ndata: %s} | {address: %p} | {left %p| right %p| parent %p} }\", fillcolor=%s, shape=%s];\n",
            (void*)node, node_type_str, node_data, (void*)node,
            (void*)node -> left, (void*)node -> right, (void*)node -> parent, color, shape);


    create_node_recursive(node -> left,  tree, dot_file);
    create_node_recursive(node -> right, tree, dot_file);
}


void create_dot_nodes(tree_t* tree, FILE* dot_file)
{
    assert(tree     != NULL);
    assert(dot_file != NULL);

    create_node_recursive(tree -> root, tree, dot_file);
}



// void process_child_node(node_t* parent, node_t* child, FILE* dot_file,
//                        const char* color, const char* label, const char* error_style)
// {
//     if (child == NULL)
//         return;
//
//     if (child -> parent == parent)
//     {
//
//         fprintf(dot_file, "    node_%p -> node_%p [color=%s, dir=both, arrowtail=normal, arrowhead=normal, label=\"%s\"];\n",
//                 (void*)parent, (void*)child, color, label);
//     }
//     else
//     {
//
//         fprintf(dot_file, "    node_%p -> node_%p [color=%s, label=\"%s\"];\n",
//                 (void*)parent, (void*)child, color, label);
//
//         fprintf(dot_file, "    error_parent_%p [shape=ellipse, style=filled, fillcolor=orange, label=\"Parent address Error\"];\n",
//                 (void*)child);
//         fprintf(dot_file, "    node_%p -> error_parent_%p [color=red%s];\n",
//                 (void*)child, (void*)child, error_style);
//     }
//
//     create_tree_connections(child, dot_file);
// }


void create_tree_connections(node_t* node, FILE* dot_file)
{
    assert(node != NULL);

    if (node -> left != NULL)
    {
        if (node -> left -> parent == node)
        {
            fprintf(dot_file, "    node_%p -> node_%p [color=blue, dir=both, arrowtail=normal, arrowhead=normal, label=\"L\"];\n",
                    (void*)node, (void*)node -> left);
        }
        else
        {
            fprintf(dot_file, "    node_%p -> node_%p [color=blue, label=\"L\"];\n",
                    (void*)node, (void*)node -> left);

            fprintf(dot_file, "    error_parent_%p [shape=ellipse, style=filled, fillcolor=orange, label=\"Parent address Error\"];\n",
                    (void*)node->left);
            fprintf(dot_file, "    node_%p -> error_parent_%p [color=red];\n", (void*)node -> left, (void*)node -> left);
        }
        create_tree_connections(node -> left, dot_file);
    }

    if (node -> right != NULL)
    {
        if (node -> right -> parent == node)
        {
            fprintf(dot_file, "    node_%p -> node_%p [color=green, dir=both, arrowtail=normal, arrowhead=normal, label=\"R\"];\n",
                    (void*)node, (void*)node -> right);
        }
        else
        {
            fprintf(dot_file, "    node_%p -> node_%p [color=green, label=\"R\"];\n",
                    (void*)node, (void*)node -> right);

            fprintf(dot_file, "    error_parent_%p [shape=ellipse, style=filled, fillcolor=orange, label=\"Parent address Error\"];\n",
                    (void*)node -> right);
            fprintf(dot_file, "    node_%p -> error_parent_%p [color=red, style=dashed];\n", (void*)node -> right, (void*)node -> right);
        }
        create_tree_connections(node -> right, dot_file);
    }
}



const char* get_node_color(node_t* node, tree_t* tree)
{
    assert(node != NULL);
    assert(tree != NULL);

    if (node == tree -> root)
        return "lightcoral";

    switch (node -> type)
    {
        case NODE_VAR:
            return "lightgreen";

        case NODE_NUM:
            return "lightyellow";

        case NODE_OP:
            switch (node -> data.op_value)
            {
                case OP_ADD: return "lightblue";
                case OP_SUB: return "lightpink";
                case OP_MUL: return "lightsalmon";
                case OP_DIV: return "plum";
                default:     return "lightgrey";
            }

        case NODE_IF:
        case NODE_WHILE:
            return "orange";

        case NODE_SEQUENCE:
            return "lightcyan";

        case NODE_ASSIGN:
            return "gold";

        case NODE_EMPTY:
            return "white";

        case NODE_EQUAL:
        case NODE_NOT_EQUAL:
        case NODE_LESS:
        case NODE_LESS_EQUAL:
        case NODE_GREATER:
        case NODE_GREATER_EQUAL:
            return "lightcoral";

        case NODE_AND:
        case NODE_OR:
        case NODE_NOT:
            return "lightpink";

        case NODE_FUNC_DECL:
            return "violet";

        case NODE_FUNC_CALL:
            return "orchid";

        case NODE_RETURN:
            return "tomato";

        case NODE_PARAM:
        case NODE_ARGS:
            return "wheat";

        default:
            return "lightgrey";
    }
}


tree_error_type write_tree_common_picture(tree_t* tree, FILE* htm_file, const char* folder_path, const char* folder_name)
{
    assert(tree        != NULL);
    assert(htm_file    != NULL);
    assert(folder_name != NULL);

    static int n_of_pictures = 0;

    char temp_dot_global_path[MAX_LENGTH_OF_FILENAME] = {};
    char temp_svg_global_path[MAX_LENGTH_OF_FILENAME] = {};

    snprintf(temp_dot_global_path, sizeof(temp_dot_global_path), "%s/tree_%d.dot",
             folder_path, n_of_pictures);
    snprintf(temp_svg_global_path, sizeof(temp_svg_global_path), "%s/tree_%d.svg",
             folder_path, n_of_pictures);

    char temp_svg_local_path[MAX_LENGTH_OF_FILENAME] = {};
    snprintf(temp_svg_local_path, sizeof(temp_svg_local_path), "%s/tree_%d.svg",
             folder_name, n_of_pictures);

    n_of_pictures++;

    tree_error_type dot_result = generate_dot_file(tree, temp_dot_global_path);
    if (dot_result != TREE_ERROR_NO)
        return dot_result;

    char command[MAX_LENGTH_OF_SYSTEM_COMMAND] = {};
    snprintf(command, sizeof(command), "dot -Tsvg -Gcharset=utf8 \"%s\" -o \"%s\"",
             temp_dot_global_path, temp_svg_global_path);

    system(command);

    FILE* test_svg = fopen(temp_svg_global_path, "r");
    if (test_svg != NULL)
    {
        fclose(test_svg);

        fprintf(htm_file, "<div style='text-align:center;'>\n");
        fprintf(htm_file, "<img src='%s' style='max-width:100%%; border:1px solid #ddd;'>\n", temp_svg_local_path);
        fprintf(htm_file, "</div>\n");
    }
    else
    {
        fprintf(htm_file, "<p style='color:red;'>Error generating SVG graph</p>\n");
    }

    remove(temp_dot_global_path);

    return TREE_ERROR_NO;
}


void write_dump_header(FILE* htm_file, time_t now, const char* comment)
{
    assert(htm_file != NULL);

    fprintf(htm_file, "<div style='border:2px solid #ccc; margin:10px; padding:15px; background:#f9f9f9;'>\n");
    fprintf(htm_file, "<h2 style='color:#333;'>Tree Dump at %s</h2>\n", ctime(&now));

    if (comment != NULL)
    {
        fprintf(htm_file, "<div style='background: #2d2d2d; padding: 10px; margin: 10px 0; border-left: 4px solid #ff6b6b;'>\n");
        fprintf(htm_file, "<p style='color: #ff6b6b; font-weight:bold; margin:0;'>%s</p>\n", comment);
        fprintf(htm_file, "</div>\n");
    }
}


tree_error_type tree_dump_to_htm(tree_t* tree, FILE* htm_file, const char* folder_path, const char* folder_name, const char* comment)
{
    assert(htm_file    != NULL);
    assert(folder_path != NULL);

    time_t now = time(NULL);

    write_dump_header(htm_file, now, comment);

    if (tree != NULL && tree->root != NULL)
    {
        tree_error_type result_writing_picture = write_tree_common_picture(tree, htm_file, folder_path, folder_name);
        if (result_writing_picture != TREE_ERROR_NO)
            return result_writing_picture;
    }
    else
    {
        fprintf(htm_file, "<p style='color:blue;'>No tree to visualize</p>\n");
    }

    write_dump_footer(htm_file);
    return TREE_ERROR_NO;
}


tree_error_type tree_dump(tree_t* tree, const char* filename)
{
    if (tree == NULL)
        return TREE_ERROR_NULL_PTR;

    char command[MAX_LENGTH_OF_SYSTEM_COMMAND] = {};
    snprintf(command, sizeof(command), "mkdir -p %s", GENERAL_FOLDER_NAME_FOR_LOGS);
    system(command);

    char folder_name[MAX_LENGTH_OF_FILENAME] = {};
    snprintf(folder_name, sizeof(folder_name), "%s_dump", filename);

    char folder_path[MAX_LENGTH_OF_FILENAME] = {};
    snprintf(folder_path, sizeof(folder_path), "%s/%s_dump", GENERAL_FOLDER_NAME_FOR_LOGS, filename);

    snprintf(command, sizeof(command), "mkdir -p %s", folder_path);
    system(command);

    char htm_filename[MAX_LENGTH_OF_FILENAME] = {};
    snprintf(htm_filename, sizeof(htm_filename), "%s/%s.htm", GENERAL_FOLDER_NAME_FOR_LOGS, filename);

    FILE* htm_file = fopen(htm_filename, "a");
    if (!htm_file)
        return TREE_ERROR_OPENING_FILE;

    tree_error_type result = tree_dump_to_htm(tree, htm_file, folder_path, folder_name, NULL);

    fclose(htm_file);
    return result;
}


tree_error_type init_tree_log(const char* filename)
{
    assert(filename);

    char htm_filename[MAX_LENGTH_OF_FILENAME] = {};
    snprintf(htm_filename, sizeof(htm_filename), "%s/%s.htm", GENERAL_FOLDER_NAME_FOR_LOGS, filename);

    FILE* htm_file = fopen(htm_filename, "w");
    if (!htm_file)
        return TREE_ERROR_OPENING_FILE;

    fprintf(htm_file, "<!DOCTYPE html>\n"
                      "<html>\n"
                      "<head>\n"
                      "<title>Tree Dump Log</title>\n"
                      "<meta charset='UTF-8'>\n"
                      "<style>\n"
                      "body { font-family: Arial, sans-serif; margin: 20px; }\n"
                      "table { border-collapse: collapse; width: 100%%; }\n"
                      "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n"
                      "th { background-color: #f2f2f2; }\n"
                      "</style>\n"
                      "</head>\n"
                      "<body>\n"
                      "<h1>Tree Dump Log</h1>\n");
    fclose(htm_file);

    return TREE_ERROR_NO;
}


tree_error_type close_tree_log(const char* filename)
{
    assert(filename);

    char htm_filename[MAX_LENGTH_OF_FILENAME] = {};
    snprintf(htm_filename, sizeof(htm_filename), "%s/%s.htm", GENERAL_FOLDER_NAME_FOR_LOGS, filename);

    FILE* htm_file = fopen(htm_filename, "a");
    if (!htm_file)
        return TREE_ERROR_OPENING_FILE;

    fprintf(htm_file, "</body>\n");
    fprintf(htm_file, "</html>\n");
    fclose(htm_file);

    return TREE_ERROR_NO;
}


tree_verify_result verify_tree(tree_t* tree)
{
    if (tree == NULL)
        return TREE_VERIFY_NULL_PTR;

    if (tree -> root != NULL && tree -> root -> parent != NULL)
        return TREE_VERIFY_INVALID_PARENT;

    return TREE_VERIFY_SUCCESS;
}


const char* tree_verify_result_to_string(tree_verify_result result)
{
    switch (result)
    {
        case TREE_VERIFY_SUCCESS:          return "Success";
        case TREE_VERIFY_NULL_PTR:         return "Null pointer";
        case TREE_VERIFY_INVALID_PARENT:   return "Invalid parent pointer";
        case TREE_VERIFY_CYCLE_DETECTED:   return "Cycle detected";
        default:                           return "Unknown error";
    }
}
