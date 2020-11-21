#include "token_dll.h"
#include "symtable.h"
#include "scanner.h"
#include "error.h"
#include <stdlib.h>

void get_next_token(Token *token) {
    if (token_list.Act != NULL) {
        *token = token_list.Act->token;

        print_token(token);
  
        DLSucc(&token_list);
    }
}

void  clear_item(Sym_table_item *item) {
    if (item == NULL) {
        return;
    }
    
    free(item->name);
    free(item->value.func.in_var_list);
    free(item->value.func.out_var_list);
    free(item);
}

int type_rule(Token *token, int *var_list) {
    get_next_token(token);
    if ((token->type == INT) || (token->type == FLOAT64) || (token->type == STRING)) {
        int size_of_list = sizeof(var_list) / sizeof(int);
        if(var_list = realloc(var_list, sizeof(var_list) + sizeof(unsigned))){
            return INTERNAL_ERROR;
        }
        var_list[size_of_list] = token->type;
    }
    else {
        return SYNTAX_ERROR;
    }
    
    return 0;
}

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

    int retval = type_rule(token);
    if (retval) {
        return retval;
    }
    retval = param_next_rule(token);
    return retval;
}

int param_rule(Token *token, Sym_table_item *item_ptr) {
    get_next_token(token);
    if (token->type == ROUND_BR_R) {
        return 0;
    }

    if (token->type != ID) {
        return SYNTAX_ERROR;
    }

    int *in_var_list = NULL;
    int retval = type_rule(token, in_var_list);
    if (retval) {
        free(in_var_list);
        return retval;
    }

    retval = param_next_rule(token);
    if (retval) {
        free(in_var_list);
    }
    else {
        item_ptr->value.func.in_var_list = in_var_list;
    }
    return retval;
}

int type_next_rule(Token *token, int *var_list) {
    get_next_token(token);

    if (token->type == ROUND_BR_R) {
        return 0;
    }

    if (token->type != COMMA) {
        return SYNTAX_ERROR;
    }

    int retval = type_rule(token, var_list);
    if (retval) {
        free(var_list);
        return retval;
    }

    retval = type_next_rule(token, var_list);
    return retval;    
}

int return_types_rule(Token *token, Sym_table_item *item_ptr) {
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

        int *out_var_list = NULL;

        DLPred(&token_list);
        retval = type_rule(token, out_var_list);
        if (retval) {
            return retval;
        }

        retval = type_next_rule(token, out_var_list);
        if (retval) {
            free(out_var_list);
            return retval;
        }

        item_ptr->value.func.out_var_list = out_var_list;

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



int fill_function_prototype_list() {
    int retval = 0;
    Token token;

    while (&token_list.Act) {
        token = token_list.Act->token;
        if (token.type == FUNC) {
            Sym_table_item *sym_table_item_ptr = malloc(sizeof(Sym_table_item));
            if (!sym_table_item_ptr) {
                return INTERNAL_ERROR;
            }
            int depth = 0;

            sym_table_item_ptr->depth = depth;
            get_next_token(&token);
            if (token.type != ID) {
                return SYNTAX_ERROR;
            }
            token.type = ID_FUNC;
            sym_table_item_ptr->name = malloc(sizeof(token.data));
            strcpy(sym_table_item_ptr->name, token.data);

            get_next_token(&token);
            if (token.type != ROUND_BR_L) {
                return SYNTAX_ERROR;
            }
                
            if (param_rule(&token, sym_table_item_ptr)) {
                clear_item(sym_table_item_ptr);
                return SYNTAX_ERROR;
            }

            if (return_types_rule(&token, sym_table_item_ptr)) {
                clear_item(sym_table_item_ptr);
                return SYNTAX_ERROR;
            }

            retval = insert_item(sym_table, sym_table_item_ptr);
            if (retval) {
                clear_item(sym_table_item_ptr);
                return retval;
            }
        }
        DLSucc(&token_list);
    }
    return 0;
}

