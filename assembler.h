#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "status.h"

enum typeInstruction {COMMENT = 1, DIRECTIVE, LABEL, COMMAND, ERROR= -1 };

enum SpecialInstruction {NORMAL_INSTRUCTION = 0, JUMP, JUMP_PLUS, STRADDR};

typedef struct directive
{
    /*char* directive;    String da diretiva, por exemplo .org*/
    int numParameters;  /*Quantidade de parametros (varia de 1 a 2)*/
    void (*function)(Status*, char* []);    /*Funcao a ser chamada pela diretiva
                                            todas possuem como parametro o
                                            status do montador e parametros da
                                            diretiva */
} Directive;

int checkInstruction(char *instruction);
int checkLine(char* line, Status* status);

/*METHODS*/

/*orgDirective  Diretiva .org, vai para a linha determinada como parametro
status      O status do montador, para mudar a linhas
param[]     No caso, so vai ser usado um parametro, a linha que deseja mudar

return int  Erro ou sucesso*/
void orgDirective(Status* status, char* param[]);
void setDirective(Status* status, char* param[]);
void alignDirective(Status* status, char* param[]);
void wfillDirective(Status* status, char* param[]);
void wordDirective(Status* status, char* param[]);

Directive checkDirective(char *command);

void checkCommand(char *command, Status* status);
void checkInstructionParameter(char* param ,Status* status, int type);
