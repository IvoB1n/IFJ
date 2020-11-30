#include <stdio.h>
#include "token_dll.h"
#include "template.h"

void template_print(unsigned i);


void template_str_concat() {
    fprintf(stdout, "DEFVAR GF@&str_concat_1\n");
    fprintf(stdout, "DEFVAR GF@&str_concat_2\n");
    fprintf(stdout, "DEFVAR GF@&str_concat_res\n");
}

void template_div_by_zero() {
    fprintf(stdout, "DEFVAR GF@&div_zero_number\n");
    fprintf(stdout, "JUMP &div_zero_label_end\n");
    fprintf(stdout, "LABEL &div_zero_label_err\n");
    fprintf(stdout, "EXIT int@9\n");
    fprintf(stdout, "LABEL &div_zero_label_end\n");
}

void template_inputs() { // (string, int) if type(string) == Nil, return 1. else,  return 0
    fprintf(stdout, "LABEL $inputs\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 1);
    fprintf(stdout, "READ LF@%%retval%u string\n", 1);
    fprintf(stdout, "DEFVAR LF@!retval%u$type\n", 1);

    fprintf(stdout, "TYPE LF@!retval%u$type LF@%%retval%u\n", 1, 1);
    fprintf(stdout, "DEFVAR LF@!error\n");
    fprintf(stdout, "MOVE LF@!error int@0\n");
    fprintf(stdout, "JUMPIFEQ  $intuts_check LF@!retval%u$type nil@nil\n", 1);

    fprintf(stdout, "LABEL !inps\n");

    fprintf(stdout, "PUSHS LF@%%retval%u\n", 1);
    fprintf(stdout, "PUSHS LF@!error\n");

    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");

    fprintf(stdout, "LABEL !inputs_check\n");
    fprintf(stdout, "MOVE LF@!error int@1\n");
    fprintf(stdout, "JUMP !inps\n");
}

void template_inputi() {
    fprintf(stdout, "LABEL $inputi\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 1);
    fprintf(stdout, "READ LF@%%retval%u int\n", 1);
    fprintf(stdout, "DEFVAR LF@!retval%u$type\n", 1);

    fprintf(stdout, "TYPE LF@!retval%u$type LF@%%retval%u\n", 1, 1);
    fprintf(stdout, "DEFVAR LF@!error\n");
    fprintf(stdout, "MOVE LF@!error int@0\n");
    fprintf(stdout, "JUMPIFNEQ  !inputi_check LF@!retval%u$type string@int\n", 1);

    fprintf(stdout, "LABEL !inpi\n");

    fprintf(stdout, "PUSHS LF@%%retval%u\n", 1);
    fprintf(stdout, "PUSHS LF@!error\n");

    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");

    fprintf(stdout, "LABEL !inputi_check\n");
    fprintf(stdout, "MOVE LF@%%retval%u int@256\n", 1);
    fprintf(stdout, "MOVE LF@!error int@1\n");
    fprintf(stdout, "JUMP !inpi\n");
}

void template_inputf() {
    fprintf(stdout, "LABEL $inputf\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 1);
    fprintf(stdout, "READ LF@%%retval%u float\n", 1);
    fprintf(stdout, "DEFVAR LF@!retval%u$type\n", 1);

    fprintf(stdout, "TYPE LF@!retval%u$type LF@%%retval%u\n", 1, 1);
    fprintf(stdout, "DEFVAR LF@!error\n");
    fprintf(stdout, "MOVE LF@!error int@0\n");
    fprintf(stdout, "JUMPIFNEQ  !inputf_check LF@!retval%u$type string@float\n", 1);

    fprintf(stdout, "LABEL !inpf\n");

    fprintf(stdout, "PUSHS LF@%%retval%u\n", 1);
    fprintf(stdout, "PUSHS LF@!error\n");

    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");

    fprintf(stdout, "LABEL !inputf_check\n");
    fprintf(stdout, "MOVE LF@%%retval%u float@0x0p+0\n", 1);
    fprintf(stdout, "MOVE LF@!error int@1\n");
    fprintf(stdout, "JUMP !inpf\n");
}

void template_inputb() {
    fprintf(stdout, "LABEL $inputb\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 1);
    fprintf(stdout, "READ LF@%%retval%u bool\n", 1);
    fprintf(stdout, "DEFVAR LF@!retval%u$type\n", 1);

    fprintf(stdout, "TYPE LF@!retval%u$type LF@%%retval%u\n", 1, 1);
    fprintf(stdout, "DEFVAR LF@!error\n");
    fprintf(stdout, "MOVE LF@!error int@0\n");
    fprintf(stdout, "JUMPIFNEQ  !inputb_check LF@!retval%u$type string@bool\n", 1);

    fprintf(stdout, "LABEL !inpb\n");

    fprintf(stdout, "PUSHS LF@%%retval%u\n", 1);
    fprintf(stdout, "PUSHS LF@!error\n");

    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");

    fprintf(stdout, "LABEL !inputb_check\n");
    fprintf(stdout, "MOVE LF@%%retval%u bool@false\n", 1);
    fprintf(stdout, "MOVE LF@!error int@1\n");
    fprintf(stdout, "JUMP !inpb\n");
}

void template_print(unsigned i) {
    //fprintf(stdout, "LABEL $print\n");
    //int n = i-1;
    fprintf(stdout, "PUSHFRAME\n");
    for (unsigned t = 1; t < i; t++) {
        fprintf(stdout, "DEFVAR LF@%%retval%u\n", t);
        fprintf(stdout, "POPS LF@%%retval%u\n", t);
    }
    for (unsigned n = i - 1; n > 0; n--) {
       fprintf(stdout, "WRITE LF@%%retval%u\n", n);
    }
    fprintf(stdout, "POPFRAME\n");
    //fprintf(stdout, "RETURN\n");
    
    
}

void template_int2float() {
    fprintf(stdout, "LABEL $int2float\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 1);
    fprintf(stdout, "MOVE LF@%%retval%u LF@%%op%u\n", 1, 1);
    fprintf(stdout, "DEFVAR LF@!float_retval\n");
    fprintf(stdout, "INT2FLOAT LF@!float_retval LF@%%retval%u\n", 1);
    fprintf(stdout, "PUSHS LF@!float_retval\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
}

void template_float2int() {
    fprintf(stdout, "LABEL $float2int\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 1);
    fprintf(stdout, "MOVE LF@%%retval%u LF@%%op%u\n", 1, 1);
    fprintf(stdout, "DEFVAR LF@!int_retval\n");
    fprintf(stdout, "FLOAT2INT LF@!int_retval LF@%%retval%u \n", 1);
    fprintf(stdout, "PUSHS LF@!int_retval\n");
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
    fprintf(stdout, "PUSHS LF@!string_retval\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
}

void template_substr() {        // [y a h o r]
                                //  0 1 2 3 4
    fprintf(stdout, "LABEL $substr\n");
    fprintf(stdout, "PUSHFRAME\n");

    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 1);
    fprintf(stdout, "MOVE LF@%%retval%u LF@%%op%u\n", 1, 1);    // LF@%%retval1 = string
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 2);
    fprintf(stdout, "MOVE LF@%%retval%u LF@%%op%u\n", 2, 2);    // LF@%%retval2 = i
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 3);
    fprintf(stdout, "MOVE LF@%%retval%u LF@%%op%u\n", 3, 3);    // LF@%%retval3 = n
    fprintf(stdout, "BREAK\n");
    fprintf(stdout, "DEFVAR LF@!bool_if\n");    // for control bool bool
    fprintf(stdout, "DEFVAR LF@!ret_int\n");    // int returns 0/1
    fprintf(stdout, "DEFVAR LF@!new_string%u\n", 1);    // string for back
    fprintf(stdout, "DEFVAR LF@!strlen\n"); // strlen of string
    fprintf(stdout, "LT LF@!bool_if LF@%%retval%u int@0\n", 2);
    fprintf(stdout, "JUMPIFEQ !substr_chyba1 LF@!bool_if bool@true\n");    //  if (i < 0)
    fprintf(stdout, "STRLEN LF@!strlen LF@%%retval%u\n", 1);    // strlen(string)
    fprintf(stdout, "GT LF@!bool_if LF@%%retval%u LF@!strlen\n", 2);    // if (i > len(s))
    fprintf(stdout, "JUMPIFEQ !substr_chyba1 LF@!bool_if bool@true\n");    
    fprintf(stdout, "LT LF@!bool_if LF@%%retval%u int@0\n", 3);     // if (n < 0)
    fprintf(stdout, "JUMPIFEQ !substr_chyba1 LF@!bool_if bool@true\n");    // if (i > len(string))
    fprintf(stdout, "DEFVAR LF@!new_strlen\n"); // for control n > len(s) - i
    fprintf(stdout, "BREAK\n");
    fprintf(stdout, "MOVE LF@!new_strlen LF@!strlen\n");
    fprintf(stdout, "BREAK\n");
    fprintf(stdout, "SUB LF@!new_strlen LF@!new_strlen LF@%%retval%u\n", 2);    // len(s) - i
    fprintf(stdout, "BREAK\n");
    fprintf(stdout, "GT LF@!bool_if LF@%%retval%u LF@!new_strlen\n", 3);    // if n > len(s) - i
    fprintf(stdout, "JUMPIFEQ !substr_chyba2 LF@!bool_if bool@true\n");

    fprintf(stdout, "LABEL !substr_cont\n"); 
    fprintf(stdout, "BREAK\n");

    fprintf(stdout, "EQ LF@!bool_if LF@%%retval%u int@0\n", 3); // if (n == 0) => return ""
    fprintf(stdout, "JUMPIFEQ !substr_chyba0 LF@!bool_if bool@true\n");

    fprintf(stdout, "DEFVAR LF@!new_string%u\n", 2);
    fprintf(stdout, "DEFVAR LF@!new_i\n");
    fprintf(stdout, "MOVE LF@!new_i int@1\n");

    fprintf(stdout, "GETCHAR LF@!new_string%u LF@%%retval%u LF@%%retval%u\n", 1, 1, 2); //  [ y 'a h o' r]
    fprintf(stdout, "LABEL !substr_cuklus\n");                                                 //    0  1 2 3 4  
    //fprintf(stdout, "ADD LF@%%retval%u LF@%%retval%u LF@%%retval%u\n", 3, 2, 3);                                                   
                                                                                        // new_str1 = a, new_str2 = null
    fprintf(stdout, "JUMPIFEQ !substr_end1 LF@!new_i LF@%%retval%u\n", 3);    // end of cuklus
    fprintf(stdout, "ADD LF@!new_i LF@!new_i int@1\n");    // i++
    fprintf(stdout, "ADD LF@%%retval%u LF@%%retval%u int@1\n", 2, 2);
    fprintf(stdout, "GETCHAR LF@!new_string%u LF@%%retval%u LF@%%retval%u\n", 2, 1, 2);
    fprintf(stdout, "BREAK\n");
    fprintf(stdout, "CONCAT LF@!new_string%u LF@!new_string%u LF@!new_string%u\n", 1, 1, 2);
    fprintf(stdout, "JUMP !substr_cuklus\n");  

    fprintf(stdout, "LABEL !substr_chyba1\n");
    fprintf(stdout, "MOVE LF@!new_string%u string@\\000\n", 1);
    fprintf(stdout, "MOVE LF@!ret_int int@1\n");
    fprintf(stdout, "PUSHS LF@!new_string%u\n", 1);
    fprintf(stdout, "PUSHS LF@!ret_int\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
    /*
    [ y 'a h o' r]
    getchar -> LF@!new_string1
    while (i != n){
        i++
        getchar -> LF@!new_string2
        LF@!new_string1 = LF@!new_string1 + LF@!new_string2
    }
    */

    fprintf(stdout, "LABEL !substr_chyba2\n");
    fprintf(stdout, "BREAK\n");
    fprintf(stdout, "MOVE LF@%%retval%u LF@!new_strlen\n", 3);
    fprintf(stdout, "JUMP !substr_cont\n");

    fprintf(stdout, "LABEL !substr_chyba0\n");
    fprintf(stdout, "MOVE LF@!ret_int int@0\n");
    fprintf(stdout, "MOVE LF@!new_string%u string@\\000\n", 1);
    fprintf(stdout, "PUSHS LF@!new_string%u\n", 1);
    fprintf(stdout, "PUSHS LF@!ret_int\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");

    fprintf(stdout, "LABEL !substr_end1\n");
    fprintf(stdout, "MOVE LF@!ret_int int@0\n");
    fprintf(stdout, "PUSHS LF@!new_string%u\n", 1);
    fprintf(stdout, "PUSHS LF@!ret_int\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
}

void template_ord() {
    fprintf(stdout, "LABEL $ord\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 1);
    fprintf(stdout, "MOVE LF@%%retval%u LF@%%op%u\n", 1, 1);    // string
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 2);
    fprintf(stdout, "MOVE LF@%%retval%u LF@%%op%u\n", 2, 2);    // i

    fprintf(stdout, "DEFVAR LF@!ascii_in_position\n");
    fprintf(stdout, "DEFVAR LF@!error\n");
    fprintf(stdout, "MOVE LF@!error int@0\n");
    fprintf(stdout, "DEFVAR LF@!bool_control\n");
    fprintf(stdout, "LT LF@!bool_control LF@%%retval%u int@0\n", 2);

    fprintf(stdout, "JUMPIFEQ !ord_chyba LF@!bool_control bool@true\n");    // if (i < 0)

    fprintf(stdout, "DEFVAR LF@!len_string\n");
    fprintf(stdout, "STRLEN LF@!len_string LF@%%retval%u\n", 1);
    fprintf(stdout, "SUB LF@!len_string LF@!len_string int@1\n");

    fprintf(stdout, "GT LF@!bool_control LF@%%retval%u LF@!len_string\n", 2);
    fprintf(stdout, "JUMPIFEQ !ord_chyba LF@!bool_control bool@true\n");    // if (i > strlen - 1)

    fprintf(stdout, "STRI2INT LF@!ascii_in_position LF@%%retval%u LF@%%retval%u\n", 1, 2);
    fprintf(stdout, "PUSHS LF@!ascii_in_position\n");
    fprintf(stdout, "PUSHS LF@!error\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");


    fprintf(stdout, "LABEL !ord_chyba\n");
    fprintf(stdout, "MOVE LF@!ascii_in_position int@256\n");
    fprintf(stdout, "MOVE LF@!error int@1\n");
    fprintf(stdout, "PUSHS LF@!ascii_in_position\n");
    fprintf(stdout, "PUSHS LF@!error\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
}


void template_chr() {
    fprintf(stdout, "LABEL $chr\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@%%retval%u\n", 1);
    fprintf(stdout, "MOVE LF@%%retval%u LF@%%op%u\n", 1, 1);    // int

    fprintf(stdout, "DEFVAR LF@!error\n");
    fprintf(stdout, "MOVE LF@!error int@0\n");
    fprintf(stdout, "DEFVAR LF@!ascii_symbol\n");

    fprintf(stdout, "DEFVAR LF@!bool_control\n");
    fprintf(stdout, "LT LF@!bool_control LF@%%retval%u int@0\n", 1);
    fprintf(stdout, "JUMPIFEQ !chr_chyba LF@!bool_control bool@true\n");    // if (i < 0)

    fprintf(stdout, "GT LF@!bool_control LF@%%retval%u int@255\n", 1);
    fprintf(stdout, "JUMPIFEQ !chr_chyba LF@!bool_control bool@true\n");    // if (i > 255)
    
    fprintf(stdout, "INT2CHAR LF@!ascii_symbol LF@%%retval%u\n", 1);
    fprintf(stdout, "PUSHS LF@!ascii_symbol\n");
    fprintf(stdout, "PUSHS LF@!error\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");


    fprintf(stdout, "LABEL !chr_chyba\n");
    fprintf(stdout, "MOVE LF@!error int@1\n");
    fprintf(stdout, "PUSHS LF@!ascii_symbol\n");
    fprintf(stdout, "PUSHS LF@!error\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
}

void print_head() {
    fprintf(stdout, ".IFJcode20\n");
    template_div_by_zero();
    template_str_concat();
    fprintf(stdout, "JUMP $main\n");
    template_inputs();
    template_inputi();
    template_inputf();
    template_int2float();
    template_float2int();
    template_len();
    template_substr();
    template_ord();
    template_chr(); 
}

//      1695e155.1455116
//      15115.e51651
// 4.
//      26.351е45
//      14е151