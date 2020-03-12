#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

int main()
{
  struct timeval tv;
  struct timespec ts;
  time_t current_time;
  time_t current_time2;
  int i = 0;

  while(1) {
  	++i;
    gettimeofday(&tv, NULL);
    current_time = time(NULL);
    time(&current_time2);
    clock_gettime(CLOCK_REALTIME, &ts);
    printf("--------------------------%1d------------------------\n", i);
    printf("time (NULL)   : %10ld %s", current_time, ctime(&current_time));
    printf("time (&var)   : %10ld %s", current_time2, ctime(&current_time2));
    printf("gettimeofday  : %10ld %s", tv.tv_sec, ctime(&tv.tv_sec));
    printf("clock_gettime : %10ld %s", ts.tv_sec, ctime(&ts.tv_sec));
    printf("---------------------------------------------------\n");
    if (i<3) sleep(5); else break;  
  }

}
