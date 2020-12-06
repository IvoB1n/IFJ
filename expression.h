/**
 * @file expression.h
 * 
 * precedent analyzer
 * 
 * @brief IFJ Projekt 2020, Tým 58
 * 
 * @author <xbobro01> Bobrov Ivan
 * @author <xcaras00> Carasec Elena
 * @author <xkozhe00> Kozhevnikov Dmitrii
 * @author <xsenic00> Senichak Yahor
 */
#include "scanner.h"

/**
 * Expression processing function, driven by precedent table
 * 
 * @param types_list pointer to Data types list
 * @param exp_t expression type (exp, func, etc)
 * @param depth expression depth
 * @param func_call_num flag for processing or code generation
 * @return 0 If the expression was processed successfully
 * @return 2 Expression syntax error
 * @return 3 Expression undefind variable error
 * @return 5 Expression data types error
 * @return 9 Expression division by zero constant error
 * @return 99 Internal error (malloc, etc)
 */
int expression (tDLList *, unsigned, unsigned, unsigned);