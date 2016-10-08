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
        retorno.numParameters = 1;
        retorno.function = &orgDirective;
        return retorno;
    }
    else if (!strcmp(command, ".wfill")){
        Directive retorno;
        retorno.numParameters = 2;
        retorno.function = &wfillDirective;
        return retorno;
    }
    else if (!strcmp(command, ".set")){
        Directive retorno;
        retorno.numParameters = 2;
        retorno.function = &setDirective;
        return retorno;
    }
    else if (!strcmp(command, ".word")){
        Directive retorno;
        retorno.numParameters = 1;
        retorno.function = &wordDirective;
        return retorno;
    }
    else if (!strcmp(command, ".align")){
        Directive retorno;
        retorno.numParameters = 1;
        retorno.function = &alignDirective;
        return retorno;
    }
    Directive retorno;
    retorno.numParameters = -1;
    return retorno;
}

int checkCommand(char *command, Status* status)
{
    if (!strcmp(command, "LD"))
    {
        char* param = strtok(NULL, " \n");
        addMemory(status, "01", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }
    else if (!strcmp(command, "LD-"))
    {
        char* param = strtok(NULL, " \n");
        addMemory(status, "02", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }
    else if (!strcmp(command, "LD|"))
    {
        char* param = strtok(NULL, " \n");
        addMemory(status, "03", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }
    else if (!strcmp(command, "LDmq"))
    {
        addMemory(status, "0A", 0);
    }
    else if (!strcmp(command, "LDmq_mx"))
    {
        addMemory(status, "09", 0);
    }


    else if (!strcmp(command, "ST"))
    {
        char* param = strtok(NULL, " \n");
        addMemory(status, "21", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }


    else if (!strcmp(command, "JMP"))
    {
        char* param = strtok(NULL, " \n");
        checkInstructionParameter(param, status, JUMP);
    }
    else if (!strcmp(command, "JUMP+"))
    {
        char* param = strtok(NULL, " \n");
        checkInstructionParameter(param, status, JUMP_PLUS);
    }


    else if (!strcmp(command, "ADD"))
    {
        char* param = strtok(NULL, " \n");
        addMemory(status, "05", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }
    else if (!strcmp(command, "ADD|"))
    {
        char* param = strtok(NULL, " \n");
        addMemory(status, "07", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }
    else if (!strcmp(command, "SUB"))
    {
        char* param = strtok(NULL, " \n");
        addMemory(status, "06", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }
    else if (!strcmp(command, "SUB|"))
    {
        char* param = strtok(NULL, " \n");
        addMemory(status, "08", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }
    else if (!strcmp(command, "MUL|"))
    {
        char* param = strtok(NULL, " \n");
        addMemory(status, "0B", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }
    else if (!strcmp(command, "DIV"))
    {
        char* param = strtok(NULL, " \n");
        addMemory(status, "0C", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }
    else if (!strcmp(command, "LSH"))
    {
        addMemory(status, "14", 0);
    }
    else if (!strcmp(command, "RSH"))
    {
        addMemory(status, "15", 0);
    }

    else if (!strcmp(command, "STaddr"))
    {
        char* param = strtok(NULL, " \n");
        checkInstructionParameter(param, status, STRADDR);
    }
    incStatus(status);
}

int checkInstructionParameter(char* param ,Status* status, int type )
{
    regex_t regex;
    regmatch_t groupArray[2];
    regcomp(&regex, "\\\"([a-z_A-Z0-9]+)\\\"", REG_EXTENDED);
    if (!regexec(&regex, param, 2, groupArray, REG_NOTBOL))
    {
        char paramResult[strlen(param) + 2];
        int len = groupArray[1].rm_eo - groupArray[1].rm_so;
        memcpy(paramResult, param + groupArray[1].rm_so, len);
        regfree(&regex);

        char string[11];
        if (isDecimalNegative(paramResult) || isHexadecimalNumber1024(paramResult))
        {
            long lineAux = strtol(paramResult, NULL, 0);
            sprintf (string, "%03lX", lineAux);
            switch (type) {
                case JUMP:
                    addMemory(status, "0D", 0);
                break;
                case JUMP_PLUS:
                    addMemory(status, "0F", 0);
                break;
                case STRADDR:
                    addMemory(status, "12", 0);
                break;
            }
        }
        else
        {
            strcat(paramResult, ":");

            LabelNode* node = getLabelNode(paramResult, status->listLabels);
            if (node == NULL)
                return -1;
            sprintf (string, "%03lX", node->label.lineNumber);
            if (node->label.left)
                switch (type) {
                    case JUMP:
                        addMemory(status, "0D", 0);
                    break;
                    case JUMP_PLUS:
                        addMemory(status, "0F", 0);
                    break;
                    case STRADDR:
                        addMemory(status, "12", 0);
                    break;
                }
            else
                switch (type) {
                    case JUMP:
                        addMemory(status, "0E", 0);
                    break;
                    case JUMP_PLUS:
                        addMemory(status, "10", 0);
                    break;
                    case STRADDR:
                        addMemory(status, "13", 0);
                    break;
                }
        }
        for (int j = 0; j < 3; ++j)
            status->memoryMap[(int)status->actualLine][(status->left ? 2 : 7) + j] = string[j];
    }
    else
        regfree(&regex);

    return -1;
}
