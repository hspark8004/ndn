#include <stdio.h>
#include <time.h>

int main() {

    time_t timer;
    struct tm* t;

    timer = time(NULL);
//    localtime_s(&t, &timer);
    t = localtime(&timer);

    printf("%d-%d-%.d %.2d:%.2d:%.2d",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec
            );

}



