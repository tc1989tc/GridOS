/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   X86线程描述
*/

#ifndef ARCH_THREAD_H
#define ARCH_THREAD_H

#include <types.h>
#include <linkage.h>
#include <asm/processor.h>

#define ARCH_THREAD_ARCH_X86_32_DEFAULT_EFLAGS ((1<<1)/*固定位*/|X86_EFLAGS_ID| X86_EFLAGS_IF | X86_EFLAGS_RF|X86_EFLAGS_IOPL/*最高的IO权限*/)
struct kt_arch_thread {
	struct thread_struct ctx;
};

/**
*	线程切换代码
*/
#define x86_thread_switch_to(prev, next)					\
do {															\
	/*															\
	* Context-switching clobbers all registers, so we clobber	\
	* them explicitly, via unused output variables.				\
	* (EAX and EBP is not listed because EBP is saved/restored	\
	* explicitly for wchan access and EAX is the return value of\
	* __switch_to())											\
	*/															\
	unsigned long ebx, ecx, edx, esi, edi;						\
	asm volatile("pushfl\n\t"		/* save    flags */			\
			 "pushl %%ebp \n\t"									\
			 "pushl %%fs \n\t"		/*FS在线程切换时保存，而不是在汇编层保存，否则导致中断恢复FS时而MEM_CTX变化后无法恢复*/ \
		     "movl %%esp,%[prev_sp]\n\t"	/* save    ESP   */ \
		     "movl %[next_sp],%%esp\n\t"	/* restore ESP   */ \
		     "movl $1f,%[prev_ip]\n\t"	/* save    EIP   */		\
		     "pushl %[next_ip]\n\t"	/* restore EIP   */			\
		      													\
		     "jmp arch_thread_switch\n"	/* regparm call  */	\
		     "1:\t"												\
			 "popl %%fs \n\t"									\
			 "popl %%ebp \n\t "									\
		     "popfl\n"			/* restore flags */				\
																\
		     /* output parameters */							\
		     : [prev_sp] "=m" (prev->arch_thread.ctx.sp),		\
		       [prev_ip] "=m" (prev->arch_thread.ctx.ip),		\
		      													\
			 /* clobbered output registers: */					\
			 "=b" (ebx), "=c" (ecx), "=d" (edx),				\
			 "=S" (esi), "=D" (edi)								\
																\
			 /* input parameters: */							\
		     : [next_sp]  "m" (next->arch_thread.ctx.sp),		\
		       [next_ip]  "m" (next->arch_thread.ctx.ip),		\
		       													\
			   /* regparm parameters for arch_thread_switch(): */		\
			   [prev]     "a" (prev),				\
			   [next]     "d" (next)				\
													\
		     : /* reloaded segment registers */		\
			"memory");								\
} while (0)


/************************************************************************/
/* ARCH specified                                                       */
/************************************************************************/
#define KT_ARCH_THREAD_CP0_STACK_SIZE (PAGE_SIZE * 4)


/* 用硬件加速的方法获取到当前的线程 */
static __always_inline void * kt_arch_get_current()
{
	void *current;
#if defined(__i386__)
	/* 思想来自wine NtCurrentTeb接口 */
	__asm__ volatile(".byte 0x64\n\tmovl (0),%0" : "=r" (current));
#else
#error "Thread.h not supports the X86-64"
#endif
	/* see km_cpu */
	return current;
}

/* 用硬件加速的方法获取到CPU指针 */
static __always_inline void * kt_arch_get_cpu()
{
	void * cpu;
#if defined(__i386__)
	__asm__ volatile(".byte 0x64\n\tmovl (4),%0" : "=r" (cpu));
#else
#error "Thread.h not supports the X86-64"
#endif
	/* see km_cpu */
	return cpu;
}

struct ko_thread;
struct kt_thread_creating_context;

static __always_inline unsigned long kt_arch_get_sp0(struct ko_thread * who)
{
	struct kt_arch_thread * p = (struct kt_arch_thread*)who;
	return p->ctx.sp0;
}
static __always_inline void kt_arch_set_sp0(struct ko_thread * who, unsigned long sp0)
{
	struct kt_arch_thread * p = (struct kt_arch_thread*)who;
	p->ctx.sp0 = sp0;
}


#endif
