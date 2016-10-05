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
        if (status.memoryMap[i][0] != ' ')
        {
            printf("%03d", i);
            for (int j = 0; j < 10; ++j)
            {
                if (!(j % 2))
                    printf(" ");
                printf("%c", status.memoryMap[i][j]);
            }
            printf("\n");
        }
    }
    printf("----------------------------\n");
}
