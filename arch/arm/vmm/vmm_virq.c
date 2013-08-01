#include <linux/bug.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/unwind.h>

#include <vmm/vmm.h>

#include "vmm_virhw.h"

/* VMM use the I bit in SPSR to save the virq status in the isr entry. So warn
 * on the I bit set would gave some false negative result. */
//#define VMM_WARN_ON_I_BIT

extern struct vmm_context* _vmm_context;

void vmm_disable_virq(void)
{
	unsigned long flags = vmm_irq_save();
	_vmm_context->virq_status = 0x01;
	vmm_irq_restore(flags);
}
EXPORT_SYMBOL(vmm_disable_virq);

static void _vmm_raise_on_pended(void)
{
	/* check any interrupt pended in vIRQ */
	if (_vmm_context->virq_pended) {
		/* trigger an soft interrupt */
		vmm_raise_softirq(RTT_VMM_IRQ_TRIGGER);
		return;
	}

#if 0
	int i;
	for (i = 0; i < ARRAY_SIZE(_vmm_context->virq_pending); i++) {
		if (_vmm_context->virq_pending[i]) {
			_vmm_context->virq_pended = 1;
			pr_info("\n");
			vmm_raise_softirq(RTT_VMM_IRQ_TRIGGER);
			return;
		}
	}
#endif
}

void vmm_enable_virq(void)
{
	unsigned long flags = vmm_irq_save();
	_vmm_context->virq_status = 0x00;
	_vmm_raise_on_pended();
	vmm_irq_restore(flags);
}
EXPORT_SYMBOL(vmm_enable_virq);

unsigned long vmm_return_virq(void)
{
	unsigned long flags;
	unsigned long level;

	level =  vmm_irq_save();
	flags = _vmm_context->virq_status;
	vmm_irq_restore(level);

	return flags;
}
EXPORT_SYMBOL(vmm_return_virq);

unsigned long vmm_save_virq(void)
{
	int status;
	unsigned long flags = vmm_irq_save();

	status = _vmm_context->virq_status;
	_vmm_context->virq_status = 0x01;
	vmm_irq_restore(flags);

	return status;
}
EXPORT_SYMBOL(vmm_save_virq);

void vmm_restore_virq(unsigned long flags)
{
	unsigned long level;

	level =  vmm_irq_save();
	_vmm_context->virq_status = flags;
	if (_vmm_context->virq_status == 0)
	{
		_vmm_raise_on_pended();
	}
	vmm_irq_restore(level);
}
EXPORT_SYMBOL(vmm_restore_virq);

unsigned long vmm_save_virq_spsr_asm(unsigned long spsr, struct pt_regs *regs)
{
	if (vmm_status) {
		if (_vmm_context->virq_status)
			return spsr | PSR_I_BIT;
	}
	return spsr;
}

void irq_enable_asm(void)
{
	if (vmm_status) {
		vmm_enable_virq();
	} else {
		asm volatile("cpsie i" : : : "memory", "cc");
	}
}

void irq_disable_asm(void)
{
	if (vmm_status) {
		vmm_disable_virq();
	} else {
		asm volatile("cpsid i" : : : "memory", "cc");
	}
}

/* should be called when the guest entering the state that the IRQ is disabled
 * by hardware, for example, entering SVC, PABT, DABT mode.
 *
 * It will the open the hardware IRQ, virtual IRQ remain unchanged.
 */
void vmm_switch_nohwirq_to_novirq(void)
{
	if (vmm_status) {
		vmm_disable_virq();
		asm volatile("cpsie i" : : : "memory", "cc");
	}
}

unsigned long vmm_restore_virq_asm(unsigned long spsr)
{
	if (vmm_status) {
#ifdef VMM_WARN_ON_I_BIT
		WARN(spsr & PSR_I_BIT, "return to svc mode with I in SPSR set\n");
#endif
		vmm_restore_virq(!!(spsr & PSR_I_BIT));
		return spsr & ~PSR_I_BIT;
	} else {
		return spsr;
	}
}

void vmm_on_ret_to_usr(unsigned long spsr)
{
	if (vmm_status) {
#ifdef VMM_WARN_ON_I_BIT
		WARN(spsr & PSR_I_BIT, "return to user mode with I in SPSR set\n");
#endif
		vmm_enable_virq();
	}
}

void vmm_on_svc_exit_irq(unsigned long spsr)
{
	if (vmm_status) {
#ifdef VMM_WARN_ON_I_BIT
		WARN(spsr & PSR_I_BIT, "exit IRQ with I in SPSR set\n");
#endif
		vmm_enable_virq();
	}
}

void vmm_dump_irq(void)
{
	int i;
	unsigned long cpsr;

	asm volatile ("mrs %0, cpsr": "=r"(cpsr));

	printk("status: %08lx, pended: %08lx, cpsr: %08lx\n",
	       _vmm_context->virq_status, _vmm_context->virq_pended, cpsr);
	printk("pending: ");
	for (i = 0; i < ARRAY_SIZE(_vmm_context->virq_pending); i++) {
		printk("%08lx, ", _vmm_context->virq_pending[i]);
	}
	printk("\n");
}

