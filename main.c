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

int main(int argc, char *argv[])
{
    FILE *in, *out;
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

    int lineCounter = 0;
    while( fgets (line, LINE_SIZE, in) != NULL )
    {
        printf("Line %d %s", ++lineCounter, line);
        char *token = strtok(line, " \n");
        status.label = status.cmdOrDir = status.error = 0;
        while (token != NULL)
        {
            switch (checkInstruction(token)){
                case COMMENT:
                    printf("COMENTARIO %s\n", token);
                    do
                        token = strtok(NULL, "\n");
                    while (token != NULL);
                    break;

                case COMMAND:
                    printf("COMANDO %s\n", token);
                    if (status.cmdOrDir)
                        status.error = COMMAND_AND_DIR_ERROR;
                    else
                    {
                        status.cmdOrDir = 1;
                        checkCommand(token, &status);
                    }
                    break;

                case DIRECTIVE:
                    printf("DIRETIVA %s\n", token);
                    if (status.cmdOrDir)
                        status.error = COMMAND_AND_DIR_ERROR;
                    else
                    {
                        status.cmdOrDir = 1;
                        Directive d = checkDirective(token);
                        if (d.numParameters < 0)
                            status.error = INVALID_DIRECTIVE_ERROR;
                        else
                        {
                            char* parameters[2];
                            for (int i = 0 ; i < d.numParameters; ++i){
                                parameters[i] = strtok(NULL, " \n");
                            }
                            int success = d.function(&status, parameters);
                            if (success < 0)
                                status.error = INVALID_PARAMETER_DIR_ERROR;
                        }
                    }
                    break;

                case LABEL:
                    printf("LABEL %s\n", token);
                    if (status.label)
                        status.error = TWO_LABEL_ERROR;
                    else if (status.cmdOrDir)
                        status.error =  LABEL_AFTER_CMD_DIR_ERROR;
                    else
                    {
                        status.label = 1;
                        LabelNode* labelNode = getLabelNode(token, status.listLabels);
                        if (labelNode == NULL)
                        {
                            status.listLabels = addLabelNode(token, status.actualLine, status.left, status.listLabels);
                        }
                        else
                        {
                            if (labelNode->label.lineNumber < 0 && labelNode->label.left < 0)
                            {
                                labelNode->label.lineNumber = status.actualLine;
                                labelNode->label.left = status.left;
                            } else
                                status.error = DUPLICATE_LABEL_ERROR;
                        }
                    }
                break;
                case ERROR:
                    status.error = INVALID_INSTRUCTION_ERROR;
                break;
            }
            if (status.error)
            {
                switch(status.error)
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
            }
            printStatus(status);
            token = strtok(NULL, " \n");
        }

    }
    freeLabelNodes(status.listLabels);
    freeSymbolNodes(status.listSymbols);

    fclose(in);


    return(0);
}
