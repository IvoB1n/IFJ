#include "main.h"

int main() {
    Token token;

    if (get_next_token(&token)) {
        free_token(&token);
        return LEXICAL_ERROR;
    }

    return 0;
}