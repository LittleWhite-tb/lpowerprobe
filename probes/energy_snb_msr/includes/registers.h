/*
 * ===========================================================================
 *
 *      Filename:  registers.h
 *
 *      Description:  Register Defines for the perfmon module
 *
 *      Version:  <VERSION>
 *      Created:  <DATE>
 *
 *      Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *      Company:  RRZE Erlangen
 *      Project:  likwid
 *      Copyright:  Copyright (c) 2010, Jan Treibig
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License, v2, as
 *      published by the Free Software Foundation
 *     
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *     
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * ===========================================================================
 * 
 * lPowerProbe - A light benchmark tool oriented for energy probing
 *               heavely based on likwid
 * Copyright (C) 2013 Universite de Versailles
 * 2011-08: Modified by Exascale Research Center
 */


#ifndef REGISTERS_H
#define REGISTERS_H

/*
 * INTEL
 */

/* MSR Registers  */
/* 3 80 bit fixed counters */
#define MSR_PERF_FIXED_CTR_CTRL   0x38D
#define MSR_PERF_FIXED_CTR0       0x309  /* Instr_Retired.Any */
#define MSR_PERF_FIXED_CTR1       0x30A  /* CPU_CLK_UNHALTED.CORE */
#define MSR_PERF_FIXED_CTR2       0x30B  /* CPU_CLK_UNHALTED.REF */
/* 4 40/48 bit configurable counters */
/* Perfmon V1 */
#define MSR_PERFEVTSEL0           0x186
#define MSR_PERFEVTSEL1           0x187
#define MSR_PERFEVTSEL2           0x188
#define MSR_PERFEVTSEL3           0x189
#define MSR_PMC0                  0x0C1
#define MSR_PMC1                  0x0C2
#define MSR_PMC2                  0x0C3
#define MSR_PMC3                  0x0C4
#define MSR_PMC4                  0x0C5
#define MSR_PMC5                  0x0C6
#define MSR_PMC6                  0x0C7
#define MSR_PMC7                  0x0C8
/* Perfmon V2 */
#define MSR_PERF_GLOBAL_CTRL      0x38F
#define MSR_PERF_GLOBAL_STATUS    0x38E
#define MSR_PERF_GLOBAL_OVF_CTRL  0x390
#define MSR_PEBS_ENABLE           0x3F1
/* Perfmon V3 */
#define MSR_OFFCORE_RSP0              0x1A6
#define MSR_UNCORE_PERF_GLOBAL_CTRL       0x391
#define MSR_UNCORE_PERF_GLOBAL_STATUS     0x392
#define MSR_UNCORE_PERF_GLOBAL_OVF_CTRL   0x393
#define MSR_UNCORE_FIXED_CTR0             0x394  /* Uncore clock cycles */
#define MSR_UNCORE_FIXED_CTR_CTRL         0x395
#define MSR_UNCORE_ADDR_OPCODE_MATCH      0x396 
#define MSR_UNCORE_PERFEVTSEL0         0x3C0
#define MSR_UNCORE_PERFEVTSEL1         0x3C1
#define MSR_UNCORE_PERFEVTSEL2         0x3C2
#define MSR_UNCORE_PERFEVTSEL3         0x3C3
#define MSR_UNCORE_PERFEVTSEL4         0x3C4
#define MSR_UNCORE_PERFEVTSEL5         0x3C5
#define MSR_UNCORE_PERFEVTSEL6         0x3C6
#define MSR_UNCORE_PERFEVTSEL7         0x3C7
#define MSR_UNCORE_PMC0                0x3B0
#define MSR_UNCORE_PMC1                0x3B1
#define MSR_UNCORE_PMC2                0x3B2
#define MSR_UNCORE_PMC3                0x3B3
#define MSR_UNCORE_PMC4                0x3B4
#define MSR_UNCORE_PMC5                0x3B5
#define MSR_UNCORE_PMC6                0x3B6
#define MSR_UNCORE_PMC7                0x3B7
/* EX type uncore */
/* U box - System Config Controller */
#define MSR_U_PMON_GLOBAL_CTRL          0xC00
#define MSR_U_PMON_GLOBAL_STATUS        0xC01
#define MSR_U_PMON_GLOBAL_OVF_CTRL      0xC02
#define MSR_U_PMON_GLOBAL_EVNT_SEL      0xC10
#define MSR_U_PMON_GLOBAL_CTR           0xC11
/* B box 0 - Home Agent 0 */
#define MSR_B0_PMON_BOX_CTRL            0xC20
#define MSR_B0_PMON_BOX_STATUS          0xC21
#define MSR_B0_PMON_BOX_OVF_CTRL        0xC22
#define MSR_B0_PMON_EVNT_SEL0           0xC30
#define MSR_B0_PMON_CTR0                0xC31
#define MSR_B0_PMON_EVNT_SEL1           0xC32
#define MSR_B0_PMON_CTR1                0xC33
#define MSR_B0_PMON_EVNT_SEL2           0xC34
#define MSR_B0_PMON_CTR2                0xC35
#define MSR_B0_PMON_EVNT_SEL3           0xC36
#define MSR_B0_PMON_CTR3                0xC37
/* S box 0 - Caching Agent 0 */
#define MSR_S0_PMON_BOX_CTRL            0xC40
#define MSR_S0_PMON_BOX_STATUS          0xC41
#define MSR_S0_PMON_BOX_OVF_CTRL        0xC42
#define MSR_S0_PMON_EVNT_SEL0           0xC50
#define MSR_S0_PMON_CTR0                0xC51
#define MSR_S0_PMON_EVNT_SEL1           0xC52
#define MSR_S0_PMON_CTR1                0xC53
#define MSR_S0_PMON_EVNT_SEL2           0xC54
#define MSR_S0_PMON_CTR2                0xC55
#define MSR_S0_PMON_EVNT_SEL3           0xC56
#define MSR_S0_PMON_CTR3                0xC57
/* B box 1 - Home Agent 1 */
#define MSR_B1_PMON_BOX_CTRL            0xC60
#define MSR_B1_PMON_BOX_STATUS          0xC61
#define MSR_B1_PMON_BOX_OVF_CTRL        0xC62
#define MSR_B1_PMON_EVNT_SEL0           0xC70
#define MSR_B1_PMON_CTR0                0xC71
#define MSR_B1_PMON_EVNT_SEL1           0xC72
#define MSR_B1_PMON_CTR1                0xC73
#define MSR_B1_PMON_EVNT_SEL2           0xC74
#define MSR_B1_PMON_CTR2                0xC75
#define MSR_B1_PMON_EVNT_SEL3           0xC76
#define MSR_B1_PMON_CTR3                0xC77
/* W box  - Power Controller */
#define MSR_W_PMON_BOX_CTRL             0xC80
#define MSR_W_PMON_BOX_STATUS           0xC81
#define MSR_W_PMON_BOX_OVF_CTRL         0xC82
#define MSR_W_PMON_EVNT_SEL0            0xC90
#define MSR_W_PMON_CTR0                 0xC91
#define MSR_W_PMON_EVNT_SEL1            0xC92
#define MSR_W_PMON_CTR1                 0xC93
#define MSR_W_PMON_EVNT_SEL2            0xC94
#define MSR_W_PMON_CTR2                 0xC95
#define MSR_W_PMON_EVNT_SEL3            0xC96
#define MSR_W_PMON_CTR3                 0xC97
/* M box 0 - Memory Controller 0 */
#define MSR_M0_PMON_BOX_CTRL            0xCA0
#define MSR_M0_PMON_BOX_STATUS          0xCA1
#define MSR_M0_PMON_BOX_OVF_CTRL        0xCA2
#define MSR_M0_PMON_TIMESTAMP           0xCA4
#define MSR_M0_PMON_DSP                 0xCA5
#define MSR_M0_PMON_ISS                 0xCA6
#define MSR_M0_PMON_MAP                 0xCA7
#define MSR_M0_PMON_MSC_THR             0xCA8
#define MSR_M0_PMON_PGT                 0xCA9
#define MSR_M0_PMON_PLD                 0xCAA
#define MSR_M0_PMON_ZDP                 0xCAB
#define MSR_M0_PMON_EVNT_SEL0           0xCB0
#define MSR_M0_PMON_CTR0                0xCB1
#define MSR_M0_PMON_EVNT_SEL1           0xCB2
#define MSR_M0_PMON_CTR1                0xCB3
#define MSR_M0_PMON_EVNT_SEL2           0xCB4
#define MSR_M0_PMON_CTR2                0xCB5
#define MSR_M0_PMON_EVNT_SEL3           0xCB6
#define MSR_M0_PMON_CTR3                0xCB7
#define MSR_M0_PMON_EVNT_SEL4           0xCB8
#define MSR_M0_PMON_CTR4                0xCB9
#define MSR_M0_PMON_EVNT_SEL5           0xCBA
#define MSR_M0_PMON_CTR5                0xCBB
/* S box 1 - Caching Agent 1 */
#define MSR_S1_PMON_BOX_CTRL            0xCC0
#define MSR_S1_PMON_BOX_STATUS          0xCC1
#define MSR_S1_PMON_BOX_OVF_CTRL        0xCC2
#define MSR_S1_PMON_EVNT_SEL0           0xCD0
#define MSR_S1_PMON_CTR0                0xCD1
#define MSR_S1_PMON_EVNT_SEL1           0xCD2
#define MSR_S1_PMON_CTR1                0xCD3
#define MSR_S1_PMON_EVNT_SEL2           0xCD4
#define MSR_S1_PMON_CTR2                0xCD5
#define MSR_S1_PMON_EVNT_SEL3           0xCD6
#define MSR_S1_PMON_CTR3                0xCD7
/* M box 1 - Memory Controller 1 */
#define MSR_M1_PMON_BOX_CTRL            0xCE0
#define MSR_M1_PMON_BOX_STATUS          0xCE1
#define MSR_M1_PMON_BOX_OVF_CTRL        0xCE2
#define MSR_M1_PMON_TIMESTAMP           0xCE4
#define MSR_M1_PMON_DSP                 0xCE5
#define MSR_M1_PMON_ISS                 0xCE6
#define MSR_M1_PMON_MAP                 0xCE7
#define MSR_M1_PMON_MSC_THR             0xCE8
#define MSR_M1_PMON_PGT                 0xCE9
#define MSR_M1_PMON_PLD                 0xCEA
#define MSR_M1_PMON_ZDP                 0xCEB
#define MSR_M1_PMON_EVNT_SEL0           0xCF0
#define MSR_M1_PMON_CTR0                0xCF1
#define MSR_M1_PMON_EVNT_SEL1           0xCF2
#define MSR_M1_PMON_CTR1                0xCF3
#define MSR_M1_PMON_EVNT_SEL2           0xCF4
#define MSR_M1_PMON_CTR2                0xCF5
#define MSR_M1_PMON_EVNT_SEL3           0xCF6
#define MSR_M1_PMON_CTR3                0xCB7
#define MSR_M1_PMON_EVNT_SEL4           0xCF8
#define MSR_M1_PMON_CTR4                0xCF9
#define MSR_M1_PMON_EVNT_SEL5           0xCFA
#define MSR_M1_PMON_CTR5                0xCFB
/* C box 0 - Coherence Engine core 0 */
#define MSR_C0_PMON_BOX_CTRL            0xD00
#define MSR_C0_PMON_BOX_STATUS          0xD01
#define MSR_C0_PMON_BOX_OVF_CTRL        0xD02
#define MSR_C0_PMON_EVNT_SEL0           0xD10
#define MSR_C0_PMON_CTR0                0xD11
#define MSR_C0_PMON_EVNT_SEL1           0xD12
#define MSR_C0_PMON_CTR1                0xD13
#define MSR_C0_PMON_EVNT_SEL2           0xD14
#define MSR_C0_PMON_CTR2                0xD15
#define MSR_C0_PMON_EVNT_SEL3           0xD16
#define MSR_C0_PMON_CTR3                0xD17
#define MSR_C0_PMON_EVNT_SEL4           0xD18
#define MSR_C0_PMON_CTR4                0xD19
#define MSR_C0_PMON_EVNT_SEL5           0xD1A
#define MSR_C0_PMON_CTR5                0xD1B
/* C box 4 - Coherence Engine core 4 */
#define MSR_C4_PMON_BOX_CTRL            0xD20
#define MSR_C4_PMON_BOX_STATUS          0xD21
#define MSR_C4_PMON_BOX_OVF_CTRL        0xD22
#define MSR_C4_PMON_EVNT_SEL0           0xD30
#define MSR_C4_PMON_CTR0                0xD31
#define MSR_C4_PMON_EVNT_SEL1           0xD32
#define MSR_C4_PMON_CTR1                0xD33
#define MSR_C4_PMON_EVNT_SEL2           0xD34
#define MSR_C4_PMON_CTR2                0xD35
#define MSR_C4_PMON_EVNT_SEL3           0xD36
#define MSR_C4_PMON_CTR3                0xD37
#define MSR_C4_PMON_EVNT_SEL4           0xD38
#define MSR_C4_PMON_CTR4                0xD39
#define MSR_C4_PMON_EVNT_SEL5           0xD3A
#define MSR_C4_PMON_CTR5                0xD3B
/* C box 2 - Coherence Engine core 2 */
#define MSR_C2_PMON_BOX_CTRL            0xD40
#define MSR_C2_PMON_BOX_STATUS          0xD41
#define MSR_C2_PMON_BOX_OVF_CTRL        0xD42
#define MSR_C2_PMON_EVNT_SEL0           0xD50
#define MSR_C2_PMON_CTR0                0xD51
#define MSR_C2_PMON_EVNT_SEL1           0xD52
#define MSR_C2_PMON_CTR1                0xD53
#define MSR_C2_PMON_EVNT_SEL2           0xD54
#define MSR_C2_PMON_CTR2                0xD55
#define MSR_C2_PMON_EVNT_SEL3           0xD56
#define MSR_C2_PMON_CTR3                0xD57
#define MSR_C2_PMON_EVNT_SEL4           0xD58
#define MSR_C2_PMON_CTR4                0xD59
#define MSR_C2_PMON_EVNT_SEL5           0xD5A
#define MSR_C2_PMON_CTR5                0xD5B
/* C box 6 - Coherence Engine core 6 */
#define MSR_C6_PMON_BOX_CTRL            0xD60
#define MSR_C6_PMON_BOX_STATUS          0xD61
#define MSR_C6_PMON_BOX_OVF_CTRL        0xD62
#define MSR_C6_PMON_EVNT_SEL0           0xD70
#define MSR_C6_PMON_CTR0                0xD71
#define MSR_C6_PMON_EVNT_SEL1           0xD72
#define MSR_C6_PMON_CTR1                0xD73
#define MSR_C6_PMON_EVNT_SEL2           0xD74
#define MSR_C6_PMON_CTR2                0xD75
#define MSR_C6_PMON_EVNT_SEL3           0xD76
#define MSR_C6_PMON_CTR3                0xD77
#define MSR_C6_PMON_EVNT_SEL4           0xD78
#define MSR_C6_PMON_CTR4                0xD79
#define MSR_C6_PMON_EVNT_SEL5           0xD7A
#define MSR_C6_PMON_CTR5                0xD7B
/* C box 1 - Coherence Engine core 1 */
#define MSR_C1_PMON_BOX_CTRL            0xD80
#define MSR_C1_PMON_BOX_STATUS          0xD81
#define MSR_C1_PMON_BOX_OVF_CTRL        0xD82
#define MSR_C1_PMON_EVNT_SEL0           0xD90
#define MSR_C1_PMON_CTR0                0xD91
#define MSR_C1_PMON_EVNT_SEL1           0xD92
#define MSR_C1_PMON_CTR1                0xD93
#define MSR_C1_PMON_EVNT_SEL2           0xD94
#define MSR_C1_PMON_CTR2                0xD95
#define MSR_C1_PMON_EVNT_SEL3           0xD96
#define MSR_C1_PMON_CTR3                0xD97
#define MSR_C1_PMON_EVNT_SEL4           0xD98
#define MSR_C1_PMON_CTR4                0xD99
#define MSR_C1_PMON_EVNT_SEL5           0xD9A
#define MSR_C1_PMON_CTR5                0xD9B
/* C box 5 - Coherence Engine core 5 */
#define MSR_C5_PMON_BOX_CTRL            0xDA0
#define MSR_C5_PMON_BOX_STATUS          0xDA1
#define MSR_C5_PMON_BOX_OVF_CTRL        0xDA2
#define MSR_C5_PMON_EVNT_SEL0           0xDB0
#define MSR_C5_PMON_CTR0                0xDB1
#define MSR_C5_PMON_EVNT_SEL1           0xDB2
#define MSR_C5_PMON_CTR1                0xDB3
#define MSR_C5_PMON_EVNT_SEL2           0xDB4
#define MSR_C5_PMON_CTR2                0xDB5
#define MSR_C5_PMON_EVNT_SEL3           0xDB6
#define MSR_C5_PMON_CTR3                0xDB7
#define MSR_C5_PMON_EVNT_SEL4           0xDB8
#define MSR_C5_PMON_CTR4                0xDB9
#define MSR_C5_PMON_EVNT_SEL5           0xDBA
#define MSR_C5_PMON_CTR5                0xDBB
/* C box 3 - Coherence Engine core 3 */
#define MSR_C3_PMON_BOX_CTRL            0xDC0
#define MSR_C3_PMON_BOX_STATUS          0xDC1
#define MSR_C3_PMON_BOX_OVF_CTRL        0xDC2
#define MSR_C3_PMON_EVNT_SEL0           0xDD0
#define MSR_C3_PMON_CTR0                0xDD1
#define MSR_C3_PMON_EVNT_SEL1           0xDD2
#define MSR_C3_PMON_CTR1                0xDD3
#define MSR_C3_PMON_EVNT_SEL2           0xDD4
#define MSR_C3_PMON_CTR2                0xDD5
#define MSR_C3_PMON_EVNT_SEL3           0xDD6
#define MSR_C3_PMON_CTR3                0xDD7
#define MSR_C3_PMON_EVNT_SEL4           0xDD8
#define MSR_C3_PMON_CTR4                0xDD9
#define MSR_C3_PMON_EVNT_SEL5           0xDDA
#define MSR_C3_PMON_CTR5                0xDDB
/* C box 7 - Coherence Engine core 7 */
#define MSR_C7_PMON_BOX_CTRL            0xDE0
#define MSR_C7_PMON_BOX_STATUS          0xDE1
#define MSR_C7_PMON_BOX_OVF_CTRL        0xDE2
#define MSR_C7_PMON_EVNT_SEL0           0xDF0
#define MSR_C7_PMON_CTR0                0xDF1
#define MSR_C7_PMON_EVNT_SEL1           0xDF2
#define MSR_C7_PMON_CTR1                0xDF3
#define MSR_C7_PMON_EVNT_SEL2           0xDF4
#define MSR_C7_PMON_CTR2                0xDF5
#define MSR_C7_PMON_EVNT_SEL3           0xDF6
#define MSR_C7_PMON_CTR3                0xDF7
#define MSR_C7_PMON_EVNT_SEL4           0xDF8
#define MSR_C7_PMON_CTR4                0xDF9
#define MSR_C7_PMON_EVNT_SEL5           0xDFA
#define MSR_C7_PMON_CTR5                0xDFB
/* R box 0 - Router 0 */
#define MSR_R0_PMON_BOX_CTRL            0xE00
#define MSR_R0_PMON_BOX_STATUS          0xE01
#define MSR_R0_PMON_BOX_OVF_CTRL        0xE02
#define MSR_R0_PMON_IPERF0_P0           0xE04
#define MSR_R0_PMON_IPERF0_P1           0xE05
#define MSR_R0_PMON_IPERF0_P2           0xE06
#define MSR_R0_PMON_IPERF0_P3           0xE07
#define MSR_R0_PMON_IPERF0_P4           0xE08
#define MSR_R0_PMON_IPERF0_P5           0xE09
#define MSR_R0_PMON_IPERF0_P6           0xE0A
#define MSR_R0_PMON_IPERF0_P7           0xE0B
#define MSR_R0_PMON_QLX_P0              0xE0C
#define MSR_R0_PMON_QLX_P1              0xE0D
#define MSR_R0_PMON_QLX_P2              0xE0E
#define MSR_R0_PMON_QLX_P3              0xE0F
#define MSR_R0_PMON_EVNT_SEL0           0xE10
#define MSR_R0_PMON_CTR0                0xE11
#define MSR_R0_PMON_EVNT_SEL1           0xE12
#define MSR_R0_PMON_CTR1                0xE13
#define MSR_R0_PMON_EVNT_SEL2           0xE14
#define MSR_R0_PMON_CTR2                0xE15
#define MSR_R0_PMON_EVNT_SEL3           0xE16
#define MSR_R0_PMON_CTR3                0xE17
#define MSR_R0_PMON_EVNT_SEL4           0xE18
#define MSR_R0_PMON_CTR4                0xE19
#define MSR_R0_PMON_EVNT_SEL5           0xE1A
#define MSR_R0_PMON_CTR5                0xE1B
#define MSR_R0_PMON_EVNT_SEL6           0xE1C
#define MSR_R0_PMON_CTR6                0xE1D
#define MSR_R0_PMON_EVNT_SEL7           0xE1E
#define MSR_R0_PMON_CTR7                0xE1F
/* R box 1 - Router 1 */
#define MSR_R1_PMON_BOX_CTRL            0xE20
#define MSR_R1_PMON_BOX_STATUS          0xE21
#define MSR_R1_PMON_BOX_OVF_CTRL        0xE22
#define MSR_R1_PMON_IPERF1_P8           0xE24
#define MSR_R1_PMON_IPERF1_P9           0xE25
#define MSR_R1_PMON_IPERF1_P10          0xE26
#define MSR_R1_PMON_IPERF1_P11          0xE27
#define MSR_R1_PMON_IPERF1_P12          0xE28
#define MSR_R1_PMON_IPERF1_P13          0xE29
#define MSR_R1_PMON_IPERF1_P14          0xE2A
#define MSR_R1_PMON_IPERF1_P15          0xE2B
#define MSR_R1_PMON_QLX_P4              0xE2C
#define MSR_R1_PMON_QLX_P5              0xE2D
#define MSR_R1_PMON_QLX_P6              0xE2E
#define MSR_R1_PMON_QLX_P7              0xE2F
#define MSR_R1_PMON_EVNT_SEL8           0xE30
#define MSR_R1_PMON_CTR8                0xE31
#define MSR_R1_PMON_EVNT_SEL9           0xE32
#define MSR_R1_PMON_CTR9                0xE33
#define MSR_R1_PMON_EVNT_SEL10          0xE34
#define MSR_R1_PMON_CTR10               0xE35
#define MSR_R1_PMON_EVNT_SEL11          0xE36
#define MSR_R1_PMON_CTR11               0xE37
#define MSR_R1_PMON_EVNT_SEL12          0xE38
#define MSR_R1_PMON_CTR12               0xE39
#define MSR_R1_PMON_EVNT_SEL13          0xE3A
#define MSR_R1_PMON_CTR13               0xE3B
#define MSR_R1_PMON_EVNT_SEL14          0xE3C
#define MSR_R1_PMON_CTR14               0xE3D
#define MSR_R1_PMON_EVNT_SEL15          0xE3E
#define MSR_R1_PMON_CTR15               0xE3F
/* Match/Mask MSRs */
#define MSR_B0_PMON_MATCH               0xE45
#define MSR_B0_PMON_MASK                0xE46
#define MSR_S0_PMON_MATCH               0xE49
#define MSR_S0_PMON_MASK                0xE4A
#define MSR_B1_PMON_MATCH               0xE4D
#define MSR_B1_PMON_MASK                0xE4E
#define MSR_M0_PMON_MM_CONFIG           0xE54
#define MSR_M0_PMON_ADDR_MATCH          0xE55
#define MSR_M0_PMON_ADDR_MASK           0xE56
#define MSR_S1_PMON_MATCH               0xE59
#define MSR_S1_PMON_MASK                0xE5A
#define MSR_M1_PMON_MM_CONFIG           0xE5C
#define MSR_M1_PMON_ADDR_MATCH          0xE5D
#define MSR_M1_PMON_ADDR_MASK           0xE5E
/* Power interfaces, RAPL */
#define MSR_RAPL_POWER_UNIT             0x606
#define MSR_PKG_RAPL_POWER_LIMIT        0x610
#define MSR_PKG_ENERGY_STATUS           0x611
#define MSR_PKG_PERF_STATUS             0x613
#define MSR_PKG_POWER_INFO              0x614


/*
 * AMD
 */

#define MSR_AMD_PERFEVTSEL0           0xC0010000
#define MSR_AMD_PERFEVTSEL1           0xC0010001
#define MSR_AMD_PERFEVTSEL2           0xC0010002
#define MSR_AMD_PERFEVTSEL3           0xC0010003
#define MSR_AMD_PMC0                  0xC0010004
#define MSR_AMD_PMC1                  0xC0010005
#define MSR_AMD_PMC2                  0xC0010006
#define MSR_AMD_PMC3                  0xC0010007


#endif /* REGISTERS_H */

