#include "expression.h"

void DLInitStack (tDLList *token_stack) {
    token_stack->Act = NULL;
    token_stack->First = NULL;
    token_stack->Last = NULL;

    Token *dollar_token;
    dollar_token->type = DOLLAR;
    dollar_token->data_size = 1;
    dollar_token->data = '$';

    PushS(token_stack, dollar_token);
}

void DLClearStack (tDLList *token_stack) {
    tDLElemPtr element;
    while ((token_stack->First != NULL)) {
        element = token_stack->First;
        token_stack->First = element->rptr;
        free(element);
    }
    token_stack->Last = NULL;
    token_stack->Act = NULL;
}

void PushS (tDLList *token_stack, Token *token) {
    tDLElemPtr firstElement = malloc(sizeof(struct tDLElem));

    if (firstElement == NULL) {
        DLError();
    } else {
        firstElement->token = *token;
        firstElement->lptr = NULL;
        firstElement->rptr = token_stack->First;
 
        if (token_stack->First == NULL) {
            token_stack->Last = firstElement;    
        } else {
            token_stack->First->lptr = firstElement;
        }

        token_stack->First = firstElement;
    }
}

tDLElemPtr PopS (tDLList *token_stack) {   
    tDLElemPtr *token = token_stack->First;

    if (token_stack->First == NULL) {
        return;
    } else {
         tDLElemPtr element = token_stack->First;

        if (token_stack->First == token_stack->Act) {
            token_stack->Act = NULL;
        }

        if (token_stack->First == token_stack->Last) {
            token_stack->First = NULL;
            token_stack->Last = NULL;
        } else {
            token_stack->First = token_stack->First->rptr; 
            token_stack->First->lptr = NULL;
        }
        free(element);
    }

    return token;
}

char get_prec_table_value (int first_sym, int second_sym) {
    return prec_table[first_sym][second_sym];
}

