// #include "scanner.h"
#include "template.h"
#include "error.h"
#include "symtable.h"
#include "expression.h"

#include <stdbool.h>
#include <string.h>

#define FUNC_CALL 100

tDLList if_stack, else_stack, for_stack;
unsigned func_call_num = 1;
tDLElemPtr token_func_start;
unsigned num_of_fors, num_of_ifs;

int push_stack(unsigned *curr_num, tDLList *stack) {
    Token *curr_token = malloc(sizeof(Token));
    if (!curr_token) {
        DLDisposeList(stack);
        return INTERNAL_ERROR;
    }
    curr_token->type = *curr_num;
   // printf("push %u\n", *curr_num);
    DLInsertLast(stack, curr_token);
    return 0;
}

void pop_stack(unsigned *pop_num, tDLList *stack) {
    *pop_num = stack->Last->token.type;
  //  printf("pop %u\n", *pop_num);
    DLDeleteLast(stack);
}

void print_token (Token *token) {
    fprintf(stderr, "                       type= %-4d data= \"%s\"%-5s size= %-9d\n", token->type, token->data, "", token->data_size);
}

void print_token_list (tDLList *L) {
    tDLElemPtr tmp = L->First;
    fprintf(stderr, "---------------\n");
    while (tmp != NULL) {
        fprintf(stderr,  "type= %-4d data= \"%s\" size= %-9d\n", tmp->token.type, tmp->token.data, tmp->token.data_size);
        tmp = tmp->rptr;
    }
    fprintf(stderr, "---------------\n");
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

    if (init_search_item(&asm_table, "_\0", *depth) == NULL) {
        Sym_table_item *asm_item = malloc(sizeof(Sym_table_item));
        if (!asm_item) {
            return INTERNAL_ERROR;
        }
        asm_item->depth = *depth;
        asm_item->name = malloc(sizeof("_\0"));
        if (!asm_item->name) {
            free_variable_item(asm_item);
            return INTERNAL_ERROR;
        }
        strcpy(asm_item->name, "_\0");
        sym_table_insert_item(&asm_table, asm_item);
        if (func_call_num == 1) {
            fprintf(stdout, "DEFVAR LF@%s%u\n", asm_item->name, asm_item->depth);
        }
    }

    return 0;
}

void decrease_depth(unsigned *depth) {
   
    //fprintf(stderr, "clean depth = %u\n", *depth);
       // //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    print_sym_table_items(&sym_table);
    sym_table_delete_on_depth(&sym_table, *depth);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    (*depth)--;
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
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
    return expression(list, is_func, depth, func_call_num);
}

/*  TYPE -> int
    TYPE -> float64
    TYPE -> string
*/
int type_rule(Token *token) {
    get_next_token(token);

    if (token->type == INT) {
        token->type = INTEGER;
    }
    else if (token->type == FLOAT64) {
        token->type = FLOAT;
    }
    else if (token->type == STRING) {
        token->type = STR_END;
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
    if (func_call_num == 1) {
        fprintf(stdout, "DEFVAR LF@%%retval%u\n", *ret_val_num);
    }
    else {
        fprintf(stdout, "MOVE LF@%%retval%u nil@nil\n", *ret_val_num);
    }
    (*ret_val_num)++;
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
    if (func_call_num == 1) {
        fprintf(stdout, "DEFVAR LF@%%retval%u\n", ret_val_num);
    }
    else {
        fprintf(stdout, "MOVE LF@%%retval%u nil@nil\n", ret_val_num);
    }
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

    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    if (func_call_num == 1) {
        fprintf(stdout, "DEFVAR LF@%s%u\n", token->data, depth);
    }
    else {
        fprintf(stdout, "MOVE LF@%s%u LF@%%op%u\n", token->data, depth, *var_num);
    }
    (*var_num)++;
    Sym_table_item *var_item = malloc(sizeof(Sym_table_item));
    if (!var_item) {
        return INTERNAL_ERROR;
    }

    Sym_table_item *asm_item = malloc(sizeof(Sym_table_item));
    if (!asm_item) {
        return INTERNAL_ERROR;
    }

    asm_item->depth = depth;
    var_item->depth = depth;

    asm_item->name = malloc(token->data_size);
    if (!asm_item->name) {
        free_variable_item(asm_item);
        return INTERNAL_ERROR;
    }

    strcpy(asm_item->name, token->data);

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
    retval = sym_table_insert_item(&asm_table, asm_item);
    if (retval) {
        free_variable_item(asm_item);
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

//fprintf(stderr,_ "%s %d\n", __FILE__, __LINE__);
    if (func_call_num  == 1) {
        fprintf(stdout, "DEFVAR LF@%s%u\n", token->data, depth);
    }
    else {
        fprintf(stdout, "MOVE LF@%s%u LF@%%op%u\n", token->data, depth, var_num);
    }
    var_num++;

    Sym_table_item *var_item = malloc(sizeof(Sym_table_item));
    if (!var_item) {
        return INTERNAL_ERROR;
    }

    Sym_table_item *asm_item = malloc(sizeof(Sym_table_item));
    if (!asm_item) {
        return INTERNAL_ERROR;
    }

    var_item->depth = depth;
    asm_item->depth = depth;

    var_item->name = malloc(token->data_size * sizeof(char));
    if (!var_item->name) {
        free_variable_item(var_item);
        return INTERNAL_ERROR;
    }
    strcpy(var_item->name, token->data);

    asm_item->name = malloc(token->data_size * sizeof(char));
    if (!asm_item->name) {
        free_variable_item(asm_item);
        return INTERNAL_ERROR;
    }
    strcpy(asm_item->name, token->data);

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

    retval = sym_table_insert_item(&asm_table, asm_item);
    if (retval) {
        free_variable_item(asm_item);
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
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
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
    unsigned if_num = 0;
    pop_stack(&if_num, &if_stack);
    if (token->type == END_OF_LINE) {
        if (func_call_num > 1) {
            fprintf(stdout, "LABEL ?if_%s_%u\n", curr_func_name, if_num);
        }
        return 0;
    }

    if (token->type != ELSE) {
        return SYNTAX_ERROR;
    }
    //increase_depth(&depth);
    int retval = push_stack(&if_num, &else_stack);
    if (retval) {
        return retval;
    }
    if (func_call_num > 1) {
        fprintf(stdout, "JUMP ?else_%s_%u\n", curr_func_name, if_num);
        fprintf(stdout, "LABEL ?if_%s_%u\n", curr_func_name, if_num);
    }
   
    get_next_token(token);
    if (token->type != CURLY_BR_L) {
        return SYNTAX_ERROR;
    }

    get_next_token(token);
    if (token->type != END_OF_LINE) {
        return SYNTAX_ERROR;
    }
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    retval = statement_rule(token, depth, curr_func_name, num_of_returns);
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    if (retval) {
        return retval;
    }

    get_next_token(token);
    if (token->type != CURLY_BR_R) {
        return SYNTAX_ERROR;
    }

   // unsigned else_num = 0;
    pop_stack(&if_num, &else_stack);
    if (func_call_num > 1) {
        fprintf(stdout, "LABEL ?else_%s_%u\n", curr_func_name, if_num);
    }

    get_next_token(token);
    if (token->type != END_OF_LINE) {
        return SYNTAX_ERROR;
    }
    //decrease_depth(&depth);
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

        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        retval = check_types_in_assignment(&left_list, &right_list, depth);
        DLLast(&left_list);
        while (left_list.Act) {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            Sym_table_item *item = sym_table_search_item(&asm_table, left_list.Act->token.data, depth);
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            if (func_call_num > 1) {
                fprintf(stdout, "POPS LF@%s%u\n", item->name, item->depth);
            }
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            DLPred(&left_list);
        }
        


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
   //     //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
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
        if (func_call_num > 1) {
            //fprintf(stdout, "CREATEFRAME\n");
        }
        get_next_token(token);
        if (token->type == ROUND_BR_R) {
            
            Sym_table_item *item =  sym_table_search_item(&sym_table, func_name, depth);
            if (item) {
                return SEMANTIC_OTHER_ERRORS;
            }
            item =  sym_table_search_item(&sym_table, func_name, 0);
            if (!item) {
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                return SEMANTIC_UNDEFINED_VAR_ERROR;
            }
            for (unsigned i = 0; i < item->value.func.num_out_var; i++) {
                Token token_return = {0, NULL, 0};
                token_return.type = item->value.func.out_var_list[i];
                DLInsertLast(list, &token_return); 
            }
            if (strcmp(func_name, "print\0") == 0) {
                return FUNC_CALL;
            }
            if (func_call_num > 1) {
                fprintf(stdout, "CREATEFRAME\n");
            }
            if (func_call_num > 1) {
                fprintf(stdout, "CALL $%s\n", func_name);
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
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                return SEMANTIC_UNDEFINED_VAR_ERROR;
            }

            for (unsigned i = 0; i < item->value.func.num_out_var; i++) {
                Token token_return = {0, NULL, 0};
                token_return.type = item->value.func.out_var_list[i];
                DLInsertLast(list, &token_return); 
            }

            if (strcmp(func_name, "print\0") != 0) {
                if (func_call_num > 1) {
                    fprintf(stdout, "CREATEFRAME\n");
                }
                unsigned i;
                for (i = 0; i < item->value.func.num_in_var; i++) {
                    Token token_return = {0, NULL, 0};
                    token_return.type = item->value.func.in_var_list[i];
                    DLInsertLast(&func_input_list_left, &token_return);
                    if (func_call_num > 1) {
                        fprintf(stdout, "DEFVAR TF@%%op%u\n", i + 1);
                    }
                }

                retval = check_types_in_assignment(&func_input_list_left, &func_input_list_right, 0);
                if (retval) {
                    DLDisposeList(&func_input_list_left);
                    DLDisposeList(&func_input_list_right);
                    return SEMANTIC_FUNCTION_ERROR;
                }

                i = item->value.func.num_in_var;
                while (i) {
                    if (func_call_num > 1) {
                        fprintf(stdout, "POPS TF@%%op%u\n", i);
                    }
                    i--;
                }
                if (func_call_num > 1) {
                    fprintf(stdout, "CALL $%s\n", func_name);
                }
            }
            else {
               // //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

                DLLast(&func_input_list_right);
               // //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                static unsigned print_num;
                unsigned i = 1;
                if (func_call_num > 1) {
                    fprintf(stdout, "CREATEFRAME\n");
                }
                while (func_input_list_right.Act) {
               ///     //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                    if (func_call_num > 1) {
                        fprintf(stdout, "DEFVAR TF@%%op%u\n", i);
                    }
                    //fprintf(stdout, "POPS TF@%%op%u\n", i);
                    i++;
                    DLPred(&func_input_list_right);
                }
                //for (unsigned n = i - 1; n > 0; n--) {
                print_num++;
                template_print(i, func_call_num);
               // fprintf(stdout, "LABEL !!!end")
                //}
                
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            }

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
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    print_sym_table_items(&sym_table);
    while (list->Act != NULL) {
        if (sym_table_search_item(&sym_table, list->Act->token.data, depth) == NULL) {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            return 0;
        }
        DLSucc(list);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    }
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    return 1;
}

int was_not_declared_on_lvl(tDLList *list, unsigned depth) {
    DLFirst(list);
 //   fprintf(stderr, "%s, %d\n", __FILE__, __LINE__);
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
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    while (left_list->Act && right_list->Act) {
        Sym_table_item *item = init_search_item(&sym_table, left_list->Act->token.data, depth);
        if (item) {
            DLSucc(left_list);
            DLSucc(right_list);
            continue;
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

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
                   

        int retval = sym_table_insert_item(&sym_table, var_item);
        if (retval) {
            free_variable_item(var_item);
            return retval;
        }

        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        if (init_search_item(&asm_table, left_list->Act->token.data, depth) == NULL) {
            if (func_call_num == 1) {
                fprintf(stdout, "DEFVAR LF@%s%u\n", left_list->Act->token.data, depth);
            }
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            Sym_table_item *asm_item = malloc(sizeof(Sym_table_item));
            if (!asm_item) {
                return INTERNAL_ERROR;
            }
            asm_item->name = malloc(left_list->Act->token.data_size);
            if (!asm_item->name) {
                free(asm_item);
                return INTERNAL_ERROR;
            }
            strcpy(asm_item->name, left_list->Act->token.data);
            asm_item->depth = depth;
            sym_table_insert_item(&asm_table, asm_item);
        }
        print_sym_table_items(&sym_table);
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
    unsigned equal;

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

        equal = assign_sym_rule(token);
        switch (equal) {
            case DECLARE:
                if (was_not_declared_on_lvl(&assign_left_list, depth) == 1) {
                    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                    return SEMANTIC_UNDEFINED_VAR_ERROR;
                }
                break;
            case EQ_SYM:
                if (was_not_declared(&assign_left_list, depth) == 0 ) {
                    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
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
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    if (equal == DECLARE) {
        retval = insert_new_vars(&assign_left_list, &assign_right_list, depth);
        if (retval) {
            DLDisposeList(&assign_left_list);
            DLDisposeList(&assign_right_list);
        }
    }

    retval = check_types_in_assignment(&assign_left_list, &assign_right_list, depth);
    if (retval) {
        DLDisposeList(&assign_left_list);
        DLDisposeList(&assign_right_list);
        return retval;
    }

    DLLast(&assign_left_list);
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    while (assign_left_list.Act) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
       // print_sym_table_items(&asm_table);
        Sym_table_item *asm_item = sym_table_search_item(&asm_table, assign_left_list.Act->token.data, depth);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        if (func_call_num > 1)  {
            fprintf(stdout, "POPS LF@%s%u\n", asm_item->name, asm_item->depth);
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
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
        //static unsigned num_of_ifs;
        num_of_ifs++;
        retval = push_stack(&num_of_ifs, &if_stack);
        if (retval) {
            return retval;
        }
        retval = increase_depth(&depth);
        if (retval) {
            return retval;
        }
        if (func_call_num == 1) {
            fprintf(stdout, "DEFVAR LF@?if_var_%s_%u\n", curr_func_name, num_of_ifs);
        }
        
        tDLList unused_list;
        DLInitList(&unused_list);
        retval = expression_rule(token, &unused_list, depth, IF);
        if (retval) {
            return retval;
        }

        if (func_call_num > 1) {
        fprintf(stdout, "POPS LF@?if_var_%s_%u\n", curr_func_name, num_of_ifs);
        fprintf(stdout, "JUMPIFNEQ ?if_%s_%u LF@?if_var_%s_%u bool@true\n", 
                curr_func_name, num_of_ifs, curr_func_name, num_of_ifs);
        }

        DLDisposeList(&unused_list);
        get_next_token(token);
        if (token->type != CURLY_BR_L) {
            return SYNTAX_ERROR;
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        retval = statement_rule(token, depth, curr_func_name, num_of_returns);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        if (retval) {
            return retval;
        }

        get_next_token(token);
        if (token->type != CURLY_BR_R) {
            return SYNTAX_ERROR;
        }

        decrease_depth(&depth);

        int retval = increase_depth(&depth);
        if (retval) {
            return retval;
        }

        retval = else_rule(token, depth, curr_func_name, num_of_returns);
        if (retval) {
            return retval;
        }
        
        decrease_depth(&depth);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        retval = statement_rule(token, depth, curr_func_name, num_of_returns);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        if (retval) {
            return retval;
        }
    }
    else if (token->type == FOR) {
        //static unsignednum_of_fors;
        num_of_fors++;
        retval = push_stack(&num_of_fors, &for_stack);
        if (retval) {
            return retval;
        }
        if (func_call_num == 1) {
            fprintf(stdout, "DEFVAR LF@?for_var_%u\n", num_of_fors);
        }

        retval = increase_depth(&depth);
        if (retval) {
            return retval;
        }

        retval = for_first_rule(token, depth);
        if (retval) {
            return retval;
        }

        if (func_call_num > 1) {
            fprintf(stdout, "LABEL ?for_start_%u\n", num_of_fors);
        }

        tDLList unused_list;
        DLInitList(&unused_list);
        retval = expression_rule(token, &unused_list, depth, FOR);
        if (retval) {
            return retval;
        }

        DLDisposeList(&unused_list);

        get_next_token(token);
        if (token->type != SEMICOLON) {
            return SYNTAX_ERROR;
        }

        if (func_call_num > 1) {
            fprintf(stdout, "POPS LF@?for_var_%u\n", num_of_fors);
            fprintf(stdout, "JUMPIFNEQ ?for_end_%u LF@?for_var_%u bool@true\n", num_of_fors, num_of_fors);
            fprintf(stdout, "JUMP ?for_block_start_%u\n", num_of_fors);
            fprintf(stdout, "LABEL ?for_block_end_%u\n", num_of_fors);
        }

        get_next_token(token);
        retval = for_last_rule(token, depth);
        if (retval) {
            return retval;
        }
        if (func_call_num > 1) {
            fprintf(stdout, "JUMP ?for_start_%u\n", num_of_fors);
            fprintf(stdout, "LABEL ?for_block_start_%u\n", num_of_fors);
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
        // fprintf(stdout, "BREAK\n");
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        retval = statement_rule(token, depth, curr_func_name, num_of_returns);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        if (retval) {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            return retval;
        }

        unsigned tmp_for = 0;
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        pop_stack(&tmp_for, &for_stack);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        if (func_call_num > 1) {
            fprintf(stdout, "JUMP ?for_block_end_%u\n", tmp_for);
            fprintf(stdout, "LABEL ?for_end_%u\n", tmp_for);
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        get_next_token(token);
       // //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        if (token->type != CURLY_BR_R) {
            return SYNTAX_ERROR;
        }
       // //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        get_next_token(token);
       // //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        if (token->type != END_OF_LINE) {
         //   //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            return SYNTAX_ERROR;
        }
       // //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        decrease_depth(&depth);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        decrease_depth(&depth);
       // //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        retval = statement_rule(token, depth, curr_func_name, num_of_returns);
       // //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
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
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        retval = statement_rule(token, depth, curr_func_name, num_of_returns);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
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
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
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
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return statement_rule(token, depth, curr_func_name, num_of_returns);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        if (retval) {
            return retval;
        }
    }
    DLPred(&token_list);

    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    if (token_list.Act == token_list.Last) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return 0;
    }

    do {  
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        get_next_token(token);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    
    } while (token_list.Act != NULL && token->type == END_OF_LINE);
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    DLPred(&token_list);
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    return 0;
}


// FUNC_DEF -> func id_func ( PARAM ) RETURN_TYPES eol STATEMENT } eol
int func_def_rule(Token *token) {
    fprintf(stderr, "func call num = %u\n", func_call_num);
    unsigned depth = 0;
    int num_of_returns = 0;
    
    DLInitList(&if_stack);
    DLInitList(&else_stack);
    DLInitList(&for_stack);
    num_of_fors = 0;
    num_of_ifs = 0;
    
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    if (func_call_num == 1) {
        sym_table_init(&asm_table);
        DLPred(&token_list);
        token_func_start = token_list.Act;
        DLSucc(&token_list);
    }
    else {
        fprintf(stderr, "else token data %s\n", token_func_start->token.data);
        token_list.Act = token_func_start;
        fprintf(stderr, "else token data %s\n", token_list.Act->token.data);
        fprintf(stderr, "'%s' next '%s' next '%s'\n", token_list.Act->token.data, token_list.Act->rptr->token.data, token_list.Act->rptr->rptr->token.data);
        get_next_token(token);
        fprintf(stderr, "after_get_next: '%s' next '%s' next '%s'\n", token_list.Act->token.data, token_list.Act->rptr->token.data, token_list.Act->rptr->rptr->token.data);

    }
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    if (token->type != FUNC) {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        return SYNTAX_ERROR;
    }
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    fprintf(stderr, "%s\n", token->data);
    get_next_token(token);
    if (token->type != ID) {
        fprintf(stderr, "%s\n", token->data);
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        return SYNTAX_ERROR;
    }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

   // fprintf(stderr, "token data %s\n", token_func_start->token.data);
    token->type = ID_FUNC;
    char *curr_func_name = token->data;
    if (func_call_num == 1) {
        fprintf(stdout, "\nLABEL $%s\n", curr_func_name);
    }
    if (strcmp(curr_func_name, "main\0") == 0) {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        if (func_call_num == 1)  {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            fprintf(stdout, "CREATEFRAME\n");
        }
    }
    if (func_call_num == 1) {
        fprintf(stdout, "PUSHFRAME\n");
    }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    int retval = increase_depth(&depth);
    if (retval) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return retval;
    }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    get_next_token(token);
    if (token->type != ROUND_BR_L) {
fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return SYNTAX_ERROR;
    }
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    if (param_rule(token, depth)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return SYNTAX_ERROR;
    }
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    if (return_types_rule(token)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return SYNTAX_ERROR;
    }
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    get_next_token(token);
    if (token->type != END_OF_LINE) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return SYNTAX_ERROR;
    }
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    retval = statement_rule(token, depth, curr_func_name, &num_of_returns);
   // //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    if (retval) {
        return retval;
    }    
    Sym_table_item *item = sym_table_search_item(&sym_table, curr_func_name, 0);
        if (!item) {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
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

    if (strcmp(curr_func_name, "main\0") != 0) {
        if (func_call_num > 1) {
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

            fprintf(stdout, "POPFRAME\n");
            fprintf(stdout, "RETURN\n");
        }
    }
    else
    {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        if (func_call_num > 1) {
            fprintf(stdout, "JUMP $$$end_of_program\n");
        }
    }
    
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    get_next_token(token);
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    if (token->type == END_OF_LINE || token_list.Act == token_list.Last) {
      //  //fprin       tf(stderr, "%s %d\n", __FILE__, __LINE__);
        DLDisposeList(&if_stack);
        DLDisposeList(&else_stack);
        DLDisposeList(&for_stack);
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            //fprintf(stderr, "func_call_num(fdr)= %u\n", func_call_num);
        if (func_call_num > 1) {
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

            sym_table_clear_all(&asm_table);
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            return 0;
        }
        else {
            func_call_num++;
        //    fprintf(stderr, "name = %s\n", token_func_start->token.data);
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

            return func_def_rule(token);
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            //return 0;
        }
        
        return 0;
    }
    DLDisposeList(&if_stack);
    DLDisposeList(&else_stack);
    DLDisposeList(&for_stack);

    return SYNTAX_ERROR;
}

// FUNC_DEF_NEXT -> ε
// FUNC_DEF_NEXT -> FUNC_DEF FUNC_DEF_NEXT
int func_def_next_rule(Token *token) {
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    if (token_list.Act == token_list.Last || token_list.Act == NULL) {
        return 0;
    }
    func_call_num = 1;
    int retval = func_def_rule(token);
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    if (retval) {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        return retval;
    }
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    do {
        get_next_token(token);
    } while (token_list.Act != NULL && token->type == END_OF_LINE);
    func_call_num = 1;
    retval = func_def_next_rule(token);
    if (retval) {
        return retval;
    }

    return 0;
}

// FUNCTIONS -> FUNC_DEF FUNC_DEF_NEXT
int functions_rule(Token *token) {
    int retval = func_def_rule(token);
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    if (retval) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return retval;
    }

    do {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        get_next_token(token);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    } while (token_list.Act != NULL && token->type == END_OF_LINE);
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    
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
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return SEMANTIC_UNDEFINED_VAR_ERROR;
    }
    if (item->value.func.num_in_var != 0 || item->value.func.num_out_var != 0 ) {
        return SEMANTIC_FUNCTION_ERROR;
    }
    fprintf(stdout, "LABEL $$$end_of_program\n");
    return 0;
}