#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include "assembler.h"

#define LINE_SIZE 256

/*
checkInstruction
Checa a isntrucao lida e retorna o que ela representa (ou pode representar)
char* instruction = instrucao a ser analisada, pode ser um label, uma diretica,
                    um comando, um comentario ou simplesmente um lixo
return int        = retorna um inteiro do typeInstruction referente ao tipo de
                    instrucao passada como parametro
*/

int main(int argc, char *argv[])
{
    FILE *in = NULL, *out = NULL;
    char line[LINE_SIZE];

    switch (argc)
    {
        case 3:
            out = fopen (argv[2], "w");

        case 2:
            in = fopen (argv[1], "r");
            if(in == NULL)
            {
              printf("Error opening file");
              return -1;
            }
        break;
        default:
            printf("Expecting 1 or 2 parameteres. Exiting program...\n");
            return -1;
    }

    Status status;
    initialize(&status);
    int success = 1;
    int lineNumber = 0;
    while( fgets (line, LINE_SIZE, in) != NULL && success >= 0){
        success = checkLine(line, &status);
        ++lineNumber;
    }

    if (success > 0){
        status.firstTime = 0;
        status.actualLine = 0;
        status.left = 1;
        int lineNumber = 0;
        if (in != NULL)
        {
            fclose(in);
            in = fopen (argv[1], "r");
        }
        // printStatus(status);
        // printf("\n\nSEGUNDA LEITURA\n\n");

        while( fgets (line, LINE_SIZE, in) != NULL && success >= 0){
            success = checkLine(line, &status);
            ++lineNumber;
        }
    }

    if (success >= 0)
        printStatus(status, out);
    else
        printError(status, lineNumber, out);

    freeStatus(status);

    if (in != NULL)
        fclose(in);
    if (out != NULL)
        fclose(out);

    return(0);
}
