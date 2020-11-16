#include "token_dll.h"

int solved;
int errflg;

void DLError() {

    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;
    return;
}

void DLInitList (tDLList *L) {
    L->Act = NULL;
    L->First = NULL;
    L->Last = NULL;
}

void DLDisposeList (tDLList *L) {
    tDLElemPtr element;

    while ((L->First != NULL)) {
        element = L->First;
        L->First = element->rptr;
        free(element);
    }

    L->Last = NULL;
    L->Act = NULL;
}

void DLInsertFirst (tDLList *L, Token *token) {
    tDLElemPtr firstElement = malloc(sizeof(struct tDLElem));

    if (firstElement == NULL) {
        DLError();
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
        DLError();
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

void DLCopyFirst (tDLList *L, Token *token) {
    if (L->First == NULL) {
        DLError();
        return;
    } else {    
        *token = L->First->token;
    }
}

void DLCopyLast (tDLList *L, Token *token) {
    if (L->First == NULL) {
        DLError();
        return;
    } else {    
        *token = L->Last->token;
    }
}

void DLDeleteFirst (tDLList *L) {
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

        free(element);
    }
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

        free(element);
    }
}

void DLPostDelete (tDLList *L) {
    tDLElemPtr element;

	if ((L->Act == NULL) || (L->Act->rptr == NULL)) {
        return;
    } else {

        element = L->Act->rptr;
        L->Act->rptr = element->rptr;
        
        if (element == L->Last) {
            L->Last = L->Act;
        } else {
            element->rptr->lptr = L->Act;
        }

        free(element);
    }
}

void DLPreDelete (tDLList *L) {
    tDLElemPtr element;

	if ((L->Act == NULL) || (L->Act->lptr == NULL)) {
        return;
    } else {

        element = L->Act->lptr;
        L->Act->lptr = element->lptr;
        
        if (element == L->First) {
            L->First = L->Act;
        } else {
            element->lptr->rptr = L->Act;
        }

        free(element);
    }
}

void DLPostInsert (tDLList *L, Token *token) {
    tDLElemPtr element;

    if (L->Act == NULL) {
        return;
    } else {

        element = malloc(sizeof(struct tDLElem));

        if (element == NULL) {
            DLError();
            return;
        } else {

            element->token = *token;
            element->lptr = L->Act; 
            element->rptr = L->Act->rptr;
            L->Act->rptr = element;

            if (L->Act == L->Last) {
                L->Last = element;
            } else {
                element->rptr->lptr = element;
            }
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
            DLError();
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

void DLCopy (tDLList *L, Token *token) {
    if (L->Act == NULL) {
        DLError();
        return;
    } else {
        *token = L->Act->token;
    }
}

void DLActualize (tDLList *L, Token *token) {
    if (L->Act == NULL) {
        return;
    } else {
        L->Act->token = *token;
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

int DLActive (tDLList *L) {
    return L->Act != NULL;
}