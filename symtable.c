#include <stdlib.h>
#include <string.h>
#include "symtable.h"
#include "error.h"

Sym_table_item *search_item(Sym_table *table, char *name, unsigned depth);
unsigned hash_code(char *name)
{
    unsigned hashval;

    for (hashval = 0; *name != '\0'; name++)
        hashval = *name + 31 * hashval;
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

int insert_item(Sym_table *table, Sym_table_item *node) {
    Sym_table_item *item = search_item(table, node->name, node->depth);

    if (item != NULL) {
        return SEMANTIC_UNDEFINED_VAR_ERROR;
    } else {
        Sym_table_item *new_item = node;        
        int index = hash_code(node->name) + node->depth * 101;
        new_item->nextPtr = (*table)[index];

        (*table)[index] = new_item;
        return 0;
    }

}

void delete_on_depth(Sym_table *table, unsigned depth) {
    int index = depth * 101;
    Sym_table_item *item_for_delete = (*table)[index];
    Sym_table_item *next_item = NULL;

    if (item_for_delete == NULL) {
        return;
    }
    unsigned other_lvl = 5;
    if (depth >= 5) {
        for (unsigned i = other_lvl * INDEX_SIZE; i < SYM_TABLE_SIZE; i++) {
            if ((*table)[i]->depth == depth) {

                next_item = item_for_delete->nextPtr;

                free(item_for_delete->name);
                free(item_for_delete->value.func.in_var_list);
                free(item_for_delete->value.func.out_var_list);
                free(item_for_delete);

                printf("Deleted index - %d\n", i);
                item_for_delete = next_item;
            }
        }
    } else {
        for (unsigned i = depth * INDEX_SIZE; i < (depth + 1) * INDEX_SIZE; i++) {
            if ((*table)[i] != NULL) {
                next_item = item_for_delete->nextPtr;

                free(item_for_delete->name);
                free(item_for_delete->value.func.in_var_list);
                free(item_for_delete->value.func.out_var_list);
                free(item_for_delete);

                printf("Deleted index - %d\n", i);
                item_for_delete = next_item;
            }
        }
    }
}

void delete_item(Sym_table *table, char* name, unsigned depth) {
    unsigned index = hash_code(name);
    Sym_table_item *item_for_delete = (*table)[index];

    if (item_for_delete == NULL) {
        return;
    }

    Sym_table_item *previous_item = NULL;
    Sym_table_item *next_item = NULL;

    while (item_for_delete != NULL) {
        next_item = item_for_delete->nextPtr;

        if ((strcmp(name, item_for_delete->name) == 0) && (depth == item_for_delete->depth)) {
            free(item_for_delete->name);
            free(item_for_delete->value.func.in_var_list);
            free(item_for_delete->value.func.out_var_list);
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

Sym_table_item *search_item(Sym_table *table, char *name, unsigned depth) {
    if (table == NULL || name == NULL) {
        return NULL;
    }

    int index = hash_code(name);
    Sym_table_item *searching_item = (*table)[index];

    while (searching_item != NULL) {
        if ((strcmp(name, searching_item->name) == 0) && (depth == searching_item->depth)) {
            return searching_item;
        }

        searching_item = searching_item->nextPtr;
    }
    return NULL;
}

void sym_table_clear_all(Sym_table *table) {
	Sym_table_item *item;
	Sym_table_item *deleted_item;
	for (int i = 0; i < SYM_TABLE_SIZE; i++) {
		item = (*table)[i];

		while (item != NULL) {
			deleted_item = item;
			item = deleted_item->nextPtr;
            free(deleted_item->name);
            free(deleted_item->value.func.in_var_list);
            free(deleted_item->value.func.out_var_list);
			free(deleted_item);
		}
		(*table)[i] = NULL;
	}
}
