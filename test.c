#include <stdio.h>
#include <sys/time.h>
#include <time.h>

int main() {
    struct timeval tv;
    struct tm *tm_info;
    char buffer[64];

    gettimeofday(&tv, NULL);
    time_t now = tv.tv_sec;
    tm_info = localtime(&now);

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    printf("Current Time: %s.%06ld\n", buffer, tv.tv_usec);

    return 0;
}
