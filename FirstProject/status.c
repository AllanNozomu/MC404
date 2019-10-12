#include "status.h"

void initialize(Status* status)
{
    status->actualLine = 0;
    status->left = 1;
    status->firstTime =1;
    status->error = 0;
    status->listLabels = NULL;
    status->listSymbols = NULL;

    /*Zera o mapa de memoria, colocando como padrao o caractere ' '*/
    for (int i = 0 ; i < 1024; ++i)
        for (int j = 0 ; j < 10; ++j)
            status->memoryMap[i][j] = ' ';
}

void incStatus(Status* status)
{
    if (!status->left)
        ++status->actualLine;
    status->left = !status->left;
}

void addMemory(Status* status, char* memory, int index)
{
    int len = strlen(memory);
    for (int i = 0 ; i < len; ++i)
    {
        /*Verifica se eh na esquerda ou na direita para por no mapa de memoria*/
        status->memoryMap[(int)status->actualLine][(status->left ?
             0 : 5) + index + i] = memory[i];
    }
}

void printStatus(Status status, FILE* out)
{
    for (int i= 0 ; i < 1024; ++i)
    {
        if (status.memoryMap[i][0] != ' ' || status.memoryMap[i][5] != ' ')
        {
            /*out == NULL quer dizer que nao houve arquivo de saida especificado*/
            if (out == NULL){
                printf("%03X ", i);
                /*Printa 0 nos locais dos espacos em branco, para completar
                linhas incompletas e tambem preencher com 0 instrucoes que nao
                tem paramentro*/
                printf("%c%c %c%c%c %c%c %c%c%c\n",
                    status.memoryMap[i][0] == ' ' ? '0' : status.memoryMap[i][0],
                    status.memoryMap[i][1] == ' ' ? '0' : status.memoryMap[i][1],
                    status.memoryMap[i][2] == ' ' ? '0' : status.memoryMap[i][2],
                    status.memoryMap[i][3] == ' ' ? '0' : status.memoryMap[i][3],
                    status.memoryMap[i][4] == ' ' ? '0' : status.memoryMap[i][4],
                    status.memoryMap[i][5] == ' ' ? '0' : status.memoryMap[i][5],
                    status.memoryMap[i][6] == ' ' ? '0' : status.memoryMap[i][6],
                    status.memoryMap[i][7] == ' ' ? '0' : status.memoryMap[i][7],
                    status.memoryMap[i][8] == ' ' ? '0' : status.memoryMap[i][8],
                    status.memoryMap[i][9] == ' ' ? '0' : status.memoryMap[i][9]
                );
            }
            /*Printa no arquivo de saida definido em out*/
            else
            {
                fprintf(out, "%03X ", i);
                fprintf(out, "%c%c %c%c%c %c%c %c%c%c\n",
                    status.memoryMap[i][0] == ' ' ? '0' : status.memoryMap[i][0],
                    status.memoryMap[i][1] == ' ' ? '0' : status.memoryMap[i][1],
                    status.memoryMap[i][2] == ' ' ? '0' : status.memoryMap[i][2],
                    status.memoryMap[i][3] == ' ' ? '0' : status.memoryMap[i][3],
                    status.memoryMap[i][4] == ' ' ? '0' : status.memoryMap[i][4],
                    status.memoryMap[i][5] == ' ' ? '0' : status.memoryMap[i][5],
                    status.memoryMap[i][6] == ' ' ? '0' : status.memoryMap[i][6],
                    status.memoryMap[i][7] == ' ' ? '0' : status.memoryMap[i][7],
                    status.memoryMap[i][8] == ' ' ? '0' : status.memoryMap[i][8],
                    status.memoryMap[i][9] == ' ' ? '0' : status.memoryMap[i][9]
                );
            }
        }
    }
}

void printError(Status status, int lineNumber,  FILE* out)
{
    /*Para cada tipo de erro ha uma mensagem diferente*/
    char* errorMsg;
    switch(status.error)
    {
        case TWO_LABEL_ERROR:
            errorMsg = "Can't have two or more labels in a line.";
        break;
        case INSTRUCTION_AND_DIR_ERROR:
            errorMsg = "Can't have more than one instruction or directive in a line.";
        break;
        case DUPLICATE_LABEL_ERROR:
            errorMsg = "This label has already been declared.";
        break;
        case INVALID_INSTRUCTION_ERROR:
            errorMsg = "Excepting a valid instruction.";
        break;
        case INVALID_PARAMETER_DIR_ERROR:
            errorMsg = "Invalid parameter passed to directive";
        break;
        case LABEL_AFTER_CMD_DIR_ERROR:
            errorMsg = "Can't define a label after a instruction or directive";
        break;
        case INVALID_PARAMETER_INS_ERROR:
            errorMsg = "Invalid parameter passed to instruction";
        break;
        case INVALID_COMMAND_ERROR:
            errorMsg = "Expecting a valid label, instruction or directive";
        break;
        case INVALID_DIRECTIVE_ERROR:
            errorMsg = "Excepting a valid directive.";
        break;
    }
    /*out == NULL quer dizer que nao houve arquivo especificado*/
    if (out == NULL)
    {
        printf("ERROR on line %d\n",lineNumber);
        printf("%s\n", errorMsg);
    }
    else
    {
        fprintf(out, "ERROR on line %d\n",lineNumber);
        fprintf(out, "%s\n", errorMsg);
    }
}

void freeStatus(Status status){
    freeLabelNodes(status.listLabels);
    freeSymbolNodes(status.listSymbols);
}
