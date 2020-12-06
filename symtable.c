#include <stdlib.h>
#include <string.h>
#include "symtable.h"
#include "error.h"

Sym_table_item *sym_table_search_item(Sym_table *table, char *name, unsigned depth);
unsigned hash_code(char *name);

Sym_table_item *init_search_item(Sym_table *table, char *name, unsigned depth) {
    if (table == NULL || name == NULL) {
        return NULL;
    }
    Sym_table_item *searching_item = NULL;
    int index = hash_code(name);
    if (depth < 5) {
        searching_item = (*table)[index + depth * INDEX_SIZE];
    }
    else {
        searching_item = (*table)[index + 5 * INDEX_SIZE];
    }
    
    while (searching_item != NULL) {
        if ((strcmp(name, searching_item->name) == 0) && (depth == searching_item->depth)) {
            return searching_item;
        }
        searching_item = searching_item->nextPtr;
    }
    return NULL;
}

int print_sym_table_items (Sym_table *table) {
   if (table == NULL) {
       return INTERNAL_ERROR;
   }
    // fprintf(stderr, "//////////////////////////////  \n");

    Sym_table_item *item ;//= (*table)[0];
    //Sym_table_item *next_item = NULL;
    
    for (int i = 0; i < SYM_TABLE_SIZE; i++) { 
        item = (*table)[i]; 
        if (item == NULL) {
            continue;
        }
       // next_item = item->nextPtr;
        while(item != NULL) {
            // fprintf(stderr, "\n");
            // fprintf(stderr, "item_name - %s   item_depth - %d   item_type - %u\n", item->name, item->depth, item->value.var.type);
            // fprintf(stderr, "\n");
            item = item->nextPtr;
        }
       
    }
    // fprintf(stderr, "////////////////////////////// \n");
    return 0;
}

unsigned hash_code(char *name)
{
    unsigned hashval;

    for (hashval = 0; *name != '\0'; name++)
        hashval = *name + 31 * hashval;
    return hashval % INDEX_SIZE;
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
    Sym_table_item *item = init_search_item(table, node->name, node->depth);

    if (item != NULL) {
        return SEMANTIC_UNDEFINED_VAR_ERROR;
    } else {
        Sym_table_item *new_item = node;        
        int index = hash_code(node->name);

        if (node->depth < 5){
            new_item->nextPtr = (*table)[index + node->depth * INDEX_SIZE];
            (*table)[index + node->depth * INDEX_SIZE] = new_item;
        }
        else {
            new_item->nextPtr = (*table)[index + 5 * INDEX_SIZE];
            (*table)[index + 5 * INDEX_SIZE] = new_item; 
        }
        return 0;
    }
}

void sym_table_delete_on_depth(Sym_table *table, unsigned depth) {
    int index = depth * INDEX_SIZE;
    Sym_table_item *item_for_delete = NULL;
    Sym_table_item *next_item = NULL;
    unsigned other_lvl = 5;

    if (depth >= 5) {
        for (unsigned i = other_lvl * INDEX_SIZE; i < SYM_TABLE_SIZE; i++) {
            if ((*table)[i] != NULL) {
                item_for_delete = (*table)[i];
    
                while (item_for_delete != NULL) {
                    next_item = item_for_delete->nextPtr;
                    
                    if (item_for_delete->depth == depth) {
                        if (item_for_delete->name) {
                            free(item_for_delete->name);
                        }
                        if (item_for_delete->value.func.num_in_var > 0) {
                            free(item_for_delete->value.func.in_var_list);
                        }
                        if (item_for_delete->value.func.num_out_var > 0) {
                            free(item_for_delete->value.func.out_var_list);
                        }
                        free(item_for_delete);
                        
                    }
                    else {
                        break;
                    }
                    
                    item_for_delete = next_item;
                    (*table)[i] = next_item;
                }
            }
        }
    } else {
        for (unsigned i = index; i < (depth + 1) * INDEX_SIZE; i++) {

            if ((*table)[i] != NULL) {
                item_for_delete = (*table)[i];

                while (item_for_delete != NULL) {
                    next_item = item_for_delete->nextPtr;

                    if (item_for_delete->name) {
                        free(item_for_delete->name);
                    }

                    if (depth == 0) {
                        if (item_for_delete->value.func.num_in_var > 0) {
                            free(item_for_delete->value.func.in_var_list);
                        }

                        if (item_for_delete->value.func.num_out_var > 0) {
                            free(item_for_delete->value.func.out_var_list);
                        }
                    }
                    free(item_for_delete);
                    item_for_delete = next_item;
                }
                (*table)[i] = NULL;
            }
        }
    }
}

void sym_table_delete_item(Sym_table *table, char* name, unsigned depth) {
    unsigned index = hash_code(name);
    Sym_table_item *item_for_delete = (*table)[index + depth * INDEX_SIZE];

    if (item_for_delete == NULL) {
        return;
    }

    Sym_table_item *previous_item = NULL;
    Sym_table_item *next_item = NULL;

    while (item_for_delete != NULL) {
        next_item = item_for_delete->nextPtr;

        if ((strcmp(name, item_for_delete->name) == 0) && (depth == item_for_delete->depth)) {
            if (item_for_delete) {

                if (item_for_delete->name) {
                    free(item_for_delete->name);
                }

                if (item_for_delete->value.func.num_in_var > 0) {
                    free(item_for_delete->value.func.in_var_list);
                }

                if (item_for_delete->value.func.num_out_var > 0) {
                    free(item_for_delete->value.func.out_var_list);
                }
                free(item_for_delete);
            }

            if (previous_item == NULL) {
                (*table)[index + depth * INDEX_SIZE] = next_item;
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


    
Sym_table_item *search_on_null_lvl(Sym_table_item *searching_item, char *name, unsigned depth, Sym_table *table) {
    int index = hash_code(name); 
    searching_item = (*table)[index + depth * INDEX_SIZE];  

    while (searching_item != NULL) {

        if ((strcmp(name, searching_item->name) == 0) && (depth == searching_item->depth)) {
            return searching_item;
        }
        searching_item = searching_item->nextPtr;
    }
    return NULL;
}

Sym_table_item *search_on_non_null_lvl(Sym_table_item *searching_item, char *name, unsigned depth, Sym_table *table) {
    int index = hash_code(name);
    while (depth > 0) {

        if (depth >= 5) {
            searching_item = (*table)[index + 5 * INDEX_SIZE];

            while (searching_item != NULL) {
                if ((strcmp(name, searching_item->name) == 0) && (depth == searching_item->depth)) {
                    return searching_item;
                }
            
                searching_item = searching_item->nextPtr;
            }
        }

        if (depth < 5) {

            searching_item = (*table)[index + depth * INDEX_SIZE];
            while (searching_item != NULL) {
                
                if (strcmp(name, searching_item->name) == 0) {
                    return searching_item;
                }
                searching_item = searching_item->nextPtr;
            } 
        }              
        depth--;
    }
    return NULL;
}

Sym_table_item *sym_table_search_item(Sym_table *table, char *name, unsigned depth) {

    if (table == NULL || name == NULL) {
        return NULL;
    }
    
    Sym_table_item *searching_item = NULL;

    if (depth == 0) {
        searching_item = search_on_null_lvl(searching_item, name, depth, table);
        if (searching_item) {
            return searching_item;
        } else {
            return NULL;
        }
    } else {
        searching_item = search_on_non_null_lvl(searching_item, name, depth, table);

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

            if (deleted_item->name) {
                free(deleted_item->name);
            }

            if (deleted_item->value.func.num_in_var > 0) {  
                free(deleted_item->value.func.in_var_list);
            }

            if (deleted_item->value.func.num_out_var > 0) {
                free(deleted_item->value.func.out_var_list);
            }
            if (deleted_item) {
			    free(deleted_item);
            }
		}
		(*table)[i] = NULL;
	}
}
