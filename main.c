#include "scanner.h"
#include "parser.h"
#include "error.h"
#include "symtable.h"
#include "semantics.h"

#include <stdio.h>

void clean_resources() {
    sym_table_clear_all(&sym_table);
    DLDisposeList(&token_list);
}

int main() {
    Token scantoken;
    DLInitList(&token_list);
    sym_table_init(&sym_table);

    int retval = scan_token(&scantoken);

    if (retval) {
        free_token(&scantoken);
        clean_resources();
        return retval;
    }

    token_list.Act = token_list.First;
    retval = fill_function_prototype_list();
    if (retval) {
        clean_resources();
        return retval;
    }

    token_list.Act = token_list.First;
    retval = parse();

    if (retval) {
        clean_resources();
        return retval;
    }

    clean_resources();

    return 0;
}