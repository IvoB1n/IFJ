#include "expression.h"
#include "scanner.h"
#include "error.h"

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
    DLInsertFirst(L, &token);
}

void exp_list_dtor(tDLList *EList, tDLList *stack) {
    DLDisposeList(EList);
    DLDisposeList(stack);
}

Token pop(tDLList *L){
    Token ret_token = L->First->token;
    DLDeleteFirst (L);

    return ret_token;
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

int reduce(tDLList *stack, int stack_size) {
    tDLElemPtr s_token = get_non_term(stack);
    switch(s_token->token.type) {
        case I:       // EXPR -> i
            if (s_token->rptr->token.type == SHIFT) {
                pop(stack);
                pop(stack);
                insert_first_by_type(stack, E);
                stack_size--;
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
            if (stack_size >= 3) {
                if (stack->First->token.type == E && stack->First->rptr->rptr->token.type == E) {
                    pop(stack);
                    pop(stack);
                    pop(stack);
                    insert_first_by_type(stack, E);
                    stack_size-=2;
                } else {
                    return SYNTAX_ERROR;
                }
            } else {
                return SYNTAX_ERROR;
            }
            break;
        case ROUND_BR_R: // EXPR -> (EXPR)
            if (stack_size >= 3) {
                if (stack->First->rptr->token.type == E && stack->First->rptr->rptr->token.type == ROUND_BR_L) {
                    pop(stack);
                    pop(stack);
                    pop(stack);
                    insert_first_by_type(stack, E);
                    stack_size-=2;
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

void exp_list_ctor(tDLList *EList, tDLList *L) {
    while (L->Act != NULL) {
        if (L->Act->token.type == END_OF_LINE || L->Act->token.type == SEMICOLON || L->Act->token.type == COMMA) {
            break;
        }
        switch (L->Act->token.type) {
            case ID:
            case INTEGER:
            case FLOAT:
            case STRING:
            L->Act->token.type = I;
            break;
        };
        DLInsertLast(EList, &(L->Act->token));
        L->Act = L->Act->rptr;
    }
    Token token;
    token.type = DOLLAR;
    DLInsertLast(EList, &token);
}

int expression() {
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    tDLList stack;
    DLInitList(&stack);
    tDLList L;
    DLInitList(&L);
    exp_list_ctor(&L, &token_list);
    L.Act = L.First;

    insert_first_by_type(&stack, DOLLAR);

    // print_exp_list(&L);
    // print_exp_list(&stack);
    

    tDLElemPtr s_token = get_non_term(&stack);

    int stack_size = 1;
    do {
        printf("--start--\n");
        // print_exp_list(&L);
        print_exp_list(&stack);

        s_token = get_non_term(&stack);

        unsigned prec_table_rule = get_prec_table_rule(s_token->token.type, L.Act->token.type);

        switch (prec_table_rule) {
            case EQUAL:
                DLInsertFirst(&stack, &L.Act->token);
                L.Act = L.Act->rptr;
                stack_size++;
                break;
            case SHIFT:
                insert_first_by_type(&stack, SHIFT);
                DLInsertFirst(&stack, &L.Act->token);
                L.Act = L.Act->rptr;
                stack_size+=2;
                break;
            case REDUCE:
                if(reduce(&stack, stack_size)) {
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
        printf("--end--\n");
        // print_exp_list(&L);
        print_exp_list(&stack);
    } while (stack.First->token.type != DOLLAR && L.Act->token.type != DOLLAR);
    
    exp_list_dtor(&stack, &L);
   
    return 0; 
}