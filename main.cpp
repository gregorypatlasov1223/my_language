#include <stdio.h>

#include "operations.h"
#include "new_input.h"


int main(int argc, const char** argv)
{
    variable_table var_table = {};
    init_variable_table(&var_table);

    node_t* program = read_program_from_file("program.txt", &var_table);
    if (!program)
    {
        printf("Mistake: couldn't download the program\n");
        return 1;
    }

    printf("The program has been successfully downloaded\n\n");

    double result = 0.0;
    tree_error_type error = execute_assignment(program, &var_table, &result);

    if (error == TREE_ERROR_NO)
    {
        printf("The program was completed successfully\n");

        if (program -> type == NODE_ASSIGN && program -> left -> type == NODE_VAR)
        {
            char* var_name = program -> left -> data.var_definition.name;
            printf("  %s = %.2f\n", var_name, result);
        }
    }
    else
    {
        printf("Execution error: %d\n", error);
    }

    free_subtree(program);
    destroy_variable_table(&var_table);

    return 0;
}
