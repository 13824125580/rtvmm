#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/io.h>
#include <asm/hardware/gic.h>

#include <vmm/vmm.h>

void vmm_raise_softirq(int irq)
{
	gic_raise_softirq(cpumask_of(0),  irq);
}
EXPORT_SYMBOL(vmm_raise_softirq);
