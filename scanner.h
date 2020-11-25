#include "token_dll.h"
// #include "expression.h"

typedef enum {
    // EXPRESSIONS
    ADD, SUB, MUL, DIV, ROUND_BR_L, ROUND_BR_R, L, LE, G, GE,
    EQ, NOT_EQ, I, DOLLAR, SHIFT, REDUCE, EQUAL, NONE, E,
    // IDENTIFIRES
    ID, ID_FUNC,
    // NUMBERS
    INTEGER, FLOAT, ZERO_MANTISSA, EXP, EXP_SIGNED, EXP_FINAL_NUM,
    // STRING
    STR, STR_SPEC_SYM, STR_HEX_SYM, STR_HEX_DIGIT_FIRST, STR_END,
    // BRACES
    SQUARE_BR_L, SQUARE_BR_R, CURLY_BR_L, CURLY_BR_R,
    // DECLARATION
    SEMICOLON, DECLARE, 
    // COMPARISON OPERATORS
    EQ_SYM,  NOT,
    // BOOLEAN OPERATORS
    AND_SYM, AND, OR_SYM, OR,
    // OTHER SYMBOLS
    COMMA, COLON, POINT,
    // MATH OPERATORS
    MOD,
    // COMMENTS
    COMMENT_STR, COMMENT_BLOCK, COMMENT_BLOCK_END_S, COMMENT_BLOCK_END_F,
    // RESERVED WORDS
    ELSE, FLOAT64, FOR, FUNC, IF, INT, PACKAGE, RETURN, STRING,
    // EOL
    END_OF_LINE
} Token_type;


/*
typedef struct {
    Token_type type;   // type of token
    char *data; //  data of token
    unsigned data_size; // size of token 
} Token;
*/


void free_token (Token *token);
int scan_token (Token *token);