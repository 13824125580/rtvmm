#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/io.h>

#include <vmm/vmm.h>
#include "../vmm_virhw.h"
#include "intc.h"

void vmm_raise_softirq(int irq)
{
	writel_relaxed(1 << (irq % 32),
		OMAP3_IRQ_BASE + INTC_SIR_SET0 + (irq / 32) * 4);
}
EXPORT_SYMBOL(vmm_raise_softirq);
