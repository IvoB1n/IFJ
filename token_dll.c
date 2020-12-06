#include "token_dll.h"

int solved;
int errflg;

void DLInitList (tDLList *L) {
    L->Act = NULL;
    L->First = NULL;
    L->Last = NULL;
}

void DLDisposeList (tDLList *L) {
    tDLElemPtr element;
    if (L->First) {

        while (L->First != NULL) {
            element = L->First;
            L->First = element->rptr;

            if (element) {

                if (element->token.data != NULL) {
                    free(element->token.data);
                }
                free(element);
            }
        }
    }
    L->Last = NULL;
    L->Act = NULL;
}

void DLInsertFirst (tDLList *L, Token *token) {
    tDLElemPtr firstElement = malloc(sizeof(struct tDLElem));

    if (firstElement == NULL) {
        return;
    } else {
        firstElement->token = *token;
        firstElement->lptr = NULL;
        firstElement->rptr = L->First;
 
        if (L->First == NULL) {
            L->Last = firstElement;    
        } else {
            L->First->lptr = firstElement;
        }

        L->First = firstElement;
    }
}

void DLInsertLast(tDLList *L, Token *token) {
    tDLElemPtr lastElement = malloc(sizeof(struct tDLElem));

    if (lastElement == NULL) {
        return;
    } else {
        lastElement->token = *token;
        lastElement->lptr = L->Last;
        lastElement->rptr = NULL;

        if (L->First == NULL) {
            L->First = lastElement;    
        } else {
            L->Last->rptr = lastElement;
        }

        L->Last = lastElement;
    }
}

void DLFirst (tDLList *L) {
    L->Act = L->First;
}

void DLLast (tDLList *L) {
    L->Act = L->Last;
}

void DLDeleteLast (tDLList *L) {
    tDLElemPtr element;

    if (L->First == NULL) {
        return;
    } else {
        element = L->Last;

        if (L->Last == L->Act) {
            L->Act = NULL;
        }

        if (L->First == L->Last) {
            L->First = NULL;
            L->Last = NULL;
        } else {
            L->Last = L->Last->lptr; 
            L->Last->rptr = NULL;
        }

        if (element) {
            if (element->token.data) {
                free(element->token.data);
            }
            free(element);
        }
        
    }
}

void DLPreInsert (tDLList *L, Token *token) {
    tDLElemPtr element;

    if (L->Act == NULL) {
        return;
    } else {
        element = malloc(sizeof(struct tDLElem));

        if (element == NULL) {
            return;
        } else {
            element->token = *token;
            element->rptr = L->Act; 
            element->lptr = L->Act->lptr;
            L->Act->lptr = element;

            if (L->Act == L->First) {
                L->First = element;
            } else {
                element->lptr->rptr = element;
            }
        }
    }
}

void DLSucc (tDLList *L) {
    if (L->Act == NULL) {
        return;
    } 
    
    if (L->Act == L->Last) {
        L->Act = NULL;
    } else {
        L->Act = L->Act->rptr;
    }
}


void DLPred (tDLList *L) {
    if (L->Act == NULL) {
        return;
    } 

    if (L->Act == L->First) {
        L->Act = NULL;
    } else {
        L->Act = L->Act->lptr;
    }
}