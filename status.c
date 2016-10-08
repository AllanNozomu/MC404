#include "status.h"

void initialize(Status* status)
{
    status->actualLine = 0;
    status->left = 1;
    status->error = 0;
    status->listLabels = NULL;
    status->listSymbols = NULL;
    for (int i = 0 ; i < 1024; ++i)
        for (int j = 0 ; j < 10; ++j)
            status->memoryMap[i][j] = ' ';
}

/*incStatus = Avanca o contador do lado esquerdo ou direito da palavra e a linha
atual caso esteja na direita
status  = Status a ser montador a ser modificado*/
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
        status->memoryMap[(int)status->actualLine][(status->left ?
             0 : 5) + index + i] = memory[i];
    }
}

void printStatus(Status status)
{
    printf("STATUS----------------------\n");
    printf("ActualLine: %ld  orientation: %s\n", status.actualLine, (status.left ? "Left" : "Right"));
    printf("Labels: ");
    printLabels(status.listLabels);
    printf("Symbols: ");
    printSymbols(status.listSymbols);
    printf("Memory Map: \n");

    for (int i= 0 ; i < 1024; ++i)
    {
        if (status.memoryMap[i][0] != ' ' || status.memoryMap[i][5] != ' ')
        {
            printf("%03d ", i);
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
    }
    printf("----------------------------\n");
}
