#include "scanner.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define EOL '\n'

void print_scan_token (Token *token) {
    fprintf(stderr, "%d, %s, %d\n", token->type, token->data, token->data_size);
}


int init_token(char c, Token *token) {
    token->data = malloc(sizeof(char));
    if (!(token->data)) {
        free(token->data);
        return INTERNAL_ERROR;
    } else {
        token->data[0] = c;
        token->data_size = 1;
    }
    return 0;
}

int expand_token(char c, Token *token) {
    token->data = realloc(token->data, (token->data_size + 1) * sizeof(char));
    if (!(token->data)) {
        free(token->data);
        return INTERNAL_ERROR;
        
    } else {
        token->data[token->data_size] = c;
        token->data_size++;
    }
    return 0;
}

void free_token(Token *token) {
    token->data = NULL;
    token->data_size = 0;
}

void parser_function (Token *token) {
    if (token->type != ELSE &&
        token->type != FLOAT64 &&
        token->type != FOR &&
        token->type != FUNC &&
        token->type != IF &&
        token->type != INT &&
        token->type != PACKAGE &&
        token->type != RETURN &&
        token->type != STRING &&
        token->type != ID) {
        if (expand_token('\0', token)) {
            return;
        }
    }
    DLInsertLast(&token_list, token);
    free_token(token);
}

Token control_reserved_words (Token *token) {
    if (expand_token('\0', token)) {
        return *token;
    }
    if (strcmp(token->data, "else") == 0) {
        token->type = ELSE; 
    } else if (strcmp(token->data, "float64") == 0) {
        token->type = FLOAT64;
    } else if (strcmp(token->data, "for") == 0) {
        token->type = FOR;
    } else if (strcmp(token->data, "func") == 0) {
        token->type = FUNC;
    } else if (strcmp(token->data, "if") == 0) {
        token->type = IF;
    } else if (strcmp(token->data, "int") == 0) {
        token->type = INT;
    } else if (strcmp(token->data, "package") == 0) {
        token->type = PACKAGE;
    } else if (strcmp(token->data, "return") == 0) {
        token->type = RETURN;
    } else if (strcmp(token->data, "string") == 0) {
        token->type = STRING;
    }
    return *token;
}

int control_exp(Token *token, char *c) {
    if (*c == '+' || *c == '-') {
        if (expand_token(*c, token)) {
            free(token->data);
            return INTERNAL_ERROR;
        } else {
            *c = getchar();
            if (*c >= '0' && *c <= '9') {
                token->type = EXP_FINAL_NUM;
                if (expand_token(*c, token)) {
                    free(token->data);
                    return INTERNAL_ERROR;
                } else {
                    *c = getchar();
                    while (*c >= '0' && *c <= '9') {
                        if (expand_token(*c, token)) {
                            free(token->data);
                            return INTERNAL_ERROR;
                        } else {
                            *c = getchar();
                        }
                    }
                    token->type = FLOAT;
                    parser_function(token);
                    return 0;
                }
            } else {
                return LEXICAL_ERROR;
            }
        }
    } else if (*c >= '0' && *c <= '9') {
        token->type = EXP_FINAL_NUM;
        if (expand_token(*c, token)) {
            free(token->data);
            return INTERNAL_ERROR;
        } else {
            *c = getchar();
            while (*c >= '0' && *c <= '9') {
                if (expand_token(*c, token)) {
                    free(token->data);
                    return INTERNAL_ERROR;
                } else {
                    *c = getchar();
                }
            }
            token->type = FLOAT;
            parser_function(token);
            return 0;
        }
    } else {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return LEXICAL_ERROR;
    }
}

int control_float(Token *token, char *c) {
    token->type = FLOAT;
    if (*c >= '0' && *c <= '9') {
        if (expand_token(*c, token)) {
            free(token->data);
            return INTERNAL_ERROR;
        } else {
            *c = getchar();
            while (*c >= '0' && *c <= '9') {
                if (expand_token(*c, token)) {
                    free(token->data);
                    return INTERNAL_ERROR;
                } 
                *c = getchar();
            }
        }
    } else {
        return LEXICAL_ERROR;
    }

    if (*c == 'E' || *c == 'e') {
        if (expand_token(*c, token)) {
            free(token->data);
            return INTERNAL_ERROR;
        }
        *c = getchar();
        switch(control_exp(token, c)) {
            case LEXICAL_ERROR: {
                return LEXICAL_ERROR;
            }
            case INTERNAL_ERROR: {
                return INTERNAL_ERROR;
            }
            default: return 0;
        }
    } 
    parser_function(token);
    return 0;
}

int scan_token(Token *token) {
    char c = getchar();
    while (c != EOF) {
        // token (
        if (c == '(') {
            token->type = ROUND_BR_L;
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            } else {
                parser_function(token);
                c = getchar(); 
            } 

        // token ) 
        } else if (c == ')') {
            token->type = ROUND_BR_R;
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            } else {
                parser_function(token);
                c = getchar(); 
            }

        // token { 
        } else if (c == '{') {
            token->type = CURLY_BR_L;
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            } else {
                parser_function(token); 
                c = getchar();
            }
        
        // token }
        } else if (c == '}') {
            token->type = CURLY_BR_R;
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            } else {
                parser_function(token); 
                c = getchar();
            }

        // token := 
        } else if (c == ':') {
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            }
            c = getchar();
            if (c == '=') {
                token->type = DECLARE;
                if (expand_token(c, token)) {
                    free(token->data);
                    return INTERNAL_ERROR;
                } else {
                    parser_function(token);
                    c = getchar();
                }
            } else {
                free(token->data);
                return LEXICAL_ERROR;
            }

        // token >
        } else if (c == '>') {
            token->type = G;
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            }
            c = getchar();
            
            // token >=
            if (c == '=') {
                token->type = GE;
                if (expand_token(c, token)) {
                    free(token->data);
                    return INTERNAL_ERROR;
                } else {
                    parser_function(token);
                    c = getchar();
                }
            } else {
                parser_function(token);
            }

        // token <     
        } else if (c == '<') {
            token->type = L;
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            }
            c = getchar();

            // token <=
            if (c == '=') {
                token->type = LE;
                if (expand_token(c, token)) {
                    free(token->data);
                    return INTERNAL_ERROR;
                } else {
                    parser_function(token);
                    c = getchar();
                }
            } else {
                parser_function(token);
            }

        // token +     
        } else if (c == '+') {
            token->type = ADD;
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            } else {
                parser_function(token);
                c = getchar();
            }

        // token -
        } else if (c == '-') {
            token->type = SUB;
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            } else {
                parser_function(token);
                c = getchar();
            }

        // token * 
        } else if (c == '*') {
            token->type = MUL;
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            } else {
                parser_function(token);
                c = getchar();
            }

        // token ,
        } else if (c == ',') {
            token->type = COMMA;
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            } else {
                parser_function(token);
                c = getchar();
            }

        // token ;
        } else if (c == ';') {
            token->type = SEMICOLON;
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            } else {
                parser_function(token);
                c = getchar();
            }
        /*
        // token &&
        } else if (c == '&') {
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            }
            c = getchar();
            if (c == '&') {
                token->type = AND;
                if (expand_token(c, token)) {
                    free(token->data);
                    return INTERNAL_ERROR;
                } else {
                    parser_function(token);
                    c = getchar();
                }
            } else {
                free(token->data);
                return LEXICAL_ERROR;
            }

        // token ||
        } else if (c == '|') {
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            }
            c = getchar();
            if (c == '|') {
                token->type = OR;
                if (expand_token(c, token)) {
                    free(token->data);
                    return INTERNAL_ERROR;
                } else {
                    parser_function(token);
                    c = getchar();
                }
            } else {
                free(token->data);
                return LEXICAL_ERROR;
            }
        */
        // token !      // send ! in boolean
        } else if (c == '!') {
            token->type = NOT;
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            }
            c = getchar();

            // token !=
            if (c == '=') {
                token->type = NOT_EQ;
                if (expand_token(c, token)) {
                    free(token->data);
                    return INTERNAL_ERROR;
                } else {
                    parser_function(token);
                    c = getchar();
                }
            } else {
                free(token->data);
                return LEXICAL_ERROR;
            }

        // token =
        } else if (c == '=') {
            token->type = EQ_SYM;
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            }
            c = getchar();

            // token ==
            if (c == '=') {
                token->type = EQ;
                if (expand_token(c, token)) {
                    free(token->data);
                    return INTERNAL_ERROR;
                } else {
                    parser_function(token);
                    c = getchar();
                }
            } else {
                parser_function(token);
            }

        // token /
        } else if (c == '/') {           
            token->type = DIV;
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            }
            c = getchar();
            // string comment
            if (c == '/') {
                c = getchar();
                while (c != EOL && c != EOF) {
                    c = getchar();
                }
                free(token->data);
                free_token(token);
                continue;
            // start block comment               
            } else if (c == '*') {  
                c = getchar();
                while (1) {
                    if (c == EOF) {
                        free(token->data);
                        return LEXICAL_ERROR;
                    // end block comment
                    } else if (c == '*') {
                        c = getchar();
                        if (c == '/') {
                            free(token->data);
                            free_token(token);
                            break;
                        } else {
                            continue;
                        }
                    } else {
                        c = getchar();
                        continue;
                    }
                }
                free(token->data);
                c = getchar();
            } else {
                parser_function(token);
            }
            
        // token ID        
        } else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '_')) {
            token->type = ID;
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            } 
            c = getchar();
            while (((c >= 'A' && c <= 'Z') || 
                    (c >= 'a' && c <= 'z') || 
                    (c == '_') || 
                    (c >= '0' && c <= '9'))) {
                if (expand_token(c,token)) {
                    free(token->data);
                    return INTERNAL_ERROR;
                } else {
                    c = getchar(); 
                }
            }
            control_reserved_words(token);
            parser_function(token);
        
        // token string 
        } else if (c == '"') {
            token->type = STR_END;
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            }
            c = getchar();
            while (c != '"') {
                if (c == EOF || c == EOL) {
                    free(token->data);
                    return LEXICAL_ERROR;                         
                } else {
                    if (c == '\\') {
                        if (expand_token(c, token)) {
                            free(token->data);
                            return INTERNAL_ERROR;
                        }
                        c = getchar();
                        if ((c == '"') || (c == 'n') || (c == 't') || (c == '\\')) {
                            if (expand_token(c, token)) {
                                free(token->data);
                                return INTERNAL_ERROR;
                            }
                            c = getchar();
                            continue;
                        } else if (c == 'x') {
                            if (expand_token(c, token)) {
                                free(token->data);
                                return INTERNAL_ERROR;
                            }
                            c = getchar();
                            if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
                                if (expand_token(c, token)) {
                                    free(token->data);
                                    return INTERNAL_ERROR;
                                }
                                c = getchar();
                                if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
                                    if (expand_token(c, token)) {
                                        free(token->data);
                                        return INTERNAL_ERROR;
                                    }
                                    c = getchar();
                                    continue;
                                } else {
                                    free(token->data);
                                    return LEXICAL_ERROR;
                                }
                            } else {
                                free(token->data);
                                return LEXICAL_ERROR;
                            }
                        } else {
                            free(token->data);
                            return LEXICAL_ERROR;
                        }
                    } else {
                        if (expand_token(c, token)) {
                            free(token->data);
                            return INTERNAL_ERROR;
                        }
                        c = getchar();
                        continue;
                    }
                }
            }
            if (expand_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            } else {
                parser_function(token);
                c = getchar();
            }    

        // token zero 
        } else if (c == '0') {
            token->type = ZERO_MANTISSA;
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            }
            c = getchar();
            if (c >= '0' && c <= '9') {
                free(token->data);
                return LEXICAL_ERROR;
            } 
            
            // token float
            if (c == '.') {
                if (expand_token(c, token)) {
                    free(token->data);
                    return INTERNAL_ERROR;
                } else {
                    c = getchar();  
                    switch(control_float(token, &c)) {
                        case LEXICAL_ERROR: {
                            free(token->data);
                            return LEXICAL_ERROR;
                        }
                        case INTERNAL_ERROR: {
                            free(token->data);
                            return INTERNAL_ERROR;
                        }
                        default: continue;
                    }
                }

            // token exp
            } else if (c == 'E' || c == 'e') {
                if (expand_token(c, token)) {
                    free(token->data);
                    return INTERNAL_ERROR;
                } else {
                    c = getchar();
                    switch(control_exp(token, &c)) {
                        case LEXICAL_ERROR: {
                            free(token->data);
                            return LEXICAL_ERROR;
                        }
                        case INTERNAL_ERROR: {
                            free(token->data);
                            return INTERNAL_ERROR;
                        }
                        default: continue;
                    } 
                }
                
            }
            token->type  = INTEGER;
            parser_function(token);

        // token integer 
        } else if (c >= '1' && c <= '9') {
            token->type = INTEGER;
            if (init_token(c, token)) {
                free(token->data);
                return INTERNAL_ERROR;
            }
            c = getchar();
            while (c >= '0' && c <= '9' ) {
                if (expand_token(c, token)) {
                    free(token->data);
                    return INTERNAL_ERROR;
                } else {
                    c = getchar();
                }
            }
            if (c == '.') {
                if (expand_token(c, token)) {
                    free(token->data);
                    return INTERNAL_ERROR;
                } else {
                    c = getchar();
                    switch(control_float(token, &c)) {
                        case LEXICAL_ERROR: {
                            free(token->data);
                            return LEXICAL_ERROR;
                        }
                        case INTERNAL_ERROR: {
                            free(token->data);
                            return INTERNAL_ERROR;
                        }
                        default: continue;
                    }
                }
            } else if (c == 'E' || c == 'e') {
                if (expand_token(c, token)) {
                    free(token->data);
                    return INTERNAL_ERROR;
                } else {
                    c = getchar();
                    switch(control_exp(token, &c)) {
                        case LEXICAL_ERROR: {
                            free(token->data);
                            return LEXICAL_ERROR;
                        }
                        case INTERNAL_ERROR: {
                            free(token->data);
                            return INTERNAL_ERROR;
                        }
                        default: continue;
                    }
                }
            }
                parser_function(token);
             
        } else if (c == EOL) {
            
            token->type = END_OF_LINE;
            parser_function(token);
            c = getchar();
        } else if (isspace(c)) {
            c = getchar();
        } else {
            free(token->data);
            return LEXICAL_ERROR;
        }
    }
    token->type = END_OF_LINE;
    parser_function(token);
    return 0;
}
