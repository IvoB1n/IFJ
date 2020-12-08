/**
 * @file scanner.h
 * 
 * Lexical analyzer
 * 
 * @brief IFJ Projekt 2020, Tým 58
 * 
 * @author <xbobro01> Bobrov Ivan
 * @author <xcaras00> Carasec Elena
 * @author <xkozhe00> Kozhevnikov Dmitrii
 * @author <xsenic00> Senichak Yahor
 */
#include "token_dll.h"

/**
 * @enum Types of tokens 
 */
typedef enum {
    // EXPRESSIONS
    ADD, SUB, MUL, DIV, ROUND_BR_L, ROUND_BR_R, L, LE, G, GE,
    EQ, NOT_EQ, I, DOLLAR, SHIFT, REDUCE, EQUAL, NONE, E,
    // IDENTIFIRES
    ID, ID_FUNC,
    // NUMBERS
    INTEGER, FLOAT, ZERO_MANTISSA, EXP, EXP_SIGNED, EXP_FINAL_NUM, BOOLEAN,
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
    ELSE, FLOAT64, FOR, FUNC, IF, INT, PACKAGE, RETURN, STRING, BOOL,
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

/**
 * Will issue the scanned token (for tests)
 * 
 * @param token Token Pointer to output token
 */
void print_scan_token (Token *token);

/**
 * Allocates memory for the read token
 * 
 * @param c Read symbol
 * @param token Token Pointer to output token
 * @return 0 If the memory was allocated successfully
 * @return 99 If the memory wasn't allocated successfully
 */
int init_token(char c, Token *token);

/**
 * Reallocates memory for the read token
 * 
 * @param c Read symbol
 * @param token Token Pointer to output token
 * @return 0 If the memory was reallocated successfully
 * @return 99 If the memory wasn't reallocated successfully
 */
int expand_token(char c, Token *token);

/**
 * Deleting data from the token after sending it to the parser
 * 
 * @param token Token Pointer to output token
 */
void free_token (Token *token);

/**
 * Sends the scanned token to the parser
 * 
 * @param token Token Pointer to output token
 */
void parser_function (Token *token);

/**
 * Checks that the read token is a keyword
 * 
 * @param token Token Pointer to output token
 * @return Token, if the token is a keyword, replace its type
 */
Token control_reserved_words (Token *token);

/**
 * Checks that the exponential number has the correct form
 * 
 * @param token Token Pointer to output token
 * @param c Read symbol
 * @return 0 The number has the correct exponential form
 * @return 1 The number hasn't the correct exponential form
 * @return 99 If the memory wasn't reallocated successfully
 */
int control_exp(Token *token, char *c);

/**
 * Checks that the floating-point number has the correct form
 * 
 * @param token Token Pointer to output token
 * @param c Read symbol
 * @return 0 The number has the correct exponential form
 * @return 1 The number hasn't the correct exponential form
 * @return 99 If the memory wasn't reallocated successfully
 */
int control_float(Token *token, char *c);

/**
 * The main function that reads symbols and writes them to a token
 * 
 * @param token Token Pointer to output token
 * @return 0 If the token was successfully read and has no errors
 * @return 1 If the token has a syntax error
 * @return 99 If the memory wasn't allocated or reallocated successfully
 */
int scan_token(Token *token);