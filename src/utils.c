#include "../include/shell.h"

char *get_output_filename(char **args)
{
    int i = 0;
    while (args[i] != NULL)
    {
        if (strcmp(args[i], ">>") == 0)
            return args[i + 1]; // >> dan sornaki dosya adini dönderdik
        if(strcmp(args[i],">") == 0)
            return args[i + 1];
    }
    return NULL;
}

bool has_output_redirect(char **args)
{
    int i = 0;

    while (args[i] != NULL)
    {
        if (strcmp(args[i], ">>") == 0)
            return true;
        else if( strcmp(args[i],">") == 0)
            return true;
        i++;
    }
    return false;
}