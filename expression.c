#include "expression.h"
#include "error.h"
#include "symtable.h"
#include <string.h>

int ops;
int done_ops;
unsigned exp_type;

#define PREC_TABLE_SIZE 14

/**
 * Controls true or false values
 * 
 * @param elem pointer to token list element
 * @return 0 successful control
 * @return 1 dividion by zero const accrued
 */
int bool_value_check(tDLElemPtr elem) {
    char* true_str = "true\0";
    char* false_str = "false\0";
    
    if ((strcmp(true_str, elem->token.data) == 0 ) ||
        (strcmp(false_str, elem->token.data) == 0)) {
        return 1;
    }
    return 0;
}

/**
 * Converts string to ifj20.code format
 * 
 * @param s_token pointer to token list node, with string
 * @return 
 */
void generate_str (tDLElemPtr s_token) {
    for (unsigned i = 1; i < s_token->token.data_size-2; i++) {
        if (s_token->token.data[i] == '#') {
            fprintf(stdout, "\\%03d", '#');
        } else if (s_token->token.data[i] == ' ') {
            fprintf(stdout, "\\%03d", ' ');
        } else if (s_token->token.data[i] == '\\' && s_token->token.data[i+1] == '\\') {
            fprintf(stdout, "\\%03d", '\\');
            i+=1;
        } else if (s_token->token.data[i] == '\\' && s_token->token.data[i+1] == 'n') {
            fprintf(stdout, "\\%03d", '\n');
            i+=1;
        } else if (s_token->token.data[i] == '\\' && s_token->token.data[i+1] == 't') {
            fprintf(stdout, "\\%03d", '\t');
            i+=1;
        } else if (s_token->token.data[i] == '\\' && s_token->token.data[i+1] == '\"') {
            fprintf(stdout, "\"");
            i+=1;
        } 
        else if ((s_token->token.data_size-1-i) >= 4) {
            if(s_token->token.data[i] == '\\' && s_token->token.data[i+1] == 'x') {
                char *s;
                s = malloc(sizeof(char)*3);
                if (!s) {
                    fprintf(stderr, "memory allocation error\n");
                    return;
                }
                s[0] = s_token->token.data[i+2];
                s[1] = s_token->token.data[i+3];
                s[2] = '\0';
                int x;
                sscanf(s, "%x", &x);
                fprintf(stdout, "\\%03u", x);
                i+=3;
                free(s);
            } else {
                fprintf(stdout, "%c", s_token->token.data[i]);
            }
        } else {
            fprintf(stdout, "%c", s_token->token.data[i]);
        }
    }
    fprintf(stdout, "\n");
}

/**
 * Generate ifj20.code variable push
 * 
 * @param s_token pointer to token list node, with variable
 * @return 
 */
void generate_push_by_type(tDLElemPtr s_token, unsigned *depth ) {
    Sym_table_item *item;
    // fprintf(stderr, "exp_push_type= %u\n", exp_type);
    if (((s_token->token.data[0] >= 'a' && s_token->token.data[0] <= 'z') ||
         (s_token->token.data[0] >= 'A' && s_token->token.data[0] <= 'Z'))&&
         !bool_value_check(s_token)) {
        item = sym_table_search_item(&sym_table, s_token->token.data, *depth);
        if (item == NULL) {
            return;
        } else {
            unsigned var_depth = item->depth;
            fprintf(stdout, "PUSHS LF@%s%u\n", s_token->token.data, var_depth);
        }
    } else if (exp_type == INTEGER) {
        fprintf(stdout, "PUSHS int@%s\n", s_token->token.data);
    } else if (exp_type == FLOAT) {
        double fl = strtod(s_token->token.data, NULL);
        fprintf(stdout, "PUSHS float@%a\n", fl);
        fprintf(stderr, " float@%a (%f)\n", fl, fl);
    } else if (exp_type == STR_END) {
        fprintf(stdout, "PUSHS string@");
        generate_str(s_token);
    } else if (exp_type == BOOLEAN && bool_value_check(s_token)) {
        fprintf(stdout, "PUSHS bool@%s\n", s_token->token.data);
    } 
}

/**
 * Precedent table 
 */
unsigned prec_table [PREC_TABLE_SIZE][PREC_TABLE_SIZE] = {
//      +       -      *       /       (       )      <       <=      >       >=      ==     !=       i      $
    {REDUCE, REDUCE, SHIFT,  SHIFT,  SHIFT, REDUCE, REDUCE, REDUCE, REDUCE, REDUCE, REDUCE, REDUCE, SHIFT, REDUCE}, // +
    {REDUCE, REDUCE, SHIFT,  SHIFT,  SHIFT, REDUCE, REDUCE, REDUCE, REDUCE, REDUCE, REDUCE, REDUCE, SHIFT, REDUCE}, // -
    {REDUCE, REDUCE, REDUCE, REDUCE, SHIFT, REDUCE, REDUCE, REDUCE, REDUCE, REDUCE, REDUCE, REDUCE, SHIFT, REDUCE}, // *
    {REDUCE, REDUCE, REDUCE, REDUCE, SHIFT, REDUCE, REDUCE, REDUCE, REDUCE, REDUCE, REDUCE, REDUCE, SHIFT, REDUCE}, // /
    {SHIFT,  SHIFT,  SHIFT,  SHIFT,  SHIFT, EQUAL,  SHIFT,  SHIFT,  SHIFT,  SHIFT,  SHIFT,  SHIFT,  SHIFT, NONE  }, // (
    {REDUCE, REDUCE, REDUCE, REDUCE, NONE,  REDUCE, REDUCE, REDUCE, REDUCE, REDUCE, REDUCE, REDUCE, NONE,  REDUCE}, // )
    {SHIFT,  SHIFT,  SHIFT,  SHIFT,  SHIFT, REDUCE, NONE,   NONE,   NONE,   NONE,   REDUCE, REDUCE, SHIFT, REDUCE}, // <
    {SHIFT,  SHIFT,  SHIFT,  SHIFT,  SHIFT, REDUCE, NONE,   NONE,   NONE,   NONE,   REDUCE, REDUCE, SHIFT, REDUCE}, // <=
    {SHIFT,  SHIFT,  SHIFT,  SHIFT,  SHIFT, REDUCE, NONE,   NONE,   NONE,   NONE,   REDUCE, REDUCE, SHIFT, REDUCE}, // >
    {SHIFT,  SHIFT,  SHIFT,  SHIFT,  SHIFT, REDUCE, NONE,   NONE,   NONE,   NONE,   REDUCE, REDUCE, SHIFT, REDUCE}, // >=
    {SHIFT,  SHIFT,  SHIFT,  SHIFT,  SHIFT, REDUCE, SHIFT,  SHIFT,  SHIFT,  SHIFT,  NONE,   NONE,   SHIFT, REDUCE}, // ==
    {SHIFT,  SHIFT,  SHIFT,  SHIFT,  SHIFT, REDUCE, SHIFT,  SHIFT,  SHIFT,  SHIFT,  NONE,   NONE,   SHIFT, REDUCE}, // !=
    {REDUCE, REDUCE, REDUCE, REDUCE, NONE,  REDUCE, REDUCE, REDUCE, REDUCE, REDUCE, REDUCE, REDUCE, NONE,  REDUCE}, // i
    {SHIFT,  SHIFT,  SHIFT,  SHIFT,  SHIFT, NONE,   SHIFT,  SHIFT,  SHIFT,  SHIFT,  SHIFT,  SHIFT,  SHIFT, NONE }}; // $

/**
 * Generate ifj20.code arithmetical operators
 * 
 * @param s_token pointer to token list node, with arithm operator
 * @return 
 */
void arithm_template(tDLElemPtr s_token) {
    if (exp_type == BOOLEAN) {
        switch(s_token->token.type) {
            case AND:
                fprintf(stdout, "ANDS\n");
                break;
            case OR:
                fprintf(stdout, "ORS\n");
                break;
        };
        done_ops+=1;
    } else if (exp_type == STR_END) {
        fprintf(stdout, "POPS GF@&str_concat_2\n");
        fprintf(stdout, "POPS GF@&str_concat_1\n");
        fprintf(stdout, "CONCAT GF@&str_concat_res GF@&str_concat_1 GF@&str_concat_2\n");
        fprintf(stdout, "PUSHS GF@&str_concat_res\n");
        done_ops+=1;
    } else {
        switch(s_token->token.type) {
            case ADD:
                fprintf(stdout, "ADDS\n");
                break;
            case SUB:
                fprintf(stdout, "SUBS\n");
                break;
            case MUL:
                fprintf(stdout, "MULS\n");
                break;
            case DIV:
                fprintf(stdout, "POPS GF@&div_zero_number\n");

                if (exp_type == FLOAT) {
                    fprintf(stdout, "JUMPIFEQ &div_zero_label_err GF@&div_zero_number float@0x0p+0\n");
                    fprintf(stdout, "PUSHS GF@&div_zero_number\n");
                    fprintf(stdout, "DIVS\n");
                } else if (exp_type == INTEGER) {
                    fprintf(stdout, "JUMPIFEQ &div_zero_label_err GF@&div_zero_number int@0\n");
                    fprintf(stdout, "PUSHS GF@&div_zero_number\n");
                    fprintf(stdout, "IDIVS\n");
                }
                break;
        };
        done_ops+=1;
    }
}

/**
 * Generate ifj20.code logical operators
 * 
 * @param s_token pointer to token list node, with arithm operator
 * @return 
 */
void logic_template(tDLElemPtr s_token) {
    switch (s_token->token.type) {
        case L:
            fprintf(stdout, "LTS\n");
            break;
        case LE:
            fprintf(stdout, "GTS\n");
            fprintf(stdout, "NOTS\n");
            break;
        case G:
            fprintf(stdout, "GTS\n");
            break;
        case GE:
            fprintf(stdout, "LTS\n");
            fprintf(stdout, "NOTS\n");
            break;
        case EQ:
            fprintf(stdout, "EQS\n");
            break;
        case NOT_EQ:
        fprintf(stdout, "EQS\n");
        fprintf(stdout, "NOTS\n");
        break;
    };
}

/**
 * Returns precedent table rule due to stack and input symbols
 * 
 * @param sym1 stack symbol type
 * @param sym2 input symbol type
 * @return precedent table rule
 */
unsigned get_prec_table_rule(unsigned sym1, unsigned sym2) {
    return prec_table[sym1][sym2];
}

/**
 * Prints expression list for testing purpose
 * 
 * @param L pointer to expression list
 * @return prints list
 */
void print_exp_list(tDLList *L) {
    tDLElemPtr tmp = L->First;
    fprintf(stderr, "---------------\n");
    while (tmp != NULL) {
        fprintf(stderr,  "type= %-4d   data= %-4s  d_size= %-4d\n", tmp->token.type, tmp->token.data, tmp->token.data_size);
        tmp = tmp->rptr;
    }
    fprintf(stderr, "---------------\n");
}

/**
 * Insert first list node with needed token type
 * 
 * @param L pointer to token list
 * @param type type to add
 * @return
 */
void insert_first_by_type(tDLList *L, unsigned type){
    Token token;

    token.type = type;
    token.data = NULL;
    token.data_size = 0;

    DLInsertFirst(L, &token);
}

/**
 * Delete first list node
 * 
 * @param L pointer to token list
 * @return
 */
void pop(tDLList *L){
    tDLElemPtr element;

    if (L->First == NULL) {
        return;
    } else {
        element = L->First;

        if (L->First == L->Act) {
            L->Act = NULL;
        }

        if (L->First == L->Last) {
            L->First = NULL;
            L->Last = NULL;
        } else {
            L->First = L->First->rptr; 
            L->First->lptr = NULL;
        }

        if (element) {
            if (element->token.type == DOLLAR || 
                element->token.type == SHIFT  ||
                element->token.type == E ){
                if (element->token.data) {
                    free(element->token.data);
                }
                free(element);
            } else if (element->token.type == I) {
                free(element);
            }
        }
    }
} 

/**
 * Insert first list node with 'E'(nonterminal) token type and copy removed token value
 * 
 * @param L pointer to token list
 * @return
 */
void insert_E(tDLList *L){    
    Token token;
    token.type = E;
    token.data_size = L->First->token.data_size;
    token.data = malloc(sizeof(char) * L->First->token.data_size);
    strcpy(token.data, L->First->token.data);

    pop(L);
    pop(L);
    DLInsertFirst(L, &token);
}

/**
 * List destructor
 * 
 * @param L pointer to token list
 * @return
 */
void exp_list_dtor(tDLList *L) {
    tDLElemPtr element;
    if (L->First) {
        while (L->First != NULL) {
            element = L->First;
            L->First = element->rptr;

            if (element) {
                if (element->token.type == DOLLAR || element->token.type == SHIFT || element->token.type == E ){
                    if (element->token.data) {
                        free(element->token.data);
                    }
                    free(element);
                } else if (element->token.type == I) {
                    free(element);
                }
            }
        }
    }

    L->Last = NULL;
    L->Act = NULL;
}

/**
 * Div by zero const control
 * 
 * @param elem pointer to token list element
 * @return 0 successful control
 * @return 1 dividion by zero const accrued
 */
int div_by_zero_check(tDLElemPtr elem) {
    char* intz = "0\0";
    char* float1z = "0.0\0";
    
    if ((strcmp(intz, elem->token.data) == 0 && elem->lptr->token.type == DIV) ||
        (strcmp(float1z, elem->token.data) == 0 && elem->lptr->token.type == DIV)) {
        return 1;
    }
    return 0;
}

/**
 * Expression token list creator
 * 
 * @param Elist pointer to token list
 * @param types_list pointer to Data types list
 * @param exp_t expression type (exp, func, etc)
 * @param depth expression depth
 * @return 0 If the expression was processed successfully
 * @return 2 Expression syntax error
 * @return 3 Expression undefind variable error
 * @return 5 Expression data types error
 * @return 9 Expression division by zero constant error
 * @return 99 Internal error (malloc, etc)
 */
unsigned exp_list_ctor(tDLList *EList, tDLList *types_list, unsigned *exp_t, unsigned depth) {
    exp_type = 0;
    unsigned err = 0;
    unsigned n_of_var = 0;
    Sym_table_item *item;

    while (token_list.Act != NULL) {
        // fprintf(stderr,  "type= %-4d   data= %-4s  d_size= %-4d\n", token_list.Act->token.type, token_list.Act->token.data, token_list.Act->token.data_size);
        if (token_list.Act->token.type == END_OF_LINE ||
            token_list.Act->token.type == CURLY_BR_L  ||
            token_list.Act->token.type == SEMICOLON   ||
            token_list.Act->token.type == COMMA) {
            break;
        } else if (token_list.Act->rptr->token.type == END_OF_LINE &&
                          token_list.Act->token.type == ROUND_BR_R && 
                          *exp_t == FUNC) {
            break;
        }

        switch (token_list.Act->token.type) {
            case ID:
                item = sym_table_search_item(&sym_table, token_list.Act->token.data, depth);
                if (item == NULL) {
                    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                    err = SEMANTIC_UNDEFINED_VAR_ERROR;
                    break;
                } else {
                    if (strcmp(token_list.Act->token.data, "_\0") == 0) {
                        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                        err = SEMANTIC_DATA_TYPES_ERROR;
                    } else {
                        exp_type = item->value.var.type;
                        DLInsertLast(EList, &(token_list.Act->token));
                        EList->Last->token.type = exp_type;
                    }
                }
                break;
            case STR_END:
                if (token_list.Act->rptr->token.type == DIV ||
                    token_list.Act->rptr->token.type == SUB ||
                    token_list.Act->rptr->token.type == MUL) {
                    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                    err = SEMANTIC_DATA_TYPES_ERROR;
                }
                exp_type = token_list.Act->token.type;
                DLInsertLast(EList, &(token_list.Act->token));
                break;
            case INTEGER:
            case FLOAT:
                if (div_by_zero_check(token_list.Act)) {
                    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                    err = SEMANTIC_ZERO_DIVIDION_ERROR;
                }
                exp_type = token_list.Act->token.type;
                DLInsertLast(EList, &(token_list.Act->token));
                break;
            case BOOLEAN:
                exp_type = token_list.Act->token.type;
                DLInsertLast(EList, &(token_list.Act->token));
                break;
            default:
                DLInsertLast(EList, &(token_list.Act->token));
                break;
        };
        token_list.Act = token_list.Act->rptr;
    }

    Token token_exp_type;
    token_exp_type.type = exp_type;
    token_exp_type.data = NULL;
    token_exp_type.data_size = 0;
    DLInsertLast(types_list, &token_exp_type);

    Token token;
    token.type = DOLLAR;
    token.data = NULL;
    token.data_size = 0;
    DLInsertLast(EList, &token);

    EList->Act = EList->First;
    while (EList->Act != NULL) {
        switch (EList->Act->token.type) {
            case ADD:
            case SUB:
            case MUL:
            case DIV:
            case L:
            case LE:
            case G:
            case GE:
            case EQ:
            case NOT_EQ:
                ops+=1;
                break;
            case EQ_SYM:
            case DECLARE:
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                err = SYNTAX_ERROR;
                break;
            case ID:
            case STR_END:
            case INTEGER:
            case FLOAT:
            case BOOLEAN:
                n_of_var+=1;
                if (EList->Act->token.type != exp_type) {
                    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                    err = SEMANTIC_DATA_TYPES_ERROR;
                }
                EList->Act->token.type = I;
                break;
        };
        EList->Act = EList->Act->rptr; 
    }
    if ((*exp_t == IF || *exp_t == FOR) && ops == 0 && n_of_var > 0) {
        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return SEMANTIC_DATA_TYPES_ERROR;
    }
    
    return err;
}

/**
 * Find first terminal token
 * 
 * @param L pointer to precedental stack
 * @return pointer to nonterminal item
 */
tDLElemPtr get_term(tDLList *L){
    tDLElemPtr non_term = L->First;
    while(non_term != NULL){
        if (non_term->token.type <= DOLLAR) {
            break;
        } else {
            non_term = non_term->rptr;
        }
    }
   return non_term;
}

/**
 * Count nonterminal items at list
 * 
 * @param L pointer to precedental stack
 * @return number of nonterminal items
 */
int count_active_non_terms(tDLList *L){
    tDLElemPtr non_term = L->First;
    int count = 0;
    while(non_term != NULL){
        if (non_term->token.type == SHIFT) {
            break;
        } else {
            count++;
            non_term = non_term->rptr;
        }
    }
   return count;
}

/**
 * Reduce rule driven by precedent table
 * 
 * @param stack pointer to token stack
 * @param stack_size number of stack elements
 * @param exp_t expression type (exp, func, etc)
 * @param depth expression depth
 * @return 0 If the expression was processed successfully
 * @return 2 Expression syntax error
 * @return 5 Expression data types error
 */
int reduce(tDLList *stack, int *stack_size, unsigned* depth, unsigned* func_call_num) {
    tDLElemPtr s_token = get_term(stack);
    switch(s_token->token.type) {
        case I:       // EXPR -> i
            if (s_token->rptr->token.type == SHIFT && *stack_size >= 3) {
                if (*func_call_num > 1) {
                    generate_push_by_type(s_token, depth);
                }
                insert_E(stack);
                *stack_size-=1;
            } else {
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                return SYNTAX_ERROR;
            }
            break;
        case SUB:     // EXPR -> EXPR - EXPR
            if (count_active_non_terms(stack) == 3) {
                if (s_token->lptr->token.type == E && s_token->rptr->token.type == E) {
                    *stack_size-=3;
                    if (*func_call_num > 1) {
                        arithm_template(s_token);
                    }
                    pop(stack);
                    pop(stack);
                    pop(stack);
                    pop(stack);
                    insert_first_by_type(stack, E);
                } else {
                    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                    return SYNTAX_ERROR;
                }
            } else if (count_active_non_terms(stack) == 2 && s_token->lptr != NULL) {
                if (s_token->lptr->token.type == E && s_token->rptr->token.type == SHIFT) {
                    if (*func_call_num > 1) {
                        fprintf(stdout, "POPS GF@&div_zero_number\n");
                        if (exp_type == INTEGER) {
                            fprintf(stdout, "PUSHS int@%d\n", 0);
                        } else if (exp_type == FLOAT) {
                            fprintf(stdout, "PUSHS float@%a\n", 0.0);
                            fprintf(stderr, "float@ (%f)\n", 0.0);
                        }
                        generate_push_by_type(s_token->lptr, depth);
                        fprintf(stdout, "SUBS\n");
                    }
                    pop(stack);
                    pop(stack);
                    pop(stack);
                    insert_first_by_type(stack, E);
                    *stack_size-=2;
                } else {
                    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                    return SYNTAX_ERROR;
                }
            } else {
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                return SYNTAX_ERROR;
            }
            break;
        case ADD:     // EXPR -> EXPR + EXPR
        case MUL:     // EXPR -> EXPR * EXPR
        case DIV:     // EXPR -> EXPR / EXPR
        case AND:
        case OR:
            if (count_active_non_terms(stack) == 3) {
                if (s_token->lptr->token.type == E && s_token->rptr->token.type == E) {
                    *stack_size-=3;
                    if (*func_call_num > 1) {
                        arithm_template(s_token);
                    }
                    pop(stack);
                    pop(stack);
                    pop(stack);
                    pop(stack);
                    insert_first_by_type(stack, E);
                } else {
                                    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

                    return SYNTAX_ERROR;
                }
            } else {
                                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

                return SYNTAX_ERROR;
            }
            break;
        case L:       // EXPR -> EXPR < EXPR
        case LE:      // EXPR -> EXPR <= EXPR
        case G:       // EXPR -> EXPR > EXPR
        case GE:      // EXPR -> EXPR >= EXPR
        case EQ:      // EXPR -> EXPR == EXPR
        case NOT_EQ:  // EXPR -> EXPR != EXPR
            if (count_active_non_terms(stack) == 3 && (ops - done_ops == 1)) {
                if (s_token->lptr->token.type == E && s_token->rptr->token.type == E) {
                    if (*func_call_num > 1) {
                        logic_template(s_token);
                    }
                    *stack_size-=3;
                    pop(stack);
                    pop(stack);
                    pop(stack);
                    pop(stack);
                    insert_first_by_type(stack, E);
                    done_ops+=1;
                } else {
                                    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

                    return SYNTAX_ERROR;
                }
            } else {
                return SEMANTIC_DATA_TYPES_ERROR;
            }
            break;
        case ROUND_BR_R: // EXPR -> (EXPR)
            if (*stack_size >= 3) {
                if (stack->First->rptr->token.type == E && stack->First->rptr->rptr->token.type == ROUND_BR_L) {
                    pop(stack);
                    pop(stack);
                    pop(stack);
                    pop(stack);
                    insert_first_by_type(stack, E);
                    *stack_size-=3;
                } else {
                                    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

                    return SYNTAX_ERROR;
                }
            } else {
                                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

                return SYNTAX_ERROR;
            }
            break;
        default:
                        fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

            return SYNTAX_ERROR; 
    };
    return 0; 
}

int expression(tDLList *types_list, unsigned exp_t, unsigned depth, unsigned func_call_num) {
    ops = 0;
    done_ops = 0;
    tDLList stack;
    DLInitList(&stack);
    tDLList L;
    DLInitList(&L);
    unsigned err = exp_list_ctor(&L,types_list, &exp_t, depth);
    if (err) {
        fprintf(stderr, "ERROR IN LIST CTOR\n");
        exp_list_dtor(&stack);
        exp_list_dtor(&L);
        return err;
    }

    L.Act = L.First;

    // fprintf(stderr, "~~~~~~~~~search~~~~~~~~~~~\n");
    // print_exp_list(&L);

    insert_first_by_type(&stack, DOLLAR);

    tDLElemPtr s_token = get_term(&stack);
    int retval = 0;
    int stack_size = 1;
    unsigned prec_table_rule;
    do {
        prec_table_rule = 0;
        if (exp_type == BOOLEAN) {
            switch (L.Act->token.type) {
                case AND:
                    prec_table_rule = get_prec_table_rule(s_token->token.type, MUL);
                    fprintf(stderr, "rule[%u][%u]=%u\n", s_token->token.type, MUL, prec_table_rule);
                    break;
                case OR:
                    prec_table_rule = get_prec_table_rule(s_token->token.type, ADD);
                    fprintf(stderr, "rule[%u][%u]=%u\n", s_token->token.type, ADD, prec_table_rule);
                    break;
                default:
                    prec_table_rule = get_prec_table_rule(s_token->token.type, L.Act->token.type);
                    fprintf(stderr, "rule[%u][%u]=%u\n", s_token->token.type, L.Act->token.type, prec_table_rule);
                    break;
            };
        } else {
            prec_table_rule = get_prec_table_rule(s_token->token.type, L.Act->token.type);
        }

        switch (prec_table_rule) {
            case EQUAL:
                DLInsertFirst(&stack, &L.Act->token);
                L.Act = L.Act->rptr;
                stack_size++;
                break;
            case SHIFT:
                stack.Act = s_token;
                Token token;
                token.type = SHIFT;
                token.data = NULL;
                token.data_size = 0;

                DLPreInsert (&stack, &token);

                DLInsertFirst(&stack, &(L.Act->token));
                L.Act = L.Act->rptr;
                stack_size+=2;
                break;
            case REDUCE:
                retval = reduce(&stack, &stack_size, &depth, &func_call_num);
                if (retval) {
                    exp_list_dtor(&stack);
                    exp_list_dtor(&L);
                    return retval;
                }
                break;
            case NONE:
                exp_list_dtor(&stack);
                exp_list_dtor(&L);
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                return SYNTAX_ERROR;
            default:
                exp_list_dtor(&stack);
                exp_list_dtor(&L);
                fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
                return SYNTAX_ERROR;
        };
        // fprintf(stderr, "---stack items---\n");
        // print_exp_list(&stack);
        
        s_token = get_term(&stack);
    } while ((s_token->token.type != DOLLAR) || (L.Act->token.type != DOLLAR));
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    exp_type = 0;
    exp_list_dtor(&stack);
    exp_list_dtor(&L);

    return 0; 
}