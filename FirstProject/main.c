#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include "assembler.h"

#define LINE_SIZE 256

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
              return 0;
            }
        break;
        /*Mesmo nao sendo um caso valido, so para enfatizar*/
        default:
            printf("Expecting 1 or 2 parameteres. Exiting program...\n");
            return 0;
    }

    /*Status do montador, inicializado*/
    Status status;
    initialize(&status);
    int success = 1;
    int lineNumber = 0;

    /*Primeira leitura, ou seja, construir as listas de labels e simbolos e
    ver possiveis discrepancias*/
    while( fgets (line, LINE_SIZE, in) != NULL && success >= 0){
        success = checkLine(line, &status);
        ++lineNumber;
    }

    /*Passa para a segunda leitura se tudo ocorreu bem na primeira*/
    if (success > 0){
        status.firstTime = 0;
        status.actualLine = 0;
        status.left = 1;
        lineNumber = 0;
        if (in != NULL)
        {
            fclose(in);
            in = fopen (argv[1], "r");
        }

        /*Segunda leitura, escrevendo agora no mapa de memoria mas ainda
        verificando erro*/
        while( fgets (line, LINE_SIZE, in) != NULL && success >= 0){
            success = checkLine(line, &status);
            ++lineNumber;
        }
    }

    if (success >= 0)
        printStatus(status, out);
    else
        printError(status, lineNumber, out);

    /*Desalocar estruturas e fechar os arquivos*/
    freeStatus(status);

    if (in != NULL)
        fclose(in);
    if (out != NULL)
        fclose(out);

    return(0);
}
