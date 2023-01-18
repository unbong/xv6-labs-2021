#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char const *argv[])
{
    
    if(argc <= 1)
    {
        printf("sleep argument invalid.<sleep number of time>");
        exit(1);
    }

    int time = atoi(argv[1]);

    int n = sleep(time);
    if(n < 0)
    {
        printf("sleep syscal falied.<%d>", n);
        exit(1);
    }
    exit(0);
}

