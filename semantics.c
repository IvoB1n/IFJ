#include "symtable.h"
#include "scanner.h"
#include "error.h"

#include <string.h>
#include <stdlib.h>

#define IN_VAR 0
#define OUT_VAR 1

void clear_item(Sym_table_item *);

int insert_embedded_functions() {
    //  id, id, =, “inputs”, (, ), eol
    Sym_table_item *item = malloc(sizeof(Sym_table_item));
    
    if (!(item)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item);
        return INTERNAL_ERROR;
    }
    
    item->name = malloc(sizeof("inputs\0"));

    if (!(item->name)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item);
        return INTERNAL_ERROR;
    }

    strcpy(item->name, "inputs\0");

    item->depth = 0;
    
    item->value.func.num_in_var = 0;
    item->value.func.in_var_list = NULL;

    item->value.func.num_out_var = 2;
    item->value.func.out_var_list = malloc(sizeof(unsigned) * item->value.func.num_out_var);

    if (!(item->value.func.out_var_list)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item);
        return INTERNAL_ERROR;
    }

    item->value.func.out_var_list[0] = STRING;
    item->value.func.out_var_list[1] = INT;

    sym_table_insert_item(&sym_table, item);

    //  id, id, =, “inputi”, (, ), eol
    Sym_table_item *item2 = malloc(sizeof(Sym_table_item));

    if (!(item2)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item2);
        return INTERNAL_ERROR;
    }
    
    item2->name = malloc(sizeof("inputi\0"));

    if (!(item2->name)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item2);
        return INTERNAL_ERROR;
    }

    strcpy(item2->name, "inputi\0");

    item2->depth = 0;
    
    item2->value.func.num_in_var = 0;
    item2->value.func.in_var_list = NULL;

    item2->value.func.num_out_var = 2;
    item2->value.func.out_var_list = malloc(sizeof(unsigned) * item2->value.func.num_out_var);

    if (!(item2->value.func.out_var_list)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item2);
        return INTERNAL_ERROR;
    }

    item2->value.func.out_var_list[0] = INT;
    item2->value.func.out_var_list[1] = INT;

    sym_table_insert_item(&sym_table, item2);

    //  id, id, =, “inputf”, (, ), eol
    Sym_table_item *item3 = malloc(sizeof(Sym_table_item));

    if (!(item3)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item3);
        return INTERNAL_ERROR;
    }
    
    item3->name = malloc(sizeof("inputf\0"));

    if (!(item3->name)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item3);
        return INTERNAL_ERROR;
    }

    strcpy(item3->name, "inputf\0");

    item3->depth = 0;
    
    item3->value.func.num_in_var = 0;
    item3->value.func.in_var_list = NULL;

    item3->value.func.num_out_var = 2;
    item3->value.func.out_var_list = malloc(sizeof(unsigned) * item3->value.func.num_out_var);

    if (!(item3->value.func.out_var_list)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item3);
        return INTERNAL_ERROR;
    }

    item3->value.func.out_var_list[0] = FLOAT64;
    item3->value.func.out_var_list[1] = INT;
   
    sym_table_insert_item(&sym_table, item3);

    //  “print”, (, …, ), eol
    Sym_table_item *item4 = malloc(sizeof(Sym_table_item));

    if (!(item4)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item4);
        return INTERNAL_ERROR;
    }
    
    item4->name = malloc(sizeof("print\0"));

    if (!(item4->name)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item4);
        return INTERNAL_ERROR;
    }

    strcpy(item4->name, "print\0");

    item4->depth = 0;
    
    item4->value.func.num_in_var = 0;
    item4->value.func.in_var_list = NULL;

    item4->value.func.num_out_var = 0;
    item4->value.func.out_var_list = NULL; 
    
    sym_table_insert_item(&sym_table, item4);

    //  id, =, “int2float”, (, id, ), eol
    Sym_table_item *item5 = malloc(sizeof(Sym_table_item));

    if (!(item5)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item5);
        return INTERNAL_ERROR;
    }
    
    item5->name = malloc(sizeof("int2float\0"));

    if (!(item5->name)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item5);
        return INTERNAL_ERROR;
    }
    strcpy(item5->name, "int2float\0");

    item5->depth = 0;
    
    item5->value.func.num_in_var = 1;
    item5->value.func.in_var_list = malloc(sizeof(unsigned) * item5->value.func.num_in_var);

    if (!(item5->value.func.in_var_list)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item5);
        return INTERNAL_ERROR;
    }

    item5->value.func.num_out_var = 1;
    item5->value.func.out_var_list = malloc(sizeof(unsigned) * item5->value.func.num_out_var);

    if (!(item5->value.func.out_var_list)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item5);
        return INTERNAL_ERROR;
    }

    item5->value.func.in_var_list[0] = INT;

    item5->value.func.out_var_list[0] = FLOAT64;

    sym_table_insert_item(&sym_table, item5);

    //  id, =, “float2int”, (, id, ), eol
    Sym_table_item *item6 = malloc(sizeof(Sym_table_item));

    if (!(item6)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item6);
        return INTERNAL_ERROR;
    }
    
    item6->name = malloc(sizeof("float2int\0"));

    if (!(item6->name)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item6);
        return INTERNAL_ERROR;
    }

    strcpy(item6->name, "float2int\0");

    item6->depth = 0;
    
    item6->value.func.num_in_var = 1;
    item6->value.func.in_var_list = malloc(sizeof(unsigned) * item6->value.func.num_in_var);

    if (!(item6->value.func.in_var_list)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item6);
        return INTERNAL_ERROR;
    }

    item6->value.func.num_out_var = 1;
    item6->value.func.out_var_list = malloc(sizeof(unsigned) * item6->value.func.num_out_var);

    if (!(item6->value.func.out_var_list)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item6);
        return INTERNAL_ERROR;
    }

    item6->value.func.in_var_list[0] = FLOAT64;

    item6->value.func.out_var_list[0] = INT;

    sym_table_insert_item(&sym_table, item6);

    //  id, =, “len”, (, id, ), eol
    Sym_table_item *item7 = malloc(sizeof(Sym_table_item));

    if (!(item7)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item7);
        return INTERNAL_ERROR;
    }
    
    item7->name = malloc(sizeof("len\0"));

    if (!(item7->name)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item7);
        return INTERNAL_ERROR;
    }

    strcpy(item7->name, "len\0");

    item7->depth = 0;
    
    item7->value.func.num_in_var = 1;
    item7->value.func.in_var_list = malloc(sizeof(unsigned) * item7->value.func.num_in_var);

    if (!(item7->value.func.in_var_list)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item7);
        return INTERNAL_ERROR;
    }

    item7->value.func.num_out_var = 1;
    item7->value.func.out_var_list = malloc(sizeof(unsigned) * item7->value.func.num_out_var);

    if (!(item7->value.func.out_var_list)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item7);
        return INTERNAL_ERROR;
    }

    item7->value.func.in_var_list[0] = STRING;

    item7->value.func.out_var_list[0] = INT;

    sym_table_insert_item(&sym_table, item7);

    //  id, id, =, “substr”, (, id, id, id, ), eol
    Sym_table_item *item8 = malloc(sizeof(Sym_table_item));

    if (!(item8)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item8);
        return INTERNAL_ERROR;
    }
    
    item8->name = malloc(sizeof("substr\0"));

    if (!(item8->name)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item8);
        return INTERNAL_ERROR;
    }

    strcpy(item8->name, "substr\0");

    item8->depth = 0;
    
    item8->value.func.num_in_var = 3;
    item8->value.func.in_var_list = malloc(sizeof(unsigned) * item8->value.func.num_in_var);

    if (!(item8->value.func.in_var_list)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item8);
        return INTERNAL_ERROR;
    }

    item8->value.func.num_out_var = 2;
    item8->value.func.out_var_list = malloc(sizeof(unsigned) * item8->value.func.num_out_var);

    if (!(item8->value.func.out_var_list)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item8);
        return INTERNAL_ERROR;
    }

    item8->value.func.in_var_list[0] = STRING;
    item8->value.func.in_var_list[1] = INT;
    item8->value.func.in_var_list[2] = INT;

    item8->value.func.out_var_list[0] = STRING;
    item8->value.func.out_var_list[1] = INT;

    sym_table_insert_item(&sym_table, item8);

    //  id, id, =, “ord”, (, id, id, ), eol
    Sym_table_item *item9 = malloc(sizeof(Sym_table_item));

    if (!(item9)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item9);
        return INTERNAL_ERROR;
    }
    
    item9->name = malloc(sizeof("ord\0"));

    if (!(item9->name)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item9);
        return INTERNAL_ERROR;
    }

    strcpy(item9->name, "ord\0");

    item9->depth = 0;
    
    item9->value.func.num_in_var = 2;
    item9->value.func.in_var_list = malloc(sizeof(unsigned) * item9->value.func.num_in_var);

    if (!(item9->value.func.in_var_list)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item9);
        return INTERNAL_ERROR;
    }

    item9->value.func.num_out_var = 2;
    item9->value.func.out_var_list = malloc(sizeof(unsigned) * item9->value.func.num_out_var);

    if (!(item9->value.func.out_var_list)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item9);
        return INTERNAL_ERROR;
    }

    item9->value.func.in_var_list[0] = STRING;
    item9->value.func.in_var_list[1] = INT;

    item9->value.func.out_var_list[0] = INT;
    item9->value.func.out_var_list[1] = INT;

    sym_table_insert_item(&sym_table, item9);

    //  id, id, =, “chr”, (, id, ), eol
    Sym_table_item *item10 = malloc(sizeof(Sym_table_item));

    if (!(item10)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item10);
        return INTERNAL_ERROR;
    }
    
    item10->name = malloc(sizeof("chr\0"));

    if (!(item10->name)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item10);
        return INTERNAL_ERROR;
    }
    strcpy(item10->name, "chr\0");

    item10->depth = 0;
    
    item10->value.func.num_in_var = 1;
    item10->value.func.in_var_list = malloc(sizeof(unsigned) * item10->value.func.num_in_var);

    if (!(item10->value.func.in_var_list)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item10);
        return INTERNAL_ERROR;
    }

    item10->value.func.num_out_var = 2;
    item10->value.func.out_var_list = malloc(sizeof(unsigned) * item10->value.func.num_out_var);

    if (!(item10->value.func.out_var_list)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clear_item(item10);
        return INTERNAL_ERROR;
    }

    item10->value.func.in_var_list[0] = INTEGER;

    item10->value.func.out_var_list[0] = STRING;
    item10->value.func.out_var_list[1] = INTEGER;

    sym_table_insert_item(&sym_table, item10);

    return 0;
}

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

    if (item->name) {
        free(item->name);
    }

    if (item->value.func.num_in_var > 0) {
        free(item->value.func.in_var_list);
    }

    if (item->value.func.num_out_var > 0) {
        free(item->value.func.out_var_list);
    }

    free(item);
}

int sem_type_rule(Token *token, Sym_table_item *item_ptr, int in_out_var) {
    sem_get_next_token(token);
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    if ((token->type == INT) || (token->type == FLOAT64) || (token->type == STRING)) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        if (in_out_var == IN_VAR) {
            if (item_ptr->value.func.num_in_var > 0) {
                item_ptr->value.func.in_var_list = realloc(item_ptr->value.func.in_var_list, (item_ptr->value.func.num_in_var + 1) * sizeof(unsigned)); // *var_list ,*(var_list + 1(int)), *(var_list + 2(int))
            
                if(!item_ptr->value.func.in_var_list) {
                    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                    //clear_item(item_ptr);
                    return INTERNAL_ERROR;
                }
            }
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            item_ptr->value.func.in_var_list[item_ptr->value.func.num_in_var] = token->type;
            item_ptr->value.func.num_in_var++;
        }
        else {
            if (item_ptr->value.func.num_out_var > 0) {
                item_ptr->value.func.out_var_list = realloc(item_ptr->value.func.out_var_list, (item_ptr->value.func.num_out_var + 1) * sizeof(unsigned)); // *var_list ,*(var_list + 1(int)), *(var_list + 2(int))
                if(!item_ptr->value.func.out_var_list) {
                    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                    //clear_item(item_ptr);
                    return INTERNAL_ERROR;
                }
            }
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            item_ptr->value.func.out_var_list[item_ptr->value.func.num_out_var] = token->type;
            item_ptr->value.func.num_out_var++;
        }
    }
    else {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        //clear_item(item_ptr);
        return SYNTAX_ERROR;
    }
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    return 0;
}

int sem_param_next_rule(Token *token, Sym_table_item *item_ptr, int in_out_var) {
    sem_get_next_token(token);
    if (token->type == ROUND_BR_R) {
        return 0;
    }

    if (token->type != COMMA) {
        //clear_item(item_ptr);
        return SYNTAX_ERROR;
    }

    sem_get_next_token(token);
    if (token->type != ID) {
        //clear_item(item_ptr);
        return SYNTAX_ERROR;
    }
    
    int retval = sem_type_rule(token, item_ptr, in_out_var);
    if (retval) {
        //clear_item(item_ptr);
        return retval;
    }
    
    return sem_param_next_rule(token, item_ptr, in_out_var);
}

int sem_param_rule(Token *token, Sym_table_item *item_ptr, int in_out_var) {
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    sem_get_next_token(token);
    if (token->type == ROUND_BR_R) {
        return 0;
    }
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    if (token->type != ID) {
      //  //clear_item(item_ptr);
        return SYNTAX_ERROR;
    }

    int retval = sem_type_rule(token, item_ptr, in_out_var);
    if (retval) {
        ////clear_item(item_ptr);
        return retval;
    }

    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    retval = sem_param_next_rule(token, item_ptr, in_out_var);
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    return retval;
}

int sem_type_next_rule(Token *token, Sym_table_item *item_ptr, int in_out_var) {
    sem_get_next_token(token);

    if (token->type == ROUND_BR_R) {
        return 0;
    }

    if (token->type != COMMA) {
        //clear_item(item_ptr);
        return SYNTAX_ERROR;
    }

    int retval = sem_type_rule(token, item_ptr, in_out_var);
    if (retval) {
        //clear_item(item_ptr);
        return retval;
    }

    retval = sem_type_next_rule(token, item_ptr, in_out_var);
    return retval;    
}

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
            //clear_item(item_ptr);
            return SYNTAX_ERROR;
        }

        DLPred(&token_list);
        retval = sem_type_rule(token, item_ptr, in_out_var);
        if (retval) {
            //clear_item(item_ptr);
            return retval;
        }

        retval = sem_type_next_rule(token, item_ptr, in_out_var);
        if (retval) {
            //clear_item(item_ptr);
            return retval;
        }
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        sem_get_next_token(token);
        if (token->type != CURLY_BR_L) {
            //clear_item(item_ptr);
            return SYNTAX_ERROR;
        }
        return 0;
    }

    retval = sem_type_rule(token, item_ptr, in_out_var);
    if (retval) {
        //clear_item(item_ptr);
        return retval;
    }
    sem_get_next_token(token);
    if (token->type != CURLY_BR_L) {
        //clear_item(item_ptr);
        return SYNTAX_ERROR;
    }

    return 0;
}

int sym_item_init(Sym_table_item *item_ptr) {
    item_ptr->value.func.in_var_list = malloc(sizeof(unsigned));
    if (!(item_ptr->value.func.in_var_list)) {
        //clear_item(item_ptr);
        return INTERNAL_ERROR;
    }
    item_ptr->value.func.num_in_var = 0;

    item_ptr->value.func.out_var_list = malloc(sizeof(unsigned));
    if (!(item_ptr->value.func.out_var_list)) {
        ////clear_item(item_ptr);
        return INTERNAL_ERROR;
    }
    item_ptr->value.func.num_out_var = 0;
    return 0;
}

int fill_function_prototype_list() {
    int retval = 0;
    Token token;

    retval = insert_embedded_functions();
    if (retval) {
        return retval;
    }

    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    while (token_list.Act != NULL) {
        sem_get_next_token(&token);
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        
        if (token.type == FUNC) {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            Sym_table_item *sym_table_item_ptr = malloc(sizeof(Sym_table_item));
            if (!sym_table_item_ptr) {
                clear_item(sym_table_item_ptr);
                return INTERNAL_ERROR;
            }

            retval = sym_item_init(sym_table_item_ptr);
            if (retval) {
                clear_item(sym_table_item_ptr);
                return retval;
            }

            int depth = 0;
            sym_table_item_ptr->depth = depth;
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

            sem_get_next_token(&token);
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            if (token.type != ID) {
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                clear_item(sym_table_item_ptr);
                return SYNTAX_ERROR;
            }

            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            token.type = ID_FUNC;
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            sym_table_item_ptr->name = malloc(sizeof(token.data));
            if (!(sym_table_item_ptr->name)) {
                clear_item(sym_table_item_ptr);
                return INTERNAL_ERROR;
            }
            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            strcpy(sym_table_item_ptr->name, token.data);

            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            sem_get_next_token(&token);
            if (token.type != ROUND_BR_L) {
                clear_item(sym_table_item_ptr);
                return SYNTAX_ERROR;
            }
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

            if (sem_param_rule(&token, sym_table_item_ptr, IN_VAR)) {
                clear_item(sym_table_item_ptr);
                return SYNTAX_ERROR;
            }

            if (sym_table_item_ptr->value.func.num_in_var == 0) {
                free(sym_table_item_ptr->value.func.in_var_list);
            }

            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
  
            if (sem_return_types_rule(&token, sym_table_item_ptr, OUT_VAR)) {
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                clear_item(sym_table_item_ptr);
                return SYNTAX_ERROR;
            }

            if (sym_table_item_ptr->value.func.num_out_var == 0) {
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                free(sym_table_item_ptr->value.func.out_var_list);
            }

            fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            retval = sym_table_insert_item(&sym_table, sym_table_item_ptr);
            if (retval) {
                clear_item(sym_table_item_ptr);
                return retval;
            }
            // -----------print item ------------
            printf("name: %s\n", sym_table_item_ptr->name);

            printf("insize: %u\n", sym_table_item_ptr->value.func.num_in_var);
            for (unsigned i = 0; i < sym_table_item_ptr->value.func.num_in_var; i++) {
                printf("i = %u\n", i);
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                printf("in_var: %d\n", sym_table_item_ptr->value.func.in_var_list[i]);
            }
            
            printf("outsize: %u\n", sym_table_item_ptr->value.func.num_out_var);
            for (unsigned i = 0; i < sym_table_item_ptr->value.func.num_out_var; i++) {
                printf("out_var: %d\n", sym_table_item_ptr->value.func.out_var_list[i]);
            }
        }
    }
    return 0;
}

