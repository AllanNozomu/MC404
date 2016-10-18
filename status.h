#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lists.h"

/*Tipos de erros, maiores que 0 para indicar que houve um erro*/
enum typeError {
    TWO_LABEL_ERROR = 1,
    INSTRUCTION_AND_DIR_ERROR,
    DUPLICATE_LABEL_ERROR,
    INVALID_INSTRUCTION_ERROR,
    INVALID_PARAMETER_DIR_ERROR,
    LABEL_AFTER_CMD_DIR_ERROR,
    INVALID_DIRECTIVE_ERROR,
    DUPLICATE_SYMBOL_ERROR,
    INVALID_PARAMETER_INS_ERROR,
    INVALID_COMMAND_ERROR
};

typedef struct status
{
    int actualLine;     /*Linha do montador*/
    int left;           /*Posicao atual da linha (direita ou esquerda)*/
    int firstTime;

    char memoryMap[1024][10];

    LabelNode* listLabels;      /*Lista dos labes*/
    SymbolNode* listSymbols;    /*Lista dos labes*/

    int label;          /*Salva se ja foi digitado um label*/
    int cmdOrDir;       /*Sala se ja foi digitado um comando ou diretriz*/
    int error;          /*Verifica se ja deu algum erro*/
} Status;

/*Prepara o status para uso, zerando as variaveis e tornando-a pronto para uso*/
void initialize(Status* status);

/*Avanca o contador do lado esquerdo ou direito da palavra e a linha
atual caso esteja na direita*/
void incStatus(Status* status);

/*Coloca no mapa de memoria no indice da linha atual do status uma cadeia de
caracteres memory*/
void addMemory(Status* status, char* memory, int index);

/*Funcoes responsaveis por fazer a saida do programa, tanto no arquivo quanto
na saida padrao*/
void printStatus(Status status, FILE* out);
/*Funcao responsavel para fazer a saida do erro*/
void printError(Status status, int lineNumber,  FILE* out);

/*Desaloca todos os pedacos de memoria alocados pelas listas de simbolos e
rotulos*/
void freeStatus(Status status);
