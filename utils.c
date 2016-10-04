#include "utils.h"

int isNumber(char* str)
{
    long value = strtol(str, NULL, 0);
    if (!value)
        for (int i = 0 ; i < strlen(str); ++i)
        {
            if (*(str + i) != '0' && (*(str + i) != 'x' && i == 1))
                return 0;
        }
    return 1;
}

int isLabel(char* str)
{
    regex_t regex;

    /*Verifica se eh formada por caracteres e '_' e termina em ':', comentario*/
    regcomp(&regex, "^[a-z_A-Z]+:$", REG_EXTENDED|REG_NOSUB);
    int retorno = !regexec(&regex, str, 0, NULL, 0);
    regfree(&regex);

    return retorno;
}

int isSymbol(char* str)
{
    regex_t regex;

    /*Verifica se eh formada por caracteres e '_', simbolo*/
    regcomp(&regex, "^[a-z_A-Z]+$", REG_EXTENDED|REG_NOSUB);
    int retorno = !regexec(&regex, str, 0, NULL, 0);
    regfree(&regex);

    return retorno;
}
