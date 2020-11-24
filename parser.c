// #include "scanner.h"
#include "error.h"
#include "symtable.h"
#include "expression.h"

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


void free_variable_item(Sym_table_item *var_item){
    if (var_item->name) {
        free(var_item->name);
    }
    free(var_item);
}

int increase_depth(unsigned *depth) {
    (*depth)++;
    Sym_table_item *var_item = malloc(sizeof(Sym_table_item));
    if (!var_item) {
        return INTERNAL_ERROR;
    }    
    var_item->depth = *depth;

    var_item->name = malloc(sizeof("_\0"));
    if (!var_item->name) {
        free_variable_item(var_item);
        return INTERNAL_ERROR;
    }
    strcpy(var_item->name, "_\0");
    var_item->value.var.type = ID;
    sym_table_insert_item(&sym_table, var_item);
    print_sym_table_items(&sym_table);
    return 0;
}

void decrease_depth(unsigned *depth) {
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    fprintf(stderr, "clean depth = %u\n", *depth);
    sym_table_delete_on_depth(&sym_table, *depth);
    (*depth)--;
}

int statement_rule(Token *token, unsigned depth, char *curr_func_name, int *num_of_returns);
int assignment_rule(Token *token, unsigned depth);
int func_call(Token *token, tDLList *list, unsigned depth);
int id_next_rule(Token *token, tDLList *assign_list, unsigned depth);

void get_next_token(Token *token) {
    if (token_list.Act != NULL) {
        *token = token_list.Act->token;

        print_token(token);
  
        DLSucc(&token_list);
    }

}

int expression_rule(Token *token, tDLList *list, unsigned depth, unsigned is_func) {
    get_next_token(token);
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    int retval = 0;
    if (token->type == ID) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        retval = func_call(token, list, depth);
        if (retval == FUNC_CALL) {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            return 0;
        }
        else if (retval) {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            printf("%d\n", retval);
            return retval;
        }
        DLPred(&token_list);
    }
fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
  
    //return 0;
    DLPred(&token_list);
    return expression(list, is_func, depth);
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
int param_next_rule(Token *token, unsigned depth) {
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

    Sym_table_item *var_item = malloc(sizeof(Sym_table_item));
    if (!var_item) {
        return INTERNAL_ERROR;
    }    
    var_item->depth = depth;

    var_item->name = malloc(token->data_size);
    if (!var_item->name) {
        free_variable_item(var_item);
        return INTERNAL_ERROR;
    }
    strcpy(var_item->name, token->data);

    int retval = type_rule(token);
    if (retval) {
        free_variable_item(var_item);
        return retval;
    }

    DLPred(&token_list);
    var_item->value.var.type = token->type;
    DLSucc(&token_list);

    retval = param_next_rule(token, depth);
    if (retval) {
        free_variable_item(var_item);
        return retval;
    }
    retval = sym_table_insert_item(&sym_table, var_item);
    print_sym_table_items(&sym_table);
    if (retval) {
        free_variable_item(var_item);
    }
    return retval;
}

/*  PARAM -> )
    PARAM -> id TYPE PARAM_NEXT
*/
int param_rule(Token *token, unsigned depth) {
    get_next_token(token);
    if (token->type == ROUND_BR_R) {
        return 0;
    }

    if (token->type != ID) {
        return SYNTAX_ERROR;
    }
fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    Sym_table_item *var_item = malloc(sizeof(Sym_table_item));
    if (!var_item) {
        return INTERNAL_ERROR;
    }
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    var_item->depth = depth;
    var_item->name = malloc(token->data_size * sizeof(char));
    if (!var_item->name) {
        free_variable_item(var_item);
        return INTERNAL_ERROR;
    }
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    strcpy(var_item->name, token->data);
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    printf("%s\n", token->data);
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    printf("%s\n", var_item->name);
fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    int retval = type_rule(token);
    if (retval) {
        free_variable_item(var_item);
        return retval;
    }
fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    DLPred(&token_list);
    var_item->value.var.type = token->type;
    DLSucc(&token_list);
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    retval = sym_table_insert_item(&sym_table, var_item);
    print_sym_table_items(&sym_table);
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    
    if (retval) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        free_variable_item(var_item);
        return retval;
    }
fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    return param_next_rule(token, depth);
}

/*  EXPR_NEXT -> , EXPR EXPR_NEXT
    EXPR_NEXT -> eps
*/
int expr_next_rule(Token *token, tDLList *list, unsigned depth, unsigned is_func) {
    get_next_token(token);

fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    if (token->type == COMMA) {
    
        int retval = expression_rule(token, list, depth, is_func);
        if (retval) {
            return retval;
        }
    
        retval = expr_next_rule(token, list, depth, is_func);
        return retval;
    }
fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    DLPred(&token_list);
    return 0;
}

/*  VARS -> eps
    VARS -> EXPR EXPR_NEXT
*/
int vars_rule(Token *token, tDLList *ret_list, unsigned depth) {
    get_next_token(token);
    DLPred(&token_list);
    if (token->type == END_OF_LINE){
        
        return 0;
    }

    int retval = expression_rule(token, ret_list, depth, NONE);
fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    if (retval) {
        DLDisposeList(ret_list);
        return retval;
    }

fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    retval = expr_next_rule(token, ret_list, depth, NONE);
    if (retval) {
        DLDisposeList(ret_list);
        return retval;
    }
    return 0;
}



int check_types_in_assignment(tDLList *left_list, tDLList *right_list, unsigned depth) {
  //  DLFirst(left_list);
   // DLFirst(right_list);
/*
    while (left_list->Act) {
        fprintf(stderr, "name = %s, l type = %u\n",left_list->Act->token.data, left_list->Act->token.type);
        DLSucc(left_list);
    }
    while (right_list->Act) {
        fprintf(stderr, "name = %s, r type = %u\n", left_list->Act->token.data, right_list->Act->token.type);
        DLSucc(right_list);
    }*/
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    DLFirst(left_list);
    DLFirst(right_list);
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    while (left_list->Act && right_list->Act) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        if (left_list->Act->token.data) {
            if (strcmp(left_list->Act->token.data, "_\0") == 0) {
                DLSucc(left_list);
                DLSucc(right_list);
                continue;
            }
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

            Sym_table_item *item = sym_table_search_item(&sym_table, left_list->Act->token.data, depth);
            if (!item) {
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                fprintf(stderr, "depth = %d\n", depth);
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

                return SEMANTIC_UNDEFINED_VAR_ERROR;
            }
                    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

            if (item->value.var.type != right_list->Act->token.type) {
                        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

                printf("left = %u, right = %u \n", left_list->Act->token.type, right_list->Act->token.type);
                return SEMANTIC_OTHER_ERRORS;
            }
        }
        else {
            if (left_list->Act->token.type != right_list->Act->token.type) {
                        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

                printf("left = %u, right = %u \n", left_list->Act->token.type, right_list->Act->token.type);
                return SEMANTIC_OTHER_ERRORS;
            }
        }
        DLSucc(left_list);
        DLSucc(right_list);
    }
    if (left_list->Act || right_list->Act) {
        return SEMANTIC_OTHER_ERRORS;
    }
    return 0;
}

/*  ELSE -> else { eol STATEMENT } eol
    ELSE -> eol
*/
int else_rule(Token *token, unsigned depth, char *curr_func_name, int *num_of_returns) {
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

    int retval = increase_depth(&depth);
    if (retval) {
        return retval;
    }

    retval = statement_rule(token, depth, curr_func_name, num_of_returns);
    if (retval) {
        return retval;
    }

    get_next_token(token);
    if (token->type != CURLY_BR_R) {
        return SYNTAX_ERROR;
    }
    
    decrease_depth(&depth);

    get_next_token(token);
    if (token->type != END_OF_LINE) {
        return SYNTAX_ERROR;
    }

    return 0;
}

/*  FOR_FIRST -> ;
    FOR_FIRST -> ASSIGNMENT ;
*/
int for_first_rule(Token *token, unsigned depth) {
    get_next_token(token);

    if (token->type == SEMICOLON) {
        return 0;
    }


    int retval = assignment_rule(token, depth);
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
int for_last_rule(Token *token, unsigned depth) {
     fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
     int retval;
    if (token->type == ID) {
         fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        tDLList left_list, right_list;
        DLInitList(&left_list);
        DLInitList(&right_list);

        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        DLInsertLast(&left_list, token);

        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        retval = id_next_rule(token, &left_list, depth);
        if (retval) {
            DLDisposeList(&left_list);
            DLDisposeList(&right_list);
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        get_next_token(token);
        if (token->type != EQ_SYM) {
            DLDisposeList(&left_list);
            DLDisposeList(&right_list);
            return SYNTAX_ERROR;
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        retval = expression_rule(token, &right_list, depth, NONE);
        if (retval) {
            DLDisposeList(&left_list);
            DLDisposeList(&right_list);
            return retval;
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        retval = expr_next_rule(token, &right_list, depth, NONE);
        if (retval) {
            DLDisposeList(&left_list);
            DLDisposeList(&right_list);
            return retval;
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);   
        retval = check_types_in_assignment(&left_list, &right_list, depth);

        DLDisposeList(&left_list);
        DLDisposeList(&right_list);
        return retval;
    }

    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    DLPred(&token_list);
    return 0;
}

/*  ASSIGN_SYM -> declare
    ASSIGN_SYM -> eq_sign
*/
int assign_sym_rule(Token *token) {
    get_next_token(token);

    if (token->type == DECLARE || token->type == EQ_SYM) {
        return token->type;
    }
    
    return SYNTAX_ERROR;
}

/*  ID_NEXT -> , id ID_NEXT
    ID_NEXT -> ε
*/
int id_next_rule(Token *token, tDLList *assign_list, unsigned depth) {
    get_next_token(token);
fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    if (token->type == COMMA) {
        get_next_token(token);
fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        if (token->type != ID) {
            return SYNTAX_ERROR;
        }
     /*   if (!sym_table_search_item(&sym_table, token->data, depth)) {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            return SEMANTIC_UNDEFINED_VAR_ERROR;
        }*/
        DLInsertLast(assign_list, token);
fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return id_next_rule(token, assign_list, depth);
    }
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    DLPred(&token_list);
    return 0;
}

int func_call(Token *token, tDLList *list, unsigned depth) {
    char *func_name = token->data;
    get_next_token(token);
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    int retval = 0;
    if (token->type == ROUND_BR_L) {
        get_next_token(token);
        if (token->type == ROUND_BR_R) {
            Sym_table_item *item =  sym_table_search_item(&sym_table, func_name, depth);
            if (item) {
                return SEMANTIC_OTHER_ERRORS;
            }
            item =  sym_table_search_item(&sym_table, func_name, 0);
            if (!item) {
                return SEMANTIC_UNDEFINED_VAR_ERROR;
            }
            for (unsigned i = 0; i < item->value.func.num_out_var; i++) {
                Token token_return = {0, NULL, 0};
                token_return.type = item->value.func.out_var_list[i];
                DLInsertLast(list, &token_return); 
            }
            return FUNC_CALL;
        }
        else {
            DLPred(&token_list);
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            tDLList func_input_list_left, func_input_list_right;
            DLInitList(&func_input_list_left);
            DLInitList(&func_input_list_right);
            retval = expression_rule(token, &func_input_list_right, depth, FUNC);
            if (retval) {
                return retval;
            }
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            retval = expr_next_rule(token, &func_input_list_right, depth, FUNC);
            if (retval) {
                return retval;
            }
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            get_next_token(token);
            if (token->type != ROUND_BR_R) {
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);  
                return SYNTAX_ERROR;
            }
                   fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            Sym_table_item *item =  sym_table_search_item(&sym_table, func_name, depth);
            if (item) {
                return SEMANTIC_OTHER_ERRORS;
            }
            item =  sym_table_search_item(&sym_table, func_name, 0);
            if (!item) {
                return SEMANTIC_UNDEFINED_VAR_ERROR;
            }
                   fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            for (unsigned i = 0; i < item->value.func.num_out_var; i++) {
                Token token_return = {0, NULL, 0};
                token_return.type = item->value.func.out_var_list[i];
                DLInsertLast(list, &token_return); 
            }
                   fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            if (strcmp(func_name, "print\0") != 0) {
                for (unsigned i = 0; i < item->value.func.num_in_var; i++) {
                    Token token_return = {0, NULL, 0};
                    token_return.type = item->value.func.in_var_list[i];
                    DLInsertLast(&func_input_list_left, &token_return); 
                }
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                retval = check_types_in_assignment(&func_input_list_left, &func_input_list_right, 0);
            }
            DLDisposeList(&func_input_list_left);
            DLDisposeList(&func_input_list_right);
            if (retval) {
                return SEMANTIC_FUNCTION_ERROR;
            }
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            return FUNC_CALL;
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return SYNTAX_ERROR;
    }
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    return 0;
}

int was_not_declared(tDLList *list, unsigned depth) {
    DLFirst(list);
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    while (list->Act != NULL) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        if (sym_table_search_item(&sym_table, list->Act->token.data, depth) == NULL) {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            return 0;
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        DLSucc(list);
    }
    return 1;
}

int was_not_declared_on_lvl(tDLList *list, unsigned depth) {
    DLFirst(list);
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    while (list->Act != NULL) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        if (init_search_item(&sym_table, list->Act->token.data, depth) == NULL) {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            return 0;
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        DLSucc(list);
    }
    return 1;
}


int insert_new_vars(tDLList *left_list, tDLList *right_list, unsigned depth) {
    DLFirst(left_list);
    DLFirst(right_list);
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    while (left_list->Act && right_list->Act) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        printf("*************************************************** \n");
        printf("name = %s, type = %u \n", left_list->Act->token.data, left_list->Act->token.type);
        printf("name = %s, type = %u \n", right_list->Act->token.data, right_list->Act->token.type);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        Sym_table_item *item = init_search_item(&sym_table, left_list->Act->token.data, depth);
        if (item) {
            DLSucc(left_list);
            DLSucc(right_list);
            continue;
        }

        left_list->Act->token.type = right_list->Act->token.type;
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        Sym_table_item *var_item = malloc(sizeof(Sym_table_item));
        if (!var_item) {
            return INTERNAL_ERROR;
        }    
        var_item->depth = depth;
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

            var_item->name = malloc(left_list->Act->token.data_size);
            if (!var_item->name) {
                free_variable_item(var_item);           
                return INTERNAL_ERROR;
            }
            strcpy(var_item->name, left_list->Act->token.data);
            var_item->value.var.type = left_list->Act->token.type;
                    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

            sym_table_insert_item(&sym_table, var_item);
            print_sym_table_items(&sym_table);
            //return 0;
       // }
        DLSucc(left_list);
        DLSucc(right_list);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    }
    return 0;
}


/*  ASSIGNMENT -> ( EXPR EXPR_NEXT )                  --- FUNC_CALL
    ASSIGNMENT -> ID_NEXT ASSIGN_SYM EXPR EXPR_NEXT    --- DECLARATION, ASSIGNMENT
*/
int assignment_rule(Token *token, unsigned depth) {
    int retval = 0;

    tDLList assign_left_list, assign_right_list;
    DLInitList(&assign_left_list);
    DLInitList(&assign_right_list);
        
    retval = func_call(token, &assign_left_list, depth); // TODO: is ot right?
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    if (retval == FUNC_CALL) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return 0;
    }
    else if (retval) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return retval;
    }
    else {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        DLPred(&token_list);
        DLPred(&token_list);
        get_next_token(token);
        DLInsertLast(&assign_left_list, token);

fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        retval = id_next_rule(token, &assign_left_list, depth);
        if (retval) {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            return retval;
        }
 fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        switch (assign_sym_rule(token)) {
            case DECLARE:
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                if (was_not_declared_on_lvl(&assign_left_list, depth) == 1) {
                    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                    return SEMANTIC_UNDEFINED_VAR_ERROR;
                }
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

               
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

                if (retval){
                    return retval;
                }
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                break;

            case EQ_SYM:
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                if (was_not_declared(&assign_left_list, depth) == 0 ) {
                    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                    return SEMANTIC_UNDEFINED_VAR_ERROR;
                }
                break;

            default:
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                return SYNTAX_ERROR;
        }
fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        retval = expression_rule(token, &assign_right_list, depth, NONE);
        if (retval) {
            return retval;
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        retval = expr_next_rule(token, &assign_right_list, depth, NONE);
        if (retval) {
            DLDisposeList(&assign_left_list);
            DLDisposeList(&assign_right_list);
            return retval;
        }
    }
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    retval = insert_new_vars(&assign_left_list, &assign_right_list, depth);
fprintf(stderr, "%s %d\n", __FILE__, __LINE__);    
    if (retval) {
        DLDisposeList(&assign_left_list);
        DLDisposeList(&assign_right_list);
    }
    
    retval = check_types_in_assignment(&assign_left_list, &assign_right_list, depth);

    DLDisposeList(&assign_left_list);
    DLDisposeList(&assign_right_list);
    return retval;
}


/*  
STATEMENT -> if EXPR { eol STATEMENT } ELSE STATEMENT
STATEMENT -> for FOR_FIRST ; EXPR ; FOR_LAST { eol STATEMENT } eol STATEMENT
STATEMENT -> return VARS eol STATEMENT
STATEMENT -> id ASSIGNMENT eol STATEMENT
*/
int statement_rule(Token *token, unsigned depth, char *curr_func_name, int *num_of_returns) {
    int retval = 0;
    do {
        get_next_token(token);
    } while (token_list.Act != NULL && token->type == END_OF_LINE);

    if (token->type == IF) {
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        retval = increase_depth(&depth);
        if (retval) {
            return retval;
        }
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        tDLList unused_list;
        DLInitList(&unused_list);
        retval = expression_rule(token, &unused_list, depth, NONE);
        if (retval) {
            return retval;
        }
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        DLDisposeList(&unused_list);
        get_next_token(token);
        if (token->type != CURLY_BR_L) {
            return SYNTAX_ERROR;
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
/*
        get_next_token(token);
        if (token->type != END_OF_LINE) {
            return SYNTAX_ERROR;
        } */
        retval = statement_rule(token, depth, curr_func_name, num_of_returns);
        if (retval) {
            return retval;
        }

        get_next_token(token);
        if (token->type != CURLY_BR_R) {
            return SYNTAX_ERROR;
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        decrease_depth(&depth);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        retval = else_rule(token, depth, curr_func_name, num_of_returns);
        if (retval) {
            return retval;
        }

        retval = statement_rule(token, depth, curr_func_name, num_of_returns);
        if (retval) {
            return retval;
        }
    }
    else if (token->type == FOR) {
        retval = increase_depth(&depth);
        if (retval) {
            return retval;
        }

        retval = for_first_rule(token, depth);
        if (retval) {
            return retval;
        }

        tDLList unused_list;
        DLInitList(&unused_list);
        retval = expression_rule(token, &unused_list, depth, NONE);
        if (retval) {
            return retval;
        }
        DLDisposeList(&unused_list);

        get_next_token(token);
        if (token->type != SEMICOLON) {
            return SYNTAX_ERROR;
        }

        get_next_token(token);
        retval = for_last_rule(token, depth);
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

        retval = increase_depth(&depth);
        if (retval) {
            return retval;
        }
        
        retval = statement_rule(token, depth, curr_func_name, num_of_returns);
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
        
        decrease_depth(&depth);
        decrease_depth(&depth);

        retval = statement_rule(token, depth, curr_func_name, num_of_returns);
        if (retval) {
            return retval;
        }
    }
    else if (token->type == ID) {
        retval = assignment_rule(token, depth);
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

    
        retval = statement_rule(token, depth, curr_func_name, num_of_returns);
        if (retval) {
            return retval;
        }
    }
    else if (token->type == RETURN) {
        tDLList ret_list;
        DLInitList(&ret_list);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        retval = vars_rule(token, &ret_list, depth);
        if (retval) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            DLDisposeList(&ret_list);
            return retval;
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        get_next_token(token);
        if (token->type != END_OF_LINE) {
            DLDisposeList(&ret_list);
            return SYNTAX_ERROR;
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        DLFirst(&ret_list);
        if (ret_list.Act) {
            fprintf(stderr, "ret list token type = %u\n", ret_list.Act->token.type);
            DLSucc(&ret_list);
        }
        DLFirst(&ret_list);
        Sym_table_item *item = sym_table_search_item(&sym_table, curr_func_name, 0);
        if (!item) {
            DLDisposeList(&ret_list);
            return SEMANTIC_UNDEFINED_VAR_ERROR;
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        if ((item->value.func.num_out_var > 0 && ret_list.Act == NULL) 
                || (item->value.func.num_out_var == 0 && ret_list.Act != NULL)) {
                DLDisposeList(&ret_list);
                return SEMANTIC_FUNCTION_ERROR;
            }
        for (unsigned i = 0; i < item->value.func.num_out_var; i++) {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

            if (item->value.func.out_var_list[i] != ret_list.Act->token.type) {
                 fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

                return SEMANTIC_FUNCTION_ERROR;
            }
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

            DLSucc(&ret_list);
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        (*num_of_returns)++;
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return statement_rule(token, depth, curr_func_name, num_of_returns);
        if (retval) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            return retval;
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    }
    DLPred(&token_list);

    do {  
        get_next_token(token);
    
    } while (token_list.Act != NULL && token->type == END_OF_LINE);

    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    DLPred(&token_list);


    return 0;
}


// FUNC_DEF -> func id_func ( PARAM ) RETURN_TYPES eol STATEMENT } eol
int func_def_rule(Token *token) {
    unsigned depth = 0;
    int num_of_returns = 0;
    int retval = increase_depth(&depth);
    if (retval) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return retval;
    }

    if (token->type != FUNC) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return SYNTAX_ERROR;
    }

    get_next_token(token);
    if (token->type != ID) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return SYNTAX_ERROR;
    }
    token->type = ID_FUNC;
    char *curr_func_name = token->data;

    get_next_token(token);
    if (token->type != ROUND_BR_L) {

        return SYNTAX_ERROR;
    }

    if (param_rule(token, depth)) {
        return SYNTAX_ERROR;
    }


    if (return_types_rule(token)) {
        return SYNTAX_ERROR;
    }

    get_next_token(token);
    if (token->type != END_OF_LINE) {
        return SYNTAX_ERROR;
    }

    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    retval = statement_rule(token, depth, curr_func_name, &num_of_returns);
    if (retval) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return retval;
    }    
    Sym_table_item *item = sym_table_search_item(&sym_table, curr_func_name, 0);
        if (!item) {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            return SEMANTIC_UNDEFINED_VAR_ERROR;
        }
    if (num_of_returns == 0 && item->value.func.num_out_var > 0) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return SEMANTIC_FUNCTION_ERROR;
    }
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    decrease_depth(&depth);
    get_next_token(token);
    if (token->type != CURLY_BR_R) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return SYNTAX_ERROR;
    }


    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);


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
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    if (retval) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return retval;
    }

    do {
        get_next_token(token);
    } while (token_list.Act != NULL && token->type == END_OF_LINE);


    retval = func_def_next_rule(token);
    if (retval) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
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
            return SEMANTIC_OTHER_ERRORS;
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
    token.data = NULL;
    token.data_size = 0;

    do {
        get_next_token(&token);
    } while (token_list.Act != NULL && token.type == END_OF_LINE);
    
    int retval = package_rule(&token);
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    if (retval) {
        return retval;
    }
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    do {
        get_next_token(&token);
    } while (token_list.Act != NULL && token.type == END_OF_LINE);
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    retval =  functions_rule(&token);
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    if (retval) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return retval;
    }

    Sym_table_item *item = sym_table_search_item(&sym_table, "main\0", 0);
    if (!item) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return SEMANTIC_UNDEFINED_VAR_ERROR;
    }
    if (item->value.func.num_in_var != 0 || item->value.func.num_out_var != 0 ) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return SEMANTIC_FUNCTION_ERROR;
    }
    return 0;
}