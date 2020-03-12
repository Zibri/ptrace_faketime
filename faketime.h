#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <string.h>


#define WSTOPEVENT(STATUS) (((STATUS) & 0xff0000) >> 16)

#if defined(__x86_64__)

/* On x86-64, RAX is set to -ENOSYS on system call entry.  How
   do we distinguish this from a system call that returns
   ENOSYS?  (marek: we don't) */
#define SYSCALL_ENTRY ((long)RET == -ENOSYS)
#define REGS_STRUCT struct user_regs_struct

#define SYSCALL (regs.orig_rax)
#define ARG1 (regs.rdi)
#define ARG2 (regs.rsi)
#define ARG3 (regs.rdx)
#define ARG4 (regs.r10)
#define ARG5 (regs.r8)
#define ARG6 (regs.r9)
#define RET (regs.rax)

//#define SYSCALL_clock_gettime 228
//#define SYSCALL_time 201

#elif defined(__i386__)

#define SYSCALL_ENTRY (RET == -ENOSYS)
#define REGS_STRUCT struct user_regs_struct
#define SYSCALL (regs.orig_eax)
#define ARG1 (regs.ebx)
#define ARG2 (regs.ecx)
#define ARG3 (regs.edx)
#define ARG4 (regs.esi)
#define ARG5 (regs.edi)
#define ARG6 (regs.ebp)
#define RET (regs.eax)

//#define SYSCALL_clock_gettime 265

#elif defined(__arm__)

/*
 * This struct defines the way the registers are stored on the
 * stack during a system call.  Note that sizeof(struct pt_regs)
 * has to be a multiple of 8.
 */

#if __thumb__
#define SYS_BASE 0
#else
#define SYS_BASE 0x900000
#endif

#define REGS_STRUCT struct pt_regs
/* ip is set to 0 on system call entry, 1 on exit.  */
#define SYSCALL_ENTRY (regs.ARM_ip == 0)

/* This layout assumes that there are no 64-bit parameters.  See
   http://lkml.org/lkml/2006/1/12/175 for the complications.  */
#define SYSCALL (regs.ARM_r7)
#define ARG1 (regs.ARM_ORIG_r0)
#define ARG2 (regs.ARM_r1)
#define ARG3 (regs.ARM_r2)
#define ARG4 (regs.ARM_r3)
#define ARG5 (regs.ARM_r4)
#define ARG6 (regs.ARM_r5)
#define RET (regs.ARM_r0)

//#define SYSCALL_clock_gettime (SYS_BASE+263)
#else

#error Not ported to your architecture.

#endif
