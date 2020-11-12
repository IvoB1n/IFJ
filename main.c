#include <stdio.h>
#include "main.h"

int main() {
    Token token;

    if (get_token(&token)) {
        fprintf(stderr, "Lexical error occured");
        return ERROR;
    }

    return 0;
}