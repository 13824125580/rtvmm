#
# Makefile for the linux arm-vmm
#

obj-$(CONFIG_ARM_VMM) += vmm.o vmm_traps.o vmm_virq.o

ifeq ($(CONFIG_ARM_VMM),y)
obj-$(CONFIG_SOC_AM33XX)		 += am33xx/softirq.o am33xx/virq.o
obj-$(CONFIG_MACH_REALVIEW_PBA8) += realview_a8/softirq.o
endif
