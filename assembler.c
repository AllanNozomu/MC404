#include "assembler.h"

int orgDirective(Status* status, char* param[])
{
    if (isHexadecimalNumber(param[0]) || isDecimal1024(param[0])){
        status->actualLine = strtol(param[0], NULL, 0);
        status->left = 1;
        return 1;
    }
    return -1;
}
int setDirective(Status* status, char* param[])
{
    if (isSymbol(param[0]))
        if (isHexadecimalNumber(param[1]) || isDecimal(param[1])){
            SymbolNode* symbolNode = getSymbolNode(param[1], status->listSymbols);
            long value = strtol(param[0], NULL, 0);
            if (symbolNode == NULL)
                status->listSymbols = addSymbolNode(param[1], value, status->listSymbols);
            else
            {
                if (symbolNode->symbol.value < 0)
                    symbolNode->symbol.value = value;
                else
                    status->error = DUPLICATE_SYMBOL_ERROR;
            }

            return 1;
        }
    return -1;
}

int alignDirective(Status* status, char* param[])
{
    if (isDecimal1024(param[0])){
        int resto = status->actualLine % strtol(param[0], NULL, 0);
        status->actualLine = status->actualLine + resto;
        status->left = 1;
        return 1;
    }
    return -1;
}

int wfillDirective(Status* status, char* param[])
{
    if (isDecimal1024(param[0]))
    {
        long qty = strtol(param[0], NULL, 0);
        char string[11];

        if (isDecimalNegative(param[1]) || isHexadecimalNumber(param[1]))
        {
            long param1 = strtol(param[1], NULL, 0);
            sprintf (string, "%010lX", param1);
        }
        else if (isLabel(param[1]))
        {
            LabelNode* node = getLabelNode(param[1], status->listLabels);
            for (int i = 0; i < 10; ++i)
                string[i] = status->memoryMap[node->label.lineNumber][i];
        }
        else if (isSymbol(param[1]))
        {

        }
        else
            return -1;

        for (int i = 0 ; i < qty; ++i)
            for (int j = 0; j < 10; ++j)
                status->memoryMap[(int)status->actualLine + i][j] = string[j];

        status->actualLine += qty;
    }
    return -1;
}

int wordDirective(Status* status, char* param[])
{
    char string[11];
    if (isDecimal(param[0]) || isHexadecimalNumber(param[0]))
    {
        long param1 = strtol(param[1], NULL, 0);
        sprintf (string, "%010lX", param1);
    }
    else if (isLabel(param[0]))
    {
        LabelNode* node = getLabelNode(param[1], status->listLabels);
        for (int i = 0; i < 10; ++i)
            string[i] = status->memoryMap[node->label.lineNumber][i];
    }
    else if (isSymbol(param[0]))
    {

    }
    else
        return -1;

    for (int j = 0; j < 10; ++j)
        status->memoryMap[(int)status->actualLine][j] = string[j];
    ++(status->actualLine);
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
