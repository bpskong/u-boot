#include <common.h>
#include <linux/mtd/nand.h>
#include <asm/arch/s3c24x0_cpu.h>

#define PAPE_SIZE 2048
#define PAGE_MASK (PAPE_SIZE - 1)

int relocate_from_nand(unsigned long addr, unsigned char *dest, int size)
{
	int i, j;
	int c, r;

	struct s3c2440_nand *nand = (struct s3c2440_nand *)S3C24X0_NAND_BASE;
	volatile unsigned char *nfcmd = (volatile unsigned char *)&nand->NFCMD;
	volatile unsigned char *nfdata = (volatile unsigned char *)&nand->NFDATA;
	volatile unsigned char *nfaddr = (volatile unsigned char *)&nand->NFADDR;
	volatile unsigned char *nfstat = (volatile unsigned char *)&nand->NFSTAT;

	nand->NFCONF = (0 << 12)| (3 << 8) | (0 << 4);
	nand->NFCONT = (1 << 4) | (1 << 1) | (1 << 0);

	/* select chip */
	nand->NFCONT &= ~(1<<1);
	for (i=0; i<10; i++);

	/* reset */
	*nfcmd = NAND_CMD_RESET;

	/* waiting ready ... */
	while (!(*nfstat & 0x1))
		for(i=0; i<10; i++);

	size = (size + PAGE_MASK) & ~(PAGE_MASK);
	unsigned int end = addr + size;

	for(; addr < end; ) {

		/* addressing ... */
		*nfcmd = NAND_CMD_READ0;
		c = addr & PAGE_MASK;
		r = addr / PAPE_SIZE;
		*nfaddr = (c & 0xff);
		*nfaddr = (c >> 8) & 0x0f;
		*nfaddr = (r & 0xff);
		*nfaddr = (r >> 8) & 0xff;
		*nfaddr = (r >> 16) & 03;
		*nfcmd = NAND_CMD_READSTART;

		/* waiting ready ... */
		while (!(*nfstat & 0x1))
			for(i=0; i<10; i++);

		/* do read one page ... */
		for(j=0; j < PAPE_SIZE; j++, addr++) {
			*dest++ = *nfdata;
		}
	 }

	/* deselect chip */
	nand->NFCONT |= (1 << 1);

	return 0;
}
