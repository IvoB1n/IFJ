#include "scanner.h"

#define EOL '\n'

int init_token(char c, Token *token) {
    if ((token->data = malloc(sizeof(char)))) {
        token->data[0] = c;
        token->data_size = 1;
    } else {
        fprintf(stderr, "Malloc error\n");
        return INTERNAL_ERROR;
    }
    return 0;
}

int expand_token(char c, Token *token) {
    if ((token->data = realloc(token->data, (token->data_size + 1) * sizeof(char)))) {
        token->data[token->data_size] = c;
        token->data_size++;
    } else {
        fprintf(stderr, "Realloc error\n");
        return INTERNAL_ERROR;
    }
    return 0;
}

void free_token (Token *token) {
    free(token->data);
    token->data = NULL;
    token->data_size = 0;
}

void parser_function (Token *token) {
    if (expand_token('\0', token)) {
        return;
    }
    free_token(token);
}

Token control_reserved_words (Token *token) {
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
            return INTERNAL_ERROR;
        } else {
            *c = getchar();
            if (*c >= '0' && *c <= '9') {
                token->type = EXP_FINAL_NUM;
                if (expand_token(*c, token)) {
                    return INTERNAL_ERROR;
                } else {
                    *c = getchar();
                    while (*c >= '0' && *c <= '9') {
                        if (expand_token(*c, token)) {
                        return INTERNAL_ERROR;
                        } else {
                            *c = getchar();
                        }
                    }
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
            return INTERNAL_ERROR;
        } else {
            *c = getchar();
            while (*c >= '0' && *c <= '9') {
                if (expand_token(*c, token)) {
                    return INTERNAL_ERROR;
                } else {
                    *c = getchar();
                }
            }
            parser_function(token);
            return 0;
        }
    } else {
        return LEXICAL_ERROR;
    }
}

int control_float(Token *token, char *c) {
    token->type = FLOAT;
    if (*c >= '0' && *c <= '9') {
        if (expand_token(*c, token)) {
            return INTERNAL_ERROR;
        } else {
            *c = getchar();
            while (*c >= '0' && *c <= '9') {
                if (expand_token(*c, token)) {
                    return INTERNAL_ERROR;
                } 
                *c = getchar();
            }
        }
    } 
    if (*c == 'E' || *c == 'e') {
        if (expand_token(*c, token)) {
             return INTERNAL_ERROR;
        }
        *c = getchar();
        switch(control_exp(token, c)) {
            case LEXICAL_ERROR: return LEXICAL_ERROR;
            case INTERNAL_ERROR: return INTERNAL_ERROR;
            default: return 0;
        }
    } else if (*c == ';' ||
               *c == ',' || 
               *c == '>' ||
               *c == '<' ||
               *c == '+' ||
               *c == '-' || 
               *c == '*' || 
               *c == '/' || 
               *c == '%' ||
               *c == '!' || 
               *c == '=' ||
               *c == ' ' || 
               *c == '|' ||
               *c == EOL || 
               *c == '&' ||
               isspace(*c)) {
               parser_function(token);
               return 0;
    }
    return LEXICAL_ERROR;
}

int get_next_token(Token *token) {
    char c = getchar();
    while (c != EOF) {
        // token (
        if (c == '(') {
            token->type = ROUND_BR_L;
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            } else {
                parser_function(token);
                c = getchar(); 
            } 

        // token ) 
        } else if (c == ')') {
            token->type = ROUND_BR_R;
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            } else {
                parser_function(token);
                c = getchar(); 
            }

        // token [
        } else if (c == '[') {
            token->type = SQUARE_BR_L;
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            } else {
                parser_function(token);
                c = getchar(); 
            }

        // token ]
        } else if (c == ']') {
            token->type = SQUARE_BR_R;
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            } else {
                parser_function(token);
                c = getchar(); 
            }

        // token { 
        } else if (c == '{') {
            token->type = CURLY_BR_L;
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            } else {
                parser_function(token); 
                c = getchar();
            }
        
        // token }
        } else if (c == '}') {
            token->type = CURLY_BR_R;
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            } else {
                parser_function(token); 
                c = getchar();
            }

        // token := 
        } else if (c == ':') {
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            }
            c = getchar();
            if (c == '=') {
                token->type = DECLARE;
                if (expand_token(c, token)) {
                    return INTERNAL_ERROR;
                } else {
                    parser_function(token);
                    c = getchar();
                }
            } else {
                return LEXICAL_ERROR;
            }

        // token >
        } else if (c == '>') {
            token->type = G;
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            }
            c = getchar();
            
            // token >=
            if (c == '=') {
                token->type = GE;
                if (expand_token(c, token)) {
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
                return INTERNAL_ERROR;
            }
            c = getchar();

            // token <=
            if (c == '=') {
                token->type = LE;
                if (expand_token(c, token)) {
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
                return INTERNAL_ERROR;
            } else {
                parser_function(token);
                c = getchar();
            }

        // token -
        } else if (c == '-') {
            token->type = SUB;
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            } else {
                parser_function(token);
                c = getchar();
            }

        // token * 
        } else if (c == '*') {
            token->type = MUL;
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            } else {
                parser_function(token);
                c = getchar();
            }

        // token %
        } else if (c == '%') {
            token->type = MOD;
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            } else {
                parser_function(token);
                c = getchar();
            }

        // token ,
        } else if (c == ',') {
            token->type = COMMA;
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            } else {
                parser_function(token);
                c = getchar();
            }

        // token .
        } else if (c == '.') {
            token->type = POINT;
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            } else {
                parser_function(token);
                c = getchar();
            }

        // token ;
        } else if (c == ';') {
            token->type = COLON;
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            } else {
                parser_function(token);
                c = getchar();
            }

        // token &&
        } else if (c == '&') {
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            }
            c = getchar();
            if (c == '&') {
                token->type = AND;
                if (expand_token(c, token)) {
                    return INTERNAL_ERROR;
                } else {
                    parser_function(token);
                    c = getchar();
                }
            } else {
                return LEXICAL_ERROR;
            }

        // token ||
        } else if (c == '|') {
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            }
            c = getchar();
            if (c == '|') {
                token->type = OR;
                if (expand_token(c, token)) {
                    return INTERNAL_ERROR;
                } else {
                    parser_function(token);
                    c = getchar();
                }
            } else {
                return LEXICAL_ERROR;
            }

        // token !
        } else if (c == '!') {
            token->type = NOT;
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            }
            c = getchar();

            // token !=
            if (c == '=') {
                token->type = NOT_EQ;
                if (expand_token(c, token)) {
                    return INTERNAL_ERROR;
                } else {
                    parser_function(token);
                    c = getchar();
                }
            } else {
                return LEXICAL_ERROR;
            }

        // token =
        } else if (c == '=') {
            token->type = EQ_SYM;
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            }
            c = getchar();

            // token ==
            if (c == '=') {
                token->type = EQ;
                if (expand_token(c, token)) {
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
                return INTERNAL_ERROR;
            }
            c = getchar();

            // string comment
            if (c == '/') {
                c = getchar();
                while (c != EOL) {
                    c = getchar();
                }
                free_token(token);
                continue;
            // start block comment               
            } else if (c == '*') {  
                c = getchar();
                while (1) {
                    if (c == EOF) {
                        return LEXICAL_ERROR;
                    // end block comment
                    } else if (c == '*') {
                        c = getchar();
                        if (c == '/') {
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
            } else {
                parser_function(token);
            }
            c = getchar();


        // token ID        
        } else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '_')) {
            token->type = ID;
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            } 
            c = getchar();
            while (((c >= 'A' && c <= 'Z') || 
                    (c >= 'a' && c <= 'z') || 
                    (c == '_') || 
                    (c >= '0' && c <= '9'))) {
                if (expand_token(c,token)) {
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
                return INTERNAL_ERROR;
            }
            while ((c = getchar()) != '"') {
                if (c == EOF || c == EOL) {
                    return LEXICAL_ERROR;                         
                } else {
                    if (expand_token(c, token)) {
                        return INTERNAL_ERROR;
                    } else {
                        continue;
                    }
                }
            }
            if (expand_token(c, token)) {
                return INTERNAL_ERROR;
            } else {
                parser_function(token);
                c = getchar();
            }    

        // token zero 
        } else if (c == '0') {
            token->type = ZERO_MANTISSA;
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            } 
            c = getchar();

            // token float
            if (c == '.') {
                if (expand_token(c, token)) {
                    return INTERNAL_ERROR;
                } else {
                    c = getchar();  
                    switch(control_float(token, &c)) {
                        case LEXICAL_ERROR: return LEXICAL_ERROR;
                        case INTERNAL_ERROR: return INTERNAL_ERROR;
                        default: continue;
                    }
                }

            // token exp
            } else if (c == 'E' || c == 'e') {
                if (expand_token(c, token)) {
                    return INTERNAL_ERROR;
                } else {
                    c = getchar();
                    switch(control_exp(token, &c)) {
                        case LEXICAL_ERROR: return LEXICAL_ERROR;
                        case INTERNAL_ERROR: return INTERNAL_ERROR;
                        default: continue;
                    } 
                }
            } else if (c == ';' ||
                       c == ',' || 
                       c == '>' ||
                       c == '<' ||
                       c == '+' ||
                       c == '-' || 
                       c == '*' || 
                       c == '/' || 
                       c == '%' ||
                       c == '!' || 
                       c == '=' ||
                       c == ' ' || 
                       c == '|' ||
                       c == '&' || 
                       c == EOL ||
                       isspace(c)) {
                        parser_function(token);

            } else {
                return LEXICAL_ERROR;                             
            }

        // token integer 
        } else if (c >= '1' && c <= '9') {
            token->type = INTEGER;
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            }
            c = getchar();
            while (c >= '0' && c <= '9' ) {
                if (expand_token(c, token)) {
                    return INTERNAL_ERROR;
                } else {
                    c = getchar();
                }
            }
            if (c == '.') {
                if (expand_token(c, token)) {
                    return INTERNAL_ERROR;
                } else {
                    c = getchar();
                    switch(control_float(token, &c)) {
                        case LEXICAL_ERROR: return LEXICAL_ERROR;
                        case INTERNAL_ERROR: return INTERNAL_ERROR;
                        default: continue;
                    }
                }
            } else if (c == 'E' || c == 'e') {
                if (expand_token(c, token)) {
                    return INTERNAL_ERROR;
                } else {
                    c = getchar();
                    switch(control_exp(token, &c)) {
                        case LEXICAL_ERROR: return LEXICAL_ERROR;
                        case INTERNAL_ERROR: return INTERNAL_ERROR;
                        default: continue;
                    }
                }
            } else if (c == ';' || 
                       c == '>' ||
                       c == '<' ||
                       c == '+' ||
                       c == '-' || 
                       c == '*' || 
                       c == '/' || 
                       c == '%' ||
                       c == '!' || 
                       c == '=' ||
                       c == ' ' || 
                       c == '|' ||
                       c == ',' ||
                       c == '&' ||
                       c == EOL || 
                       isspace(c)) {
                        parser_function(token);
            } else {
                return LEXICAL_ERROR;
            }
        } else if (c == EOL) {
            
            token->type = END_OF_LINE;
            parser_function(token);
            c = getchar();
        } else if (isspace(c)) {
            c = getchar();
        } else {
            return LEXICAL_ERROR;
        }
    }
    return 0;
}
