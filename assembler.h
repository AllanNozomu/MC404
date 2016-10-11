#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "status.h"

enum typeInstruction {COMMENT = 1, DIRECTIVE, LABEL, COMMAND, ERROR= -1 };

enum SpecialInstruction {NORMAL_INSTRUCTION = 0, JUMP, JUMP_PLUS, STRADDR};

enum typeError {
    TWO_LABEL_ERROR = 1,
    COMMAND_AND_DIR_ERROR,
    DUPLICATE_LABEL_ERROR,
    INVALID_INSTRUCTION_ERROR,
    INVALID_PARAMETER_DIR_ERROR,
    LABEL_AFTER_CMD_DIR_ERROR,
    INVALID_DIRECTIVE_ERROR,
    DUPLICATE_SYMBOL_ERROR
};

typedef struct directive
{
    /*char* directive;    String da diretiva, por exemplo .org*/
    int numParameters;  /*Quantidade de parametros (varia de 1 a 2)*/
    int (*function)(Status*, char* []);    /*Funcao a ser chamada pela diretiva
                                            todas possuem como parametro o
                                            status do montador e parametros da
                                            diretiva */
} Directive;

int checkInstruction(char *instruction);
int checkLine(char* line, Status* status, int lineNumber);

/*METHODS*/

/*orgDirective  Diretiva .org, vai para a linha determinada como parametro
status      O status do montador, para mudar a linhas
param[]     No caso, so vai ser usado um parametro, a linha que deseja mudar

return int  Erro ou sucesso*/
int orgDirective(Status* status, char* param[]);
int setDirective(Status* status, char* param[]);
int alignDirective(Status* status, char* param[]);
int wfillDirective(Status* status, char* param[]);
int wordDirective(Status* status, char* param[]);

Directive checkDirective(char *command);

int checkCommand(char *command, Status* status);
int checkInstructionParameter(char* param ,Status* status, int type);
