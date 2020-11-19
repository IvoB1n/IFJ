#include "scanner.h"
#include "symtable.h"
#include "error.h"

#include <stdbool.h>
#include <string.h>

#define FUNC_CALL 100

void print_token (Token *token) {
    printf("                       type= %-4d data= \"%s\"%-5s size= %-9d\n", token->type, token->data, "", token->data_size);
}

void print_token_list (tDLList *L) {
    tDLElemPtr tmp = L->First;
    printf("---------------\n");
    while (tmp != NULL) {
        printf( "type= %-4d data= \"%s\" size= %-9d\n", tmp->token.type, tmp->token.data, tmp->token.data_size);
        tmp = tmp->rptr;
    }
    printf("---------------\n");
}


int statement_rule(Token *token);
int assignment_rule(Token *token);
int func_call(Token *token);

void get_next_token(Token *token) {
    if (token_list.Act != NULL) {
        *token = token_list.Act->token;

        print_token(token);
  
        DLSucc(&token_list);
    }

}

int ivo_expr(Token *token) {
    printf("%s %d\n", __FILE__, __LINE__);
    get_next_token(token);
    return 0;
}

int expression_rule(Token *token) {
    get_next_token(token);
    printf("%s %d\n", __FILE__, __LINE__);
    int retval = 0;
    if (token->type == ID) {
        printf("%s %d\n", __FILE__, __LINE__);
        retval = func_call(token);
        if (retval == FUNC_CALL) {
            printf("%s %d\n", __FILE__, __LINE__);
            return 0;
        }
        else if (retval) {
            printf("%s %d\n", __FILE__, __LINE__);
            printf("%d\n", retval);
            return retval;
        }
        DLPred(&token_list);
    }
printf("%s %d\n", __FILE__, __LINE__);
    DLPred(&token_list);
    ivo_expr(token);
    return 0;
}

/*  TYPE -> int
    TYPE -> float64
    TYPE -> string
*/
int type_rule(Token *token) {
    get_next_token(token);
    if (token->type == INT) {

    }
    else if (token->type == FLOAT64) {

    }
    else if (token->type == STRING) {

    }
    else {
        return SYNTAX_ERROR;
    }
    
    return 0;
}

/*  TYPE_NEXT -> , TYPE TYPE_NEXT
    TYPE_NEXT -> )
*/
int type_next_rule(Token *token) {
    get_next_token(token);

    if (token->type == ROUND_BR_R) {
        return 0;
    }

    if (token->type != COMMA) {
        return SYNTAX_ERROR;
    }

    int retval = type_rule(token);
    if (retval) {
        return retval;
    }

    retval = type_next_rule(token);
    return retval;    
}

/*  RETURN_TYPES -> {
    RETURN_TYPES -> ( TYPE TYPE_NEXT {
    RETURN_TYPES -> TYPE {
*/
int return_types_rule(Token *token) {
    int retval = 0;
    get_next_token(token);

    if (token->type == CURLY_BR_L) {
        return 0;
    }

    if (token->type == ROUND_BR_L) {
        get_next_token(token);
        if (token->type == ROUND_BR_R) {
            get_next_token(token);
            if (token->type == CURLY_BR_L) {
                return 0;
            }
            return SYNTAX_ERROR;
        }
        DLPred(&token_list);
        retval = type_rule(token);
        if (retval) {
            return retval;
        }

        retval = type_next_rule(token);
        if (retval) {
            return retval;
        }

        get_next_token(token);
        if (token->type != CURLY_BR_L) {
            return SYNTAX_ERROR;
        }
        return 0;
    }

    retval = type_rule(token);
    if (retval) {
        return retval;
    }
    get_next_token(token);
    if (token->type != CURLY_BR_L) {
        return SYNTAX_ERROR;
    }

    return 0;
}

/*  PARAM_NEXT -> )
    PARAM_NEXT -> , id TYPE PARAM_NEXT
*/
int param_next_rule(Token *token) {
    get_next_token(token);
    if (token->type == ROUND_BR_R) {
        return 0;
    }

    if (token->type != COMMA) {
        return SYNTAX_ERROR;
    }

    get_next_token(token);
    if (token->type != ID) {
        return SYNTAX_ERROR;
    }

    // send to hashtable

    int retval = type_rule(token);
    if (retval) {
        return retval;
    }

    retval = param_next_rule(token);
    return retval;
}

/*  PARAM -> )
    PARAM -> id TYPE PARAM_NEXT
*/
int param_rule(Token *token) {
    get_next_token(token);
    if (token->type == ROUND_BR_R) {
        return 0;
    }

    if (token->type != ID) {
        return SYNTAX_ERROR;
    }

    int retval = type_rule(token);
    if (retval) {
        return retval;
    }

    retval = param_next_rule(token);
    return retval;
}

/*  EXPR_NEXT -> , EXPR EXPR_NEXT
    EXPR_NEXT -> eps
*/
int expr_next_rule(Token *token) {
    get_next_token(token);

 /*   if (token->type == END_OF_LINE) {
    
        DLPred(&token_list);
        return 0;
    }*/
printf("%s %d\n", __FILE__, __LINE__);
    if (token->type == COMMA) {
    
        int retval = expression_rule(token);
        if (retval) {
            return retval;
        }
    
        retval = expr_next_rule(token);
        return retval;
    }
printf("%s %d\n", __FILE__, __LINE__);
    DLPred(&token_list);
    return 0;
}

/*  VARS -> eps
    VARS -> EXPR EXPR_NEXT
*/
int vars_rule(Token *token) {
    get_next_token(token);
    DLPred(&token_list);
    if (token->type == END_OF_LINE){
        
        return 0;
    }
printf("%s %d\n", __FILE__, __LINE__);

    int retval = expression_rule(token);
printf("%s %d\n", __FILE__, __LINE__);
    if (retval) {
        return retval;
    }

printf("%s %d\n", __FILE__, __LINE__);
    retval = expr_next_rule(token);
    if (retval) {
    
        return retval;
    }

    return 0;
}


/*  ELSE -> else { eol STATEMENT } eol
    ELSE -> eol
*/
int else_rule(Token *token) {
    get_next_token(token);

    if (token->type == END_OF_LINE) {
        return 0;
    }

    if (token->type != ELSE) {
        return SYNTAX_ERROR;
    }

    get_next_token(token);
    if (token->type != CURLY_BR_L) {
        return SYNTAX_ERROR;
    }

    get_next_token(token);
    if (token->type != END_OF_LINE) {
        return SYNTAX_ERROR;
    }

    int retval = statement_rule(token);
    if (retval) {
        return retval;
    }

    get_next_token(token);
    if (token->type != CURLY_BR_R) {
        return SYNTAX_ERROR;
    }

    get_next_token(token);
    if (token->type != END_OF_LINE) {
        return SYNTAX_ERROR;
    }

    return 0;
}

/*  FOR_FIRST -> ;
    FOR_FIRST -> ASSIGNMENT ;
*/
int for_first_rule(Token *token) {
    get_next_token(token);

    if (token->type == SEMICOLON) {
        return 0;
    }


    int retval = assignment_rule(token);
    if (retval) {
        return retval;
    }

    get_next_token(token);
    if (token->type != SEMICOLON) {
        return SYNTAX_ERROR;
    }

    return 0;
}

/*  FOR_LAST -> ε
    FOR_LAST -> id eq_sign EXPR
*/
int for_last_rule(Token *token) {
    if (token->type == ID) {
        get_next_token(token);
        if (token->type != EQ_SYM) {
            return SYNTAX_ERROR;
        }

        int retval = expression_rule(token);
        if (retval) {
            return retval;
        }
        return 0;
    }

    DLPred(&token_list);
    return 0;
}

/*  ASSIGN_SYM -> declare
    ASSIGN_SYM -> eq_sign
*/
int assign_sym_rule(Token *token) {
    get_next_token(token);


    if (token->type == DECLARE || token->type == EQ_SYM) {
        return 0;
    }
    

    return SYNTAX_ERROR;
}

/*  ID_NEXT -> , id ID_NEXT
    ID_NEXT -> ε
*/
int id_next_rule(Token *token) {
    get_next_token(token);

    if (token->type == COMMA) {
        get_next_token(token);
        if (token->type != ID) {
            return SYNTAX_ERROR;
        }
        return id_next_rule(token);
    }
    DLPred(&token_list);
    return 0;
}

int func_call(Token *token) {
    get_next_token(token);
printf("%s %d\n", __FILE__, __LINE__);
    int retval = 0;
    if (token->type == ROUND_BR_L) {
        get_next_token(token);
        if (token->type == ROUND_BR_R) {
            return FUNC_CALL;
        }
        else {
            DLPred(&token_list);
        printf("%s %d\n", __FILE__, __LINE__);
            retval = expression_rule(token);
            if (retval) {
                return retval;
            }
    printf("%s %d\n", __FILE__, __LINE__);
            retval = expr_next_rule(token);
            if (retval) {
                return retval;
            }
    printf("%s %d\n", __FILE__, __LINE__);
            get_next_token(token);
            if (token->type != ROUND_BR_R) {
                printf("%s %d\n", __FILE__, __LINE__);  
                return SYNTAX_ERROR;
            }
            printf("%s %d\n", __FILE__, __LINE__);
            return FUNC_CALL;
        }
        printf("%s %d\n", __FILE__, __LINE__);
        return SYNTAX_ERROR;
    }
    printf("%s %d\n", __FILE__, __LINE__);
    return 0;
}


/*  ASSIGNMENT -> ( EXPR EXPR_NEXT )                  --- FUNC_CALL
    ASSIGNMENT -> ID_NEXT ASSIGN_SYM EXPR EXPR_NEXT    --- DECLARATION, ASSIGNMENT
*/
int assignment_rule(Token *token) {
    int retval = 0;
    //get_next_token(token);

    retval = func_call(token);
    if (retval == FUNC_CALL) {
        return 0;
    }
    else if (retval) {
        return retval;
    }
    else {
        
        DLPred(&token_list);
        retval = id_next_rule(token);
        if (retval) {
            return retval;
        }

        retval = assign_sym_rule(token);
        

        if (retval) {
            return retval;
        }


        retval = expression_rule(token);
        if (retval) {
            return retval;
        }


        return expr_next_rule(token);
    }

    return 0;
}


/*  
STATEMENT -> if EXPR { eol STATEMENT } ELSE STATEMENT
STATEMENT -> for FOR_FIRST ; EXPR ; FOR_LAST { eol STATEMENT } eol STATEMENT
STATEMENT -> return VARS eol STATEMENT
STATEMENT -> id ASSIGNMENT eol STATEMENT
*/
int statement_rule(Token *token) {
    int retval = 0;

    do {
    
        get_next_token(token);
    } while (token_list.Act != NULL && token->type == END_OF_LINE);


    if (token->type == IF) {
        retval = expression_rule(token);
        if (retval) {
            return retval;
        }

        get_next_token(token);
        if (token->type != CURLY_BR_L) {
            return SYNTAX_ERROR;
        }

        get_next_token(token);
        if (token->type != END_OF_LINE) {
            return SYNTAX_ERROR;
        }

        retval = statement_rule(token);
        if (retval) {
            return retval;
        }

        get_next_token(token);
        if (token->type != CURLY_BR_R) {
            return SYNTAX_ERROR;
        }

        retval = else_rule(token);
        if (retval) {
            return retval;
        }

        retval = statement_rule(token);
        if (retval) {
            return retval;
        }
    }
    else if (token->type == FOR) {
        

        retval = for_first_rule(token);
        if (retval) {
            return retval;
        }


        retval = expression_rule(token);
        if (retval) {
            return retval;
        }


        get_next_token(token);
        if (token->type != SEMICOLON) {
            return SYNTAX_ERROR;
        }


        get_next_token(token);
        retval = for_last_rule(token);
        if (retval) {
            return retval;
        }


        get_next_token(token);
        if (token->type != CURLY_BR_L) {
            return SYNTAX_ERROR;
        }

        get_next_token(token);
        if (token->type != END_OF_LINE) {
            return SYNTAX_ERROR;
        }

        retval = statement_rule(token);
        if (retval) {
            return retval;
        }

        get_next_token(token);
        if (token->type != CURLY_BR_R) {
            return SYNTAX_ERROR;
        }

        get_next_token(token);
        if (token->type != END_OF_LINE) {
            return SYNTAX_ERROR;
        }

        retval = statement_rule(token);
        if (retval) {
            return retval;
        }
    }
    else if (token->type == ID) {
    
        retval = assignment_rule(token);
        if (retval) {
            
            return retval;
        }

        get_next_token(token);
        if (token->type != END_OF_LINE) {
            return SYNTAX_ERROR;
        }

        do {
            get_next_token(token);
        } while (token_list.Act != NULL && token->type == END_OF_LINE);
        DLPred(&token_list);

    
        retval = statement_rule(token);
        if (retval) {
            return retval;
        }
    }
    else if (token->type == RETURN) {
    printf("%s %d\n", __FILE__, __LINE__);
        retval = vars_rule(token);
        if (retval) {
        printf("%s %d\n", __FILE__, __LINE__);
            return retval;
        }
    printf("%s %d\n", __FILE__, __LINE__);
        get_next_token(token);
        if (token->type != END_OF_LINE) {
            return SYNTAX_ERROR;
        }
    printf("%s %d\n", __FILE__, __LINE__);
        return statement_rule(token);
        if (retval) {
        printf("%s %d\n", __FILE__, __LINE__);
            return retval;
        }
    printf("%s %d\n", __FILE__, __LINE__);
    }
    DLPred(&token_list);

    do {  
        get_next_token(token);
    
    } while (token_list.Act != NULL && token->type == END_OF_LINE);

printf("%s %d\n", __FILE__, __LINE__);

    DLPred(&token_list);
    return 0;
}


// FUNC_DEF -> func id_func ( PARAM ) RETURN_TYPES eol STATEMENT } eol
int func_def_rule(Token *token) {

   // get_next_token(token);
    if (token->type != FUNC) {
        return SYNTAX_ERROR;
    }

    get_next_token(token);
    if (token->type != ID) {
        return SYNTAX_ERROR;
    }
    token->type = ID_FUNC;
  /*  FuncNodePtr func_node = malloc(sizeof(struct func_node));
    if (!func_node) {
        return INTERNAL_ERROR;
    }
    func_node->function_name = token->data;
*/

    get_next_token(token);
    if (token->type != ROUND_BR_L) {
      //  free(func_node);
        return SYNTAX_ERROR;
    }


    if (param_rule(token)) {
    //        free(func_node);
        return SYNTAX_ERROR;
    }


    if (return_types_rule(token)) {
//        free(func_node);
        return SYNTAX_ERROR;
    }


    get_next_token(token);
    if (token->type != END_OF_LINE) {
//        free(func_node);
        return SYNTAX_ERROR;
    }
    //DLInsertLast(func_list, func_node, FUNCTYPE);

printf("%s %d\n", __FILE__, __LINE__);
    int retval = statement_rule(token);
    printf("%s %d\n", __FILE__, __LINE__);
    if (retval) {
        return retval;
    }



    get_next_token(token);

    if (token->type != CURLY_BR_R) {
        return SYNTAX_ERROR;
    }


    get_next_token(token);

    if (token->type == END_OF_LINE || token_list.Act == token_list.Last) {
    

        return 0;
    }
    return SYNTAX_ERROR;
}

// FUNC_DEF_NEXT -> ε
// FUNC_DEF_NEXT -> FUNC_DEF FUNC_DEF_NEXT
int func_def_next_rule(Token *token) {
    if (token_list.Act == token_list.Last || token_list.Act == NULL) {
        return 0;
    }

    int retval = func_def_rule(token);


    if (retval) {
        return retval;
    }



    do {
        get_next_token(token);
    
    } while (token_list.Act != NULL && token->type == END_OF_LINE);

    retval = func_def_next_rule(token);

    if (retval) {
        return retval;
    }

    return 0;
}

// FUNCTIONS -> FUNC_DEF FUNC_DEF_NEXT
int functions_rule(Token *token) {
    int retval = func_def_rule(token);

    if (retval) {
        return retval;
    }

    do {
        get_next_token(token);
    } while (token_list.Act != NULL && token->type == END_OF_LINE);


    retval = func_def_next_rule(token);
    if (retval) {
        return retval;
    }

    return 0;
}

// PCKG -> package id eol
int package_rule(Token *token) {
    if (token->type != PACKAGE) {
        return SYNTAX_ERROR;
    }
    
    get_next_token(token);
    if (strcmp(token->data, "main\0") != 0) {
        printf("----- %s---\n", token->data);
            return SYNTAX_ERROR;
    }

    get_next_token(token);
    if (token->type != END_OF_LINE) {
            return SYNTAX_ERROR;
    }
    return 0;
}

// GO -> PCKG FUNCTIONS
int parse() {
    Token token;


    do {
        get_next_token(&token);
    } while (token_list.Act != NULL && token.type == END_OF_LINE);
    
    int retval = package_rule(&token);


    if (retval) {
        return retval;
    }

    do {
        get_next_token(&token);
    } while (token_list.Act != NULL && token.type == END_OF_LINE);


    retval =  functions_rule(&token);

    return retval;
}