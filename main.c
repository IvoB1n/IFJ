#include <stdio.h>
#include "main.h"

int main() {
    Token token;

    if (get_next_token(&token, NULL)) {
        fprintf(stderr, "Lexical error occured");
        free_token(&token);
        return ERROR;
    }

    return 0;
}