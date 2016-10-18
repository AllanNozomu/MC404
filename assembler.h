#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "status.h"

/*Tipos de comando*/
enum typeCommand {COMMENT = 1, DIRECTIVE, LABEL, INSTRUCTION, ERROR= -1 };

/*Verifica se eh uma isntrucao "especial" (que varia de acordo com o parametro,
por exemplo, os JUMPS), ou nao*/
enum SpecialInstruction {NORMAL_INSTRUCTION = 0, JUMP, JUMP_PLUS, STRADDR};

typedef struct directive
{
    int numParameters;              /*Quantidade de parametros (varia de 1 a 2)*/
    void (*function)(Status*, char* []);    /*Funcao a ser chamada pela diretiva
                                            todas possuem como parametro o
                                            status do montador e parametros da
                                            diretiva */
} Directive;

/*Checa o tipo de comando, retona os descritos ou erro -1*/
int checkCommandType(char *instruction);
/*Vai checar a linha inteira, vai chamar uma seria de checkCommanTypes*/
int checkLine(char* line, Status* status);

/*Funcoes das diretivas*/
void orgDirective(Status* status, char* param[]);
void setDirective(Status* status, char* param[]);
void alignDirective(Status* status, char* param[]);
void wfillDirective(Status* status, char* param[]);
void wordDirective(Status* status, char* param[]);

/*Checa qual diretiva sera retornada de acordo com o comando passado*/
Directive checkDirective(char *command);

/*checa qual a isntrucao*/
void checkInstruction(char *command, Status* status);
/*Checa qual instrucao especial*/
void checkInstructionParameter(char* param ,Status* status, int type);
