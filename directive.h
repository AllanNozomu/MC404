#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "status.h"

typedef struct directive
{
    /*char* directive;    String da diretiva, por exemplo .org*/
    int numParameters;  /*Quantidade de parametros (varia de 1 a 2)*/
    int (*function)(Status*, char* []);    /*Funcao a ser chamada pela diretiva
                                            todas possuem como parametro o
                                            status do montador e parametros da
                                            diretiva */
} Directive;

/*METHODS*/

/*orgDirective  Diretiva .org, vai para a linha determinada como parametro
status      O status do montador, para mudar a linhas
param[]     No caso, so vai ser usado um parametro, a linha que deseja mudar

return int  Erro ou sucesso*/
int orgDirective(Status* status, char* param[]);

int setDirective(Status* status, char* param[]);

int alignDirective(Status* status, char* param[]);

int wfillDirective(Status* status, char* param[]);

int wordDirective(Status* status, char* param[]);

Directive checkDirective(char *command);
