#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SYM_TABLE_SIZE 1021
#define INDEX_SIZE 101

struct Sym_table;
// struct Sym_table_item;
/*
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
*/
union Data {
    struct variable {
        unsigned type;
    } var;
    struct function {
        unsigned *in_var_list;
        unsigned *out_var_list;
    } func;
}; 

// Symbol table item
typedef struct Sym_table_item {
    char *name;  // name
    unsigned depth; // glubina
    union Data value; // data
    struct Sym_table_item *nextPtr; // ptr to next item
} Sym_table_item;

// Symtable
typedef Sym_table_item *Sym_table[SYM_TABLE_SIZE];

Sym_table sym_table;

void init_sym_table(Sym_table *table);
int insert_item(Sym_table *table, Sym_table_item *node);
void delete_on_depth(Sym_table *table, unsigned depth);
void delete_item(Sym_table *table, char* name, unsigned depth);
Sym_table_item *search_item(Sym_table *table, char *name, unsigned depth);
void sym_table_clear_all(Sym_table *table);
