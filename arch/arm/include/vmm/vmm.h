#ifndef __LINUX_VMM_H__
#define __LINUX_VMM_H__

#include <linux/compiler.h>

#include "vmm_config.h"

struct irq_domain;
struct pt_regs;

extern int vmm_status;
extern struct vmm_context *_vmm_context;

/* VMM context routines */
void vmm_context_init(void* context);
struct vmm_context* vmm_context_get(void);

void vmm_set_status(int status);
int vmm_get_status(void);

void vmm_mem_init(void);
void vmm_raise_softirq(int irq);

/* VMM vIRQ routines */
unsigned long vmm_save_virq(void);
unsigned long vmm_return_virq(void);

void vmm_restore_virq(unsigned long flags);
void vmm_enable_virq(void);
void vmm_disable_virq(void);
void vmm_enter_hw_noirq(void);

void vmm_raise_softirq(int irq);

#endif
