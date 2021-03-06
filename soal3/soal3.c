#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <time.h>
#include <sys/prctl.h>

#define KILL "\
#!/bin/bash\n\
pkill soal3\n\
rm Killer.sh\n\
"

// #define TERM "\
// #!/bin/bash\n\
// pkill soal3\n\
// rm Killer.sh\n\
// "

void daemonSkeleton()
{
    pid_t pid, sid;

    pid = fork();

    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    umask(0);

    sid = setsid();
    if (sid < 0)
    {
        exit(EXIT_FAILURE);
    }

    // if ((chdir("/")) < 0)
    // {
    //     exit(EXIT_FAILURE);
    // }

    // close(STDIN_FILENO);
    // close(STDOUT_FILENO);
    // close(STDERR_FILENO);
}

void caesarShift(char word[], int key)
{
    char ch;
    for (int i = 0; word[i] != '\0'; ++i)
    {
        ch = word[i];

        if (ch >= 'a' && ch <= 'z')
        {
            ch = ch + key;

            if (ch > 'z')
            {
                ch = ch - 'z' + 'a' - 1;
            }

            word[i] = ch;
        }
        else if (ch >= 'A' && ch <= 'Z')
        {
            ch = ch + key;

            if (ch > 'Z')
            {
                ch = ch - 'Z' + 'A' - 1;
            }

            word[i] = ch;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc == 2 && strcmp(argv[1], "-z") == 0)
    {
        //Killer bash program
        FILE *fp = NULL;
        fp = fopen("Killer.sh", "w");
        fputs(KILL, fp);
        fclose(fp);
    }
    else if (argc == 2 && strcmp(argv[1], "-x") == 0)
    {
        //Killer bash program
        FILE *fp = NULL;
        fp = fopen("Killer.sh", "w");
        fprintf(fp, "#!/bin/bash\nkill %d\nrm Killer.sh\n", getpid() + 1);
        fclose(fp);
    }

    else
    {
        printf("Argumen salah!\nMasukkan \"-z\" sebagai argumen 1 atau \"-x\" sebagai argumen 2!\n");

        exit(0);
    }

    daemonSkeleton();

    while (1)
    {
        //untuk mendapatkan waktu saat program dieksekusi
        time_t rawtime;
        struct tm *timeinfo;
        char stringTime[sizeof "YYYY-MM-DD_HH:MM:SS"];
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(stringTime, sizeof(stringTime), "%Y-%m-%d_%X", timeinfo);

        int status0;

        if (fork() == 0)
        {
            //membuat direktori baru dengan nama waktu yang telah ditentukan
            char *argv[] = {"mkdir", stringTime, NULL};
            execv("/bin/mkdir", argv);
        }

        sleep(1);

        // while (wait(&status0) > 0)
        //     ;

        if (fork() == 0)
        {
            //masuk ke direktori yang telah dibuat
            chdir(stringTime);

            for (int i = 0; i < 10; i++)
            {
                time_t rawtime2;
                struct tm *timeinfo2;
                char stringTime2[sizeof "YYYY-MM-DD_HH:MM:SS"];
                time(&rawtime2);
                timeinfo2 = localtime(&rawtime2);
                // strftime(stringTime2, sizeof(stringTime2), "%Y-%m-%d_%X", timeinfo2);

                time_t now = time(NULL);
                struct tm now_tm = *localtime(&now);
                struct tm then_tm = now_tm;
                then_tm.tm_sec -= 1;
                mktime(&then_tm);
                strftime(stringTime2, sizeof(stringTime2), "%Y-%m-%d_%X", &then_tm);

                char url[40];
                //modifikasi string url agar bisa download file sesuai kriteria
                sprintf(url, "https://picsum.photos/%ld", ((now - 1) % 1000) + 50);

                if (fork() == 0)
                {
                    char *argv[] = {"wget", url, "-O", stringTime2, NULL};
                    execv("/usr/bin/wget", argv);
                }
                sleep(5);
            }

            char statusMessage[] = {"Download Success"};
            //caesar cypher 5
            caesarShift(statusMessage, 5);
            //printf("\n\n%s\n\n", status);

            //masukkan kedalam file
            FILE *fp = NULL;
            fp = fopen("status.txt", "w");
            fprintf(fp, "%s", statusMessage);
            fclose(fp);

            //kembali ke direktori sebelumnya
            chdir("..");

            //membuat string dengan nama file untuk melakukan zip
            char zipName[40];
            strcpy(zipName, stringTime);
            strcat(zipName, ".zip");

            //melakukan zip direktori stringTime dengan format nama zipName
            char *argv[] = {"zip", "-rm", zipName, stringTime, NULL};
            execv("/usr/bin/zip", argv);
        }
        sleep(39);
    }
}