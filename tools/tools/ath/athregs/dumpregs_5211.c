/*-
 * Copyright (c) 2002-2008 Sam Leffler, Errno Consulting
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce at minimum a disclaimer
 *    similar to the "NO WARRANTY" disclaimer below ("Disclaimer") and any
 *    redistribution must be conditioned upon including a substantially
 *    similar Disclaimer requirement for further binary redistribution.
 *
 * NO WARRANTY
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF NONINFRINGEMENT, MERCHANTIBILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.
 *
 * $FreeBSD$
 */
#include "diag.h"

#include "ah.h"
#include "ah_internal.h"
#include "ar5211/ar5211reg.h"
#include "ar5211/ar5211phy.h"

#include "dumpregs.h"

#define	N(a)	(sizeof(a) / sizeof(a[0]))

static struct dumpreg ar5211regs[] = {
    DEFBASICfmt(AR_CR,		"CR",		AR_CR_BITS),
    DEFBASIC(AR_RXDP,		"RXDP"),
    DEFBASICfmt(AR_CFG,		"CFG",		AR_CFG_BITS),
    DEFBASICfmt(AR_IER,		"IER",		AR_IER_BITS),
    DEFBASIC(AR_RTSD0,		"RTSD0"),
    DEFBASIC(AR_RTSD1,		"RTSD1"),
    DEFBASICfmt(AR_TXCFG,	"TXCFG",	AR_TXCFG_BITS),
    DEFBASIC(AR_RXCFG,		"RXCFG"),
    DEFBASIC(AR5211_JUMBO_LAST,	"JLAST"),
    DEFBASIC(AR_MIBC,		"MIBC"),
    DEFBASIC(AR_TOPS,		"TOPS"),
    DEFBASIC(AR_RXNPTO,		"RXNPTO"),
    DEFBASIC(AR_TXNPTO,		"TXNPTO"),
    DEFBASIC(AR_RFGTO,		"RFGTO"),
    DEFBASIC(AR_RFCNT,		"RFCNT"),
    DEFBASIC(AR_MACMISC,	"MISC"),
    DEFVOID(AR5311_QDCLKGATE,	"AR5311_QDCLKGATE"),

    DEFINT(AR_ISR,		"ISR"),
    DEFINT(AR_ISR_S0,		"ISR_S0"),
    DEFINT(AR_ISR_S1,		"ISR_S1"),
    DEFINT(AR_ISR_S2,		"ISR_S2"),
    DEFINT(AR_ISR_S3,		"ISR_S3"),
    DEFINT(AR_ISR_S4,		"ISR_S4"),
    DEFINT(AR_IMR,		"IMR"),
    DEFINT(AR_IMR_S0,		"IMR_S0"),
    DEFINT(AR_IMR_S1,		"IMR_S1"),
    DEFINT(AR_IMR_S2,		"IMR_S2"),
    DEFINT(AR_IMR_S3,		"IMR_S3"),
    DEFINT(AR_IMR_S4,		"IMR_S4"),
    /* NB: don't read the RAC so we don't affect operation */
    DEFVOID(AR_ISR_RAC,		"ISR_RAC"),
    DEFINT(AR_ISR_S0_S,		"ISR_S0_S"),
    DEFINT(AR_ISR_S1_S,		"ISR_S1_S"),
    DEFINT(AR_ISR_S2_S,		"ISR_S2_S"),
    DEFINT(AR_ISR_S3_S,		"ISR_S3_S"),
    DEFINT(AR_ISR_S4_S,		"ISR_S4_S"),

    DEFQCU(AR_Q0_TXDP,		"Q0_TXDP"),
    DEFQCU(AR_Q1_TXDP,		"Q1_TXDP"),
    DEFQCU(AR_Q2_TXDP,		"Q2_TXDP"),
    DEFQCU(AR_Q3_TXDP,		"Q3_TXDP"),
    DEFQCU(AR_Q4_TXDP,		"Q4_TXDP"),
    DEFQCU(AR_Q5_TXDP,		"Q5_TXDP"),
    DEFQCU(AR_Q6_TXDP,		"Q6_TXDP"),
    DEFQCU(AR_Q7_TXDP,		"Q7_TXDP"),
    DEFQCU(AR_Q8_TXDP,		"Q8_TXDP"),
    DEFQCU(AR_Q9_TXDP,		"Q9_TXDP"),

    DEFQCU(AR_Q_TXE,		"Q_TXE"),
    DEFQCU(AR_Q_TXD,		"Q_TXD"),

    DEFQCU(AR_Q0_CBRCFG,	"Q0_CBR"),
    DEFQCU(AR_Q1_CBRCFG,	"Q1_CBR"),
    DEFQCU(AR_Q2_CBRCFG,	"Q2_CBR"),
    DEFQCU(AR_Q3_CBRCFG,	"Q3_CBR"),
    DEFQCU(AR_Q4_CBRCFG,	"Q4_CBR"),
    DEFQCU(AR_Q5_CBRCFG,	"Q5_CBR"),
    DEFQCU(AR_Q6_CBRCFG,	"Q6_CBR"),
    DEFQCU(AR_Q7_CBRCFG,	"Q7_CBR"),
    DEFQCU(AR_Q8_CBRCFG,	"Q8_CBR"),
    DEFQCU(AR_Q9_CBRCFG,	"Q9_CBR"),

    DEFQCU(AR_Q0_RDYTIMECFG,	"Q0_RDYT"),
    DEFQCU(AR_Q1_RDYTIMECFG,	"Q1_RDYT"),
    DEFQCU(AR_Q2_RDYTIMECFG,	"Q2_RDYT"),
    DEFQCU(AR_Q3_RDYTIMECFG,	"Q3_RDYT"),
    DEFQCU(AR_Q4_RDYTIMECFG,	"Q4_RDYT"),
    DEFQCU(AR_Q5_RDYTIMECFG,	"Q5_RDYT"),
    DEFQCU(AR_Q6_RDYTIMECFG,	"Q6_RDYT"),
    DEFQCU(AR_Q7_RDYTIMECFG,	"Q7_RDYT"),
    DEFQCU(AR_Q8_RDYTIMECFG,	"Q8_RDYT"),
    DEFQCU(AR_Q9_RDYTIMECFG,	"Q9_RDYT"),

    DEFQCU(AR_Q_ONESHOTARM_SC,	"Q_ONESHOTARM_SC"),
    DEFQCU(AR_Q_ONESHOTARM_CC,	"Q_ONESHOTARM_CC"),

    DEFQCU(AR_Q0_MISC,		"Q0_MISC"),
    DEFQCU(AR_Q1_MISC,		"Q1_MISC"),
    DEFQCU(AR_Q2_MISC,		"Q2_MISC"),
    DEFQCU(AR_Q3_MISC,		"Q3_MISC"),
    DEFQCU(AR_Q4_MISC,		"Q4_MISC"),
    DEFQCU(AR_Q5_MISC,		"Q5_MISC"),
    DEFQCU(AR_Q6_MISC,		"Q6_MISC"),
    DEFQCU(AR_Q7_MISC,		"Q7_MISC"),
    DEFQCU(AR_Q8_MISC,		"Q8_MISC"),
    DEFQCU(AR_Q9_MISC,		"Q9_MISC"),

    DEFQCU(AR_Q0_STS,		"Q0_STS"),
    DEFQCU(AR_Q1_STS,		"Q1_STS"),
    DEFQCU(AR_Q2_STS,		"Q2_STS"),
    DEFQCU(AR_Q3_STS,		"Q3_STS"),
    DEFQCU(AR_Q4_STS,		"Q4_STS"),
    DEFQCU(AR_Q5_STS,		"Q5_STS"),
    DEFQCU(AR_Q6_STS,		"Q6_STS"),
    DEFQCU(AR_Q7_STS,		"Q7_STS"),
    DEFQCU(AR_Q8_STS,		"Q8_STS"),
    DEFQCU(AR_Q9_STS,		"Q9_STS"),

    DEFQCU(AR_Q_RDYTIMESHDN,	"Q_RDYTIMSHD"),

    DEFQCU(AR_D0_QCUMASK,	"D0_MASK"),
    DEFQCU(AR_D1_QCUMASK,	"D1_MASK"),
    DEFQCU(AR_D2_QCUMASK,	"D2_MASK"),
    DEFQCU(AR_D3_QCUMASK,	"D3_MASK"),
    DEFQCU(AR_D4_QCUMASK,	"D4_MASK"),
    DEFQCU(AR_D5_QCUMASK,	"D5_MASK"),
    DEFQCU(AR_D6_QCUMASK,	"D6_MASK"),
    DEFQCU(AR_D7_QCUMASK,	"D7_MASK"),
    DEFQCU(AR_D8_QCUMASK,	"D8_MASK"),
    DEFQCU(AR_D9_QCUMASK,	"D9_MASK"),

    DEFDCU(AR_D0_LCL_IFS,	"D0_IFS"),
    DEFDCU(AR_D1_LCL_IFS,	"D1_IFS"),
    DEFDCU(AR_D2_LCL_IFS,	"D2_IFS"),
    DEFDCU(AR_D3_LCL_IFS,	"D3_IFS"),
    DEFDCU(AR_D4_LCL_IFS,	"D4_IFS"),
    DEFDCU(AR_D5_LCL_IFS,	"D5_IFS"),
    DEFDCU(AR_D6_LCL_IFS,	"D6_IFS"),
    DEFDCU(AR_D7_LCL_IFS,	"D7_IFS"),
    DEFDCU(AR_D8_LCL_IFS,	"D8_IFS"),
    DEFDCU(AR_D9_LCL_IFS,	"D9_IFS"),

    DEFDCU(AR_D0_RETRY_LIMIT,	"D0_RTRY"),
    DEFDCU(AR_D1_RETRY_LIMIT,	"D1_RTRY"),
    DEFDCU(AR_D2_RETRY_LIMIT,	"D2_RTRY"),
    DEFDCU(AR_D3_RETRY_LIMIT,	"D3_RTRY"),
    DEFDCU(AR_D4_RETRY_LIMIT,	"D4_RTRY"),
    DEFDCU(AR_D5_RETRY_LIMIT,	"D5_RTRY"),
    DEFDCU(AR_D6_RETRY_LIMIT,	"D6_RTRY"),
    DEFDCU(AR_D7_RETRY_LIMIT,	"D7_RTRY"),
    DEFDCU(AR_D8_RETRY_LIMIT,	"D8_RTRY"),
    DEFDCU(AR_D9_RETRY_LIMIT,	"D9_RTRY"),

    DEFDCU(AR_D0_CHNTIME,	"D0_CHNT"),
    DEFDCU(AR_D1_CHNTIME,	"D1_CHNT"),
    DEFDCU(AR_D2_CHNTIME,	"D2_CHNT"),
    DEFDCU(AR_D3_CHNTIME,	"D3_CHNT"),
    DEFDCU(AR_D4_CHNTIME,	"D4_CHNT"),
    DEFDCU(AR_D5_CHNTIME,	"D5_CHNT"),
    DEFDCU(AR_D6_CHNTIME,	"D6_CHNT"),
    DEFDCU(AR_D7_CHNTIME,	"D7_CHNT"),
    DEFDCU(AR_D8_CHNTIME,	"D8_CHNT"),
    DEFDCU(AR_D9_CHNTIME,	"D9_CHNT"),

    DEFDCU(AR_D0_MISC,		"D0_MISC"),
    DEFDCU(AR_D1_MISC,		"D1_MISC"),
    DEFDCU(AR_D2_MISC,		"D2_MISC"),
    DEFDCU(AR_D3_MISC,		"D3_MISC"),
    DEFDCU(AR_D4_MISC,		"D4_MISC"),
    DEFDCU(AR_D5_MISC,		"D5_MISC"),
    DEFDCU(AR_D6_MISC,		"D6_MISC"),
    DEFDCU(AR_D7_MISC,		"D7_MISC"),
    DEFDCU(AR_D8_MISC,		"D8_MISC"),
    DEFDCU(AR_D9_MISC,		"D9_MISC"),

    DEFDCU(AR_D0_SEQNUM,	"D0_SEQ"),
    DEFDCU(AR_D1_SEQNUM,	"D1_SEQ"),
    DEFDCU(AR_D2_SEQNUM,	"D2_SEQ"),
    DEFDCU(AR_D3_SEQNUM,	"D3_SEQ"),
    DEFDCU(AR_D4_SEQNUM,	"D4_SEQ"),
    DEFDCU(AR_D5_SEQNUM,	"D5_SEQ"),
    DEFDCU(AR_D6_SEQNUM,	"D6_SEQ"),
    DEFDCU(AR_D7_SEQNUM,	"D7_SEQ"),
    DEFDCU(AR_D8_SEQNUM,	"D8_SEQ"),
    DEFDCU(AR_D9_SEQNUM,	"D9_SEQ"),

    DEFBASIC(AR_D_GBL_IFS_SIFS,	"D_SIFS"),
    DEFBASIC(AR_D_GBL_IFS_SLOT,	"D_SLOT"),
    DEFBASIC(AR_D_GBL_IFS_EIFS,	"D_EIFS"),
    DEFBASIC(AR_D_GBL_IFS_MISC,	"D_MISC"),
    DEFBASIC(AR_D_FPCTL,	"D_FPCTL"),
    DEFBASIC(AR_D_TXPSE,	"D_TXPSE"),
    DEFVOID(AR_D_TXBLK_CMD,	"D_CMD"),
#if 0
    DEFVOID(AR_D_TXBLK_DATA,	"D_DATA"),
#endif
    DEFVOID(AR_D_TXBLK_CLR,	"D_CLR"),
    DEFVOID(AR_D_TXBLK_SET,	"D_SET"),
    DEFBASICfmt(AR_RC,		"RC",		AR_RC_BITS),
    DEFBASICfmt(AR_SCR,		"SCR",		AR_SCR_BITS),
    DEFBASICfmt(AR_INTPEND,	"INTPEND",	AR_INTPEND_BITS),
    DEFBASIC(AR_SFR,		"SFR"),
    DEFBASICfmt(AR_PCICFG,	"PCICFG",	AR_PCICFG_BITS),
    DEFBASIC(AR_GPIOCR,		"GPIOCR"),
    DEFBASIC(AR_GPIODO,		"GPIODO"),
    DEFBASIC(AR_GPIODI,		"GPIODI"),
    DEFBASIC(AR_SREV,		"SREV"),
    DEFVOID(AR_EEPROM_ADDR,	"EEADDR"),
    DEFVOID(AR_EEPROM_DATA,	"EEDATA"),
    DEFVOID(AR_EEPROM_CMD,	"EECMD"),
    DEFVOID(AR_EEPROM_STS,	"EESTS"),
    DEFVOID(AR_EEPROM_CFG,	"EECFG"),
    DEFBASIC(AR_STA_ID0,	"STA_ID0"),
    DEFBASICfmt(AR_STA_ID1,	"STA_ID1",	AR_STA_ID1_BITS),
    DEFBASIC(AR_BSS_ID0,	"BSS_ID0"),
    DEFBASIC(AR_BSS_ID1,	"BSS_ID1"),
    DEFBASIC(AR_SLOT_TIME,	"SLOTTIME"),
    DEFBASIC(AR_TIME_OUT,	"TIME_OUT"),
    DEFBASIC(AR_RSSI_THR,	"RSSI_THR"),
    DEFBASIC(AR_USEC,		"USEC"),
    DEFBASICfmt(AR_BEACON,	"BEACON",	AR_BEACON_BITS),
    DEFBASIC(AR_CFP_PERIOD,	"CFP_PER"),
    DEFBASIC(AR_TIMER0,		"TIMER0"),
    DEFBASIC(AR_TIMER1,		"TIMER1"),
    DEFBASIC(AR_TIMER2,		"TIMER2"),
    DEFBASIC(AR_TIMER3,		"TIMER3"),
    DEFBASIC(AR_CFP_DUR,	"CFP_DUR"),
    DEFBASICfmt(AR_RX_FILTER,	"RXFILTER",	AR_RX_FILTER_BITS),
    DEFBASIC(AR_MCAST_FIL0,	"MCAST_0"),
    DEFBASIC(AR_MCAST_FIL1,	"MCAST_1"),
    DEFBASICfmt(AR_DIAG_SW,	"DIAG_SW",	AR_DIAG_SW_BITS),
    DEFBASIC(AR_TSF_L32,	"TSF_L32"),
    DEFBASIC(AR_TSF_U32,	"TSF_U32"),
    DEFBASIC(AR_TST_ADDAC,	"TST_ADAC"),
    DEFBASIC(AR_DEF_ANTENNA,	"DEF_ANT"),

    DEFBASIC(AR_LAST_TSTP,	"LAST_TST"),
    DEFBASIC(AR_NAV,		"NAV"),
    DEFBASIC(AR_RTS_OK,		"RTS_OK"),
    DEFBASIC(AR_RTS_FAIL,	"RTS_FAIL"),
    DEFBASIC(AR_ACK_FAIL,	"ACK_FAIL"),
    DEFBASIC(AR_FCS_FAIL,	"FCS_FAIL"),
    DEFBASIC(AR_BEACON_CNT,	"BEAC_CNT"),

    DEFVOID(AR_PHY_TURBO,	"PHY_TURBO"),
    DEFVOID(AR_PHY_CHIP_ID,	"PHY_CHIP_ID"),
    DEFVOID(AR_PHY_ACTIVE,	"PHY_ACTIVE"),
    DEFVOID(AR_PHY_AGC_CONTROL,	"PHY_AGC_CONTROL"),
    DEFVOID(AR_PHY_PLL_CTL,	"PHY_PLL_CTL"),
    DEFVOID(AR_PHY_RX_DELAY,	"PHY_RX_DELAY"),
    DEFVOID(AR_PHY_TIMING_CTRL4,"PHY_TIMING_CTRL4"),
    DEFVOID(AR_PHY_RADAR_0,	"PHY_RADAR_0"),
    DEFVOID(AR_PHY_IQCAL_RES_PWR_MEAS_I,"PHY_IQCAL_RES_PWR_MEAS_I"),
    DEFVOID(AR_PHY_IQCAL_RES_PWR_MEAS_Q,"PHY_IQCAL_RES_PWR_MEAS_Q"),
    DEFVOID(AR_PHY_IQCAL_RES_IQ_CORR_MEAS,"PHY_IQCAL_RES_IQ_CORR_MEAS"),
    DEFVOID(AR_PHY_CURRENT_RSSI,"PHY_CURRENT_RSSI"),
    DEFVOID(AR5211_PHY_MODE,	"PHY_MODE"),
};

static __constructor void
ar5211_ctor(void)
{
#define	MAC5211	SREV(2,0), SREV(4,5)
	register_regs(ar5211regs, N(ar5211regs), MAC5211, PHYANY);
	register_keycache(128, MAC5211, PHYANY);

	register_range(0x9800, 0x987c, DUMP_BASEBAND, MAC5211, PHYANY);
	register_range(0x9900, 0x995c, DUMP_BASEBAND, MAC5211, PHYANY);
	register_range(0x9c00, 0x9c1c, DUMP_BASEBAND, MAC5211, PHYANY);
}
