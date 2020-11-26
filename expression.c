#include "expression.h"
// #include "scanner.h"
#include "error.h"
#include "symtable.h"
// #include "template.h"
#include <string.h>

int ops;
int done_ops;

#define PREC_TABLE_SIZE 14

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

void arithm_template(tDLElemPtr s_token, int* stack_size) {
    // printf("ops= %d  done_ops= %d stack_size= %d\n", ops, done_ops, *stack_size);
    if (s_token->lptr->token.data == NULL) {
        if (*stack_size != 2 || (*stack_size == 2 && done_ops == 0)) {
            fprintf(stdout, "PUSHS %s\n", s_token->rptr->token.data);
        }
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
                fprintf(stdout, "DIVS\n");
                break;
        };
        done_ops+=1;
    } else {
        if (*stack_size != 2 || (*stack_size == 2 && done_ops == 0)) {
            fprintf(stdout, "PUSHS int@%s\n", s_token->lptr->token.data);
        }        switch(s_token->token.type) {
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
                fprintf(stdout, "DIVS\n");
                break;
        };
        done_ops+=1;
    }
}

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

unsigned get_prec_table_rule(unsigned sym1, unsigned sym2) {
    return prec_table[sym1][sym2];
}

void print_exp_list(tDLList *L) {
    tDLElemPtr tmp = L->First;
    printf("---------------\n");
    while (tmp != NULL) {
        printf( "type= %-4d   data= %-4s\n", tmp->token.type, tmp->token.data);
        tmp = tmp->rptr;
    }
    printf("---------------\n");
}

void insert_first_by_type(tDLList *L, unsigned type){
    Token token;

    token.type = type;
    token.data = NULL;
    token.data_size = 0;

    DLInsertFirst(L, &token);
}

void insert_E(tDLList *L){    
    Token token;
    token.type = E;

    token.data = malloc(sizeof(char) * L->First->token.data_size);
    strcpy(token.data, L->First->token.data);
    token.data_size = 0;

    DLDeleteFirst (L);
    DLDeleteFirst (L);
    DLInsertFirst(L, &token);
}

void exp_list_dtor(tDLList *EList, tDLList *stack) {
    // printf("%s %d\n", __FILE__, __LINE__);
    DLDisposeList(EList);
    // printf("%s %d\n", __FILE__, __LINE__);
    DLDisposeList(stack);
    // printf("%s %d\n", __FILE__, __LINE__);
}

int div_by_zero_check(tDLElemPtr elem) {
    char* intz = "0\0";
    char* float1z = "0.0\0";
    
    if ((strcmp(intz, elem->token.data) == 0 && elem->lptr->token.type == DIV) ||
        (strcmp(float1z, elem->token.data) == 0 && elem->lptr->token.type == DIV)) {
        return 1;
    }
    return 0;
}

unsigned exp_list_ctor(tDLList *EList, tDLList *token_list, tDLList *types_list, unsigned *exp_t, unsigned depth) {
    unsigned exp_type = 0;
    unsigned err = 0;
    Sym_table_item *item;

    while (token_list->Act != NULL) {
        if (token_list->Act->token.type == END_OF_LINE ||
             token_list->Act->token.type == CURLY_BR_L ||
              token_list->Act->token.type == SEMICOLON ||
                token_list->Act->token.type == COMMA) {
            break;
        } else if (token_list->Act->rptr->token.type == END_OF_LINE &&
                          token_list->Act->token.type == ROUND_BR_R && 
                                          *exp_t == FUNC) {
            break;
        }

        switch (token_list->Act->token.type) {
            case ID:
                item = sym_table_search_item(&sym_table, token_list->Act->token.data, depth);
                if (item == NULL) {
                    // printf("%s %d\n", __FILE__, __LINE__);
                    err = SEMANTIC_UNDEFINED_VAR_ERROR;
                    break;
                } else {
                    if (strcmp(token_list->Act->token.data, "_\0") == 0) {
                        err = SEMANTIC_OTHER_ERRORS;
                    } else {
                        exp_type = item->value.var.type;
                        DLInsertLast(EList, &(token_list->Act->token));
                        EList->Last->token.type = exp_type;
                    }
                }
                break;
            case STR_END:
                if (token_list->Act->rptr->token.type == DIV ||
                    token_list->Act->rptr->token.type == SUB ||
                    token_list->Act->rptr->token.type == MUL) {
                    err = SEMANTIC_DATA_TYPES_ERROR;
                }
                exp_type = token_list->Act->token.type;
                DLInsertLast(EList, &(token_list->Act->token));
                break;
            case INTEGER:
            case FLOAT:
                if (div_by_zero_check(token_list->Act)) {
                    // printf("%s %d\n", __FILE__, __LINE__);
                    err = SEMANTIC_ZERO_DIVIDION_ERROR;
                }
                exp_type = token_list->Act->token.type;
                DLInsertLast(EList, &(token_list->Act->token));
                break;
            default:
                DLInsertLast(EList, &(token_list->Act->token));
                break;
        };
        // fprintf(stdout, "searched type= %u\n", exp_type);

        // DLInsertLast(EList, &(token_list->Act->token));
        token_list->Act = token_list->Act->rptr;
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
                err = SYNTAX_ERROR;
                break;
            case ID:
            case STR_END:
            case INTEGER:
            case FLOAT:
                if (EList->Act->token.type != exp_type) {
                    // printf("%s %d\n", __FILE__, __LINE__);
                    err = SEMANTIC_DATA_TYPES_ERROR;
                }
                EList->Act->token.type = I;
                break;
        };
        EList->Act = EList->Act->rptr; 
        // switch (EList->Act->token.type) {
        //     case ID:
        //         // EList->Act->token.type = exp_type
        //         break;
        //     case STR_END:
        //     case INTEGER:
        //     case FLOAT:
        //         if (EList->Act->token.type != exp_type) {
        //             return SEMANTIC_DATA_TYPES_ERROR;
        //         }
        //         EList->Act->token.type = I;
        //         EList->Act = EList->Act->rptr; 
        //         break;
        // };
    }
    return err;
}

void pop(tDLList *L){
    // Token ret_token = L->First->token;
    DLDeleteFirst (L);
} 

tDLElemPtr get_non_term(tDLList *L){
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

int reduce(tDLList *stack, int *stack_size) {
    tDLElemPtr s_token = get_non_term(stack);
    switch(s_token->token.type) {
        case I:       // EXPR -> i
            if (s_token->rptr->token.type == SHIFT && *stack_size >= 3) {
                // printf("ops= %d  done_ops= %d stack_size= %d\n", ops, done_ops, *stack_size);
                if (ops == 0) {
                    double fl = strtod(s_token->token.data, NULL);
                    fprintf(stdout, "PUSHS float@%a\n", fl);
                }
                insert_E(stack);
                *stack_size-=1;
            } else {
                return SYNTAX_ERROR;
            }
            break;
        case ADD:     // EXPR -> EXPR + EXPR
        case SUB:     // EXPR -> EXPR - EXPR
        case MUL:     // EXPR -> EXPR * EXPR
        case DIV:     // EXPR -> EXPR / EXPR
            if (count_active_non_terms(stack) == 3) { // create tmp, 
                if (s_token->lptr->token.type == E && s_token->rptr->token.type == E) {
                    if (s_token->rptr->token.data != NULL) {
                        fprintf(stdout, "PUSHS int@%s\n", s_token->rptr->token.data);
                    }
                    *stack_size-=3;
                    arithm_template(s_token, stack_size);
                    pop(stack);
                    pop(stack);
                    pop(stack);
                    pop(stack); // kostyl
                    insert_first_by_type(stack, E);
                } else {
                    return SYNTAX_ERROR;
                }
            } else if (count_active_non_terms(stack) == 2 && s_token->lptr != NULL) {
                if (s_token->lptr->token.type == E && s_token->rptr->token.type == SHIFT) {
                    fprintf(stdout, "PUSHS %s\n", "0");
                    fprintf(stdout, "PUSHS %s\n", s_token->lptr->token.data);
                    fprintf(stdout, "SUBS\n");
                    pop(stack);
                    pop(stack);
                    pop(stack); // kostyl
                    insert_first_by_type(stack, E);
                    *stack_size-=2;
                } else {
                    return SYNTAX_ERROR;
                }
            } else {
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
                    // printf("ops= %d  done_ops= %d stack_size= %d\n", ops, done_ops, *stack_size);
                    
                    if (s_token->rptr->token.data != NULL) {
                        fprintf(stdout, "PUSHS %s\n", s_token->rptr->token.data);
                        fprintf(stdout, "PUSHS %s\n", s_token->lptr->token.data);
                    }
                    // fprintf(stdout, "DEFVAR LF@cond_res\n");
                    // fprintf(stdout, "DEFVAR LF@%%retval%u\n", 1);
                    logic_template(s_token);
                    *stack_size-=3;
                    // arithm_template(s_token, stack_size);
                    pop(stack);
                    pop(stack);
                    pop(stack);
                    pop(stack); // kostyl
                    insert_first_by_type(stack, E);
                    done_ops+=1;
                } else {
                    return SYNTAX_ERROR;
                }
            } else {
                // printf ("AAAAAAAAAAAAAAAAAAA\n");
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
                    return SYNTAX_ERROR;
                }
            } else {
                return SYNTAX_ERROR;
            }
            break;
        default:
            return SYNTAX_ERROR; 
    };
    return 0; 
}

int expression(tDLList *types_list, unsigned exp_t, unsigned depth) {
    ops = 0;
    done_ops = 0;
    // printf("~~~~~~~~~~~~~~~~~search~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    tDLList stack;
    DLInitList(&stack);
    tDLList L;
    DLInitList(&L);
    unsigned err = exp_list_ctor(&L, &token_list, types_list, &exp_t, depth);
    if (err) {
        fprintf(stderr, "ERROR IN LIST CTOR\n");
        exp_list_dtor(&stack, &L);
        return err;
    }

    L.Act = L.First;

    insert_first_by_type(&stack, DOLLAR);

    // printf("PrintExpList!!1!\n");
    // print_exp_list(&L);
    // print_exp_list(&stack);

    tDLElemPtr s_token = get_non_term(&stack);
    int retval = 0;
    int stack_size = 1;
    do {
        // printf("%s %d\n", __FILE__, __LINE__);

        // printf("--start in= %d neterm= %d\n", L.Act->token.type, s_token->token.type);
        // print_exp_list(&L);
        // print_exp_list(&stack);

        unsigned prec_table_rule = get_prec_table_rule(s_token->token.type, L.Act->token.type);

        switch (prec_table_rule) {
            case EQUAL:
                DLInsertFirst(&stack, &L.Act->token);
                L.Act = L.Act->rptr;
                stack_size++;
                break;
            case SHIFT:
                stack.Act = s_token;
                // insert_first_by_type(&stack, SHIFT);
                Token token;
                token.type = SHIFT;
                token.data = NULL;
                token.data_size = 0;

                DLPreInsert (&stack, &token);

                DLInsertFirst(&stack, &L.Act->token);
                L.Act = L.Act->rptr;
                stack_size+=2;
                break;
            case REDUCE:
                retval = reduce(&stack, &stack_size);
                if (retval) {
                    exp_list_dtor(&stack, &L);
                    // printf("%s %d\n", __FILE__, __LINE__);
                    return retval;
                }
                break;
            case NONE:
                exp_list_dtor(&stack, &L);
                // printf("%s %d\n", __FILE__, __LINE__);
                return SYNTAX_ERROR;
            default:
                exp_list_dtor(&stack, &L);
                // printf("%s %d\n", __FILE__, __LINE__);
                return SYNTAX_ERROR;
        };
        // printf("--end-- next= %d\n", L.Act->token.type);
        // printf("---stack items---\n");
        // print_exp_list(&stack);
        
        s_token = get_non_term(&stack);
        // printf("stack.F= %d  L.Act= %d\n", stack.First->token.type, L.Act->token.type);
    } while ((s_token->token.type != DOLLAR) || (L.Act->token.type != DOLLAR));
    // fprintf(stdout, "PUSH LF@res\n");
    // printf("%s %d\n", __FILE__, __LINE__);
    exp_list_dtor(&stack, &L);

    return 0; 
}