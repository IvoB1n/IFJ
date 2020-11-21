#include "scanner.h"
#include "parser.h"
#include "error.h"
#include "symtable.h"

#include <stdio.h>

void clean_resources() {
    DLDisposeList(&token_list);
    //sym_table_clear_all(sym_table);
}

int main() {
    Token scantoken;

    DLInitList(&token_list);
   // init_sym_table(sym_table);
    int retval = scan_token(&scantoken);
    if (retval) {
        // fprintf(stderr, "Lexical error occured\n");
        clean_resources();
        free_token(&scantoken);
        return retval;
    }

    token_list.Act = token_list.First;

    retval = parse();
    if (retval) {
        // fprintf(stderr, "Syntax error\n");
        clean_resources();
        return retval;
    }
    clean_resources();
    return 0;
}