#include <stdio.h>
#include "token_dll.h"
#include "template.h"


void print_head() {
    fprintf(stdout, ".IFJcode20\n");
    fprintf(stdout, "JUMP $main\n");
}

void template_inputs() { // (string, int) if type(string) == Nil, return 1. else,  return 0
    fprintf(stdout, "LABEL $inputs\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 1);
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 2);
    fprintf(stdout, "READ LF@%%retval%u string\n", 1);
    fprintf(stdout, "DEFVAR LF@!retval%u$type\n", 1);

    fprintf(stdout, "TYPE LF@!retval%u$type LF@%%retval%u\n", 1, 1);
    fprintf(stdout, "JUMPIFEQ  $intuts_check LF@%%retval%u string@nil\n", 1);
    fprintf(stdout, "DEFVAR LF@!error%u\n", 0);
    fprintf(stdout, "JUMP !inps\n");

    fprintf(stdout, "LABEL !inps\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");

    fprintf(stdout, "LABEL !inputs_check\n");
    fprintf(stdout, "DEFVAR LF@!error%u\n", 1);
    fprintf(stdout, "JUMP !inps\n");
}

void template_inputi() {
    fprintf(stdout, "LABEL $inputi\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 1);
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 2);
    fprintf(stdout, "READ LF@%%retval%u int\n", 1);
    fprintf(stdout, "DEFVAR LF@!retval%u$type\n", 1);

    fprintf(stdout, "TYPE LF@!retval%u$type LF@%%retval%u\n", 1, 1);
    fprintf(stdout, "JUMPIFEQ  !inputi_check LF@%%retval%u$type int@nil\n", 1);
    fprintf(stdout, "DEFVAR LF@!error%u\n", 0);
    fprintf(stdout, "JUMP !inpi\n");

    fprintf(stdout, "LABEL !inpi\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");

    fprintf(stdout, "LABEL !inputi_check\n");
    fprintf(stdout, "DEFVAR LF@!error%u\n", 1);
    fprintf(stdout, "JUMP !inpi\n");
}

void template_inputf() {
    fprintf(stdout, "LABEL $inputf\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 1);
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 2);
    fprintf(stdout, "READ LF@%%retval%u float\n", 1);
    fprintf(stdout, "DEFVAR LF@!retval%u$type\n", 1);

    fprintf(stdout, "TYPE LF@retval%u$type LF@%%retval%u\n", 1, 1);
    fprintf(stdout, "JUMPIFEQ  !intutf_check LF@%%retval%u float@nil\n", 1);
    fprintf(stdout, "DEFVAR LF@!error%u\n", 0);
    fprintf(stdout, "JUMP !inpf\n");
    
    fprintf(stdout, "LABEL !inpf\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");

    fprintf(stdout, "LABEL !inputf_check\n");
    fprintf(stdout, "DEFVAR LF@!error%u\n", 1);
    fprintf(stdout, "JUMP !inpf\n");
}

void template_print(unsigned i) {
    fprintf(stdout, "LABEL $print\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "WRITE LF@%%op%u\n", i);
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
}

void template_int2float() {
    fprintf(stdout, "LABEL $int2float\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 1);
    fprintf(stdout, "MOVE LF@%%retval%u LF@%%op%u\n", 1, 1);
    fprintf(stdout, "DEFVAR LF@!float_retval\n");
    fprintf(stdout, "INT2FLOAT LF@%%retval%u LF@!float_retval\n", 1);
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
}

void template_float2int() {
    fprintf(stdout, "LABEL $float2int\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 1);
    fprintf(stdout, "MOVE LF@%%retval%u LF@%%op%u\n", 1, 1);
    fprintf(stdout, "DEFVAR LF@!int_retval\n");
    fprintf(stdout, "FLOAT2INT LF@%%retval%u LF@!int_retval\n", 1);
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
}

void template_len() {
    fprintf(stdout, "LABEL $len\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 1);
    fprintf(stdout, "MOVE LF@%%retval%u LF@%%op%u\n", 1, 1);
    fprintf(stdout, "DEFVAR LF@!string_retval\n");
    fprintf(stdout, "STRLEN LF@%%retval%u LF@!string_retval\n", 1);
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
}

void template_substr() {
    fprintf(stdout, "LABEL $len\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 1);
    fprintf(stdout, "MOVE LF@%%retval%u LF@%%op%u\n", 1, 1);

    fprintf(stdout, "DEFVAR LF@!start\n");
    fprintf(stdout, "DEFVAR LF@!string\n");
    fprintf(stdout, "DEFVAR LF@!len\n");
}

void template_ord() {
    fprintf(stdout, "LABEL $ord\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 1);
    fprintf(stdout, "MOVE LF@%%retval%u LF@%%op%u\n", 1, 1);
    fprintf(stdout, "DEFVAR LF@!string\n");
    fprintf(stdout, "DEFVAR LF@!position\n");

    fprintf(stdout, "STRI2INT LF@%%retval%u LF@!string LF@!position\n", 1);
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
    
    fprintf(stdout, "LABEL !STR_NOT_OK\n");
    fprintf(stdout, "MOVE LF@%%retval%u int@1", 1);
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
}

void template_chr() {
    
}

// void arithm_template(tDLElemPtr s_token) {
    
// }