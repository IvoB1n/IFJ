#include "scanner.h"
#include "error.h"
#include <stdlib.h>
#include <stdio.h>

#define EOL '\n'  // ???

int init_token(char c, Token *token) {
    //printf("init token1\n");
    if (token->data = malloc(sizeof(char))) {
        token->data[token->data_size] = c;
        token->data_size++;
        //printf( "%d, %s, %d\n", token->type, token->data, token->data_size);
    } else {
        fprintf(stderr, "Malloc error\n");
        return 1;
    }
    return 0;
}

int expand_token(char c, Token *token) {
    //printf("expand token1\n");
    if (token->data = realloc(token->data, (token->data_size + 1) * sizeof(char))) {
        token->data[token->data_size] = c;
        token->data_size++;
        //printf( "%d, %s, %d\n", token->type, token->data, token->data_size);
    } else {
        fprintf(stderr, "Realloc error\n");
        return 1;
    }
    return 0;
}

void free_token (Token *token) {
    //printf("free memory\n");
    free(token->data);
    token->data = NULL;
    //token->data[0] = '\0';
    token->data_size = 0;
    //printf( "%d, %s, %d\n", token->type, token->data, token->data_size);
}

void parser_function (Token *token) {
    printf("GO TO PARSER\n");
    printf( "%s, %d\n", token->data, token->data_size);
    //printf("parser2\n");
    free_token(token);
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
                fprintf(stderr, "Bad number\n");
                return 4;
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
        fprintf(stderr, "Bad number\n");
        return 4;
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
            }
        }
    } 
    if (*c == 'E' || *c == 'e') {
        *c = getchar();
        control_exp(token, c);
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
               *c == EOL || *c == '\t' || *c == '\f' || *c == '\r' || *c == '\v' ||
               *c == '&') {
               parser_function(token);
               return 0;
            } else {
        fprintf(stderr, "Bad number\n");
        return 4;
    }
}

int get_next_token(Token *token, FILE *f) {
    char c = getchar();
    while (c != EOF) {

        //printf("111\n");
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
                fprintf(stderr, "Bad token\n");
                return 2;
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
                fprintf(stderr, "Bad token\n");
                return 2;
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
                fprintf(stderr, "Bad token\n");
                return 2;
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
                fprintf(stderr, "Bad token\n");
                return 2;
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
                    printf("%c", c);
                    c = getchar();
                }
                printf("\n");
                //c = getchar();
                free_token(token);
                continue;
            // start block comment               
            } else if (c == '*') {  
                c = getchar();
                while (1) {
                    if (c == EOF) {
                        fprintf(stderr, "Bad comment\n");
                        return 3;
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
                    c = getchar();              // gfskgm*
                }
            }
            parser_function(token);
        
        // token string 
        } else if (c == '"') {
            token->type = STR_END;
            if (init_token(c, token)) {
                return INTERNAL_ERROR;
            }
            while ((c = getchar()) != '"') {
                if (c == EOF || c == EOL) {
                    fprintf(stderr, "Bad string\n");        /// a = 0; "??????
                    return 5;                               /// "gnoisgo"
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
                    control_float(token, &c); // napisat
                }

            // token exp
            } else if (c == 'E' || c == 'e') {
                if (expand_token(c, token)) {
                    return INTERNAL_ERROR;
                } else {
                    c = getchar();
                    control_exp(token, &c);  // napisat
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
                       c == '&' || c == '\t' || c == '\f' || c == '\r' || c == '\v' ||
                       c == EOL) {
                        parser_function(token);
                        //continue;

            } else {
                fprintf(stderr, "Bad number\n");        /// a = 0; ??????
                return 4;                               // 0afimogsf
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
                    control_float(token, &c);
                }
            } else if (c == 'E' || c == 'e') {
                if (expand_token(c, token)) {
                    return INTERNAL_ERROR;
                } else {
                    c = getchar();
                    control_exp(token, &c);
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
                       c == EOL || c == '\t' || c == '\f' || c == '\r' || c == '\v' ||
                       c == '&') {
                        parser_function(token);
                        //continue;
            } else {
                fprintf(stderr, "Bad number\n");        /// a = 0*2; ??????
                return 4;
            }
        } else if (c == EOL) {
            //printf("End of line\n");
            parser_function(token);
            c = getchar();
        } else if (c == ' ' || c == '\t' || c == '\f' || c == '\r' || c == '\v') {
            //printf("whitespace\n");
            //parser_function(token);
            c = getchar();
            //printf("%c\n", c);
        }
    }
    return 0;
}

