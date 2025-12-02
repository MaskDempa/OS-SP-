#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    printf("=== Process Information ===\n");
    printf("Current Process ID (PID): %d\n", getpid());
    printf("Parent Process ID (PPID): %d\n", getppid());
    printf("Process Group ID: %d\n", getpgid(0));
    printf("Real User ID: %d\n", getuid());
    printf("Real Group ID: %d\n", getgid());
    printf("Effective User ID: %d\n", geteuid());
    printf("Effective Group ID: %d\n", getegid());
    
    return 0;
}