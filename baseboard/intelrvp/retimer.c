/* Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Intel BASEBOARD-RVP Retimer specific configuration */

#include "bb_retimer.h"
#include "compile_time_macros.h"
#include "common.h"

/* USB Retimers configuration */
#ifdef CONFIG_USBC_RETIMER_INTEL_BB
const struct bb_usb_control bb_controls[] = {
	[TYPE_C_PORT_0] = {
		.shared_nvm = USB_PORT0_BB_RETIMER_SHARED_NVM,
		.usb_ls_en_gpio = GPIO_USB_C0_LS_EN,
		.retimer_rst_gpio = GPIO_USB_C0_RETIMER_RST,
		.force_power_gpio = GPIO_USB_C0_RETIMER_FORCE_PWR,
	},
#ifdef HAS_TASK_PD_C1
	[TYPE_C_PORT_1] = {
		.shared_nvm = USB_PORT1_BB_RETIMER_SHARED_NVM,
		.usb_ls_en_gpio = GPIO_USB_C1_LS_EN,
		.retimer_rst_gpio = GPIO_USB_C1_RETIMER_RST,
		.force_power_gpio = GPIO_USB_C1_RETIMER_FORCE_PWR,
	},
#endif /* HAS_TASK_PD_C1 */
};
BUILD_ASSERT(ARRAY_SIZE(bb_controls) == CONFIG_USB_PD_PORT_MAX_COUNT);

struct usb_retimer usb_retimers[] = {
	[TYPE_C_PORT_0] = {
		.driver = &bb_usb_retimer,
		.i2c_port = I2C_PORT0_BB_RETIMER,
		.i2c_addr_flags = I2C_PORT0_BB_RETIMER_ADDR,
	},
#ifdef HAS_TASK_PD_C1
	[TYPE_C_PORT_1] = {
		.driver = &bb_usb_retimer,
		.i2c_port = I2C_PORT1_BB_RETIMER,
		.i2c_addr_flags = I2C_PORT1_BB_RETIMER_ADDR,
	},
#endif /* HAS_TASK_PD_C1 */
};
BUILD_ASSERT(ARRAY_SIZE(usb_retimers) == CONFIG_USB_PD_PORT_MAX_COUNT);
#endif /* CONFIG_USBC_RETIMER_INTEL_BB */
