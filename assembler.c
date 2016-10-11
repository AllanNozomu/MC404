#include "assembler.h"

int checkInstruction(char *instruction)
{
    if (isComment(instruction))
        return COMMENT;

    if (isDirective(instruction))
        return DIRECTIVE;

    if (isLabel(instruction))
        return LABEL;

    if (isCommand(instruction))
        return COMMAND;

    return ERROR;
}

int checkLine(char* line, Status* status, int lineNumber)
{
    // printf("Line %s", line);
    char *token = strtok(line, " \n");
    status->label = status->cmdOrDir = status->error = 0;
    while (token != NULL)
    {
        switch (checkInstruction(token)){
            case COMMENT:
                // printf("COMENTARIO %s\n", token);
                do
                    token = strtok(NULL, "\n");
                while (token != NULL);
                break;

            case COMMAND:
                // printf("COMANDO %s\n", token);
                if (status->cmdOrDir)
                    status->error = COMMAND_AND_DIR_ERROR;
                else
                {
                    status->cmdOrDir = 1;
                    checkCommand(token, status);
                    incStatus(status);
                }
                break;

            case DIRECTIVE:
                // printf("DIRETIVA %s\n", token);
                if (status->cmdOrDir)
                    status->error = COMMAND_AND_DIR_ERROR;
                else
                {
                    status->cmdOrDir = 1;
                    Directive d = checkDirective(token);
                    if (d.numParameters < 0)
                        status->error = INVALID_DIRECTIVE_ERROR;
                    else
                    {
                        char* parameters[2];
                        for (int i = 0 ; i < d.numParameters; ++i){
                            parameters[i] = strtok(NULL, " \n");
                        }
                        d.function(status, parameters);
                    }
                }
                break;

            case LABEL:
                // printf("LABEL %s\n", token);
                if (status->label)
                    status->error = TWO_LABEL_ERROR;
                else if (status->cmdOrDir)
                    status->error =  LABEL_AFTER_CMD_DIR_ERROR;
                else
                {
                    status->label = 1;
                    if (!status->firstTime)
                        break;
                    char nameLabel[strlen(token)];
                    strncpy(nameLabel, token, strlen(token)-1);
                    nameLabel[strlen(token)-1] = '\0';
                    LabelNode* labelNode = getLabelNode(nameLabel, status->listLabels);
                    if (labelNode == NULL)
                    {
                        status->listLabels = addLabelNode(nameLabel, status->actualLine, status->left, status->listLabels);
                    }
                    else
                    {
                        if (labelNode->label.lineNumber < 0 && labelNode->label.left < 0)
                        {
                            labelNode->label.lineNumber = status->actualLine;
                            labelNode->label.left = status->left;
                        } else
                            status->error = DUPLICATE_LABEL_ERROR;
                    }
                }
            break;
            case ERROR:
                status->error = INVALID_INSTRUCTION_ERROR;
            break;
        }
        if (status->error)
        {
            printf("ERROR on line %d\n",lineNumber);
            switch(status->error)
            {
                case TWO_LABEL_ERROR:
                    printf("Can't have two or more labels in a line.\n");
                break;
                case COMMAND_AND_DIR_ERROR:
                    printf("Can't have more than one Command or Directive in a line.\n");
                break;
                case DUPLICATE_LABEL_ERROR:
                    printf("This label has already been declared.\n");
                break;
                case INVALID_INSTRUCTION_ERROR:
                    printf("Expecting a valid label, command or directive\n");
                break;
                case INVALID_PARAMETER_DIR_ERROR:
                    printf("Invalid parameter passed to directive\n");
                break;
                case LABEL_AFTER_CMD_DIR_ERROR:
                    printf("Can't define a label after a command or directive\n");
                break;
            }
            return -1;
        }
        token = strtok(NULL, " \n");
    }
    return 1;
}

int orgDirective(Status* status, char* param[])
{
    if (isHexadecimalNumber(param[0]) || isDecimal1024(param[0])){
        status->actualLine = strtol(param[0], NULL, 0);
        status->left = 1;
        return 0;
    }
    return -1;
}
int setDirective(Status* status, char* param[])
{
    if (!status->firstTime)
        return 0;
    if (isSymbol(param[0]))
        if (isHexadecimalNumber(param[1]) || isDecimal(param[1])){
            SymbolNode* symbolNode = getSymbolNode(param[0], status->listSymbols);
            long value = strtol(param[1], NULL, 0);
            if (symbolNode == NULL)
                status->listSymbols = addSymbolNode(param[0], value, status->listSymbols);
            else
            {
                if (symbolNode->symbol.value < 0)
                    symbolNode->symbol.value = value;
                else{
                    status->error = DUPLICATE_SYMBOL_ERROR;
                }
            }
            return 0;
        }
    status->error = INVALID_PARAMETER_DIR_ERROR;
}

int alignDirective(Status* status, char* param[])
{
    if (isDecimal1024(param[0])){
        int resto = status->actualLine % strtol(param[0], NULL, 0);
        status->actualLine = status->actualLine + resto;
        status->left = 1;
        return 0;
    }
    status->error = INVALID_PARAMETER_DIR_ERROR;
}

int wfillDirective(Status* status, char* param[])
{
    if (isDecimal1024(param[0]))
    {
        long qty = strtol(param[0], NULL, 0);
        char string[11];
        if (!status->firstTime)
        {
            if (isDecimalNegative(param[1]) || isHexadecimalNumber(param[1]))
            {
                long param1 = strtol(param[1], NULL, 0);
                sprintf (string, "%010lX", param1);
            }
            else
            {
                LabelNode* labelNode = getLabelNode(param[1], status->listLabels);
                if (labelNode != NULL)
                    for (int i = 0; i < 10; ++i)
                        sprintf (string, "%010lX", labelNode->label.lineNumber);
                else
                {
                    SymbolNode* symbolNode = getSymbolNode(param[1], status->listSymbols);
                    if (symbolNode != NULL)
                        sprintf (string, "%010lX", symbolNode->symbol.value);
                    else
                        status->error = INVALID_PARAMETER_DIR_ERROR;
                }
            }

            for (int i = 0 ; i < qty; ++i){
                for (int j = 0; j < 10; ++j)
                    status->memoryMap[(int)status->actualLine + i][j] = string[j];
                }
            }
        status->actualLine += qty;
        return 0;
    }
    status->error = INVALID_PARAMETER_DIR_ERROR;
}

int wordDirective(Status* status, char* param[])
{
    if (!status->firstTime)
    {
        char string[11];
        if (isDecimal(param[0]) || isHexadecimalNumber(param[0]))
        {
            long param1 = strtol(param[0], NULL, 0);
            sprintf (string, "%010lX", param1);
        }
        else
        {
            LabelNode* labelNode = getLabelNode(param[0], status->listLabels);
            if (labelNode != NULL)
                for (int i = 0; i < 10; ++i)
                    sprintf (string, "%010lX", labelNode->label.lineNumber);
            else
            {
                SymbolNode* symbolNode = getSymbolNode(param[0], status->listSymbols);
                if (symbolNode != NULL)
                    sprintf (string, "%010lX", symbolNode->symbol.value);
                else
                    status->error = INVALID_PARAMETER_DIR_ERROR;
            }
        }

        for (int i = 0; i < 10; ++i)
            status->memoryMap[(int)status->actualLine][i] = string[i];
    }
    ++(status->actualLine);
    return 0;
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
    if (!strcmp(command, "LDmq"))
        addMemory(status, "0A", 0);
    else if (!strcmp(command, "LSH"))
        addMemory(status, "14", 0);
    else if (!strcmp(command, "RSH"))
        addMemory(status, "15", 0);

    char* param = strtok(NULL, " \n");

    if (!strcmp(command, "LD"))
    {
        addMemory(status, "01", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }
    else if (!strcmp(command, "LD-"))
    {
        addMemory(status, "02", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }
    else if (!strcmp(command, "LD|"))
    {
        addMemory(status, "03", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }
    else if (!strcmp(command, "LDmq_mx"))
    {
        addMemory(status, "09", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }
    else if (!strcmp(command, "ST"))
    {
        addMemory(status, "21", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }


    else if (!strcmp(command, "JMP"))
        checkInstructionParameter(param, status, JUMP);
    else if (!strcmp(command, "JUMP+"))
        checkInstructionParameter(param, status, JUMP_PLUS);


    else if (!strcmp(command, "ADD"))
    {
        addMemory(status, "05", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }
    else if (!strcmp(command, "ADD|"))
    {
        addMemory(status, "07", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }
    else if (!strcmp(command, "SUB"))
    {
        addMemory(status, "06", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }
    else if (!strcmp(command, "SUB|"))
    {
        addMemory(status, "08", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }
    else if (!strcmp(command, "MUL"))
    {
        addMemory(status, "0B", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }
    else if (!strcmp(command, "DIV"))
    {
        addMemory(status, "0C", 0);
        checkInstructionParameter(param, status, NORMAL_INSTRUCTION);
    }


    else if (!strcmp(command, "STaddr"))
        checkInstructionParameter(param, status, STRADDR);
}

int checkInstructionParameter(char* param ,Status* status, int type )
{
    if (status->firstTime)
        return 0;
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

    return 0;
}
