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
    fprintf(stdout, "STRLEN LF@!string_retval LF@%%retval%u\n", 1);
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
}

void template_substr() {        // [y a h o r]
                                //  0 1 2 3 4
    fprintf(stdout, "LABEL $len\n");
    fprintf(stdout, "PUSHFRAME\n");

    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 1);
    fprintf(stdout, "MOVE LF@%%retval%u LF@%%op%u\n", 1, 1);    // string
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 2);
    fprintf(stdout, "MOVE LF@%%retval%u LF@%%op%u\n", 2, 2);    // i
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 3);
    fprintf(stdout, "MOVE LF@%%retval%u LF@%%op%u\n", 3, 3);    // n
    fprintf(stdout, "DEFVAR LF@!bool_if\n");
    fprintf(stdout, "DEFVAR LF@!ret_int\n");    // int returns 0/1
    fprintf(stdout, "LT LF@!bool_if LF@%%retval%u int@0\n", 1);
    fprintf(stdout, "JUMPIFEQ !chyba1 LF@!bool_if bool@true\n");
    fprintf(stdout, "LT LF@!bool_if LF@%%retval%u int@0\n", 2);
    fprintf(stdout, "JUMPIFEQ !chyba1 LF@!bool_if bool@true\n");
    fprintf(stdout, "DEFVAR LF@!new_string%u\n", 1);
    fprintf(stdout, "DEFVAR LF@!strlen\n");
    fprintf(stdout, "STRLEN LF@!strlen LF@%%retval%u\n", 1);
    fprintf(stdout, "DEFVAR LF@!new_strlen\n");
    fprintf(stdout, "SUB LF@!new_strlen LF@!strlen int@1\n");
    fprintf(stdout, "GT LF@!bool_if LF@%%retval%u LF@!new_strlen\n", 2);
    fprintf(stdout, "JUMPIFEQ !chyba1 LF@!bool_if bool@true\n");

    fprintf(stdout, "SUB LF@!new_strlen LF@!strlen LF@%%retval%u\n", 2);
    fprintf(stdout, "GT LF@!bool_if LF@!new_strlen LF@%%retval%u\n", 3);
    fprintf(stdout, "JUMPIFEQ !chyba2 LF@!bool_if bool@true\n");

    fprintf(stdout, "LABEL !cont\n"); 

    
    fprintf(stdout, "EQ LF@!bool_if LF@%%retval%u int@0\n", 3);
    fprintf(stdout, "JUMPIFEQ !chyba0 LF@!bool_if bool@true\n");

    fprintf(stdout, "DEFVAR LF@!new_string%u\n", 2);

    
    fprintf(stdout, "GETCHAR LF@!new_string%u LF@%%retval%u LF@%%retval%u\n", 1, 1, 2); //  [ y 'a h o' r]
    fprintf(stdout, "LABEL !cuklus\n");                                                 //    0  1 2 3 4  
                                                                                        // new_str1 = a, new_str2 = null
    fprintf(stdout, "JUMPIFEQ !end1 LF@%%retval%u LF@%%retval%u\n", 2, 3);    // end of cuklus
    fprintf(stdout, "ADD LF@%%retval%u int@1\n", 2);    // i++
    fprintf(stdout, "GETCHAR LF@!new_string%u LF@%%retval%u LF@%%retval%u\n", 2, 1, 2);
    fprintf(stdout, "CONCAT LF@!new_string%u LF@!new_string%u LF@!new_string%u\n", 1, 1, 2);
    fprintf(stdout, "JUMP !cuklus\n");  

    fprintf(stdout, "LABEL !chyba1\n");
    fprintf(stdout, "MOVE LF@!ret_int int@1\n");
    fprintf(stdout, "PUSHS LF@!new_string%u\n", 1);
    fprintf(stdout, "PUSHS LF@!ret_int\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");

    fprintf(stdout, "LABEL !chyba2\n");
    fprintf(stdout, "MOVE LF@%%retval%u LF@!new_strlen\n", 3);
    fprintf(stdout, "JUMP !cont\n");

    fprintf(stdout, "LABEL !chyba0\n");
    fprintf(stdout, "MOVE LF@!ret_int int@0\n");
    fprintf(stdout, "MOVE LF@!new_string%u string@\\000\n", 1);
    fprintf(stdout, "PUSHS LF@!new_string%u\n", 1);
    fprintf(stdout, "PUSHS LF@!ret_int\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");

    fprintf(stdout, "LABEL !end1\n");
    fprintf(stdout, "MOVE LF@!ret_int int@0\n");
    fprintf(stdout, "PUSHS LF@!new_string%u\n", 1);
    fprintf(stdout, "PUSHS LF@!ret_int\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
}
/*               
int substr(char *string, int i, int n) {
    if (n < 0 || ((i < 0) || (i >= strlen(string)))) {
        return 1;
    }

    if (n > strlen(string) - i) {
        n = strlen(string) - i;
    }
    char *podstr;
    char *time_str
    while (i != n) {
        time_str = getchar (string, i);
        concat(podstr, time_str);
        i++;
    }
    return 0;
    return podstr;

}

*/






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