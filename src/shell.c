#include "../include/shell.h"

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
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n')
    {
        buffer[len - 1] = '\0';
    }
    return 1;
}

void parse_input(char *buffer, char **args)
{
    int i = 0;
    char *token;
    
    // strtok kullanarak boşluklara göre ayır
    token = strtok(buffer, " \t\n");
    while (token != NULL && i < MAX_INPUT_SIZE / 2)
    {
        args[i] = token;
        i++;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;  // Son eleman NULL olmalı
}

void execute_external_command(char **args) // ls , cat, vs
{
    if (args[0])
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork hatasi");
            return;
        }
        else if (pid == 0)
        {
            // child process basladi.komutu calistircaz simdi
            if (execvp(args[0], args) < 0)
            {
                perror("komut calistirilamadi");
                exit(EXIT_FAILURE);
            }
        }
        else if (pid > 0)
        {
            // parent proccess kısmı
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

void execute_output_redirect(char **args)
{
    if (args[0] == NULL)
        return;

    int i = 0;
    int append_mode = 0;
    char *filename = NULL;
    while (args[i] != NULL)
    {
        if (strcmp(args[i], ">>") == 0)
        {
            args[i] = NULL; // >> ve dosya adını temizledik.ona gore eklencek cunku dosyaya.
            if (args[i + 1] != NULL)
                filename = args[i + 1];
            append_mode = 1;
            break;
        }
        else if (strcmp(args[i], ">") == 0)
        {
            args[i] = NULL;
            if (args[i + 1] != NULL)
                filename = args[i + 1];
            break;
        }
        i++;
    }

    // Dosya adı kontrolü
    if (filename == NULL)
    {
        printf("Dosya adi belirtilmedi\n");
        return;
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork hatasi");
        return;
    }
    else if (pid > 0)
    {
        wait(NULL);
    }
    else if (pid == 0)
    {
        int control;
        if (append_mode == 1)
        {
            control = open(filename, O_WRONLY | O_APPEND | O_CREAT, 0644);
        }
        else if (append_mode == 0)
        {
            control = open(filename, O_WRONLY | O_TRUNC | O_CREAT, 0644);
        }
        if (control < 0)
        {
            perror("dosya acilamadi");
            exit(EXIT_FAILURE);
        }
        if (dup2(control, STDOUT_FILENO) < 0)
        {
            perror("yonlendirme hatasi");
            close(control);
            exit(EXIT_FAILURE);
        }
        if (execvp(args[0], args) < 0)
        {
            perror("komut calistiralamadi");
            exit(EXIT_FAILURE);
        }
    }
}
int main()
{
    char buffer[MAX_INPUT_SIZE];
    char *args[MAX_INPUT_SIZE / 2 + 1]; // her string tokeni icin birer pointer. yani işaretçi dizisi olusturdum.
    while (1)
    {
        take_input(buffer);

        if (strcmp(buffer, "exit") == 0 || strcmp(buffer, "quit") == 0)
        {
            printf("cikis yapiliyor");
            break;
        }
        parse_input(buffer, args);

        if (args[0] != NULL)
        {
            if (has_output_redirect(args))
            {
                execute_output_redirect(args);
            }
            else
                execute_external_command(args);
        }
    }

    return 0;
}