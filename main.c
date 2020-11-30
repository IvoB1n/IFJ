#include "scanner.h"
#include "parser.h"
#include "error.h"
#include "symtable.h"
#include "semantics.h"

#include <stdio.h>

void clean_resources() {
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    DLDisposeList(&token_list);
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    sym_table_clear_all(&sym_table);
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

}

int main() {
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    Token scantoken;
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    DLInitList(&token_list);
    sym_table_init(&sym_table);
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    int retval = scan_token(&scantoken);
      //  fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    if (retval) {
        //    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

        free_token(&scantoken);
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clean_resources();
        return retval;
    }
   // fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    token_list.Act = token_list.First;
   /// fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    retval = fill_function_prototype_list();
   /// fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    if (retval) {
      //  fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clean_resources();
        return retval;
    }

    token_list.Act = token_list.First;
    retval = parse();
   // fprintf(stderr, "%s %d\n", __FILE__, __LINE__);


    if (retval) {
        //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
        clean_resources();
        return retval;
    }
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    clean_resources();
    //fprintf(stderr, "%s %d\n", __FILE__, __LINE__);

    return 0;
}