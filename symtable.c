#include <stdlib.h>
#include <string.h>
#include "symtable.h"
#include "error.h"

Sym_table_item *sym_table_search_item(Sym_table *table, char *name, unsigned depth);
unsigned hash_code(char *name)
{
    unsigned hashval;

    for (hashval = 0; *name != '\0'; name++)
        hashval = *name + 31 * hashval;
    return hashval % SYM_TABLE_SIZE;
}

void sym_table_init(Sym_table *table) {
    if (table == NULL) {
        return;
    }

    for (int i = 0; i < SYM_TABLE_SIZE; i++) {
        (*table)[i] = NULL;
    }
}

int sym_table_insert_item(Sym_table *table, Sym_table_item *node) {
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    Sym_table_item *item = sym_table_search_item(table, node->name, node->depth);
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    if (item != NULL) {
        return SEMANTIC_UNDEFINED_VAR_ERROR;
    } else {
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        Sym_table_item *new_item = node;        
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        int index = hash_code(node->name) + node->depth * 101;
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        new_item->nextPtr = (*table)[index];
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        (*table)[index] = new_item;
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return 0;
    }

}

void sym_table_delete_on_depth(Sym_table *table, unsigned depth) {
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

                item_for_delete = next_item;
            }
        }
    }
}

void sym_table_delete_item(Sym_table *table, char* name, unsigned depth) {
    unsigned index = hash_code(name);
    Sym_table_item *item_for_delete = (*table)[index];
            //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    if (item_for_delete == NULL) {
        return;
    }
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    Sym_table_item *previous_item = NULL;
    Sym_table_item *next_item = NULL;
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    while (item_for_delete != NULL) {
        next_item = item_for_delete->nextPtr;
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        if ((strcmp(name, item_for_delete->name) == 0) && (depth == item_for_delete->depth)) {
            free(item_for_delete->name);
            free(item_for_delete->value.func.in_var_list);
            free(item_for_delete->value.func.out_var_list);
            free(item_for_delete);
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

            if (previous_item == NULL) {
                (*table)[index] = next_item;
                return;
            } else {
                previous_item->nextPtr = next_item;
                return;
            }
        }
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        previous_item = item_for_delete;
        item_for_delete = item_for_delete->nextPtr;
    }
}
Sym_table_item *search_on_null_lvl(Sym_table_item *searching_item, char *name, unsigned depth) {
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    while (searching_item != NULL) {
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        if ((strcmp(name, searching_item->name) == 0) && (depth == searching_item->depth)) {
            //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            return searching_item;
        }
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        searching_item = searching_item->nextPtr;
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    }
    return NULL;
}

Sym_table_item *search_on_non_null_lvl(Sym_table_item *searching_item, char *name, unsigned depth, Sym_table *table) {
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    while ((searching_item != NULL) || (depth != 0)) {
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        printf("%s    %s\n", name, searching_item->name);
        if (strcmp(name, searching_item->name) == 0) {
            //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            return searching_item;
        }
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        if (searching_item->depth == depth + 1) {
            //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            depth--;
            if (depth == 0) {
                return NULL;
            }
            //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            searching_item = sym_table_search_item(table, name, depth);
        }
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        searching_item = searching_item->nextPtr;
    }
    return NULL;
}

Sym_table_item *sym_table_search_item(Sym_table *table, char *name, unsigned depth) {
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    if (table == NULL || name == NULL) {
        return NULL;
    }
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    int index = hash_code(name);
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    Sym_table_item *searching_item = (*table)[index];
    if (searching_item == NULL) {
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        return NULL;
    }
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    printf("%s \n", name);
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    if (depth == 0) {
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        searching_item = search_on_null_lvl(searching_item, name, depth);
        if (searching_item) {
            return searching_item;
        } else {
            return NULL;
        }
    } else {
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        searching_item = search_on_non_null_lvl(searching_item, name, depth, table);
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        if (searching_item) {
            return searching_item;
        } else {
            return NULL;
        }
    }
}

void sym_table_clear_all(Sym_table *table) {
	Sym_table_item *item = NULL;
	Sym_table_item *deleted_item = NULL;
	for (int i = 0; i < SYM_TABLE_SIZE; i++) {
		item = (*table)[i];
		while (item != NULL) {
			deleted_item = item;
			item = deleted_item->nextPtr;

            //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            if (deleted_item->name) {
                printf("name: %s\n", deleted_item->name);
                free(deleted_item->name);
            }

            //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
         //   printf("insize: %u\n", deleted_item->value.func.num_in_var);
            if (deleted_item->value.func.num_in_var > 0) {
                for (unsigned i = 0; i < deleted_item->value.func.num_in_var; i++) {
         //           printf("in_var: %d\n", deleted_item->value.func.in_var_list[i]);
                }    
                free(deleted_item->value.func.in_var_list);
            }
            //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
              printf("outsize: %u\n", deleted_item->value.func.num_out_var);
            if (deleted_item->value.func.num_out_var > 0) {
                for (unsigned i = 0; i < deleted_item->value.func.num_out_var; i++) {
          //          printf("out_var: %d\n", deleted_item->value.func.out_var_list[i]);
                }
                free(deleted_item->value.func.out_var_list);
            }
            //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
            if (deleted_item) {
         //       //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
			    free(deleted_item);
            }
         //   //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
		}
		(*table)[i] = NULL;
	}
}
