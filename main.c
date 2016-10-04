#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include "directive.h"

#define LINE_SIZE 256

enum typeInstruction {COMMENT = 1, DIRECTIVE, LABEL, COMMAND, ERROR= -1 };

enum typeError {
    TWO_LABEL_ERROR = 1,
    COMMAND_AND_DIR_ERROR,
    DUPLICATE_LABEL_ERROR,
    INVALID_INSTRUCTION_ERROR,
    INVALID_PARAMETER_DIR_ERROR,
    LABEL_AFTER_CMD_DIR_ERROR,
    INVALID_DIRECTIVE_ERROR
};

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
    regex_t regex;

    int retorno = ERROR;
    /*Tratamento com Regular Expressions*/
    /*Verifica se comeca com '#', um comentario*/
    regcomp(&regex, "^#", REG_EXTENDED|REG_NOSUB);
    if (!regexec(&regex, instruction, 0, NULL, 0))
        retorno = COMMENT;

    /*Verifica se comeca com '.' e por letras minusculas, diretiva*/
    regcomp(&regex, "^\\.[a-z]+$", REG_EXTENDED|REG_NOSUB);
    if (!regexec(&regex, instruction, 0, NULL, 0))
        retorno = DIRECTIVE;

    /*Verifica se eh formada por caracteres e '_', comentario*/
    regcomp(&regex, "[^a-z_A-Z]+:$", REG_EXTENDED|REG_NOSUB);
    if (!regexec(&regex, instruction, 0, NULL, 0))
        retorno = LABEL;

    /*Verifica se comeca com letra maiuscula, comando*/
    regcomp(&regex, "^[A-Z]", REG_EXTENDED|REG_NOSUB);
    if (!regexec(&regex, instruction, 0, NULL, 0))
        retorno = COMMAND;

    regfree(&regex);
    printf("Instruction Id: %d\n", retorno);
    return retorno;
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
        printf("STATUS--------------\nActualLine: %ld left %d\n", status.actualLine, status.left);
        printf("Line %d-------------\n%s", ++lineCounter, line);
        char *token = strtok(line, " \n");
        status.label = status.cmdOrDir = status.error = 0;
        while (token != NULL)
        {
            switch (checkInstruction(token)){
                case COMMENT:
                    do
                        token = strtok(NULL, "");
                    while (token != NULL);
                    break;

                case COMMAND:
                    if (status.cmdOrDir)
                        status.error = COMMAND_AND_DIR_ERROR;
                    else
                    {
                        status.cmdOrDir = 1;
                        incStatus(&status);
                    }
                    break;

                case DIRECTIVE:
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
                    if (status.label)
                        status.error = TWO_LABEL_ERROR;
                    else if (status.cmdOrDir)
                        status.error =  LABEL_AFTER_CMD_DIR_ERROR;
                    else
                    {
                        status.label = 1;
                        Node* labelNode = getNode(token, &status);
                        if (labelNode == NULL)
                        {
                            status.listLabels = addNode(token, status.actualLine, status.left, &status);
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
            token = strtok(NULL, " \n");
        }

    }
    printf("STATUS--------------\nActualLine: %ld left %d\n", status.actualLine, status.left);
    freeNodes(status.listLabels);

    fclose(in);


    return(0);
}
