#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

/// @brief  检索key是否在dir中如果存在返回1 不存在0
/// @param dir 
/// @param key 
/// @return 
int search(char * dir , char * key)
{
    char * dir2 = dir;
    char * key2;
    int index=0;

    while(*dir2 != 0){

        key2 = key;
        while(*key2 != 0)
        {
            if(*dir2 == *key2)
            {
                dir2++;
                key2++;
                index++;
            }
            else{
                dir2++;
                index++;
                break;
            }
        }   

        if(*key2 == 0)
        {
            return 1;
        }
    }

    return 0;
}

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}


int grabfolder(char * dir, char * key)
{

    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    //printf("%s \n", dir);
    if((fd = open(dir, 0))< 0)
    {
        fprintf(2, "first argu is invaild.\n<%s>", dir);
        exit(1);
    }

    if(fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", dir);
        close(fd);
        exit(1);
    }

    switch (st.type)
    {
    case T_FILE:
        /* code */
        if (search(fmtname(dir), key )){
            printf("%s\n", dir);
        }
        break;
    
    case T_DIR:
        if(strlen(dir) + 1 + DIRSIZ+1 > sizeof buf)
        {
            printf("path too long\n");
            break;
        }

        strcpy(buf, dir);
        p = buf + strlen(buf);
        *p++ = '/';

        while(read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if(de.inum == 0 || 
            !strcmp(de.name, ".") || 
            !strcmp(de.name, "..")  )
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if(stat(buf, &st)<0)
            {
                printf("cannot stat %s\n", buf);
                continue;
            }
            // if(search(fmtname(buf), key))
            // {
            //     printf("%s %s", fmtname(buf), key);
            // }
            
            grabfolder(buf, key);
        }
        break;
    default:
        break;
    }
    close(fd);
    return 0;
}

int main(int argc, char *argv[])
{
    /* code */

    // 检查参数个数

    if(argc != 3)
    {
        printf("find arg invalid \n");
    }

    char * dir = argv[1];
    char * key = argv[2];
    // int n ;
    // if( ( n = search(dir, key)))
    // {   
    //     fprintf(1, "n: %d", n);
    // }
    // else {
    //     fprintf(2, "n: %d", n);
    // }

    int n = grabfolder(dir, key);
    if(n < 0)
    {
        fprintf(2, "find error\n");
        exit(-1);
    }

    exit( 0);
}
