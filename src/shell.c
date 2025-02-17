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

// eski kendi yazdigim parse_input fonksiyonum segmentasyon fault yedi. ai strok kullanmamın daha guvenli olacagınıs öyledi.sebeplerini öğrendim.eski fonksiyonum explanis.txt kisminda

void parse_input(char *buffer, char **args)
{
    int i = 0;
    char *token;
    
    // strtok kullanarak boşluklara göre tokenlarimizi alalim.
    token = strtok(buffer, " \t\n");
    while (token != NULL && i < MAX_INPUT_SIZE / 2)
    {
        args[i] = token;
        i++;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;  // Son eleman NULL yapalım.
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

void execute_pipe_command(char **args)
{
    pid_t pid1,pid2;
    char **second_cmd;
    int pipe_found = 0;

    int i = 0;
    while(args[i] != NULL)
    {
        if(strcmp(args[i],"|") == 0)
        {
            args[i] = '\0';
            second_cmd = &args[i + 1];
            pipe_found = 1;
            break;
        }
        i++;
    }
    if(!pipe_found || second_cmd[0] == NULL)
    {
        printf("pipe komutu hatasi \ eksik arguman");
        return;
    }

    int pipearr[2];
    if(pipe(pipearr) < 0)
    {
        perror("pipe olusturulamadi");
        exit(EXIT_FAILURE);
    }
    pid1 = fork();
    if(pid1 < 0)
    {
        perror("fork1 hatasi");
        exit(EXIT_FAILURE);
    }
    else if(pid1 == 0)
    {
        close(pipearr[0]); // okuma ucunu kapadik.burda yazma kullaniaz cunku
        dup2(pipearr[1],STDOUT_FILENO);
        close(pipearr[1]);
        
        
        if(execvp(args[0],args) < 0)
        {
            perror("ilk komut calistirilamadi");
            exit(EXIT_FAILURE);
        }
    
    
    }
    else if(pid1 > 0)
    {
        pid2 = fork();
        if(pid2 < 0)
        {
            perror("fork2 hatasi");
            exit(EXIT_FAILURE);
        }
        else if(pid2 == 0)
        {
            close(pipearr[1]);
            dup2(pipearr[0],STDIN_FILENO);
            close(pipearr[0]);
            if(execvp(second_cmd[0],second_cmd) < 0)
            {
                perror("ikinci komut calistirilamadi");
                exit(EXIT_FAILURE);
            }
        }
        else if(pid2 > 0)
        {
            close(pipearr[0]);
            close(pipearr[1]);
            waitpid(pid1,NULL,0);
            waitpid(pid2,NULL,0);

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
            else if(has_pipe(args))
                execute_pipe_command(args);
            else
                execute_external_command(args);
        }
    }

    return 0;
}