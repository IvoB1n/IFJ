#include <stdlib.h>
#include <string.h>
#include "symtable.h"

unsigned hash_code(char *key)
{
    unsigned hashval;

    for (hashval = 0; *key != '\0'; key++)
        hashval = key + 31 * hashval;
    return hashval % SYM_TABLE_SIZE;
}

void init_sym_table(Sym_table *table) {
    if (table == NULL) {
        return;
    }

    for (int i = 0; i < SYM_TABLE_SIZE; i++) {
        (*table)[i] = NULL;
    }
}


void free_sym_table(Sym_table *table) {
    if (table == NULL) {
        return;
    }

    Sym_table_item *item;
    Sym_table_item *item_for_delete;
    
    for (int i = 0; i < SYM_TABLE_SIZE; i++) {
        item = (*table)[i];

        while (item != NULL) {
            item_for_delete = item;
            item = item_for_delete->nextPtr;
            free(item_for_delete);
        }

        (*table)[i] = NULL;
    }
}

void insert_item(Sym_table *table, char *key, Data data) {
    Sym_table_item *item = search_item(table, key);

    if (item != NULL) {
        item->data = data;
        return;
    } else {
        Sym_table_item *new_item = malloc(sizeof(struct Sym_table_item));

        if (new_item == NULL) {
            fprintf("Bad malloc\n");
            return;
        }
        new_item->key = key;
        new_item->data = data;
        int index = hash_code(key);
        new_item->nextPtr = (*table)[index];

        (*table)[index] = new_item;
    }

}


void delete_item(Sym_table *table, char key) {
    int index = hash_code(key);
    Sym_table_item *item_for_delete = (*table)[index];

    if (item_for_delete == NULL) {
        return;
    }

    Sym_table_item *previous_item = NULL;
    Sym_table_item *next_item = NULL;

    while (item_for_delete != NULL) {
        next_item = item_for_delete->nextPtr;

        if (strcmp(key, item_for_delete->key) == 0) {
            free(item_for_delete);

            if (previous_item == NULL) {
                (*table)[index] = next_item;
                return;
            } else {
                previous_item->nextPtr = next_item;
                return;
            }
        }

        previous_item = item_for_delete;
        item_for_delete = item_for_delete->nextPtr;
    }
}

Sym_table_item *search_item(Sym_table *table, char *key) {
    if (table == NULL || key == NULL) {
        return NULL;
    }

    int index = hash_code(key);
    Sym_table_item *searching_item = (*table)[index];

    while (searching_item != NULL) {
        if (strcmp(key, searching_item->key) == 0) {
            return searching_item;
        }

        searching_item = searching_item->nextPtr;
    }
}
