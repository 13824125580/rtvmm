#include <linux/kernel.h>
#include <linux/module.h>

#include <vmm/vmm.h>

struct vmm_context* _vmm_context = NULL;
int vmm_status = 0;
EXPORT_SYMBOL(vmm_status);

void vmm_set_status(int status)
{
	vmm_status = status;
}
EXPORT_SYMBOL(vmm_set_status);

int vmm_get_status(void)
{
	return vmm_status;
}
EXPORT_SYMBOL(vmm_get_status);

void vmm_context_init(void* context_addr)
{
	_vmm_context = (struct vmm_context*)context_addr;
}
EXPORT_SYMBOL(vmm_context_init);

struct vmm_context* vmm_context_get(void)
{
	return _vmm_context;
}
EXPORT_SYMBOL(vmm_context_get);
