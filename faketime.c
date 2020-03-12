#define _GNU_SOURCE

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/syscall.h>        /* For SYS_write etc */

#include "faketime.h"

#define P(B64) (fprintf(stderr, "%s: %lld\n", #B64, B64))
#ifdef DEBUG
#define LOG(TIME) (fprintf(stderr, "UNIX Time returned by syscall: %lld\n", TIME))
#else
#define LOG(TIME)
#endif

int main(int argc, char *argv[])
{
  pid_t child;
  int status;
  struct user_regs_struct regs;
  int counter = 0;
  int in_call1 = 0;
  int in_call2 = 0;
  int in_call3 = 0;
  unsigned long now = (unsigned long) time(NULL);

  if (argc < 3) {
    fprintf(stderr, "Usage:   %s unixtime command\n", basename(argv[0]));
    fprintf(stderr, "Example: %s 2147483647 date\n", basename(argv[0]));
    return -1;
  }

  unsigned long newtime = atoi(argv[1]);

  child = fork();
  if (child == 0) {
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execvp(argv[2], argv + 2);
  } else {

    ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD | PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORK | PTRACE_O_TRACECLONE | PTRACE_O_TRACEEXEC | PTRACE_O_TRACEEXIT);
    fprintf(stderr, "Child started %d\n", child);
    while (1) {
      wait(&status);
      if (WIFEXITED(status) || WIFSIGNALED(status)) {
        fprintf(stderr, "Child exited %d\n", child);
        break;
      }
      ptrace(PTRACE_GETREGS, child, NULL, &regs);
      if (SYSCALL == __NR_time) {
        in_call1 ^= 1;
        if (in_call1 == 0) {
          if (!ARG1) {
            LOG(RET);
            RET = time(NULL) - now + newtime;
            ptrace(PTRACE_SETREGS, child, NULL, &regs);
          } else {
            long long oldtime = ptrace(PTRACE_PEEKDATA, child, ARG1, NULL);
            LOG(oldtime);
            ptrace(PTRACE_POKEDATA, child, ARG1, (unsigned long) time(NULL) - now + newtime);
          }
        }
      } else if (SYSCALL == __NR_clock_gettime) {
        in_call2 ^= 1;
        if (in_call2 == 0) {
          if ((ARG1|1)==1) {
            long long oldtime = ptrace(PTRACE_PEEKDATA, child, ARG2, NULL);
            LOG(oldtime);
            ptrace(PTRACE_POKEDATA, child, ARG2, (unsigned long) time(NULL) - now + newtime);
          }
        }
      } else if (SYSCALL == __NR_gettimeofday) {
        in_call3 ^= 1;
        if (in_call3 == 0) {
          long long oldtime = ptrace(PTRACE_PEEKDATA, child, ARG1, NULL);
          LOG(oldtime);
          ptrace(PTRACE_POKEDATA, child, ARG1, (unsigned long) time(NULL) - now + newtime);
        }
      }
      counter += in_call1 + in_call2 + in_call3;

      ptrace(PTRACE_SYSCALL, child, NULL, NULL);
    }
  }
  return 0;
}
