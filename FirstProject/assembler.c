#include "assembler.h"

int checkCommandType(char *instruction)
{
    if (isComment(instruction))
        return COMMENT;

    if (isDirective(instruction))
        return DIRECTIVE;

    if (isLabel(instruction))
        return LABEL;

    if (isInstruction(instruction))
        return INSTRUCTION;

    return ERROR;
}

int checkLine(char* line, Status* status)
{
    char *token = strtok(line, " \n\t");
    /*Zera variaveis que verificam se ja houve labels, diretivas e instrucoes*/
    status->label = status->cmdOrDir = status->error = 0;
    /*Vai pegando token a token separado pelos espacos e tratando-os*/
    while (token != NULL)
    {
        switch (checkCommandType(token)){
            case COMMENT:
                do
                    /*Acaba com a linha*/
                    token = strtok(NULL, "\n");
                while (token != NULL);
                break;

            case INSTRUCTION:
                if (status->cmdOrDir)
                    status->error = INSTRUCTION_AND_DIR_ERROR;
                else
                {
                    status->cmdOrDir = 1;
                    checkInstruction(token, status);
                    incStatus(status);
                }
                break;

            case DIRECTIVE:
                if (status->cmdOrDir)
                    status->error = INSTRUCTION_AND_DIR_ERROR;
                else
                {
                    status->cmdOrDir = 1;
                    Directive d = checkDirective(token);
                    if (d.numParameters < 0)
                        status->error = INVALID_DIRECTIVE_ERROR;
                    else
                    {
                        /*Le a quantidade de parametros necessarios*/
                        char* parameters[2];
                        for (int i = 0 ; i < d.numParameters; ++i){
                            parameters[i] = strtok(NULL, " \n\t");
                        }
                        d.function(status, parameters);
                    }
                }
                break;

            case LABEL:
                if (status->label)
                    status->error = TWO_LABEL_ERROR;
                else if (status->cmdOrDir)
                    status->error =  LABEL_AFTER_CMD_DIR_ERROR;
                else
                {
                    status->label = 1;
                    /*Rotulos sao tratados somente na primeira leitura*/
                    if (!status->firstTime)
                        break;
                    char nameLabel[strlen(token)];

                    /*Salva somente o corpo ignorando o ':'*/
                    strncpy(nameLabel, token, strlen(token)-1);
                    nameLabel[strlen(token)-1] = '\0';

                    /*Verifica a existencia do rotulo e sua nova criacao*/
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
                status->error = INVALID_COMMAND_ERROR;
            break;
        }
        if (status->error > 0)
            return -1;

        token = strtok(NULL, " \n\t");
    }
    return 1;
}

/*Comando das diretivas*/

void orgDirective(Status* status, char* param[])
{
    if (isHexadecimalNumber(param[0]) || isDecimal1024(param[0])){
        status->actualLine = (int)strtol(param[0], NULL, 0);
        status->left = 1;
        return;
    }
    status->error = INVALID_PARAMETER_DIR_ERROR;
}

void setDirective(Status* status, char* param[])
{
    if (!status->firstTime)
        return;
    if (isSymbol(param[0]))
        if (isHexadecimalNumber(param[1]) || isDecimal(param[1])){
            SymbolNode* symbolNode = getSymbolNode(param[0], status->listSymbols);
            int value = (int)strtol(param[1], NULL, 0);
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
            return;
        }
    status->error = INVALID_PARAMETER_DIR_ERROR;
}

void alignDirective(Status* status, char* param[])
{
    if (isDecimal1024(param[0])){
        if (!status->left)
            incStatus(status);
        int resto = status->actualLine % strtol(param[0], NULL, 0);
        status->actualLine = status->actualLine + resto;
        status->left = 1;
        return;
    }
    status->error = INVALID_PARAMETER_DIR_ERROR;
}

void wfillDirective(Status* status, char* param[])
{
    if (isDecimal1024(param[0]))
    {
        int qty = (int)strtol(param[0], NULL, 0);
        char string[11];
        if (!status->firstTime)
        {
            if (isDecimalNegative(param[1]) || isHexadecimalNumber(param[1]))
            {
                int param1 = (int)strtol(param[1], NULL, 0);
                sprintf (string, "%010X", param1);
            }
            else
            {
                LabelNode* labelNode = getLabelNode(param[1], status->listLabels);
                if (labelNode != NULL)
                    for (int i = 0; i < 10; ++i)
                        sprintf (string, "%010X", labelNode->label.lineNumber);
                else
                {
                    SymbolNode* symbolNode = getSymbolNode(param[1], status->listSymbols);
                    if (symbolNode != NULL)
                        sprintf (string, "%010X", symbolNode->symbol.value);
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
        return;
    }
    status->error = INVALID_PARAMETER_DIR_ERROR;
}

void wordDirective(Status* status, char* param[])
{
    if (!status->firstTime)
    {
        char string[11];
        if (isDecimal(param[0]) || isHexadecimalNumber(param[0]))
        {
            int param1 = (int)strtol(param[0], NULL, 0);
            sprintf (string, "%010X", param1);
        }
        else
        {
            LabelNode* labelNode = getLabelNode(param[0], status->listLabels);
            if (labelNode != NULL)
                for (int i = 0; i < 10; ++i)
                    sprintf (string, "%010X", labelNode->label.lineNumber);
            else
            {
                SymbolNode* symbolNode = getSymbolNode(param[0], status->listSymbols);
                if (symbolNode != NULL)
                    sprintf (string, "%010X", symbolNode->symbol.value);
                else
                    status->error = INVALID_PARAMETER_DIR_ERROR;
            }
        }

        for (int i = 0; i < 10; ++i)
            status->memoryMap[(int)status->actualLine][i] = string[i];
    }
    ++(status->actualLine);
    return;
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

/*Verifica a instrucao*/

void checkInstruction(char *command, Status* status)
{
    /*Instrucoes basicas sem parametro*/
    if (!strcmp(command, "LDmq"))
        addMemory(status, "0A", 0);

    else if (!strcmp(command, "LSH"))
        addMemory(status, "14", 0);

    else if (!strcmp(command, "RSH"))
        addMemory(status, "15", 0);

    /*Instrucoes com parametros*/
    else
    {
        char* param = strtok(NULL, " \n\t");

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

        else
            status->error = INVALID_INSTRUCTION_ERROR;
    }
}

/*Checa as instrucoes que contem parametro*/
void checkInstructionParameter(char* param ,Status* status, int type )
{
    /*Instrucoes com parametros sao tratados somente na segunda leitura*/
    if (status->firstTime)
        return;
    if (param == NULL)
    {
        status->error = INVALID_PARAMETER_INS_ERROR;
        return;
    }
    regex_t regex;

    /*Verifica o tipo de parametro, se esta entre aspas*/
    regcomp(&regex, "^\\\"[a-z_A-Z0-9]+\\\"$", REG_EXTENDED|REG_NOSUB);
    int retorno = !regexec(&regex, param, 0, NULL, 0);
    regfree(&regex);
    if (retorno)
    {
        /*Pega somente o corpo que esta entre as aspas*/
        char paramResult[strlen(param) - 1];
        int i;
        for (i = 0; i < strlen(param) - 2; ++i)
            paramResult[i] = *(param + i + 1);
        paramResult[i] = '\0';

        char string[4];
        /*Verifica se o parametro eh um numero*/
        if (isDecimalNegative(paramResult) || isHexadecimalNumber1024(paramResult))
        {
            int lineAux = (int)strtol(paramResult, NULL, 0);
            sprintf (string, "%03X", lineAux);

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
        /*Verifica se o parametro eh um rotulo ou um simbolo*/
        else
        {
            LabelNode* node = getLabelNode(paramResult, status->listLabels);
            if (node == NULL){
                status->error = INVALID_PARAMETER_INS_ERROR;
                return;
            }
            sprintf (string, "%03X", node->label.lineNumber);
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
        addMemory(status, string, 2);
    }
    else
        status->error = INVALID_PARAMETER_INS_ERROR;
}
