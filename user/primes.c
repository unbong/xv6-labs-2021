#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

 /// @brief   读取一个上一个进程传递的第一个值后，作为过滤的值，并循环读取父进程传递过来的值，如果不能被整除，传递给下一个进程。
 /// @param p_a 
 void filter(int p_a)
{
    
    //fprintf(1, "pid: %d in start\n",getpid());
    // 读取父进程传过来的第一个数， 如果父进程关闭了管道则不再创建子进程
    int filnum = 0;
    int n = read(p_a, &filnum, 1);

    if(filnum != 0)
    {
        fprintf(1, "prime %d\n",filnum);
        //fprintf(1, "filnum: %d, n: %d\n",filnum, n);
    }
        
        
    if( n <= 0)
    {
        close(p_a);
        return ;
    }
    // 创建管道
    int p_c[2];
    pipe(p_c);

    if(fork() == 0)
    {
        close(p_c[1]);
        filter(p_c[0]);
        //close(p_c[0]);
        //wait(0);
        exit(0);
    }

    // 读取父进程通过管道传递的值
    int  num;
    while(read(p_a, &num, 1)){
        
        //fprintf(1,"num %d\n", num);
        // 如果有余数则传递给子进程
        if(num % filnum != 0)
        {
            //fprintf(1,"!=0 %d\n", num);
            write(p_c[1], &num, 1);
        }
    }

    close(p_a);
    close(p_c[1]);
    close(p_c[0]);
    //fprintf(1, "pid: %d in end\n",getpid());
    wait(0);
    exit(0);
}


int main(int argc, char const *argv[])
{

    int p[2];
    pipe(p);

    if(fork() ==0)
    {
        close(p[1]);
        // int num;
        // while(read(p[0], &num, 1))
        // {
        //     fprintf(1, " fork %d\n", num);
        // }
        // exit(0);

        filter(p[0]);
    }
    
    for( int i = 2; i<=35; i++)
    {
        write(p[1], &i, 1);
    }

    close(p[1]);
    close(p[0]);
    wait(0);
    exit(0);
}
