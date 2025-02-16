#include "shell.h"

int take_input(char *buffer)
{
    char *user = getenv("USER"); // getenv zaten char donduruyor.

    if (!user)
        user = "USER";

    printf("%s@shell>", user);
    char *kontrol = fgets(buffer, MAX_INPUT_SIZE, stdin);
    if (kontrol == NULL)
        return 0;

    // \n i kaldırma
    int len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n')
    {
        buffer[len - 1] = '\0';
    }
    return 1;
}

void parse_input(char *buffer, char **args)
{
    int i = 0;

    char *fake_buffer = buffer; // inputun başlangıcına işaret ettik
    char *temp_ptr;

    args[0] = fake_buffer; // "ls -l /home"
    i++;

    temp_ptr = strstr(buffer, " ");
    while (temp_ptr != NULL)
    {
        *temp_ptr = '\0'; // boslugu null ile degistirdik.
        //"ls\0-l /home"
        fake_buffer = temp_ptr + 1; // bir sonraki kelimeye gectik.

        while (*fake_buffer == '\0')
            fake_buffer++;           // diğer boslukları gectik ikk bosluktan sonrakileri mesela varsa eger.stringe gelince durur zaten
        if (*fake_buffer != '\0')    // iste stringe gelince null olmaycagi icin tokenlarımıza yenisini eklemis olduk.
            args[i++] = fake_buffer; // args[1] = -l /home baslangicini aldi. i 2 oldu.
                                     // bu sekilde tokenlarımızı almıs olucaz.
    }
    args[i] = NULL;
}

int main()
{
    char buffer[MAX_INPUT_SIZE];
    char *args[MAX_INPUT_SIZE / 2 + 1]; // her string tokeni icin birer pointer. yani işaretçi dizisi olusturdum.
    int i = 0;
    while (1)
    {
        take_input(buffer);

        if (strcmp(buffer, "exit") == 0 || strcmp(buffer, "quit") == 0)
        {
            printf("cikis yapiliyor");
            break;
        }
        parse_input(buffer, args);
        i = 0;
        while (args[i] != NULL)
        {
            printf("args[%d]: %s\n", i, args[i]);
            i++;
        }
    }

    return 0;
}