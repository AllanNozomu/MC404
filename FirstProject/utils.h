#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

#define MAXINT 2147483647

/*Checa se eh um decimal positivo de tamanho 1024*/
int isDecimal1024(char* str);
/*Checa se eh um decimal positivo*/
int isDecimal(char* str);
/*Checa se eh um decimal que pode ser negativo*/
int isDecimalNegative(char* str);

/*Checa se eh um hexadecimal*/
int isHexadecimalNumber(char* str);
/*Checa se eh um hexadecimal de tamanho maximo 2014*/
int isHexadecimalNumber1024(char* str);

/*Verifica se eh um label*/
int isLabel(char* str);
/*Verifica se eh um simbolo*/
int isSymbol(char* str);
/*Verifica se eh um comentario*/
int isComment(char* str);
/*Verifica se eh uma possivel instrucao (nao verifica se eh de fato uma existente)*/
int isInstruction(char* str);
/*Verifica se eh uma possivel diretiva (nao veritica se eh de fato uma existente)*/
int isDirective(char* str);
