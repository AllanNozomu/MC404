#include "directive.h"

int orgDirective(Status* status, char* param[])
{
    if (isNumber(param[0])){
        status->actualLine = strtol(param[0], NULL, 0);
        status->left = 1;
        return 1;
    }
    return -1;
}
int setDirective(Status* status, char* param[])
{
    if (isSymbol(param[0]) && isNumber(param[1])){

        return 1;
    }
}

int alignDirective(Status* status, char* param[])
{
    if (isNumber(param[0])){
        int resto = status->actualLine % strtol(param[0], NULL, 0);
        status->actualLine = status->actualLine + resto;
        status->left = 1;
        return 1;
    }
}

int wfillDirective(Status* status, char* param[])
{
    if (isNumber(param[0]))
    {
        long param0 = strtol(param[0], NULL, 0);
        char string[11];
        if (isNumber(param[1]))
        {
            int param1 = strtol(param[1], NULL, 0);
            sprintf (string, "%010X", param1);
        }
        else
        {
            Node* node = getNode(param[1], status);
            for (int i = 0; i < 10; ++i)
                string[i] = status->memoryMap[node->label.lineNumber][i];
        }

        for (int i = 0 ; i < param0; ++i)
        {
            for (int j = 0; j < 10; ++j)
            {
                status->memoryMap[(int)status->actualLine + i][j] = string[j];
                printf("%c", status->memoryMap[(int)status->actualLine + i][j]);
            }
            printf("\n");
        }
    }
}

int wordDirective(Status* status, char* param[])
{
    char string[11];
    if (isNumber(param[0]))
    {
        int param1 = strtol(param[1], NULL, 0);
        sprintf (string, "%010X", param1);
    }
    else
    {
        Node* node = getNode(param[1], status);
        for (int i = 0; i < 10; ++i)
            string[i] = status->memoryMap[node->label.lineNumber][i];
    }

    for (int j = 0; j < 10; ++j){
        status->memoryMap[(int)status->actualLine][j] = string[j];
        printf("%c", status->memoryMap[(int)status->actualLine][j]);
    }
    printf("\n");
}

Directive checkDirective(char *command)
{
    if (!strcmp(command, ".org")){
        Directive retorno;
        // retorno.directive = ".org";
        retorno.numParameters = 1;
        retorno.function = &orgDirective;
        return retorno;
    }
    else if (!strcmp(command, ".wfill")){
        Directive retorno;
        // retorno.directive = ".org";
        retorno.numParameters = 2;
        retorno.function = &wfillDirective;
        return retorno;
    }
    else if (!strcmp(command, ".set")){
        Directive retorno;
        // retorno.directive = ".org";
        retorno.numParameters = 2;
        retorno.function = &setDirective;
        return retorno;
    }
    else if (!strcmp(command, ".word")){
        Directive retorno;
        // retorno.directive = ".org";
        retorno.numParameters = 1;
        retorno.function = &wordDirective;
        return retorno;
    }
    else if (!strcmp(command, ".align")){
        Directive retorno;
        // retorno.directive = ".org";
        retorno.numParameters = 1;
        retorno.function = &alignDirective;
        return retorno;
    }
    Directive retorno;
    // retorno.directive = ".org";
    retorno.numParameters = -1;
    return retorno;
}
