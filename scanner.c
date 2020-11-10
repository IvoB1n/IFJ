#include "scanner.h"
//#include "error.h"
#include <stdlib.h>
#include <stdio.h>

#define EOL '\n'  // ???

int init_token(char c, Token *token) {
    if (token->data = malloc(sizeof(char))) {
        token->data[token->data_size] = c;
        token->data_size++;
    } else {
        fprintf(stderr, "Malloc error\n");
        return 1;
    }
    return 0;
}

int expand_token(char c, Token *token) {
    if (token->data = realloc(token->data, (token->data_size + 1) * sizeof(char))) {
        token->data[token->data_size] = c;
        token->data_size++;
    } else {
        fprintf(stderr, "Realloc error\n");
        return 1;
    }
    return 0;
}

char control_float(Token *token, FILE *f) {
    token->type = FLOAT;
    char c = getchar(f);
    
    if (c >= '0' && c <= '9') {
        if (expand_token(c, token)) {
            //continue;
        } else {
            c = getchar(f);
            while (c >= '0' && c <= '9') {
                if (expand_token(c, token)) {
                continue;
                }
            }
        }
    } 
    if (c == 'E' || c == 'e') {
        control_exp(token, *f);
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
               c == '&') {
               parser_function(token);
               return c;
            } else {
        fprintf(stderr, "Bad number\n");
        return 4;
    }
}

char control_exp(Token *token, FILE f) {
    char c = getchar(f);
    if (c == '+' || c == '-') {
        if (expand_token(c, token)) {
            //continue;
        } else {
            c = getchar(f);
            if (c >= '0' && c <= '9') {
                token->type = EXP_FINAL_NUM;
                if (expand_token(c, token)) {
                    //continue;
                } else {
                    c = getchar(f);
                    while (c >= '0' && c <= '9') {
                        if (expand_token(c, token)) {
                        //continue;
                        } else {
                            c = getchar(f);
                        }
                    }
                    parser_function(token);
                    return c;
                }
            } else {
                fprintf(stderr, "Bad number\n");
                return 4;
            }
        }
    } else if (c >= '0' && c <= '9') {
        token->type = EXP_FINAL_NUM;
        if (expand_token(c, token)) {
        //continue;
        } else {
            c = getchar(f);
            while (c >= '0' && c <= '9') {
                if (expand_token(c, token)) {
                    continue;
                } else {
                    c = getchar(f);
                }
            }
            parser_function(token);
            return c;
        }
    } else {
        fprintf(stderr, "Bad number\n");
        return 4;
    }
}    

void parser_function (Token *token) {
    printf( "%s, %s, %s\n", token->type, token->data, token->data_size);
    free_token(token);
}

void free_token (Token *token) {
    free(token->data);
}

int get_next_token(Token *token, FILE *f) {
    char c = getchar(f);
    while (c != EOF) {

        // token (
        if (c == '(') {
            token->type = ROUND_BR_L;
            if (init_token(c, token)) {
                continue;
            } else {
                parser_function(token);
                c = getchar(f); 
            } 

        // token ) 
        } else if (c == ')') {
            token->type = ROUND_BR_R;
            if (init_token(c, token)) {
                continue;
            } else {
                parser_function(token);
                c = getchar(f); 
            }

        // token [
        } else if (c == '[') {
            token->type = SQUARE_BR_L;
            if (init_token(c, token)) {
                continue;
            } else {
                parser_function(token);
                c = getchar(f); 
            }

        // token ]
        } else if (c == ']') {
            token->type = SQUARE_BR_R;
            if (init_token(c, token)) {
                continue;
            } else {
                parser_function(token);
                c = getchar(f); 
            }

        // token { 
        } else if (c == '{') {
            token->type = CURLY_BR_L;
            if (init_token(c, token)) {
                continue;
            } else {
                parser_function(token); 
                c = getchar(f);
            }
        
        // token }
        } else if (c == '}') {
            token->type = CURLY_BR_R;
            if (init_token(c, token)) {
                continue;
            } else {
                parser_function(token); 
                c = getchar(f);
            }

        // token := 
        } else if (c == ':') {
            if (init_token(c, token)) {
                continue;
            }
            c = getchar(f);
            if (c == '=') {
                token->type = DECLARE;
                if (expand_token(c, token)) {
                    continue;
                } else {
                    parser_function(token);
                    c = getchar(f);
                }
            } else {
                fprintf(stderr, "Bad token\n");
                return 2;
            }

        // token >
        } else if (c == '>') {
            token->type = G;
            if (init_token(c, token)) {
                continue;
            }
            c = getchar(f);
            
            // token >=
            if (c == '=') {
                token->type = GE;
                if (expand_token(c, token)) {
                    continue;
                } else {
                    parser_function(token);
                    c = getchar(f);
                }
            } else {
                parser_function(token);
            }

        // token <     
        } else if (c == '<') {
            token->type = L;
            if (init_token(c, token)) {
                continue;
            }
            c = getchar(f);

            // token <=
            if (c == '=') {
                token->type = LE;
                if (expand_token(c, token)) {
                    continue;
                } else {
                    parser_function(token);
                    c = getchar(f);
                }
            } else {
                parser_function(token);
            }

        // token +     
        } else if (c == '+') {
            token->type = ADD;
            if (init_token(c, token)) {
                continue;
            } else {
                parser_function(token);
                c = getchar(f);
            }

        // token -
        } else if (c == '-') {
            token->type = SUB;
            if (init_token(c, token)) {
                continue;
            } else {
                parser_function(token);
                c = getchar(f);
            }

        // token * 
        } else if (c == '*') {
            token->type = MUL;
            if (init_token(c, token)) {
                continue;
            } else {
                parser_function(token);
                c = getchar(f);
            }

        // token %
        } else if (c == '%') {
            token->type = MOD;
            if (init_token(c, token)) {
                continue;
            } else {
                parser_function(token);
                c = getchar(f);
            }

        // token ,
        } else if (c == ',') {
            token->type = COMMA;
            if (init_token(c, token)) {
                continue;
            } else {
                parser_function(token);
                c = getchar(f);
            }

        // token ;
        } else if (c == ';') {
            token->type = COLON;
            if (init_token(c, token)) {
                continue;
            } else {
                parser_function(token);
                c = getchar(f);
            }

        // token &&
        } else if (c == '&') {
            if (init_token(c, token)) {
                continue;
            }
            c = getchar(f);
            if (c == '&') {
                token->type = AND;
                if (expand_token(c, token)) {
                    continue;
                } else {
                    parser_function(token);
                    c = getchar(f);
                }
            } else {
                fprintf(stderr, "Bad token\n");
                return 2;
            }

        // token ||
        } else if (c == '|') {
            if (init_token(c, token)) {
                continue;
            }
            c = getchar(f);
            if (c == '|') {
                token->type = OR;
                if (expand_token(c, token)) {
                    continue;
                } else {
                    parser_function(token);
                    c = getchar(f);
                }
            } else {
                fprintf(stderr, "Bad token\n");
                return 2;
            }

        // token !
        } else if (c == '!') {
            token->type = NOT;
            if (init_token(c, token)) {
                continue;
            }
            c = getchar(f);

            // token !=
            if (c == '=') {
                token->type = NOT_EQ;
                if (expand_token(c, token)) {
                    continue;
                } else {
                    parser_function(token);
                    c = getchar(f);
                }
            } else {
                fprintf(stderr, "Bad token\n");
                return 2;
            }

        // token =
        } else if (c == '=') {
            token->type = EQ_SYM;
            if (init_token(c, token)) {
                continue;
            }
            c = getchar(f);

            // token ==
            if (c == '=') {
                token->type = EQ;
                if (expand_token(c, token)) {
                    continue;
                } else {
                    parser_function(token);
                    c = getchar(f);
                }
            } else {
                parser_function(token);
            }

        // token /
        } else if (c == '/') {           
            token->type = DIV;
            if (init_token(c, token)) {
                continue;
            }
            c = getchar(f);

            // string comment
            if (c == '/') {
                while (c = getchar(f) != EOL);    // ???sghagaeg\n
                //c = getchar(f);
                continue;
            // start block comment               
            } else if (c == '*') {  
                c = getchar(f);
                while (1) {
                    if (c == EOF) {
                        fprintf(stderr, "Bad comment\n");
                        return 3;
                    // end block comment
                    } else if (c == '*') {
                        c = getchar(f);
                        if (c == '/') {
                            break;
                        } else {
                            continue;
                        }
                    } else {
                        c = getchar(f);
                        continue;
                    }
                }
            } else {
                parser_function(token);
            }
            c = getchar(f);


        // token ID        
        } else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '_')) {
            token->type = ID;
            if (init_token(c, token)) {
                continue;
            } 
            c = getchar(f);
            while (((c >= 'A' && c <= 'Z') || 
                    (c >= 'a' && c <= 'z') || 
                    (c == '_') || 
                    (c >= '0' && c <= '9'))) {
                if (expand_token(c,token)) {
                    continue;
                } else {
                    c = getchar(f);              // gfskgm*
                }
            }
            parser_function(token);
        
        // token string 
        } else if (c == '"') {
            token->type = STR_END;
            if (init_token(c, token)) {
                continue;
            }
            while ((c = getchar(f)) != '"') {
                if (c == EOF || c == EOL) {
                    fprintf(stderr, "Bad string\n");        /// a = 0; "??????
                    return 5;                               /// "gnoisgo"
                } else {
                    if (expand_token(c, token)) {
                        continue;
                    } else {
                        continue;
                    }
                }
            }
            parser_function(token);

        // token zero 
        } else if (c == '0') {
            token->type = ZERO_MANTISSA;
            if (init_token(c, token)) {
                continue;
            } 
            c = getchar(f);

            // token float
            if (c == '.') {
                if (expand_token(c, token)) {
                continue;
                } else {  
                    control_float(token, f); // napisat
                }

            // token exp
            } else if (c == 'E' || c == 'e') {
                if (expand_token(c, token)) {
                continue;
                } else {
                    control_exp(token, *f);  // napisat
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
                       c == '&') {
                        parser_function(token);
                        continue;

            } else {
                fprintf(stderr, "Bad number\n");        /// a = 0; ??????
                return 4;                               // 0afimogsf
            }

        // token integer 
        } else if (c >= '1' && c <= '9') {
            token->type = INTEGER;
            if (init_token(c, token)) {
                continue;
            }
            c = getchar(f);
            while (c >= '0' && c <= '9' ) {
                if (expand_token(c, token)) {
                    continue;
                } else {
                    c = getchar(f);
                }
            }
            if (c == '.') {
                if (expand_token(c, token)) {
                continue;
                } else {
                    control_float(token, f);
                }
            } else if (c == 'E' || c == 'e') {
                if (expand_token(c, token)) {
                    continue;
                } else {
                    control_exp(token, *f);
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
                       c == '&') {
                        parser_function(token);
                        continue;
            } else {
                fprintf(stderr, "Bad number\n");        /// a = 0*2; ??????
                return 4;
            }
        } else if (c == EOL) {
            parser_function(token);
            c = getchar(f);
        } else if (c == ' ' || c == '\t' || c == '\f' || c == '\r' || c == '\v') {
            c = getchar(f);
        }
    }
}

