#include <stdio.h>
#include <unistd.h>

int main() {
    if (fork() || fork()) 
        fork();
    
    printf("forked! ");
    printf("%d", fork());
    return 0;
}