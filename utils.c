#include "utils.h"

int isDecimal1024(char* str)
{
    regex_t regex;

    regcomp(&regex, "^[0-9]{1,4}$", REG_EXTENDED|REG_NOSUB);
    int retorno = !regexec(&regex, str, 0, NULL, 0);
    regfree(&regex);
    if (retorno)
    {
        long value = strtol(str, NULL, 0);
        if (0 <= value && value < 1024)
            return 1;
    }

    return 0;
}

int isDecimal(char* str)
{
    regex_t regex;

    regcomp(&regex, "^[0-9]{1,10}$", REG_EXTENDED|REG_NOSUB);
    int retorno = !regexec(&regex, str, 0, NULL, 0);
    regfree(&regex);
    if (retorno)
    {
        long value = strtol(str, NULL, 0);
        if (0 <= value && value < MAXINT)
            return 1;
    }

    return 0;
}

int isDecimalNegative(char* str)
{
    regex_t regex;

    regcomp(&regex, "^-?[0-9]{1,32}$", REG_EXTENDED|REG_NOSUB);
    int retorno = !regexec(&regex, str, 0, NULL, 0);
    regfree(&regex);
    if (retorno)
    {
        long value = strtol(str, NULL, 0);
        if (-MAXINT <= value && value < MAXINT)
            return 1;
    }

    return 0;
}

int isHexadecimalNumber(char* str)
{
    regex_t regex;

    regcomp(&regex, "^0x[0-9a-fA-F]{10}$", REG_EXTENDED|REG_NOSUB);
    int retorno = !regexec(&regex, str, 0, NULL, 0);
    regfree(&regex);

    return retorno;
}

int isHexadecimalNumber1024(char* str)
{
    regex_t regex;

    regcomp(&regex, "^0x[0-9a-fA-F]{10}$", REG_EXTENDED|REG_NOSUB);
    int retorno = !regexec(&regex, str, 0, NULL, 0);
    regfree(&regex);
    if (retorno)
    {
        long value = strtol(str, NULL, 0);
        if (0 <= value && value < 1024)
            return 1;
    }

    return 0;
}

int isLabel(char* str)
{
    regex_t regex;

    /*Verifica se eh formada por caracteres e '_' e termina em ':', comentario*/
    regcomp(&regex, "^[a-z_A-Z][a-z_A-Z0-9]+:$", REG_EXTENDED|REG_NOSUB);
    int retorno = !regexec(&regex, str, 0, NULL, 0);
    regfree(&regex);

    return retorno;
}

int isSymbol(char* str)
{
    regex_t regex;

    /*Verifica se eh formada por caracteres e '_', simbolo*/
    regcomp(&regex, "^[a-z_A-Z][a-z_A-Z0-9]+$", REG_EXTENDED|REG_NOSUB);
    int retorno = !regexec(&regex, str, 0, NULL, 0);
    regfree(&regex);

    return retorno;
}

int isComment(char* str)
{
    regex_t regex;

    /*Verifica se comeca com '#', um comentario*/
    regcomp(&regex, "^#", REG_EXTENDED|REG_NOSUB);
    int retorno = !regexec(&regex, str, 0, NULL, 0);
    regfree(&regex);

    return retorno;
}

int isCommand(char* str)
{
    regex_t regex;

    regcomp(&regex, "^[A-Z][a-zA-Z]+", REG_EXTENDED|REG_NOSUB);
    int retorno = !regexec(&regex, str, 0, NULL, 0);
    regfree(&regex);

    return retorno;
}

int isDirective(char* str)
{
    regex_t regex;

    regcomp(&regex, "^\\.[a-z]+$", REG_EXTENDED|REG_NOSUB);
    int retorno = !regexec(&regex, str, 0, NULL, 0);
    regfree(&regex);

    return retorno;
}
