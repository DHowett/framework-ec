/* Copyright 2021 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * TI bq257x0 battery charger driver.
 */

#ifndef __CROS_EC_BQ257X0_REGS_H
#define __CROS_EC_BQ257X0_REGS_H

#include "bq25710.h"

/*
 * BQ257x0 family register definitions.
 *
 * Common registers are prefixed with BQ257X0, chip specific registers
 * and registers with incompatible definitions are prefixed with
 * BQ25710, BQ25720, etc.
 */

/*
 * ChargerStatus Register (0x20)
 */
#define BQ257X0_CHARGER_STATUS_ICO_DONE_SHIFT		14
#define BQ257X0_CHARGER_STATUS_ICO_DONE_BITS		1

/*
 * ChargeOption0 Register (0x12)
 */
#define BQ257X0_CHARGE_OPTION_0_EN_LWPWR_SHIFT		15
#define BQ257X0_CHARGE_OPTION_0_EN_LWPWR_BITS		1
#define BQ257X0_CHARGE_OPTION_0_EN_LEARN_SHIFT		5
#define BQ257X0_CHARGE_OPTION_0_EN_LEARN_BITS		1
#define BQ257X0_CHARGE_OPTION_0_IADP_GAIN_SHIFT		4
#define BQ257X0_CHARGE_OPTION_0_IADP_GAIN_BITS		1
#define BQ257X0_CHARGE_OPTION_0_EN_IDPM_SHIFT		1
#define BQ257X0_CHARGE_OPTION_0_EN_IDPM_BITS		1
#define BQ257X0_CHARGE_OPTION_0_CHRG_INHIBIT_SHIFT	0
#define BQ257X0_CHARGE_OPTION_0_CHRG_INHIBIT_BITS	1

/*
 * ChargeOption1 Register (0x30)
 */
#define BQ25710_CHARGE_OPTION_1_EN_PSYS_SHIFT		12
#define BQ25710_CHARGE_OPTION_1_EN_PSYS_BITS		1

#define BQ25720_CHARGE_OPTION_1_PSYS_CONFIG_SHIFT	12
#define BQ25720_CHARGE_OPTION_1_PSYS_CONFIG_BITS	2
#define BQ25720_CHARGE_OPTION_1_PSYS_CONFIG__PBUS_PBAT	0
#define BQ25720_CHARGE_OPTION_1_PSYS_CONFIG__OFF	3

#define BQ257X0_CHARGE_OPTION_1_CMP_REF_SHIFT		7
#define BQ257X0_CHARGE_OPTION_1_CMP_REF_BITS		1
#define BQ257X0_CHARGE_OPTION_1_CMP_REF__2P3		0
#define BQ257X0_CHARGE_OPTION_1_CMP_REF__1P2		1

/*
 * ChargeOption2 Register (0x31)
 */
#define BQ257X0_CHARGE_OPTION_2_PKPWR_TOVLD_DEG_SHIFT	14
#define BQ257X0_CHARGE_OPTION_2_PKPWR_TOVLD_DEG_BITS	2
#define BQ25720_CHARGE_OPTION_2_PKPWR_TOVLD_DEG__10MS	3

#define BQ257X0_CHARGE_OPTION_2_PKPWR_TMAX_SHIFT	8
#define BQ257X0_CHARGE_OPTION_2_PKPWR_TMAX_BITS		2

#define BQ257X0_CHARGE_OPTION_2_EN_EXTILIM_SHIFT	7
#define BQ257X0_CHARGE_OPTION_2_EN_EXTILIM_BITS		1

#define BQ257X0_CHARGE_OPTION_2_EN_ACOC_SHIFT		3
#define BQ257X0_CHARGE_OPTION_2_EN_ACOC_BITS		1
#define BQ257X0_CHARGE_OPTION_2_EN_ACOC__DISABLE	0
#define BQ257X0_CHARGE_OPTION_2_EN_ACOC__ENABLE		1

#define BQ257X0_CHARGE_OPTION_2_ACOC_VTH_SHIFT		2
#define BQ257X0_CHARGE_OPTION_2_ACOC_VTH_BITS		1
#define BQ257X0_CHARGE_OPTION_2_ACOC_VTH__1P33		0
#define BQ257X0_CHARGE_OPTION_2_ACOC_VTH__2P00		1

#define BQ257X0_CHARGE_OPTION_2_BATOC_VTH_SHIFT		0
#define BQ257X0_CHARGE_OPTION_2_BATOC_VTH_BITS		1
#define BQ257X0_CHARGE_OPTION_2_BATOC_VTH__1P33		0
#define BQ257X0_CHARGE_OPTION_2_BATOC_VTH__2P00		1

/*
 * ChargeOption3 Register (0x32)
 */
#define BQ257X0_CHARGE_OPTION_3_RESET_REG_SHIFT		14
#define BQ257X0_CHARGE_OPTION_3_RESET_REG_BITS		1

#define BQ257X0_CHARGE_OPTION_3_EN_ICO_MODE_SHIFT	11
#define BQ257X0_CHARGE_OPTION_3_EN_ICO_MODE_BITS	1

#define BQ257X0_CHARGE_OPTION_3_IL_AVG_SHIFT		3
#define BQ257X0_CHARGE_OPTION_3_IL_AVG_BITS		2
#define BQ257X0_CHARGE_OPTION_3_IL_AVG__10A		1

/*
 * ChargeOption4 Register (0x36)
 */
#define BQ25720_CHARGE_OPTION_4_VSYS_UVP_SHIFT		13
#define BQ25720_CHARGE_OPTION_4_VSYS_UVP_BITS		3
#define BQ25720_CHARGE_OPTION_4_VSYS_UVP__2P4		0
#define BQ25720_CHARGE_OPTION_4_VSYS_UVP__3P2		1
#define BQ25720_CHARGE_OPTION_4_VSYS_UVP__4P0		2
#define BQ25720_CHARGE_OPTION_4_VSYS_UVP__4P8		3
#define BQ25720_CHARGE_OPTION_4_VSYS_UVP__5P6		4
#define BQ25720_CHARGE_OPTION_4_VSYS_UVP__6P4		5
#define BQ25720_CHARGE_OPTION_4_VSYS_UVP__7P2		6
#define BQ25720_CHARGE_OPTION_4_VSYS_UVP__8P0		7

#define BQ25720_CHARGE_OPTION_4_IDCHG_DEG2_SHIFT	6
#define BQ25720_CHARGE_OPTION_4_IDCHG_DEG2_BITS		2
#define BQ25720_CHARGE_OPTION_4_IDCHG_DEG2__1P6MS	1
#define BQ25720_CHARGE_OPTION_4_IDCHG_DEG2__12MS	3

#define BQ25720_CHARGE_OPTION_4_IDCHG_TH2_SHIFT		3
#define BQ25720_CHARGE_OPTION_4_IDCHG_TH2_BITS		3
#define BQ25720_CHARGE_OPTION_4_IDCHG_TH2__1P25		0
#define BQ25720_CHARGE_OPTION_4_IDCHG_TH2__1P5		1

#define BQ25720_CHARGE_OPTION_4_PP_IDCHG2_SHIFT		2
#define BQ25720_CHARGE_OPTION_4_PP_IDCHG2_BITS		1
#define BQ25720_CHARGE_OPTION_4_PP_IDCHG2__DISABLE	0
#define BQ25720_CHARGE_OPTION_4_PP_IDCHG2__ENABLE	1

/*
 * Vmin Active Protection Register (0x37)
 */
#define BQ25720_VMIN_AP_VSYS_TH2_SHIFT			2
#define BQ25720_VMIN_AP_VSYS_TH2_BITS			6

/*
 * ProchotOption0 Register (0x33)
 */
#define BQ257X0_PROCHOT_OPTION_0_ILIM2_VTH_SHIFT	11
#define BQ257X0_PROCHOT_OPTION_0_ILIM2_VTH_BITS		5
#define BQ257X0_PROCHOT_OPTION_0_ILIM2_VTH__1P10	1
#define BQ257X0_PROCHOT_OPTION_0_ILIM2_VTH__1P40	7
#define BQ257X0_PROCHOT_OPTION_0_ILIM2_VTH__1P50	9
#define BQ257X0_PROCHOT_OPTION_0_ILIM2_VTH__2P30	25
#define BQ257X0_PROCHOT_OPTION_0_ILIM2_VTH__2P50	26
#define BQ257X0_PROCHOT_OPTION_0_ILIM2_VTH__4P50	30
#define BQ257X0_PROCHOT_OPTION_0_ILIM2_VTH__NA		31

/*
 * ProchotOption1 Register (0x34)
 */
#define BQ257X0_PROCHOT_OPTION_1_IDCHG_VTH_SHIFT	10
#define BQ257X0_PROCHOT_OPTION_1_IDCHG_VTH_BITS		6

#define BQ257X0_PROCHOT_OPTION_1_PP_COMP_SHIFT		6
#define BQ257X0_PROCHOT_OPTION_1_PP_COMP_BITS		1
#define BQ257X0_PROCHOT_OPTION_1_PP_COMP__DISABLE	0
#define BQ257X0_PROCHOT_OPTION_1_PP_COMP__ENABLE	1

#define BQ257X0_PROCHOT_OPTION_1_PP_INOM_SHIFT		4
#define BQ257X0_PROCHOT_OPTION_1_PP_INOM_BITS		1
#define BQ257X0_PROCHOT_OPTION_1_PP_INOM__DISABLE	0
#define BQ257X0_PROCHOT_OPTION_1_PP_INOM__ENABLE	1

#define BQ257X0_PROCHOT_OPTION_1_PP_IDCHG_SHIFT		3
#define BQ257X0_PROCHOT_OPTION_1_PP_IDCHG_BITS		1
#define BQ257X0_PROCHOT_OPTION_1_PP_IDCHG__DISABLE	0
#define BQ257X0_PROCHOT_OPTION_1_PP_IDCHG__ENABLE	1

#define BQ257X0_PROCHOT_OPTION_1_PP_VDPM_SHIFT		7
#define BQ257X0_PROCHOT_OPTION_1_PP_VDPM_BITS		1
#define BQ257X0_PROCHOT_OPTION_1_PP_VDPM__DISABLE	0
#define BQ257X0_PROCHOT_OPTION_1_PP_VDPM__ENABLE	1

#define BQ257X0_PROCHOT_OPTION_1_PP_VSYS_SHIFT		2
#define BQ257X0_PROCHOT_OPTION_1_PP_VSYS_BITS		1
#define BQ257X0_PROCHOT_OPTION_1_PP_VSYS__DISABLE	0
#define BQ257X0_PROCHOT_OPTION_1_PP_VSYS__ENABLE	1

#define BQ257X0_PROCHOT_OPTION_1_PP_BATPRES_SHIFT	1
#define BQ257X0_PROCHOT_OPTION_1_PP_BATPRES_BITS	1
#define BQ257X0_PROCHOT_OPTION_1_PP_BATPRES__DISABLE	0
#define BQ257X0_PROCHOT_OPTION_1_PP_BATPRES__ENABLE	1

#define BQ257X0_PROCHOT_OPTION_1_PP_ACOK_SHIFT		0
#define BQ257X0_PROCHOT_OPTION_1_PP_ACOK_BITS		1
#define BQ257X0_PROCHOT_OPTION_1_PP_ACOK__DISABLE	0
#define BQ257X0_PROCHOT_OPTION_1_PP_ACOK__ENABLE	1

/*
 * ChargeCurrent Register (0x14)
 */
#define BQ257X0_CHARGE_CURRENT_CHARGE_CURRENT_SHIFT	6
#define BQ257X0_CHARGE_CURRENT_CHARGE_CURRENT_BITS	7

/*
 * IIN_DPM Register (0x22)
 */
#define BQ257X0_IIN_DPM_CURRENT_SHIFT			8
#define BQ257X0_IIN_DPM_CURRENT_BITS			7
#define BQ257X0_IIN_DPM_CURRENT_STEP_MA			50

/*
 * IIN_HOST Register (0x3f)
 */
#define BQ257X0_IIN_HOST_CURRENT_SHIFT			8
#define BQ257X0_IIN_HOST_CURRENT_BITS			7
#define BQ257X0_IIN_HOST_CURRENT_STEP_MA		50

/*
 * ADCOption Register (0x35)
 */
#define BQ257X0_ADC_OPTION_ADC_START_SHIFT		14
#define BQ257X0_ADC_OPTION_ADC_START_BITS		1
#define BQ257X0_ADC_OPTION_ADC_FULLSCALE_SHIFT		13
#define BQ257X0_ADC_OPTION_ADC_FULLSCALE_BITS		1

#define BQ257X0_ADC_OPTION_EN_ADC_VBUS_SHIFT		6
#define BQ257X0_ADC_OPTION_EN_ADC_VBUS_BITS		1
#define BQ257X0_ADC_OPTION_EN_ADC_ALL			GENMASK(7, 0)

/*
 * ADCVBUS/PSYS Register (0x23)
 */
#define BQ257X0_ADC_VBUS_PSYS_VBUS_SHIFT		8
#define BQ257X0_ADC_VBUS_PSYS_VBUS_BITS			8
#define BQ257X0_ADC_VBUS_PSYS_PSYS_SHIFT		0
#define BQ257X0_ADC_VBUS_PSYS_PSYS_BITS			8

/*
 * VSYS_MIN Register (0x3e)
 */
#define BQ25710_MIN_SYSTEM_VOLTAGE_SHIFT		8
#define BQ25710_MIN_SYSTEM_VOLTAGE_BITS			6
#define BQ25720_VSYS_MIN_VOLTAGE_SHIFT			8
#define BQ25720_VSYS_MIN_VOLTAGE_BITS			8

/*
 * BQ257x0 register field accessor macros.
 */

/*
 * Returns the bitmask of a register field.
 *
 *  _chip	chip name
 *  _reg	chip register name
 *  _field	register field name
 */

#define BQ_FIELD_MASK(_chip, _reg, _field)				\
	GENMASK(							\
		(_chip##_##_reg##_##_field##_SHIFT +			\
		 _chip##_##_reg##_##_field##_BITS - 1),			\
		_chip##_##_reg##_##_field##_SHIFT)

/*
 * Given a register value, returns the value of the specified field.
 *
 *  _chip	chip name
 *  _reg	chip register name
 *  _field	register field name
 *  _x		the value of the register to be examined
 */

#define GET_BQ_FIELD(_chip, _reg, _field, _x)			\
	(((_x) >> _chip##_##_reg##_##_field##_SHIFT) &		\
	 GENMASK(_chip##_##_reg##_##_field##_BITS - 1, 0))

/*
 * Given a register value, sets the specified field to the given value,
 * and returns the resulting register value.
 *
 *  _chip	chip name
 *  _reg	chip register name
 *  _field	register field name
 *  _v		register field value
 *  _x		the initial value of the register
 */

#define SET_BQ_FIELD(_chip, _reg, _field, _v, _x)			\
	(((_x) & ~BQ_FIELD_MASK(_chip, _reg, _field)) |			\
	 (((_v) &							\
	   GENMASK(_chip##_##_reg##_##_field##_BITS - 1, 0)) <<		\
	  _chip##_##_reg##_##_field##_SHIFT))

/*
 * Given a register value, sets the specified field to the predefined
 * constant, and returns the resulting register value.
 *
 *  _chip	chip name
 *  _reg	chip register name
 *  _field	register field name
 *  _c		register field value constant name
 *  _x		the initial value of the register
 */

#define SET_BQ_FIELD_BY_NAME(_chip, _reg, _field, _c, _x)		\
	SET_BQ_FIELD(_chip, _reg, _field,				\
		     _chip##_##_reg##_##_field##__##_c, (_x))

#endif /* __CROS_EC_BQ257X0_REGS_H */
