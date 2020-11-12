#include <stdio.h>
#include <stdlib.h>
// #include "scanner.h"

#define FALSE 0
#define TRUE 1

extern int errflg;

typedef enum {
    ID, ID_FUNC,
    // NUMBERS
    INTEGER, FLOAT, ZERO_MANTISSA, EXP, EXP_SIGNED, EXP_FINAL_NUM,
    // STRING
    STR, STR_SPEC_SYM, STR_HEX_SYM, STR_HEX_DIGIT_FIRST, STR_END,
    // BRACES
    ROUND_BR_L, ROUND_BR_R, SQUARE_BR_L, SQUARE_BR_R, CURLY_BR_L, CURLY_BR_R,
    // DECLARATION
    SEMICOLON, DECLARE, 
    // COMPARISON OPERATORS
    G, GE, L, LE, EQ_SYM, EQ, NOT, NOT_EQ,
    // BOOLEAN OPERATORS
    AND_SYM, AND, OR_SYM, OR,
    // OTHER SYMBOLS
    COMMA, COLON, POINT,
    // MATH OPERATORS
    ADD, SUB, MUL, MOD, DIV,
    // COMMENTS
    COMMENT_STR, COMMENT_BLOCK, COMMENT_BLOCK_END_S, COMMENT_BLOCK_END_F,
    // RESERVED WORDS
    ELSE, FLOAT64, FOR, FUNC, IF, INT, PACKAGE, RETURN, STRING,
    // EOL
    END_OF_LINE
} Token_type;
typedef enum {
    TOKEN,
    FUNCTYPE
} NodeType;

typedef struct {
    Token_type type;   // type of token
    char *data; //  data of token
    unsigned data_size; // size of token 
} Token;

typedef struct tDLElem {                 /* prvek dvousměrně vázaného seznamu */ 
        void *data;                                            /* užitečná data */
        struct tDLElem *lptr;          /* ukazatel na předchozí prvek seznamu */
        struct tDLElem *rptr;        /* ukazatel na následující prvek seznamu */
} *tDLElemPtr;

typedef struct {                                  /* dvousměrně vázaný seznam */
    tDLElemPtr First;                      /* ukazatel na první prvek seznamu */
    tDLElemPtr Act;                     /* ukazatel na aktuální prvek seznamu */
    tDLElemPtr Last;                    /* ukazatel na posledni prvek seznamu */
} tDLList;

typedef struct funcNode {
    char *name; //  data of token
    int *ret_types;
    int *get_types; 
} *FuncNodePtr;



tDLList *token_list = NULL;

tDLList *func_list = NULL;
/* prototypy jednotlivých funkcí */

/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
void DLError();

/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
void DLInitList (tDLList *);

/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
void DLDisposeList (tDLList *);

/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
void DLInsertFirst (tDLList *, void *, unsigned);

/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
void DLInsertLast(tDLList *, void *, unsigned);

/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
void DLFirst (tDLList *);

/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
void DLLast (tDLList *);

/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
void DLCopyFirst (tDLList *, Token *);

/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
void DLCopyLast (tDLList *, Token *);

/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
void DLDeleteFirst (tDLList *);

/*
** Zruší poslední prvek seznamu L.
** Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
** Pokud byl seznam L prázdný, nic se neděje.
**/ 
void DLDeleteLast (tDLList *);

/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
void DLPostDelete (tDLList *);

/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
void DLPreDelete (tDLList *);

/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
void DLPostInsert (tDLList *, Token *);

/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
void DLPreInsert (tDLList *, Token *);

/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
void DLCopy (tDLList *, Token *);

/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
void DLActualize (tDLList *, Token *);

/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
void DLSucc (tDLList *);

/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
void DLPred (tDLList *);

/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
int DLActive (tDLList *);
