#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SYM_TABLE_SIZE 1021
struct Sym_table;
struct Sym_table_item;

// Data types
typedef enum {
    // nil data type
    NIL_TYPE,
    // integer data type
    INT_TYPE,
    // boolean data type
    BOOL_TYPE,
    // double data type
    DOUBLE_TYPE,
    // string data type
    STRING_TYPE
} Data_type;

union Data {
    void *ptr;
    int i;
    bool b;
    float f;
    char *str;
} value;  

// Symbol data
typedef struct {
    //char *key;  // ID
    bool declare;   // declare or not
    Data_type type; // data type of symbol
    Data value;
} Data;


// Symbol table item
typedef struct Sym_table_item {
    char *key;  // ID
    Data data; // data
    struct Sym_table *nextPtr; // ptr to next item
} Sym_table_item;

// Symtable
typedef Sym_table_item *Sym_table[SYM_TABLE_SIZE];
