#include <stdio.h>

typedef enum {
    ID,
    // NUMBERS
    INTEGER, FLOAT, ZERO_MANTISSA, EXP, EXP_SIGNED, EXP_FINAL_NUM,
    // STRING
    STR, STR_SPEC_SYM, STR_HEX_SYM, STR_HEX_DIGIT_FIRST, STR_END,
    // BRACES
    ROUND_BR_L, ROUND_BR_R, SQUARE_BR_L, SQUARE_BR_R, CURLY_BR_L, CURLY_BR_R,
    // DECLARATION
    SEMICOLON, DECLARE, 
    // COMPARISON OPERATORS
    G, GE, L, LE, EQ_SYM, EQ, NOT, NOT_EQ,
    // BOOLEAN OPERATORS
    AND_SYM, AND, OR_SYM, OR,
    // OTHER SYMBOLS
    COMMA, COLON,
    // MATH OPERATORS
    ADD, SUB, MUL, MOD, DIV,
    // COMMENTS
    COMMENT_STR, COMMENT_BLOCK, COMMENT_BLOCK_END_S, COMMENT_BLOCK_END_F,
    // RESERVED WORDS
    ELSE, FLOAT64, FOR, FUNC, IF, INT, PACKAGE, RETURN, STRING,
    // EOL
    END_OF_LINE
} Token_type;

typedef struct {
    Token_type type;   // type of token
    char *data; //  data of token
    unsigned data_size; // size of token 
} Token;

