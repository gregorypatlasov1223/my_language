#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "tree_common.h"
#include "function_parse.h"
#include "tree_error_types.h"

function_table* init_function_table(function_table* table)
{
    assert(table != NULL);

    table -> functions = NULL;
    table -> count = 0;

    return table;
}


function_info* find_function(function_table* func_table, const char* function_name)
{
    assert(func_table    != NULL);
    assert(function_name != NULL);

    function_info current = func_table -> functions;
    while (current != NULL)
    {
        if (strcmp(current -> name, function_name) == 0)
            return current;
        current = current -> next;
    }

    return NULL;
}


void destroy_function_info(function_info* func_info)
{
    assert(func_info != NULL);

    free(func_info -> name);

    if (func_info -> param_names != NULL)
    {
        for (int i = 0; i < func_info -> param_count; i++)
        {
            free(func_info -> param_names[i]);
        }
        free(func_info -> param_names);
    }

    free(func_info);
}

tree_error_type add_params_to_function(function_info* func_info, int param_count, char** param_names)
{
    if (param_names != NULL && param_count > 0)
    {
        func_info -> param_names = (char**)calloc(param_count, sizeof(char*));
        if (func_info -> param_names == NULL)
        {
            free(func_info -> name);
            free(func_info);
            return TREE_ERROR_MEMORY;
        }

        for (int i = 0; i < param_count; i++)
        {
            func_info -> param_names[i] = strdup(param_names[i]);
            if (func_info -> param_names[i] == NULL)
            {
                for (j = 0, j < i; j++)
                {
                    free(func_info -> param_names[j]);
                }
                free(func_info -> param_names);
                free(func_info -> name);
                free(func_info);
                return TREE_ERROR_MEMORY;
            }
        }
    }
}


tree_error_type add_function(function_table* table, const char* func_name, int param_count, char** param_names, node_t* body)
{
    assert(body        != NULL);
    assert(table       != NULL);
    assert(func_name   != NULL);
    assert(param_names != NULL);

    if (find_function(table, func_name))
        return TREE_ERROR_FUNCTION_REDEFINITION;

    function_info* func_info = (function_info*)calloc(1, sizeof(function_info));
    if (func_info == NULL)
        return TREE_ERROR_MEMORY;

    func_info -> name = strdup(func_name);
    if (func_info -> name == NULL)
    {
        free(func_info);
        return TREE_ERROR_MEMORY;
    }

    func_info -> param_count = param_count;
    func_info -> body = body;

    tree_error_type params_result = add_params_to_function(func_info, param_count, param_names);
    if (params_result != TREE_ERROR_NO)
    {
        destroy_function_info(func_info);
        return params_result;
    }

    func_info -> next = table -> functions;
    table -> functions = func_info;
    table -> count++;

    return TREE_ERROR_NO;
}


void destroy_function_table(function_table* table)
{
    assert(table != NULL);

    function_info* current = table -> functions;
    while (current != NULL)
    {
        function_info* next = current -> next;


        current = next;
    }

    table -> functions = NULL;
    table -> count = 0;
}



