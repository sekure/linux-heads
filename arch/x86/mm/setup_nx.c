#include <linux/spinlock.h>
#include <linux/errno.h>
#include <linux/init.h>

#include <asm/pgtable.h>
#include <asm/proto.h>

#if defined(CONFIG_X86_64) || defined(CONFIG_X86_PAE)
static int disable_nx;

#ifndef CONFIG_PAX_PAGEEXEC
/*
 * noexec = on|off
 *
 * Control non-executable mappings for processes.
 *
 * on      Enable
 * off     Disable
 */
static int __init noexec_setup(char *str)
{
	if (!str)
		return -EINVAL;
	if (!strncmp(str, "on", 2)) {
		disable_nx = 0;
	} else if (!strncmp(str, "off", 3)) {
		disable_nx = 1;
	}
	x86_configure_nx();
	return 0;
}
early_param("noexec", noexec_setup);
#endif

#endif

void x86_configure_nx(void)
{
#if defined(CONFIG_X86_64) || defined(CONFIG_X86_PAE)
	if (cpu_has_nx && !disable_nx)
		__supported_pte_mask |= _PAGE_NX;
	else
#endif
		__supported_pte_mask &= ~_PAGE_NX;
}

void __init x86_report_nx(void)
{
	if (!cpu_has_nx) {
		printk(KERN_NOTICE "Notice: NX (Execute Disable) protection "
		       "missing in CPU!\n");
	} else {
#if defined(CONFIG_X86_64) || defined(CONFIG_X86_PAE)
		if (disable_nx) {
			printk(KERN_INFO "NX (Execute Disable) protection: "
			       "disabled by kernel command line option\n");
		} else {
			printk(KERN_INFO "NX (Execute Disable) protection: "
			       "active\n");
		}
#else
		/* 32bit non-PAE kernel, NX cannot be used */
		printk(KERN_NOTICE "Notice: NX (Execute Disable) protection "
		       "cannot be enabled: non-PAE kernel!\n");
#endif
	}
}
