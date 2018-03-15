#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    setuid(0);
    argv[0] = "/usr/sbin/lpadmin";
    execvp("/usr/sbin/lpadmin",argv);
    return 0;
}
