// #include "scanner.h"
#include "template.h"
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
  //  print_sym_table_items(&sym_table);
    return 0;
}

void decrease_depth(unsigned *depth) {
   
 //   fprintf(stderr, "clean depth = %u\n", *depth);
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

      //  print_token(token);
  
        DLSucc(&token_list);
    }

}

int expression_rule(Token *token, tDLList *list, unsigned depth, unsigned is_func) {
    get_next_token(token);
   
    int retval = 0;
    if (token->type == ID) {
       
        retval = func_call(token, list, depth);
        if (retval == FUNC_CALL) {
           
            return 0;
        }
        else if (retval) {
            return retval;
        }
        DLPred(&token_list);
    }
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
int type_next_rule(Token *token, unsigned *ret_val_num) {
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
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", *ret_val_num);
    fprintf(stdout, "MOVE LF@%%retval%u nil@nil\n", *ret_val_num);

    retval = type_next_rule(token, ret_val_num);
    return retval;    
}

/*  RETURN_TYPES -> {
    RETURN_TYPES -> ( TYPE TYPE_NEXT {
    RETURN_TYPES -> TYPE {
*/
int return_types_rule(Token *token ) {
    int retval = 0;
    get_next_token(token);

    if (token->type == CURLY_BR_L) {
        return 0;
    }

    unsigned ret_val_num = 1;
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

    fprintf(stdout, "DEFVAR LF@%%retval%u\n", ret_val_num);
    fprintf(stdout, "MOVE LF@%%retval%u nil@nil\n", ret_val_num);
    ret_val_num++;
        retval = type_next_rule(token, &ret_val_num);
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
int param_next_rule(Token *token, unsigned depth, unsigned *var_num) {
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
    fprintf(stdout, "DEFVAR LF@%s\n", token->data);
    fprintf(stdout, "MOVE LF@%s LF@%%op%u\n", token->data, *var_num);
    (*var_num)++;
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

    retval = param_next_rule(token, depth, var_num);
    if (retval) {
        free_variable_item(var_item);
        return retval;
    }
    retval = sym_table_insert_item(&sym_table, var_item);
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
    unsigned var_num = 1;
    if (token->type != ID) {
        return SYNTAX_ERROR;
    }

    fprintf(stdout, "DEFVAR LF@%s%u\n", token->data, var_num);
    fprintf(stdout, "MOVE LF@%s%u LF@%%op%u\n", token->data, var_num, var_num);
    var_num++;

    Sym_table_item *var_item = malloc(sizeof(Sym_table_item));
    if (!var_item) {
        return INTERNAL_ERROR;
    }
   

    var_item->depth = depth;
    var_item->name = malloc(token->data_size * sizeof(char));
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
   

    retval = sym_table_insert_item(&sym_table, var_item);
    
    if (retval) {
        free_variable_item(var_item);
        return retval;
    }


    return param_next_rule(token, depth, &var_num);
}

/*  EXPR_NEXT -> , EXPR EXPR_NEXT
    EXPR_NEXT -> eps
*/
int expr_next_rule(Token *token, tDLList *list, unsigned depth, unsigned is_func) {
    get_next_token(token);


    if (token->type == COMMA) {
    
        int retval = expression_rule(token, list, depth, is_func);
        if (retval) {
            return retval;
        }
    
        retval = expr_next_rule(token, list, depth, is_func);
        return retval;
    }

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

    if (retval) {
        DLDisposeList(ret_list);
        return retval;
    }

    retval = expr_next_rule(token, ret_list, depth, NONE);
    if (retval) {
        DLDisposeList(ret_list);
        return retval;
    }
    return 0;
}



int check_types_in_assignment(tDLList *left_list, tDLList *right_list, unsigned depth) {
    DLFirst(left_list);
    DLFirst(right_list);
   
    while (left_list->Act && right_list->Act) {
       
        if (left_list->Act->token.data) {
            if (strcmp(left_list->Act->token.data, "_\0") == 0) {
                DLSucc(left_list);
                DLSucc(right_list);
                continue;
            }

            Sym_table_item *item = sym_table_search_item(&sym_table, left_list->Act->token.data, depth);
            if (!item) {
                return SEMANTIC_UNDEFINED_VAR_ERROR;
            }
                   

            if (item->value.var.type != right_list->Act->token.type) {
                return SEMANTIC_OTHER_ERRORS;
            }
        }
        else {
            if (left_list->Act->token.type != right_list->Act->token.type) {
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
    
     int retval;
    if (token->type == ID) {
        

        tDLList left_list, right_list;
        DLInitList(&left_list);
        DLInitList(&right_list);

       
        DLInsertLast(&left_list, token);

       
        retval = id_next_rule(token, &left_list, depth);
        if (retval) {
            DLDisposeList(&left_list);
            DLDisposeList(&right_list);
        }
       

        get_next_token(token);
        if (token->type != EQ_SYM) {
            DLDisposeList(&left_list);
            DLDisposeList(&right_list);
            return SYNTAX_ERROR;
        }
       
        retval = expression_rule(token, &right_list, depth, NONE);
        if (retval) {
            DLDisposeList(&left_list);
            DLDisposeList(&right_list);
            return retval;
        }
       
        retval = expr_next_rule(token, &right_list, depth, NONE);
        if (retval) {
            DLDisposeList(&left_list);
            DLDisposeList(&right_list);
            return retval;
        }
          
        retval = check_types_in_assignment(&left_list, &right_list, depth);

        DLDisposeList(&left_list);
        DLDisposeList(&right_list);
        return retval;
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
        return token->type;
    }
    
    return SYNTAX_ERROR;
}

/*  ID_NEXT -> , id ID_NEXT
    ID_NEXT -> ε
*/
int id_next_rule(Token *token, tDLList *assign_list, unsigned depth) {
    get_next_token(token);

    if (token->type == COMMA) {
        get_next_token(token);

        if (token->type != ID) {
            return SYNTAX_ERROR;
        }

        DLInsertLast(assign_list, token);

        return id_next_rule(token, assign_list, depth);
    }
   
    DLPred(&token_list);
    return 0;
}

int func_call(Token *token, tDLList *list, unsigned depth) {
    char *func_name = token->data;
    get_next_token(token);
   
    int retval = 0;
    if (token->type == ROUND_BR_L) {
        fprintf(stdout, "CREATEFRAME\n");
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
           
            tDLList func_input_list_left, func_input_list_right;
            DLInitList(&func_input_list_left);
            DLInitList(&func_input_list_right);
            retval = expression_rule(token, &func_input_list_right, depth, FUNC);
            if (retval) {
                return retval;
            }
           
            retval = expr_next_rule(token, &func_input_list_right, depth, FUNC);
            if (retval) {
                return retval;
            }
           
            get_next_token(token);
            if (token->type != ROUND_BR_R) {
                 
                return SYNTAX_ERROR;
            }
                  
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
                  
            if (strcmp(func_name, "print\0") != 0) {
                unsigned i;
                for (i = 0; i < item->value.func.num_in_var; i++) {
                    Token token_return = {0, NULL, 0};
                    token_return.type = item->value.func.in_var_list[i];
                    DLInsertLast(&func_input_list_left, &token_return); 
                    fprintf(stdout, "DEFVAR TF@%%op%u\n", i + 1);
                }

                retval = check_types_in_assignment(&func_input_list_left, &func_input_list_right, 0);
                if (retval) {
                    DLDisposeList(&func_input_list_left);
                    DLDisposeList(&func_input_list_right);
                    return retval;
                }

                i = item->value.func.num_in_var;
                while (i) {
                    fprintf(stdout, "POPS TF@%%op%u\n", i);
                    i--;
                }
                
            }
            /*
            if (retval) {
                DLDisposeList(&func_input_list_left);
                DLDisposeList(&func_input_list_right);
                return SEMANTIC_FUNCTION_ERROR;
            }*/

            fprintf(stdout, "CALL $%s\n", func_name);

            DLDisposeList(&func_input_list_left);
            DLDisposeList(&func_input_list_right);
            return FUNC_CALL;
        }
        return SYNTAX_ERROR;
    }
   
    return 0;
}

int was_not_declared(tDLList *list, unsigned depth) {
    DLFirst(list);
   
    while (list->Act != NULL) {
       
        if (sym_table_search_item(&sym_table, list->Act->token.data, depth) == NULL) {
            return 0;
        }
        DLSucc(list);
    }
    return 1;
}

int was_not_declared_on_lvl(tDLList *list, unsigned depth) {
    DLFirst(list);
   
    while (list->Act != NULL) {
        if (init_search_item(&sym_table, list->Act->token.data, depth) == NULL) {
            return 0;
        }
        DLSucc(list);
    }
    return 1;
}


int insert_new_vars(tDLList *left_list, tDLList *right_list, unsigned depth) {
    DLFirst(left_list);
    DLFirst(right_list);

    while (left_list->Act && right_list->Act) {
        Sym_table_item *item = init_search_item(&sym_table, left_list->Act->token.data, depth);
        if (item) {
            DLSucc(left_list);
            DLSucc(right_list);
            continue;
        }

        left_list->Act->token.type = right_list->Act->token.type;
       
        Sym_table_item *var_item = malloc(sizeof(Sym_table_item));
        if (!var_item) {
            return INTERNAL_ERROR;
        }    
        var_item->depth = depth;
        var_item->name = malloc(left_list->Act->token.data_size);
        if (!var_item->name) {
            free_variable_item(var_item);           
            return INTERNAL_ERROR;
        }
        strcpy(var_item->name, left_list->Act->token.data);
        var_item->value.var.type = left_list->Act->token.type;
                   

        sym_table_insert_item(&sym_table, var_item);

        fprintf(stdout, "DEFVAR LF@%s\n", left_list->Act->token.data);

        DLSucc(left_list);
        DLSucc(right_list);
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
        
    retval = func_call(token, &assign_left_list, depth);
   
    if (retval == FUNC_CALL) {
        return 0;
    }
    else if (retval) {
        return retval;
    }
    else {
        DLPred(&token_list);
        DLPred(&token_list);
        get_next_token(token);
        DLInsertLast(&assign_left_list, token);

        retval = id_next_rule(token, &assign_left_list, depth);
        if (retval) {
            return retval;
        }

        switch (assign_sym_rule(token)) {
            case DECLARE:
                if (was_not_declared_on_lvl(&assign_left_list, depth) == 1) {
                    return SEMANTIC_UNDEFINED_VAR_ERROR;
                }
                break;

            case EQ_SYM:
           
                if (was_not_declared(&assign_left_list, depth) == 0 ) {
                    return SEMANTIC_UNDEFINED_VAR_ERROR;
                }
                break;

            default:
                return SYNTAX_ERROR;
        }

        retval = expression_rule(token, &assign_right_list, depth, NONE);
        if (retval) {
            return retval;
        }
       
        retval = expr_next_rule(token, &assign_right_list, depth, NONE);
        if (retval) {
            DLDisposeList(&assign_left_list);
            DLDisposeList(&assign_right_list);
            return retval;
        }
    }
   
    retval = insert_new_vars(&assign_left_list, &assign_right_list, depth);
    
    if (retval) {
        DLDisposeList(&assign_left_list);
        DLDisposeList(&assign_right_list);
    }
    
    retval = check_types_in_assignment(&assign_left_list, &assign_right_list, depth);
    if (retval) {
        DLDisposeList(&assign_left_list);
        DLDisposeList(&assign_right_list);
    }

    DLLast(&assign_left_list);
    while (assign_left_list.Act) {
        fprintf(stdout, "POPS LF@%s\n", assign_left_list.Act->token.data);
        DLPred(&assign_left_list);
    }

    DLDisposeList(&assign_left_list);
    DLDisposeList(&assign_right_list);
    return 0;
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
               

        retval = increase_depth(&depth);
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
        if (token->type != CURLY_BR_L) {
            return SYNTAX_ERROR;
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
       
        retval = vars_rule(token, &ret_list, depth);
        if (retval) {
       
            DLDisposeList(&ret_list);
            return retval;
        }
       
        get_next_token(token);
        if (token->type != END_OF_LINE) {
            DLDisposeList(&ret_list);
            return SYNTAX_ERROR;
        }
       

        Sym_table_item *item = sym_table_search_item(&sym_table, curr_func_name, 0);
        if (!item) {
            DLDisposeList(&ret_list);
            return SEMANTIC_UNDEFINED_VAR_ERROR;
        }

        DLLast(&ret_list);
        if ((item->value.func.num_out_var > 0 && ret_list.Act == NULL) 
                || (item->value.func.num_out_var == 0 && ret_list.Act != NULL)) {
                DLDisposeList(&ret_list);
                return SEMANTIC_FUNCTION_ERROR;
        }        
        
        for (unsigned i = item->value.func.num_out_var - 1; ret_list.Act; i--) {
            if (item->value.func.out_var_list[i] != ret_list.Act->token.type) {
                return SEMANTIC_FUNCTION_ERROR;
            }
            //  fprintf(stdout, "POPS LF@%%retval%u\n", i + 1);
            DLPred(&ret_list);
        }

        (*num_of_returns)++;
       
        return statement_rule(token, depth, curr_func_name, num_of_returns);
        if (retval) {
            return retval;
        }
    }
    DLPred(&token_list);

    do {  
        get_next_token(token);
    
    } while (token_list.Act != NULL && token->type == END_OF_LINE);

    DLPred(&token_list);
    return 0;
}


// FUNC_DEF -> func id_func ( PARAM ) RETURN_TYPES eol STATEMENT } eol
int func_def_rule(Token *token) {
    unsigned depth = 0;
    int num_of_returns = 0;
    int retval = increase_depth(&depth);
    if (retval) {
        return retval;
    }

    if (token->type != FUNC) {
        return SYNTAX_ERROR;
    }

    get_next_token(token);
    if (token->type != ID) {
        return SYNTAX_ERROR;
    }
    token->type = ID_FUNC;
    char *curr_func_name = token->data;
    fprintf(stdout, "\nLABEL $%s\n", curr_func_name);
    if (strcmp(curr_func_name, "main\0") == 0) {
        fprintf(stdout, "CREATEFRAME\n");
    }
    fprintf(stdout, "PUSHFRAME\n");

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

   
    retval = statement_rule(token, depth, curr_func_name, &num_of_returns);
    if (retval) {
        return retval;
    }    
    Sym_table_item *item = sym_table_search_item(&sym_table, curr_func_name, 0);
        if (!item) {
           
            return SEMANTIC_UNDEFINED_VAR_ERROR;
        }
    if (num_of_returns == 0 && item->value.func.num_out_var > 0) {
       
        return SEMANTIC_FUNCTION_ERROR;
    }
   
    decrease_depth(&depth);
    get_next_token(token);
    if (token->type != CURLY_BR_R) {
       
        return SYNTAX_ERROR;
    }

    if (strcmp(curr_func_name, "main\0") != 0) {
        fprintf(stdout, "POPFRAME\n");
        fprintf(stdout, "RETURN\n");
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
   
    if (retval) {
        return retval;
    }

    do {
        get_next_token(&token);
    } while (token_list.Act != NULL && token.type == END_OF_LINE);
   
    print_head();

    retval =  functions_rule(&token);
    if (retval) {
        return retval;
    }

    Sym_table_item *item = sym_table_search_item(&sym_table, "main\0", 0);
    if (!item) {
        return SEMANTIC_UNDEFINED_VAR_ERROR;
    }
    if (item->value.func.num_in_var != 0 || item->value.func.num_out_var != 0 ) {
        return SEMANTIC_FUNCTION_ERROR;
    }
    return 0;
}