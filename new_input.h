#ifndef NEW_INPUT_H
#define NEW_INPUT_H

#include "tree_base.h"
#include "variable_parse.h"


enum  token_type
{
    TOKEN_IDENTIFIER = 0,
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_ASSIGN,        // =
    TOKEN_SEMICOLON,     // ;
    TOKEN_LPAREN,        // (
    TOKEN_RPAREN,        // )
    TOKEN_LBRACE,        // {
    TOKEN_RBRACE,        // }
    TOKEN_EQUAL,         // ==
    TOKEN_NOT_EQUAL,     // !=
    TOKEN_LESS,          // <
    TOKEN_LESS_EQUAL,    // <=
    TOKEN_GREATER,       // >
    TOKEN_GREATER_EQUAL, // >=
    TOKEN_AND,           // &&
    TOKEN_OR,            // ||
    TOKEN_NOT,           // !
    TOKEN_COMMA,         // ,
    TOKEN_FUNC,          // Freestayla
    TOKEN_RETURN,        // Rakamakafo
    TOKEN_IF,            // Изюм-Фактор
    TOKEN_WHILE,         // Вова-АЙболит-Лечит-говнокод
    TOKEN_EOF,           // Конец строки
    TOKEN_ERROR          // Ошибка
};

struct symbol_and_token
{
    token_type type;
    char target_symbol;
}

struct token_t
{
    token_type type;
    union {
        char* identifier;  // Для TOKEN_IDENTIFIER
        double number;     // Для TOKEN_NUMBER
    } value;
};

struct standard_token_info
{
    token_type type_of_token;
    const char* token_string;
};

struct struct_for_compare_token_and_node
{
    token_type type_of_token;
    node_type  node_type;
};


token_t* lexer(const char* string, size_t* token_count);
void free_tokens(token_t* tokens, size_t token_count);
int check_symbol(const char** string, token_t* tokens, size_t* current_index, char target_symbol, token_type type);


node_t* parse_program(const char* source, variable_table* var_table);
node_t* read_program_from_file(const char* filename, variable_table* var_table);

#endif // NEW_INPUT_H
