#ifndef NEW_INPUT_H
#define NEW_INPUT_H

#include "tree_base.h"
#include "variable_parse.h"

// Типы токенов
enum  token_type
{
    TOKEN_IDENTIFIER   = 0,
    TOKEN_NUMBER       = 1,
    TOKEN_PLUS         = 2,
    TOKEN_MINUS        = 3,
    TOKEN_MULTIPLY     = 4,
    TOKEN_DIVIDE       = 5,
    TOKEN_ASSIGN       = 6,  // =
    TOKEN_SEMICOLON    = 7,  // ;
    TOKEN_LPAREN       = 8,  // (
    TOKEN_RPAREN       = 9,  // )
    TOKEN_EOF          = 10, // Конец строки
    TOKEN_ERROR        = 11  // Ошибка
};

struct token_t
{
    token_type type;
    union {
        char* identifier;  // Для TOKEN_IDENTIFIER
        double number;     // Для TOKEN_NUMBER
    } value;
};


token_t* lexer(const char* string, size_t* token_count);
void free_tokens(token_t* tokens, size_t token_count);
// Функции синтаксического анализа
node_t* get_assignment(const char* source, variable_table* var_table);

// Основная функция
node_t* parse_program(const char* source, variable_table* var_table);
node_t* read_program_from_file(const char* filename, variable_table* var_table);

#endif // NEW_INPUT_H
