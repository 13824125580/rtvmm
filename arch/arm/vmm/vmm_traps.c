#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/traps.h>
#include <asm/cp15.h>
#include <asm/cacheflush.h>

void trap_set_vector(void *start, unsigned int length)
{
	unsigned char *ptr;
	unsigned char *vector;

	ptr = start;
	vector = (unsigned char*)vectors_page;

	/* only set IRQ and FIQ */
#if defined(CONFIG_CPU_USE_DOMAINS)
	/* IRQ */
	memcpy((void *)0xffff0018, (void*)(ptr + 0x18), 4);
	memcpy((void *)(0xffff0018 + 0x20), (void*)(ptr + 0x18 + 0x20), 4);

	/* FIQ */
	memcpy((void *)0xffff001C, (void*)(ptr + 0x1C), 4);
	memcpy((void *)(0xffff001C + 0x20), (void*)(ptr + 0x1C + 0x20), 4);
#else
	/* IRQ */
	memcpy(vector + 0x18, (void*)(ptr + 0x18), 4);
	memcpy(vector + 0x18 + 0x20, (void*)(ptr + 0x18 + 0x20), 4);

	/* FIQ */
	memcpy(vector + 0x1C, (void*)(ptr + 0x1C), 4);
	memcpy(vector + 0x1C + 0x20, (void*)(ptr + 0x1C + 0x20), 4);
#endif
	flush_icache_range(0xffff0000, 0xffff0000 + length);
	if (!vectors_high())
		flush_icache_range(0x00, 0x00 + length);
}
EXPORT_SYMBOL(trap_set_vector);
