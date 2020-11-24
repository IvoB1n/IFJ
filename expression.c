#include "expression.h"
// #include "scanner.h"
#include "error.h"
#include "symtable.h"
#include <string.h>

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


unsigned get_prec_table_rule(unsigned sym1, unsigned sym2) {
    return prec_table[sym1][sym2];
}

void print_exp_list(tDLList *L) {
    tDLElemPtr tmp = L->First;
    printf("---------------\n");
    while (tmp != NULL) {
        printf( "type= %-4d\n", tmp->token.type);
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

void exp_list_dtor(tDLList *EList, tDLList *stack) {
    printf("%s %d\n", __FILE__, __LINE__);
    DLDisposeList(EList);
    printf("%s %d\n", __FILE__, __LINE__);
    DLDisposeList(stack);
    printf("%s %d\n", __FILE__, __LINE__);
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

unsigned exp_list_ctor(tDLList *EList, tDLList *L, tDLList *types_list, unsigned *exp_t, unsigned depth) {
    unsigned exp_type = 0;
    unsigned err = 0;
    Sym_table_item *item;

    while (L->Act != NULL) {
        if (L->Act->token.type == END_OF_LINE ||
             L->Act->token.type == CURLY_BR_L ||
              L->Act->token.type == SEMICOLON ||
                L->Act->token.type == COMMA) {
            break;
        } else if (L->Act->rptr->token.type == END_OF_LINE &&
                          L->Act->token.type == ROUND_BR_R && 
                                          *exp_t == FUNC) {
            break;
        }

        switch (L->Act->token.type) {
            case ID:
                item = sym_table_search_item(&sym_table, L->Act->token.data, depth);
                if (item == NULL) {
                    printf("%s %d\n", __FILE__, __LINE__);
                    err = SEMANTIC_UNDEFINED_VAR_ERROR;
                    break;
                } else {
                    if (strcmp(L->Act->token.data, "_\0") == 0) {
                        err = SEMANTIC_OTHER_ERRORS;
                    } else {
                        exp_type = item->value.var.type;
                        DLInsertLast(EList, &(L->Act->token));
                        EList->Last->token.type = exp_type;
                    }
                }
                break;
            case STR_END:
                if (L->Act->rptr->token.type == DIV ||
                    L->Act->rptr->token.type == SUB ||
                    L->Act->rptr->token.type == MUL) {
                    err = SEMANTIC_DATA_TYPES_ERROR;
                }
                exp_type = L->Act->token.type;
                DLInsertLast(EList, &(L->Act->token));
                break;
            case INTEGER:
            case FLOAT:
                if (div_by_zero_check(L->Act)) {
                    printf("%s %d\n", __FILE__, __LINE__);
                    err = SEMANTIC_ZERO_DIVIDION_ERROR;
                }
                exp_type = L->Act->token.type;
                DLInsertLast(EList, &(L->Act->token));
                break;
            default:
                DLInsertLast(EList, &(L->Act->token));
                break;
        };
        fprintf(stdout, "searched type= %u\n", exp_type);

        // DLInsertLast(EList, &(L->Act->token));
        L->Act = L->Act->rptr;
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
            case EQ_SYM:
            case DECLARE:
                err = SYNTAX_ERROR;
                break;
            case ID:
            case STR_END:
            case INTEGER:
            case FLOAT:
                if (EList->Act->token.type != exp_type) {
                    printf("%s %d\n", __FILE__, __LINE__);
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
                pop(stack);
                pop(stack);
                insert_first_by_type(stack, E);
                *stack_size-=1;
            } else {
                return SYNTAX_ERROR;
            }
            break;
        case ADD:     // EXPR -> EXPR + EXPR
        case SUB:     // EXPR -> EXPR - EXPR
        case MUL:     // EXPR -> EXPR * EXPR
        case DIV:     // EXPR -> EXPR / EXPR
        case L:       // EXPR -> EXPR < EXPR
        case LE:      // EXPR -> EXPR <= EXPR
        case G:       // EXPR -> EXPR > EXPR
        case GE:      // EXPR -> EXPR >= EXPR
        case EQ:      // EXPR -> EXPR == EXPR
        case NOT_EQ:  // EXPR -> EXPR != EXPR
            // printf("FFUUUUUUUUUUUUUNCK CALLL");
            if (count_active_non_terms(stack) == 3) {
                if (s_token->lptr->token.type == E && s_token->rptr->token.type == E) {
                    pop(stack);
                    pop(stack);
                    pop(stack);
                    pop(stack); // kostyl
                    insert_first_by_type(stack, E);
                    *stack_size-=3;
                } else {
                    return SYNTAX_ERROR;
                }
            } else if (count_active_non_terms(stack) == 2 && s_token->lptr != NULL) {
                if (s_token->lptr->token.type == E && s_token->rptr->token.type == SHIFT) {
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
    printf("~~~~~~~~~~~~~~~~~search~~~~~~~~~~~~~~~~~~~~~~~~~\n");
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

    printf("PrintExpList!!1!\n");
    print_exp_list(&L);
    // print_exp_list(&stack);

    tDLElemPtr s_token = get_non_term(&stack);

    int stack_size = 1;
    do {
        printf("%s %d\n", __FILE__, __LINE__);

        printf("--start in= %d neterm= %d\n", L.Act->token.type, s_token->token.type);
        // print_exp_list(&L);
        print_exp_list(&stack);

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
                printf("S_SIZE %d\n", stack_size);
                // L, LE, G, GE, EQ, NOT_EQ
                // if (L.Act->token.type != DOLLAR && ((s_token->token.type == L) || (s_token->token.type == LE) || (s_token->token.type == G) || (s_token->token.type == GE) || (s_token->token.type == EQ) || (s_token->token.type == NOT_EQ))) {
                //     exp_list_dtor(&stack, &L);
                //     printf("%s %d\n", __FILE__, __LINE__);
                //     return SEMANTIC_DATA_TYPES_ERROR;
                // }
                if(reduce(&stack, &stack_size)) {
                    exp_list_dtor(&stack, &L);
                    printf("%s %d\n", __FILE__, __LINE__);
                    return SYNTAX_ERROR;
                }
                break;
            case NONE:
                exp_list_dtor(&stack, &L);
                printf("%s %d\n", __FILE__, __LINE__);
                return SYNTAX_ERROR;
            default:
                exp_list_dtor(&stack, &L);
                printf("%s %d\n", __FILE__, __LINE__);
                return SYNTAX_ERROR;
        };
        printf("--end-- next= %d\n", L.Act->token.type);
        print_exp_list(&stack);
        
        s_token = get_non_term(&stack);
        printf("stack.F= %d  L.Act= %d\n", stack.First->token.type, L.Act->token.type);
    } while ((s_token->token.type != DOLLAR) || (L.Act->token.type != DOLLAR));
    
    printf("%s %d\n", __FILE__, __LINE__);
    exp_list_dtor(&stack, &L);

    return 0; 
}