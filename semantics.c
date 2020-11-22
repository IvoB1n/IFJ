#include "symtable.h"
#include "scanner.h"
#include "error.h"

#include <string.h>
#include <stdlib.h>

int num_of_el = 0;

void sem_print_token (Token *token) {
    printf("                       type= %-4d data= \"%s\"%-5s size= %-9d\n", token->type, token->data, "", token->data_size);
}

void sem_get_next_token(Token *token) {
    if (token_list.Act != NULL) {
        *token = token_list.Act->token;
        sem_print_token(token);
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

int sem_type_rule(Token *token, unsigned *var_list) {
    sem_get_next_token(token);
     fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    if ((token->type == INT) || (token->type == FLOAT64) || (token->type == STRING)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        printf("%ld\n", sizeof(var_list));
       // int size_of_list = sizeof(var_list) / sizeof(unsigned);
        printf("num: %d\n", num_of_el);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        if (num_of_el == 0) {
            var_list = malloc(sizeof(unsigned));
        }
        else {
            var_list = realloc(var_list, sizeof(var_list) + sizeof(unsigned)); // *var_list ,*(var_list + 1(int)), *(var_list + 2(int))
        }
        if(!var_list) {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            return INTERNAL_ERROR;
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        var_list[num_of_el] = token->type;
        num_of_el++;
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    }
    else {
         fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return SYNTAX_ERROR;
    }
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    return 0;
}

int sem_param_next_rule(Token *token, unsigned *var_list) {
    sem_get_next_token(token);
    if (token->type == ROUND_BR_R) {
        return 0;
    }

    if (token->type != COMMA) {
        return SYNTAX_ERROR;
    }

    sem_get_next_token(token);
    if (token->type != ID) {
        return SYNTAX_ERROR;
    }

    int retval = sem_type_rule(token, var_list);
    if (retval) {
        return retval;
    }
    retval = sem_param_next_rule(token, var_list);
    return retval;
}

int sem_param_rule(Token *token, Sym_table_item *item_ptr) {
     fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    sem_get_next_token(token);
    if (token->type == ROUND_BR_R) {
        return 0;
    }
 fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    if (token->type != ID) {
        return SYNTAX_ERROR;
    }
 fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    unsigned *in_var_list = NULL;
    int retval = sem_type_rule(token, in_var_list);
    if (retval) {
        free(in_var_list);
        return retval;
    }

    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    retval = sem_param_next_rule(token, in_var_list);
    if (retval) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        free(in_var_list);
    }
    else {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        printf("%ld\n", sizeof(in_var_list));
        printf("%u\n", in_var_list[0]);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

      //  printf("%u\n", in_var_list[]);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        item_ptr->value.func.in_var_list = in_var_list;
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        memcpy(item_ptr->value.func.in_var_list, in_var_list, sizeof(*in_var_list));
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    }
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    printf("%ld\n", sizeof(in_var_list));
    return retval;
}

int sem_type_next_rule(Token *token, unsigned *var_list) {
    sem_get_next_token(token);

    if (token->type == ROUND_BR_R) {
        return 0;
    }

    if (token->type != COMMA) {
        return SYNTAX_ERROR;
    }

    int retval = sem_type_rule(token, var_list);
    if (retval) {
        free(var_list);
        return retval;
    }

    retval = sem_type_next_rule(token, var_list);
    return retval;    
}

int sem_return_types_rule(Token *token, Sym_table_item *item_ptr) {
    int retval = 0;
    sem_get_next_token(token);

    if (token->type == CURLY_BR_L) {
        return 0;
    }

    unsigned *out_var_list = NULL;
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
        retval = sem_type_rule(token, out_var_list);
        if (retval) {
            return retval;
        }

        retval = sem_type_next_rule(token, out_var_list);
        if (retval) {
            free(out_var_list);
            return retval;
        }

        memcpy(item_ptr->value.func.out_var_list, out_var_list, sizeof(*out_var_list));

        sem_get_next_token(token);
        if (token->type != CURLY_BR_L) {
            return SYNTAX_ERROR;
        }
        return 0;
    }

    retval = sem_type_rule(token, out_var_list);
    if (retval) {
        return retval;
    }
    sem_get_next_token(token);
    if (token->type != CURLY_BR_L) {
        return SYNTAX_ERROR;
    }

    return 0;
}



int fill_function_prototype_list() {
    int retval = 0;
    Token token;
 fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    while (token_list.Act != NULL) {
         fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        sem_get_next_token(&token);
         fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        //sem_print_token(&token);
        if (token.type == FUNC) {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            Sym_table_item *sym_table_item_ptr = malloc(sizeof(Sym_table_item));
            if (!sym_table_item_ptr) {
                return INTERNAL_ERROR;
            }

            int depth = 0;
            sym_table_item_ptr->depth = depth;
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

            //sem_print_token(&token);
            sem_get_next_token(&token);
            //sem_get_next_token(&token);
            // sem_print_token(&token);
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            if (token.type != ID) {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                free(sym_table_item_ptr);
                return SYNTAX_ERROR;
            }

            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            token.type = ID_FUNC;
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            sym_table_item_ptr->name = malloc(sizeof(token.data));
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            strcpy(sym_table_item_ptr->name, token.data);

            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            sem_get_next_token(&token);
            if (token.type != ROUND_BR_L) {
                return SYNTAX_ERROR;
            }
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            if (sem_param_rule(&token, sym_table_item_ptr)) {
                clear_item(sym_table_item_ptr);
                return SYNTAX_ERROR;
            }
            num_of_el = 0;
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            if (sem_return_types_rule(&token, sym_table_item_ptr)) {
                clear_item(sym_table_item_ptr);
                return SYNTAX_ERROR;
            }
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            retval = insert_item(&sym_table, sym_table_item_ptr);
            if (retval) {
                clear_item(sym_table_item_ptr);
                return retval;
            }
            // -----------print item ------------
            printf("name: %s\n", sym_table_item_ptr->name);
            unsigned *list = sym_table_item_ptr->value.func.in_var_list;
            int size = sizeof(*list) / sizeof(int);
            for (int i = 0; i < size; i++) {
                printf("%d\n", i);
                //printf("%d\n", item);
                printf("in_var: %d\n", list[i]);
            }
            
            int out_size = sizeof(*(sym_table_item_ptr->value.func.out_var_list)) / sizeof(int);
            for (int i = 0; i < out_size; i++) {
                printf("out_var: %d\n", sym_table_item_ptr->value.func.out_var_list[i]);
            }
            // -----------end print item ------------
        }
    }
    return 0;
}

