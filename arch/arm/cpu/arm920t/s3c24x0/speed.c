/*
 * (C) Copyright 2001-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, d.mueller@elsoft.ch
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/* This code work for only the S3C2440 */

#include <common.h>
#ifdef CONFIG_S3C24X0

#include <asm/io.h>
#include <asm/arch/s3c24x0_cpu.h>

#define MPLL 0
#define UPLL 1

/* ------------------------------------------------------------------------- */
/* NOTE: This describes the proper use of this file.
 *
 * CONFIG_SYS_CLK_FREQ should be defined as the input frequency of the PLL.
 *
 * get_FCLK(), get_HCLK(), get_PCLK() and get_UCLK() return the clock of
 * the specified bus in HZ.
 */
/* ------------------------------------------------------------------------- */

static ulong get_PLLCLK(int pllreg)
{
	struct s3c24x0_clock_power *clk_power = s3c24x0_get_base_clock_power();
	ulong r, m, p, s;

	if (pllreg == MPLL)
		r = readl(&clk_power->MPLLCON);
	else if (pllreg == UPLL)
		r = readl(&clk_power->UPLLCON);
	else
		hang();

	m = ((r & 0xFF000) >> 12) + 8;
	p = ((r & 0x003F0) >> 4) + 2;
	s = r & 0x3;

	return (CONFIG_SYS_CLK_FREQ * m * 2) / (p << s);
}

/* return FCLK frequency */
ulong get_FCLK(void)
{
	return get_PLLCLK(MPLL);
}

/* return HCLK frequency */
ulong get_HCLK(void)
{
	struct s3c24x0_clock_power *clk_power = s3c24x0_get_base_clock_power();
	unsigned long clkdiv;
	unsigned long camdiv;
	int hdiv = 1;

	clkdiv = clk_power->CLKDIVN;
	camdiv = clk_power->CAMDIVN;
	switch (clkdiv & S3C2440_CLKDIVN_HDIVN_MASK) {
	case S3C2440_CLKDIVN_HDIVN_1:
			hdiv = 1;
		break;
	case S3C2440_CLKDIVN_HDIVN_2:
			hdiv = 2;
		break;
	case S3C2440_CLKDIVN_HDIVN_4_8:
			hdiv = (camdiv & S3C2440_CAMDIVN_HCLK4_HALF) ? 8 : 4;
		break;
	case S3C2440_CLKDIVN_HDIVN_3_6:
			hdiv = (camdiv & S3C2440_CAMDIVN_HCLK3_HALF) ? 6 : 3;
		break;
		}
	return get_FCLK() / hdiv;

}

/* return PCLK frequency */
ulong get_PCLK(void)
{
	struct s3c24x0_clock_power *clk_power = s3c24x0_get_base_clock_power();
	unsigned long clkdiv;
	unsigned long camdiv;
	int hdiv = 1;

	clkdiv = clk_power->CLKDIVN;
	camdiv = clk_power->CAMDIVN;
	switch (clkdiv & S3C2440_CLKDIVN_HDIVN_MASK) {
		case S3C2440_CLKDIVN_HDIVN_1:
			hdiv = 1;
			break;
		case S3C2440_CLKDIVN_HDIVN_2:
			hdiv = 2;
			break;
		case S3C2440_CLKDIVN_HDIVN_4_8:
			hdiv = (camdiv & S3C2440_CAMDIVN_HCLK4_HALF) ? 8 : 4;
			break;
		case S3C2440_CLKDIVN_HDIVN_3_6:
			hdiv = (camdiv & S3C2440_CAMDIVN_HCLK3_HALF) ? 6 : 3;
			break;
	}
	return get_FCLK() / hdiv / ((clkdiv & S3C2440_CLKDIVN_PDIVN)? 2:1);
}

/* return UCLK frequency */
ulong get_UCLK(void)
{
	return get_PLLCLK(UPLL);
}

#endif /* CONFIG_S3C24X0 */
