#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"



int main(int argc, char const *argv[])
{
    
    
    int p_c[2]; // pipe to child
    int c_p[2]; // pipe to parent
    //int p_p[2]; // pipe to parent

    pipe(p_c);
    pipe(c_p);
    
    if(  fork() == 0){

        // 关闭子到父的写;
        close(c_p[0]);
        close(p_c[1]);
        // close(0);
        // dup(p[0]);
        // close(p[0]);

        int pid = getpid();  
        fprintf(c_p[1], "%d" , pid );
        //int n = write(c_p[1], &pid, 3);
        // if(n < 0){
        //     printf("child write failed\n");
        //     exit(1);
        // }


        char c_pid[10];
        int n = read(p_c[0], c_pid, 10);
        if(n < 0){
            printf("child read from pipe failed\n");
            exit(1);
        }
        //sleep(30);
        fprintf(1, "%s received pong\n", c_pid);
        
        exit(0);
    }

    close(c_p[1]);
    close(p_c[0]);
    int pid = getpid();  

    fprintf(p_c[1], "%d" , pid );
    wait(0);

    char c_pid[10];
    read(c_p[0], c_pid, 10);

    fprintf(1, "%s received ping\n", c_pid);
    exit(0);
}
