#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lists.h"

enum typeError {
    TWO_LABEL_ERROR = 1,
    COMMAND_AND_DIR_ERROR,
    DUPLICATE_LABEL_ERROR,
    INVALID_INSTRUCTION_ERROR,
    INVALID_PARAMETER_DIR_ERROR,
    LABEL_AFTER_CMD_DIR_ERROR,
    INVALID_DIRECTIVE_ERROR,
    DUPLICATE_SYMBOL_ERROR,
    INVALID_PARAMETER_INS_ERROR
};

typedef struct status
{
    int actualLine;    /*Linha do montador*/
    int left;           /*Posicao atual da linha (direita ou esquerda)*/
    int firstTime;

    char memoryMap[1024][10];

    LabelNode* listLabels;   /*Lista dos labes*/
    SymbolNode* listSymbols;   /*Lista dos labes*/

    int label;          /*Salva se ja foi digitado um label*/
    int cmdOrDir;       /*Sala se ja foi digitado um comando ou diretriz*/
    int error;          /*Verifica se ja deu algum erro*/
} Status;


void initialize(Status* status);
/*incStatus = Avanca o contador do lado esquerdo ou direito da palavra e a linha
atual caso esteja na direita
status  = Status a ser montador a ser modificado*/
void addMemory(Status* status, char* memory, int index);
void incStatus(Status* status);

void printStatus(Status status, FILE* out);
void printError(Status status, int lineNumber,  FILE* out);

void freeStatus(Status status);
