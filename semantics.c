#include "symtable.h"
#include "scanner.h"
#include "error.h"

#include <string.h>
#include <stdlib.h>

#define IN_VAR 0
#define OUT_VAR 1

void clear_item(Sym_table_item *);

/**
 * Inserting built-in functions
 * 
 * @return 0 if the built-in function items were created successfully
 * @return 99 If the memory wasn't allocated successfully
 */
int insert_embedded_functions() {
    Sym_table_item *item = malloc(sizeof(Sym_table_item));
    
    if (!(item)) {
        clear_item(item);
        return INTERNAL_ERROR;
    }
    
    item->name = malloc(sizeof("inputs\0"));

    if (!(item->name)) {
        clear_item(item);
        return INTERNAL_ERROR;
    }

    strcpy(item->name, "inputs\0");

    item->depth = 0;
    item->nextPtr = NULL;
    
    item->value.func.num_in_var = 0;
    item->value.func.in_var_list = NULL;

    item->value.func.num_out_var = 2;
    item->value.func.out_var_list = malloc(sizeof(unsigned) * item->value.func.num_out_var);

    if (!(item->value.func.out_var_list)) {
        clear_item(item);
        return INTERNAL_ERROR;
    }

    item->value.func.out_var_list[0] = STR_END;
    item->value.func.out_var_list[1] = INTEGER;

    sym_table_insert_item(&sym_table, item);

    Sym_table_item *item2 = malloc(sizeof(Sym_table_item));

    if (!(item2)) {
        clear_item(item2);
        return INTERNAL_ERROR;
    }
    
    item2->name = malloc(sizeof("inputi\0"));

    if (!(item2->name)) {
        clear_item(item2);
        return INTERNAL_ERROR;
    }

    strcpy(item2->name, "inputi\0");

    item2->depth = 0;
    item2->nextPtr = NULL;
    
    item2->value.func.num_in_var = 0;
    item2->value.func.in_var_list = NULL;

    item2->value.func.num_out_var = 2;
    item2->value.func.out_var_list = malloc(sizeof(unsigned) * item2->value.func.num_out_var);

    if (!(item2->value.func.out_var_list)) {
        clear_item(item2);
        return INTERNAL_ERROR;
    }

    item2->value.func.out_var_list[0] = INTEGER;
    item2->value.func.out_var_list[1] = INTEGER;

    sym_table_insert_item(&sym_table, item2);

    Sym_table_item *item3 = malloc(sizeof(Sym_table_item));

    if (!(item3)) {
        clear_item(item3);
        return INTERNAL_ERROR;
    }
    
    item3->name = malloc(sizeof("inputf\0"));

    if (!(item3->name)) {
        clear_item(item3);
        return INTERNAL_ERROR;
    }

    strcpy(item3->name, "inputf\0");

    item3->depth = 0;
    item3->nextPtr = NULL;
    item3->value.func.num_in_var = 0;
    item3->value.func.in_var_list = NULL;

    item3->value.func.num_out_var = 2;
    item3->value.func.out_var_list = malloc(sizeof(unsigned) * item3->value.func.num_out_var);

    if (!(item3->value.func.out_var_list)) {
        clear_item(item3);
        return INTERNAL_ERROR;
    }

    item3->value.func.out_var_list[0] = FLOAT;
    item3->value.func.out_var_list[1] = INTEGER;
   
    sym_table_insert_item(&sym_table, item3);

    Sym_table_item *item4 = malloc(sizeof(Sym_table_item));

    if (!(item4)) {
        clear_item(item4);
        return INTERNAL_ERROR;
    }
    
    item4->name = malloc(sizeof("print\0"));

    if (!(item4->name)) {
        clear_item(item4);
        return INTERNAL_ERROR;
    }

    strcpy(item4->name, "print\0");

    item4->depth = 0;
    item4->nextPtr = NULL;
    item4->value.func.num_in_var = 0;
    item4->value.func.in_var_list = NULL;

    item4->value.func.num_out_var = 0;
    item4->value.func.out_var_list = NULL; 
    
    sym_table_insert_item(&sym_table, item4);

    Sym_table_item *item5 = malloc(sizeof(Sym_table_item));

    if (!(item5)) {
        clear_item(item5);
        return INTERNAL_ERROR;
    }
    
    item5->name = malloc(sizeof("int2float\0"));

    if (!(item5->name)) {
        clear_item(item5);
        return INTERNAL_ERROR;
    }
    strcpy(item5->name, "int2float\0");

    item5->depth = 0;
    item5->nextPtr = NULL;
    item5->value.func.num_in_var = 1;
    item5->value.func.in_var_list = malloc(sizeof(unsigned) * item5->value.func.num_in_var);

    if (!(item5->value.func.in_var_list)) {
        clear_item(item5);
        return INTERNAL_ERROR;
    }

    item5->value.func.num_out_var = 1;
    item5->value.func.out_var_list = malloc(sizeof(unsigned) * item5->value.func.num_out_var);

    if (!(item5->value.func.out_var_list)) {
        clear_item(item5);
        return INTERNAL_ERROR;
    }

    item5->value.func.in_var_list[0] = INTEGER;

    item5->value.func.out_var_list[0] = FLOAT;

    sym_table_insert_item(&sym_table, item5);

    Sym_table_item *item6 = malloc(sizeof(Sym_table_item));

    if (!(item6)) {
        clear_item(item6);
        return INTERNAL_ERROR;
    }
    
    item6->name = malloc(sizeof("float2int\0"));

    if (!(item6->name)) {
        clear_item(item6);
        return INTERNAL_ERROR;
    }

    strcpy(item6->name, "float2int\0");

    item6->depth = 0;
    item6->nextPtr = NULL;
    item6->value.func.num_in_var = 1;
    item6->value.func.in_var_list = malloc(sizeof(unsigned) * item6->value.func.num_in_var);

    if (!(item6->value.func.in_var_list)) {
        clear_item(item6);
        return INTERNAL_ERROR;
    }

    item6->value.func.num_out_var = 1;
    item6->value.func.out_var_list = malloc(sizeof(unsigned) * item6->value.func.num_out_var);

    if (!(item6->value.func.out_var_list)) {
        clear_item(item6);
        return INTERNAL_ERROR;
    }

    item6->value.func.in_var_list[0] = FLOAT;

    item6->value.func.out_var_list[0] = INTEGER;

    sym_table_insert_item(&sym_table, item6);

    Sym_table_item *item7 = malloc(sizeof(Sym_table_item));

    if (!(item7)) {
        clear_item(item7);
        return INTERNAL_ERROR;
    }
    
    item7->name = malloc(sizeof("len\0"));

    if (!(item7->name)) {
        clear_item(item7);
        return INTERNAL_ERROR;
    }

    strcpy(item7->name, "len\0");

    item7->depth = 0;
    item7->nextPtr = NULL;
    item7->value.func.num_in_var = 1;
    item7->value.func.in_var_list = malloc(sizeof(unsigned) * item7->value.func.num_in_var);

    if (!(item7->value.func.in_var_list)) {
        clear_item(item7);
        return INTERNAL_ERROR;
    }

    item7->value.func.num_out_var = 1;
    item7->value.func.out_var_list = malloc(sizeof(unsigned) * item7->value.func.num_out_var);

    if (!(item7->value.func.out_var_list)) {
        clear_item(item7);
        return INTERNAL_ERROR;
    }

    item7->value.func.in_var_list[0] = STR_END;

    item7->value.func.out_var_list[0] = INTEGER;

    sym_table_insert_item(&sym_table, item7);

    Sym_table_item *item8 = malloc(sizeof(Sym_table_item));

    if (!(item8)) {
        clear_item(item8);
        return INTERNAL_ERROR;
    }
    
    item8->name = malloc(sizeof("substr\0"));

    if (!(item8->name)) {
        clear_item(item8);
        return INTERNAL_ERROR;
    }

    strcpy(item8->name, "substr\0");

    item8->depth = 0;
    item8->nextPtr = NULL;
    item8->value.func.num_in_var = 3;
    item8->value.func.in_var_list = malloc(sizeof(unsigned) * item8->value.func.num_in_var);

    if (!(item8->value.func.in_var_list)) {
        clear_item(item8);
        return INTERNAL_ERROR;
    }

    item8->value.func.num_out_var = 2;
    item8->value.func.out_var_list = malloc(sizeof(unsigned) * item8->value.func.num_out_var);

    if (!(item8->value.func.out_var_list)) {
        clear_item(item8);
        return INTERNAL_ERROR;
    }

    item8->value.func.in_var_list[0] = STR_END;
    item8->value.func.in_var_list[1] = INTEGER;
    item8->value.func.in_var_list[2] = INTEGER;

    item8->value.func.out_var_list[0] = STR_END;
    item8->value.func.out_var_list[1] = INTEGER;

    sym_table_insert_item(&sym_table, item8);

    Sym_table_item *item9 = malloc(sizeof(Sym_table_item));

    if (!(item9)) {
        clear_item(item9);
        return INTERNAL_ERROR;
    }
    
    item9->name = malloc(sizeof("ord\0"));

    if (!(item9->name)) {
        clear_item(item9);
        return INTERNAL_ERROR;
    }

    strcpy(item9->name, "ord\0");

    item9->depth = 0;
    item9->nextPtr = NULL;
    item9->value.func.num_in_var = 2;
    item9->value.func.in_var_list = malloc(sizeof(unsigned) * item9->value.func.num_in_var);

    if (!(item9->value.func.in_var_list)) {
        clear_item(item9);
        return INTERNAL_ERROR;
    }

    item9->value.func.num_out_var = 2;
    item9->value.func.out_var_list = malloc(sizeof(unsigned) * item9->value.func.num_out_var);

    if (!(item9->value.func.out_var_list)) {
        clear_item(item9);
        return INTERNAL_ERROR;
    }

    item9->value.func.in_var_list[0] = STR_END;
    item9->value.func.in_var_list[1] = INTEGER;

    item9->value.func.out_var_list[0] = INTEGER;
    item9->value.func.out_var_list[1] = INTEGER;

    sym_table_insert_item(&sym_table, item9);

    Sym_table_item *item10 = malloc(sizeof(Sym_table_item));

    if (!(item10)) {
        clear_item(item10);
        return INTERNAL_ERROR;
    }
    
    item10->name = malloc(sizeof("chr\0"));

    if (!(item10->name)) {
        clear_item(item10);
        return INTERNAL_ERROR;
    }
    strcpy(item10->name, "chr\0");

    item10->depth = 0;
    item10->nextPtr = NULL;
    item10->value.func.num_in_var = 1;
    item10->value.func.in_var_list = malloc(sizeof(unsigned) * item10->value.func.num_in_var);

    if (!(item10->value.func.in_var_list)) {
        clear_item(item10);
        return INTERNAL_ERROR;
    }

    item10->value.func.num_out_var = 2;
    item10->value.func.out_var_list = malloc(sizeof(unsigned) * item10->value.func.num_out_var);

    if (!(item10->value.func.out_var_list)) {
        clear_item(item10);
        return INTERNAL_ERROR;
    }

    item10->value.func.in_var_list[0] = INTEGER;

    item10->value.func.out_var_list[0] = STR_END;
    item10->value.func.out_var_list[1] = INTEGER;

    sym_table_insert_item(&sym_table, item10);

    Sym_table_item *item11 = malloc(sizeof(Sym_table_item));
    
    if (!(item11)) {
        clear_item(item11);
        return INTERNAL_ERROR;
    }
    
    item11->name = malloc(sizeof("inputb\0"));

    if (!(item11->name)) {
        clear_item(item11);
        return INTERNAL_ERROR;
    }

    strcpy(item11->name, "inputb\0");

    item11->depth = 0;
    item11->nextPtr = NULL;
    
    item11->value.func.num_in_var = 0;
    item11->value.func.in_var_list = NULL;

    item11->value.func.num_out_var = 2;
    item11->value.func.out_var_list = malloc(sizeof(unsigned) * item11->value.func.num_out_var);

    if (!(item11->value.func.out_var_list)) {
        clear_item(item11);
        return INTERNAL_ERROR;
    }

    item11->value.func.out_var_list[0] = BOOLEAN;
    item11->value.func.out_var_list[1] = INTEGER;

    sym_table_insert_item(&sym_table, item11);

    return 0;
}

void sem_get_next_token(Token *token) {
    if (token_list.Act != NULL) {
        *token = token_list.Act->token;
        DLSucc(&token_list);
    }
}

/**
 * @brief Free allocated memory for item
 * 
 * @param item 
 */
void  clear_item(Sym_table_item *item) {
    if (item == NULL) {
        return;
    }

    if (item->name) {
        free(item->name);
    }

    if (item->value.func.in_var_list) {
        free(item->value.func.in_var_list);
    }

    if (item->value.func.out_var_list) {
        free(item->value.func.out_var_list);
    }

    free(item);
}

/*  TYPE -> int
    TYPE -> float64
    TYPE -> string

    Checking types and inserting them to list
*/
int sem_type_rule(Token *token, Sym_table_item *item_ptr, int in_out_var) {
    sem_get_next_token(token);
    if ((token->type == INT) || (token->type == FLOAT64) || (token->type == STRING) || (token->type = BOOL)) {
        if (token->type == INT) {
            token->type = INTEGER;
        }
        else if (token->type == FLOAT64) {
            token->type = FLOAT;
        }
        else if (token->type == STRING) {
            token->type = STR_END;
        }
        else if (token->type == BOOL) {
            token->type = BOOLEAN;
        }

        if (in_out_var == IN_VAR) {
            if (item_ptr->value.func.num_in_var > 0) {
                item_ptr->value.func.in_var_list = realloc(item_ptr->value.func.in_var_list, (item_ptr->value.func.num_in_var + 1) * sizeof(unsigned)); 
                if(!item_ptr->value.func.in_var_list) {
                    return INTERNAL_ERROR;
                }
            }
            item_ptr->value.func.in_var_list[item_ptr->value.func.num_in_var] = token->type;
            item_ptr->value.func.num_in_var++;
        }
        else {
            if (item_ptr->value.func.num_out_var > 0) {
                item_ptr->value.func.out_var_list = realloc(item_ptr->value.func.out_var_list, (item_ptr->value.func.num_out_var + 1) * sizeof(unsigned));
                if(!item_ptr->value.func.out_var_list) {
                    return INTERNAL_ERROR;
                }
            }
            item_ptr->value.func.out_var_list[item_ptr->value.func.num_out_var] = token->type;
            item_ptr->value.func.num_out_var++;
        }
    }
    else {
        return SYNTAX_ERROR;
    }
    return 0;
}

/*  PARAM_NEXT -> )
    PARAM_NEXT -> , id TYPE PARAM_NEXT

    Filling in function with input parameters(continuation)
*/
int sem_param_next_rule(Token *token, Sym_table_item *item_ptr, int in_out_var) {
    sem_get_next_token(token);
    if (token->type == ROUND_BR_R) {
        return 0;
    }

    if (token->type != COMMA) {
        clear_item(item_ptr);
        return SYNTAX_ERROR;
    }

    do {
        sem_get_next_token(token);
    } while (token_list.Act != NULL && token->type == END_OF_LINE);

    //sem_get_next_token(token);
    if (token->type != ID) {
        clear_item(item_ptr);
        return SYNTAX_ERROR;
    }
    
    int retval = sem_type_rule(token, item_ptr, in_out_var);
    if (retval) {
        clear_item(item_ptr);
        return retval;
    }
    
    return sem_param_next_rule(token, item_ptr, in_out_var);
}

/*  PARAM -> )
    PARAM -> id TYPE PARAM_NEXT

    Filling in function with input parameters
*/
int sem_param_rule(Token *token, Sym_table_item *item_ptr, int in_out_var) {
    sem_get_next_token(token);
    if (token->type == ROUND_BR_R) {
        return 0;
    }
    if (token->type != ID) {
        return SYNTAX_ERROR;
    }

    int retval = sem_type_rule(token, item_ptr, in_out_var);
    if (retval) {
        return retval;
    }

    retval = sem_param_next_rule(token, item_ptr, in_out_var);
    return retval;
}

/*  TYPE_NEXT -> , TYPE TYPE_NEXT
    TYPE_NEXT -> )

    Filling in list of return types(continuation)
*/
int sem_type_next_rule(Token *token, Sym_table_item *item_ptr, int in_out_var) {
    sem_get_next_token(token);

    if (token->type == ROUND_BR_R) {
        return 0;
    }

    if (token->type != COMMA) {
        return SYNTAX_ERROR;
    }

    int retval = sem_type_rule(token, item_ptr, in_out_var);
    if (retval) {
        return retval;
    }

    retval = sem_type_next_rule(token, item_ptr, in_out_var);
    return retval;    
}

/*  RETURN_TYPES -> {
    RETURN_TYPES -> ( TYPE TYPE_NEXT {
    RETURN_TYPES -> TYPE {

    Filling in list of return types
*/
int sem_return_types_rule(Token *token, Sym_table_item *item_ptr, int in_out_var) {
    int retval = 0;
    sem_get_next_token(token);

    if (token->type == CURLY_BR_L) {
        return 0;
    }

    if (token->type == ROUND_BR_L) {
        sem_get_next_token(token);
        if (token->type == ROUND_BR_R) {
            sem_get_next_token(token);
            if (token->type == CURLY_BR_L) {
                return 0;
            }
            return SYNTAX_ERROR;
        }

        DLPred(&token_list);
        retval = sem_type_rule(token, item_ptr, in_out_var);
        if (retval) {
            return retval;
        }

        retval = sem_type_next_rule(token, item_ptr, in_out_var);
        if (retval) {
            return retval;
        }
        sem_get_next_token(token);
        if (token->type != CURLY_BR_L) {
            return SYNTAX_ERROR;
        }
        return 0;
    }

    retval = sem_type_rule(token, item_ptr, in_out_var);
    if (retval) {
        return retval;
    }
    sem_get_next_token(token);
    if (token->type != CURLY_BR_L) {
        return SYNTAX_ERROR;
    }

    return 0;
}

/**
 * @brief Item initialiser
 * 
 * @param item_ptr Ptr to item
 * @return int 0 on succerss, errcode otherwise
 */
int sym_item_init(Sym_table_item *item_ptr) {
    item_ptr->value.func.in_var_list = malloc(sizeof(unsigned));
    if (!(item_ptr->value.func.in_var_list)) {
        return INTERNAL_ERROR;
    }
    item_ptr->value.func.num_in_var = 0;

    item_ptr->value.func.out_var_list = malloc(sizeof(unsigned));
    if (!(item_ptr->value.func.out_var_list)) {
        return INTERNAL_ERROR;
    }
    item_ptr->value.func.num_out_var = 0;
    item_ptr->nextPtr = NULL;
    return 0;
}

/**
 * @brief Filling list with declared functions
 * 
 * @return int 
 */
int fill_function_prototype_list() {
    int retval = 0;
    Token token;
    retval = insert_embedded_functions();
    if (retval) {
        return retval;
    }

    while (token_list.Act != NULL) {
        sem_get_next_token(&token);
        
        if (token.type == FUNC) {
            Sym_table_item *sym_table_item_ptr = malloc(sizeof(Sym_table_item));
            if (!sym_table_item_ptr) {
                clear_item(sym_table_item_ptr);
                return INTERNAL_ERROR;
            }
            retval = sym_item_init(sym_table_item_ptr);
            if (retval) {
                return retval;
            }

            int depth = 0;
            sym_table_item_ptr->depth = depth;

            sem_get_next_token(&token);
            if (token.type != ID) {
                return SYNTAX_ERROR;
            }

            token.type = ID_FUNC;
            sym_table_item_ptr->name = malloc(token.data_size);
            if (!(sym_table_item_ptr->name)) {
                return INTERNAL_ERROR;
            }

            strcpy(sym_table_item_ptr->name, token.data);

            sem_get_next_token(&token);
            if (token.type != ROUND_BR_L) {
                return SYNTAX_ERROR;
            }

            if (sem_param_rule(&token, sym_table_item_ptr, IN_VAR)) {
                return SYNTAX_ERROR;
            }
  
            if (sem_return_types_rule(&token, sym_table_item_ptr, OUT_VAR)) {
                return SYNTAX_ERROR;
            }

            if (sym_table_item_ptr->value.func.num_out_var == 0) {
                free(sym_table_item_ptr->value.func.out_var_list);
            }

            retval = sym_table_insert_item(&sym_table, sym_table_item_ptr);
            if (retval) {
                clear_item(sym_table_item_ptr);
                return retval;
            }
        }
    }
    
    return 0;
}

