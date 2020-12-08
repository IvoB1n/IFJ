/**
 * @file symtable.h
 * 
 * Symbol table interface
 * 
 * @brief IFJ Projekt 2020, Tým 58
 * 
 * @author <xbobro01> Bobrov Ivan
 * @author <xcaras00> Carasec Elena
 * @author <xkozhe00> Kozhevnikov Dmitrii
 * @author <xsenic00> Senichak Yahor
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SYM_TABLE_SIZE 606 // Symbol table size
#define INDEX_SIZE 101      // Index level size

struct Sym_table;

/**
 * @union Data
 */
union Data {
    struct variable {
        unsigned type;
    } var;
    struct function {
        unsigned *in_var_list;
        unsigned num_in_var;
        unsigned *out_var_list;
        unsigned num_out_var;
    } func;
}; 

/**
 * @struct Symbol table item
 */
typedef struct Sym_table_item {
    char *name;                         // name of the table item
    unsigned depth;                     // the depth at which the item is located
    union Data value;                   // data that the item contains
    struct Sym_table_item *nextPtr;     // pointer to the next table item
} Sym_table_item;

// Symbol table
typedef Sym_table_item *Sym_table[SYM_TABLE_SIZE];

Sym_table sym_table, asm_table;


/**
 * Checking whether an item is present in the symbol table
 * 
 * @param table Pointer to table
 * @param name Identifier of function or variable
 * @param depth The depth at which the item is located
 * @return NULL if the item is not found in the table
 * @return Item if the item is found in the table
 */
Sym_table_item *init_search_item(Sym_table *table, char *name, unsigned depth);

/**
 * Writes out the items in the symbol table
 * 
 * @param table Pointer to table
 * @return 0 If all items are successfully written out
 * @return 99 If the symbol table is not initialized
 */
int print_sym_table_items (Sym_table *table);

/**
 * Calculating the hash
 * 
 * @param name Identifier of function or variable
 * @return Hash for the item
 */
unsigned hash_code(char *name);

/**
 * Initialisation of symbol table
 * 
 * @param table Pointer to table
 */
void sym_table_init(Sym_table *table);

/**
 * Appends item to symbol table of
 * 
 * @param table Pointer to table
 * @param node Pointer to a table item
 * @return 0 If the item was added successfully
 * @return 3 if table wasn't declared
 */
int sym_table_insert_item(Sym_table *table, Sym_table_item *node);

/**
 * Clears the symbol table level at a certain depth
 * 
 * @param table Pointer to table
 * @param depth The depth at which the item is located
 * @return 0 If the items were deleted successfully
 * @return 3 if table wasn't declared
 */
void sym_table_delete_on_depth(Sym_table *table, unsigned depth);

/**
 * Deletes an item in the symbol table
 * 
 * @param table Pointer to table
 * @param name Pointer to identifier of function or variable
 * @param depth The depth at which the item is located
 */
void sym_table_delete_item(Sym_table *table, char* name, unsigned depth);

/**
 * Searches for an item in the symbol table
 * 
 * @param table Pointer to table
 * @param name Pointer to identifier of function or variable
 * @param depth The depth at which the item is located
 * @returns Item, if found in the table
 * @returns NULL, if not found in the table
 */
Sym_table_item *sym_table_search_item(Sym_table *table, char *name, unsigned depth);

/**
 * Help function for searching item in the symbol table on null level
 * 
 * @param searching_item A pointer to the item that we want to find
 * @param name Pointer to identifier of function or variable
 * @param depth The depth at which the item is located
 * @param table Pointer to table
 * @returns Item, if found in the table
 * @returns NULL, if not found in the table
 */
Sym_table_item *search_on_null_lvl(Sym_table_item *searching_item, char *name, unsigned depth, Sym_table *table);

/**
 * Help function for searching item in the symbol table on not null level
 * 
 * @param searching_item A pointer to the item that we want to find
 * @param name Pointer to identifier of function or variable
 * @param depth The depth at which the item is located
 * @param table Pointer to table
 * @returns Item, if found in the table
 * @returns NULL, if not found in the table
 */
Sym_table_item *search_on_non_null_lvl(Sym_table_item *searching_item, char *name, unsigned depth, Sym_table *table);

/**
 * Function frees all used resources for symbol table
 * 
 * @param table Pointer to table
 */
void sym_table_clear_all(Sym_table *table);