/* 
** Téma: Dvousměrně vázaný lineární seznam from IAL
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                              Úpravy: Kamil Jeřábek, září 2020
*/

/**
 * @file token_dll.h
 * 
 * Two-way bound linear list
 * 
 * @brief IFJ Projekt 2020, Tým 58
 * 
 * @author <xbobro01> Bobrov Ivan
 * @author <xcaras00> Carasec Elena
 * @author <xkozhe00> Kozhevnikov Dmitrii
 * @author <xsenic00> Senichak Yahor
 */

#include<stdio.h>
#include<stdlib.h>

#define FALSE 0
#define TRUE 1

extern int errflg;
extern int solved;

/**
 * @struct Token
 */
typedef struct {
    unsigned type;       // type of token
    char *data;          //  data of token
    unsigned data_size;  // size of token 
} Token;

/**
 * @struct tDLElemPtr
 */
typedef struct tDLElem {               // two-way bound list element
        Token token;                   // token
        struct tDLElem *lptr;          // pointer to previous list element
        struct tDLElem *rptr;          // pointer to the following list element
} *tDLElemPtr;

/**
 * @struct tDLList
 */
typedef struct {                       // two-way bound list
    tDLElemPtr First;                  // pointer to the first element of the list
    tDLElemPtr Act;                    // pointer to current list item
    tDLElemPtr Last;                   // pointer to the last element of the list
} tDLList;

tDLList token_list;

/**
 * Initializes list L before it is first used
 * 
 * @param L Pointer to list
 */
void DLInitList (tDLList *L);

/**
 * Cancels all elements of list L and returns the list to the state it was in after initialization 
 * Disturbed list elements will be correctly released by calling Operation Free
 *
 * @param L Pointer to list
 */
void DLDisposeList (tDLList *L);

/**
 * Inserts a new element at the top of the list L
 *
 * @param L Pointer to list
 * @param token Token Pointer to output token
 */
void DLInsertFirst (tDLList *L, Token *token);

/**
 * Inserts a new element at the end of the list L
 *
 * @param L Pointer to list
 * @param token Token Pointer to output token
 */
void DLInsertLast(tDLList *L, Token *token);

/**
 * Sets the activity to the first element of the list L
 *
 * @param L Pointer to list
 */
void DLFirst (tDLList *L);

/**
 * Sets the activity to the last item in the L list
 *
 * @param L Pointer to list
 */
void DLLast (tDLList *L);

/**
 * Cancels the last element of list L
 *
 * @param L Pointer to list
 */
void DLDeleteLast (tDLList *L);

/**
 * Inserts an element before the active element of the L list
 *
 * @param L Pointer to list
 * @param token Token Pointer to output token
 */
void DLPreInsert (tDLList *L, Token *token);

/**
 * Moves the activity to the next item in the list L
 *
 * @param L Pointer to list
 */
void DLSucc (tDLList *L);

/**
 * Moves the activity to the previous element of the list L
 *
 * @param L Pointer to list
 */
void DLPred (tDLList *L);