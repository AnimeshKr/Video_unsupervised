
#include<cstdio>
#include<stdlib.h>

int main()
{
    char *s = "echo $LD_LIBRARY_PATH";
    system(s);
    return 0;
}


