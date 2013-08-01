#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/irqdomain.h>

#include <asm/io.h>
#include <asm/irq.h>

#include <vmm/vmm.h>
#include "../vmm_virhw.h"
#include "intc.h"

void vmm_irq_handle(void __iomem *base_addr, struct irq_domain *domain,
				 struct pt_regs *regs)
{
	unsigned long flags;
	struct vmm_context* _vmm_context;

	_vmm_context = vmm_context_get();

	while (_vmm_context->virq_pended) {
		int index;

		flags = vmm_irq_save();
		_vmm_context->virq_pended = 0;
		vmm_irq_restore(flags);

		/* get the pending interrupt */
		for (index = 0; index < IRQS_NR_32; index++) {
			int pdbit;

			for (pdbit = __builtin_ffs(_vmm_context->virq_pending[index]);
			     pdbit != 0;
			     pdbit = __builtin_ffs(_vmm_context->virq_pending[index])) {
				unsigned long inner_flag;
				int irqnr;

				pdbit--;

				inner_flag = vmm_irq_save();
				_vmm_context->virq_pending[index] &= ~(1 << pdbit);
				vmm_irq_restore(inner_flag);

				irqnr = irq_find_mapping(domain, pdbit + index * 32);
				handle_IRQ(irqnr, regs);
			}
		}
	}
}
