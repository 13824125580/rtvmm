#ifndef __VMM_VIRTHWH__
#define __VMM_VIRTHWH__

#define REALVIEW_NR_IRQS        96
#define IRQS_NR_32              ((REALVIEW_NR_IRQS + 31)/32)
#define RTT_VMM_IRQ_TRIGGER     10

struct vmm_context
{
	/* the status of vGuest irq */
	volatile unsigned long virq_status;

	/* has interrupt pended on vGuest OS IRQ */
	volatile unsigned long virq_pended;

	/* pending interrupt for vGuest OS */
	volatile unsigned long virq_pending[IRQS_NR_32];
};

/* IRQ operation under VMM */
static inline unsigned long vmm_irq_save(void)
{
	unsigned long flags;

	asm volatile(
		"	mrs	%0, cpsr	@ arch_local_irq_save\n"
		"	cpsid	i"
		: "=r" (flags) : : "memory", "cc");
	return flags;
}

static inline void vmm_irq_restore(unsigned long flags)
{
	asm volatile(
		"	msr	cpsr_c, %0	@ local_irq_restore"
		:
		: "r" (flags)
		: "memory", "cc");
}

static inline void vmm_irq_enable(void)
{
	asm volatile(
		"	cpsie i			@ arch_local_irq_enable"
		:
		:
		: "memory", "cc");
}

static inline void vmm_irq_disable(void)
{
	asm volatile(
		"	cpsid i			@ arch_local_irq_disable"
		:
		:
		: "memory", "cc");
}

#endif
