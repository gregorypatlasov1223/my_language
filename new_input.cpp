#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "DSL.h"
#include "tree_base.h"
#include "new_input.h"
#include "tree_common.h"


void free_tokens(token_t* tokens, size_t token_count)
{
    assert(tokens != NULL);

    for (size_t i = 0; i < token_count; i++)
    {
        if (tokens -> type == TOKEN_IDENTIFIER && tokens[i].value.identifier != NULL)
        {
            free(tokens[i].value.identifier);
        }
    }

    free(tokens);
}


static int check_symbol(const char** string, token_t* tokens, size_t* current_index, char target_symbol, token_type type)
{
    // TODO: assert
    if (**string == target_symbol)
    {
        tokens[(*current_index)++].type = type;
        (*string)++;
        return 1;
    }

    return 0;
}


token_t* lexer(const char* string, int* token_count)
{
    assert(string != NULL);

    // TODO: assert

    size_t current_index = 0;
    size_t capacity = INITIAL_CAPACITY;

    token_t* tokens = (token_t*)calloc(capacity, sizeof(token_t));

    while (*string != '\0')
    {
        if (current_index >= capacity - 1)
        {
            capacity *= COEFFICIENT;
            token_t* new_tokens = (token_t*)realloc(tokens, capacity * sizeof(token_t));
            if (new_tokens == NULL)
            {
                free_tokens(tokens, current_index);
                *token_count = 0;
                return NULL;
            }
            tokens = new_tokens;
        }

        if (isspace(*string))
        {
            string++;
            continue;
        }

        // TODO: define
        if (check_symbol(&string, tokens, &current_index, '(', TOKEN_LPAREN))    continue;
        if (check_symbol(&string, tokens, &current_index, ')', TOKEN_RPAREN))    continue;
        if (check_symbol(&string, tokens, &current_index, '+', TOKEN_PLUS))      continue;
        if (check_symbol(&string, tokens, &current_index, '-', TOKEN_MINUS))     continue;
        if (check_symbol(&string, tokens, &current_index, '*', TOKEN_MULTIPLY))  continue;
        if (check_symbol(&string, tokens, &current_index, '/', TOKEN_DIVIDE))    continue;
        if (check_symbol(&string, tokens, &current_index, '=', TOKEN_ASSIGN))    continue;
        if (check_symbol(&string, tokens, &current_index, ';', TOKEN_SEMICOLON)) continue;

        if (isdigit(*string))
        {
            double value = 0;
            while (isdigit(*string))
            {
                value = value * 10 + (*string - '0');
                string++;
            }
            tokens[current_index].type = TOKEN_NUMBER;
            tokens[current_index].value.number = value;
            current_index++;
            continue;
        }

        if (isalpha(*string))
        {
            char buffer[MAX_SIZE_OF_BUFFER] = {};
            int index = 0;

            // первый символ обязательно буква
            buffer[index] = *string;
            index++;
            string++;

            while (isalnum(*string) || *string == '_')
            {
                if (index < MAX_SIZE_OF_BUFFER - 1)
                {
                    buffer[index++] = *string;
                }
                string++;
            }
            buffer[index] = '\0';

            tokens[current_index].type = TOKEN_IDENTIFIER;
            tokens[current_index].value.identifier = strdup(buffer);
            current_index++;
            continue;
        }

        tokens[current_index++].type = TOKEN_ERROR;
        break;
    }

    tokens[current_index].type = TOKEN_EOF;
    tokens[current_index].value.identifier = NULL;
    current_index++;

    if (current_index < capacity)
    {
        token_t* resized_tokens = (token_t*)realloc(tokens, current_index * sizeof(token_t));
        if (resized_tokens != NULL)
            tokens = resized_tokens;
    }

    *token_count = current_index;
    return tokens;
}


static node_t* get_term(token_t* tokens, int* index, int token_count, variable_table* var_table);
static node_t* get_factor(token_t* tokens, int* index, int token_count, variable_table* var_table);

// E ::= T {('+' | '-') T}*
static node_t* get_expression(token_t* tokens, int* index, int token_count, variable_table* var_table)
{
    node_t* left = get_term(tokens, index, token_count, var_table);
    if (left == NULL)
        return NULL;

    while (*index < token_count)
    {
        token_type type = tokens[*index].type;

        if (type != TOKEN_PLUS && type != TOKEN_MINUS)
            break;

        (*index)++;

        node_t* right = get_term(tokens, index, token_count, var_table);
        if (right == NULL)
        {
            free_subtree(left);
            return NULL;
        }

        if (type == TOKEN_PLUS)
            left = ADD(left, right);
        else
            left = SUB(left, right);
    }

    return left;
}

// T ::= F {('*' | '/') F}*
static node_t* get_term(token_t* tokens, int* index, int token_count, variable_table* var_table)
{
    node_t* left = get_factor(tokens, index, token_count, var_table);
    if (left == NULL)
        return NULL;

    while (*index < token_count)
    {
        token_type type = tokens[*index].type;

        if (type != TOKEN_MULTIPLY && type != TOKEN_DIVIDE)
            break;

        (*index)++;

        node_t* right = get_factor(tokens, index, token_count, var_table);
        if (right == NULL)
        {
            free_subtree(left);
            return NULL;
        }

        if (type == TOKEN_MULTIPLY)
            left = MUL(left, right);
        else
            left = DIV(left, right);
    }

    return left;
}

// Парсинг фактора: F ::= '(' E ')' | Identifier | Number
static node_t* get_factor(token_t* tokens, int* index, int token_count, variable_table* var_table)
{
    if (*index >= token_count)
        return NULL;

    token_t token = tokens[*index];

    if (token.type == TOKEN_LPAREN)
    {
        (*index)++; // пропускаем '('

        node_t* expr = get_expression(tokens, index, token_count, var_table);
        if (expr == NULL)
            return NULL;

        if (*index >= token_count || tokens[*index].type != TOKEN_RPAREN)
        {
            free_subtree(expr);
            printf("Error: Expected ')'\n");
            return NULL;
        }

        (*index)++; // пропускаем ')'

        return expr;
    }

    if (token.type == TOKEN_NUMBER)
    {
        (*index)++;
        return NUM(token.value.number);
    }

    if (token.type == TOKEN_IDENTIFIER)
    {
        (*index)++;

        tree_error_type error = add_variable(var_table, token.value.identifier);
        if (error != TREE_ERROR_NO && error != TREE_ERROR_VARIABLE_ALREADY_EXISTS &&
            error != TREE_ERROR_REDEFINITION_VARIABLE)
        {
            printf("Error adding variable to table: %d\n", error);
            return NULL;
        }

        return VAR(token.value.identifier);
    }

    printf("Error: Expected number, variable, or '(' in expression\n");
    return NULL;
}

// ============================================================================
// A ::= Identifier '=' E ';'
// ============================================================================

node_t* get_assignment(const char* source, variable_table* var_table)
{
    int token_count = 0;
    token_t* tokens = lexer(source, &token_count);
    if (!tokens)
        return NULL;

    int index = 0;
    if (index >= token_count || tokens[index].type != TOKEN_IDENTIFIER)
    {
        printf("Error: Expected identifier at the beginning\n");
        free_tokens(tokens, token_count);
        return NULL;
    }

    char* var_name = strdup(tokens[index].value.identifier); //запомнили имя переменной
    index++;

    // чекаем на '='
    if (index >= token_count || tokens[index].type != TOKEN_ASSIGN)
    {
        printf("Error: Expected '=' after identifier\n");
        free(var_name);
        free_tokens(tokens, token_count);
        return NULL;
    }
    index++;

    node_t* expr = get_expression(tokens, &index, token_count, var_table);
    if (!expr)
    {
        free(var_name);
        free_tokens(tokens, token_count);
        return NULL;
    }

    if (index >= token_count || tokens[index].type != TOKEN_SEMICOLON)
    {
        printf("Error: Expected ';' after expression\n");
        free(var_name);
        free_subtree(expr);
        free_tokens(tokens, token_count);
        return NULL;
    }
    index++;

    if (index < token_count && tokens[index].type != TOKEN_EOF)
    {
        printf("Error: Unexpected tokens after expression\n");
        free(var_name);
        free_subtree(expr);
        free_tokens(tokens, token_count);
        return NULL;
    }

    node_t* var_node = VAR(var_name);
    free(var_name);

    value_of_tree_element data = {};
    node_t* assign_node = create_node(NODE_ASSIGN, data, var_node, expr);

    free_tokens(tokens, token_count);

    return assign_node;
}


size_t get_file_size(FILE *file)
{
    assert(file != NULL);

    struct stat stat_buffer = {};

    int file_descriptor = fileno(file);
    if (file_descriptor == -1)
    {
        fprintf(stderr, "Error: Cannot get file descriptor\n");
        return 0;
    }

    if (fstat(file_descriptor, &stat_buffer) != 0)
    {
        fprintf(stderr, "Error: Cannot get file stats\n");
        return 0;
    }

    return (size_t)stat_buffer.st_size;
}


node_t* parse_program(const char* source, variable_table* var_table)
{
    return get_assignment(source, var_table);
}


node_t* read_program_from_file(const char* filename, variable_table* var_table)
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        printf("Cannot open file: %s\n", filename);
        return NULL;
    }

    long size = get_file_size(file);

    char* buffer = (char*) calloc(size + 1, sizeof(char));
    if (!buffer)
    {
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);

    node_t* result = parse_program(buffer, var_table);

    free(buffer);
    return result;
}



