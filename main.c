#include "scanner.h"
#include "parser.h"
#include "error.h"

#include <stdio.h>

int main() {
    Token scantoken;

    int retval = scan_token(&scantoken);
    if (retval) {
        // fprintf(stderr, "Lexical error occured\n");
        free_token(&scantoken);
        return retval;
    }

    token_list.Act = token_list.First;

    retval = parse();
    if (retval) {
        // fprintf(stderr, "Syntax error\n");
        return retval;
    }

    return 0;
}