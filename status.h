#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lists.h"

typedef struct status
{
    long actualLine;    /*Linha do montador*/
    int left;           /*Posicao atual da linha (direita ou esquerda)*/

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
void incStatus(Status* status);

void printStatus(Status status);
