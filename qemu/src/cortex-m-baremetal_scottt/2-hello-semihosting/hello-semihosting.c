/* ARM semihosting allows an ARM device connected to a development machine
 * to make service requests.
 * See: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0471k/pge1358787045051.html */
static inline int __semihost(int nr_syscall, const void *arg);

/* See http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0471k/pge1358787045051.html */
enum {
	SYS_WRITE0 = 0x04,
	angel_SWIreason_ReportException = 0x18,
};

/* See http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0471k/pge1358787050566.html */
enum {
	ADP_Stopped_ApplicationExit = 0x20026,
};

/* main() is called by Reset_Handler() in startup.c */
int main()
{
	__semihost(SYS_WRITE0, "hello world\n");
	__semihost(angel_SWIreason_ReportException, (void*)ADP_Stopped_ApplicationExit);
	for (;;)
		;
	return 0;
}

static inline int __semihost(int nr_syscall, const void *arg)
{
	register int n __asm__ ("r0") = nr_syscall;
	register const void *a __asm__ ("r1") = arg;
	__asm__ volatile ("bkpt 0xab"
			: "+r" (n) /* Outputs */
			: "r" (a)  /* Inputs */
			: "r2", "r3", "ip", "lr", "memory", "cc"
			/* Clobbers r0 and r1, and lr if in supervisor mode */);
	/* We're being conservative with clobbered registers here and
	 * doing the same thing as newlib, which has the following comment:
	 *
	 * Accordingly to page 13-77 of ARM DUI 0040D other registers
	 * can also be clobbered.  Some memory positions may also be
	 * changed by a system call, so they should not be kept in registers. */
	return n;
}
