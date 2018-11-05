#include "ifttt.h"
#include <ctype.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#define pre "/sys/bus/w1/devices/"
#define device "28-01131bc32868"
#define nm "/w1_slave"
#define low "lowest temperature is"
#define high "highest temperature is"
#define nows "current temperature is"
double gettemp(int fd, char *buff);
int main(int argc, char **argv)
{
    char *d;

    int fd;
    char *file;
    char buff[500];
    double tmp;
    double nexttmp;
    double tl, th;
    double sent;
    char *l;
    char *h;
    char *now;
    time_t star;
    if (argc < 2)
    {
		d = device;
        printf("pls input you device code");
    }
    else
    {
        printf("%s\n", argv[1]);
        d = argv[1];
    }

    if ((file = malloc(strlen(pre) + strlen(d) + strlen(nm) + 1)) == NULL)
    {
        printf("fail to allocat file name");
        return 1;
    }
    sprintf(file, "%s%s%s", pre, d, nm);
    printf("%s", file);
    fd = open(file, O_RDONLY);
    free(file);
    if (fd < 0)
    {
        printf("fail to open file");
        return 1;
    }

    tmp = gettemp(fd, buff);
    printf("\n%5.3f\n", tmp);
    tl = th = sent = tmp;
    l = malloc(strlen(low) + 7);
    h = malloc(strlen(high) + 7);
    now = malloc(strlen(nows) + 7);
    sprintf(l, "%s%5.3f", low, tl);
    sprintf(h, "%s%5.3f", high, th);
    sprintf(now, "%s%5.3f",nows, tmp);
    ifttt("https://maker.ifttt.com/trigger/alarm_triggered/with/key/dcZuaJf3oHEAyHhu-55bJE", now, h, l);

    while (lseek(fd, 0, SEEK_SET) != -1)
    {
        star = time(NULL);
        while (difftime(time(NULL), star) < 1)
        {

            if (read(fd, buff, 4096) < 0)
            {
                printf("read nothing or fail");
                close(fd);
                return 1;
            }

            nexttmp = gettemp(fd, buff);
            tl = (nexttmp < tl) ? nexttmp : tl;
            th = (nexttmp > th) ? nexttmp : th;
            if (nexttmp > (sent + 1.0) || nexttmp < (sent - 1.0))
            {
                tmp = nexttmp;
                sent = tmp;
                printf("\n%5.3f\n", tmp);
                sprintf(l, "%s%5.3f", low, tl);
                sprintf(h, "%s%5.3f", high, th);
                sprintf(now, "%s%5.3f", nows, tmp);
                ifttt("https://maker.ifttt.com/trigger/alarm_triggered/with/key/dcZuaJf3oHEAyHhu-55bJE", now, h, l);
            }
            else
            {
                tmp = nexttmp;
                printf("\n%5.3f\n", tmp);
            }
        }
    }
    printf("inner shift wrong");

    printf("is %s\n", buff);
    close(fd);

    return 0;
}

double gettemp(int fd, char buff[])
{
    char *t;
    int sign;
    double temp;
    if (read(fd, buff, 4096) < 0)
    {
        printf("read nothing or fail");
        close(fd);
        return 1;
    }

    if (strstr(buff, "YES") == NULL)
    {
        printf("crc not work");
        close(fd);
        return 1;
    }

    if ((t = strstr(buff, "t=")) == NULL)
    {
        printf("t not found");
        close(fd);
        return 1;
    }

    t += 2;

    if (*t == '-')
    {
        sign = -1;
        ++t;
    }
    else
    {
        sign = 1;
    }

    temp = 0.0;
    while (isdigit(*t))
    {
        temp = temp * 10 + (*t - '0');
        ++t;
    }
    temp = temp / 1000;
    return temp;
}

