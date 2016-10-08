#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

#define MAXINT 2147483647

/*Checa se o parametro eh um numero ou nao (decimal ou hexadecimal)*/
int isDecimal1024(char* str);
int isDecimal(char* str);
int isDecimalNegative(char* str);
int isHexadecimalNumber(char* str);
int isHexadecimalNumber1024(char* str);
int isLabel(char* str);
int isSymbol(char* str);
int isComment(char* str);
int isDirective(char* str);
