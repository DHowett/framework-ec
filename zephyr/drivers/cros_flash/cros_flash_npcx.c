/* Copyright 2021 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#define DT_DRV_COMPAT nuvoton_npcx_cros_flash

#include <drivers/cros_flash.h>
#include <drivers/flash.h>
#include <drivers/gpio.h>
#include <drivers/spi.h>
#include <kernel.h>
#include <logging/log.h>
#include <soc.h>

#include "flash.h"
#include "gpio.h"
#include "spi_flash_reg.h"
#include "../drivers/flash/spi_nor.h"

LOG_MODULE_REGISTER(cros_flash, LOG_LEVEL_ERR);

static int all_protected; /* Has all-flash protection been requested? */
static int addr_prot_start;
static int addr_prot_length;
static uint8_t flag_prot_inconsistent;
static uint8_t saved_sr1;
static uint8_t saved_sr2;

/* Device data */
struct cros_flash_npcx_data {
	const struct device *flash_dev;
	const struct device *spi_ctrl_dev;
};

static struct spi_config spi_cfg;

#define FLASH_DEV DT_NODELABEL(int_flash)
#define SPI_CONTROLLER_DEV DT_NODELABEL(spi_fiu0)

#define DRV_DATA(dev) ((struct cros_flash_npcx_data *)(dev)->data)

#define SPI_NOR_CMD_RDSR2 0x35

/* cros ec flash local functions */
static int cros_flash_npcx_get_status_reg(const struct device *dev,
					  uint8_t cmd_code, uint8_t *data)
{
	uint8_t opcode;
	struct cros_flash_npcx_data *dev_data = DRV_DATA(dev);

	struct spi_buf spi_buf[2] = {
		[0] = {
			.buf = &opcode,
			.len = 1,
		},
		[1] = {
			.buf = data,
			.len = 1,
		}
	};

	const struct spi_buf_set tx_set = {
		.buffers = spi_buf,
		.count = 2,
	};

	const struct spi_buf_set rx_set = {
		.buffers = spi_buf,
		.count = 2,
	};

	if (data == 0)
		return -EINVAL;

	opcode = cmd_code;
	return spi_transceive(dev_data->spi_ctrl_dev, &spi_cfg, &tx_set,
			      &rx_set);
}

static int cros_flash_npcx_wait_ready(const struct device *dev)
{
	int wait_period = 10; /* 10 us period t0 check status register */
	int timeout = (10 * USEC_PER_SEC) / wait_period; /* 10 seconds */

	do {
		uint8_t reg;

		cros_flash_npcx_get_status_reg(dev, SPI_NOR_CMD_RDSR, &reg);
		if ((reg & SPI_NOR_WIP_BIT) == 0)
			break;
		k_usleep(wait_period);
	} while (--timeout); /* Wait for busy bit clear */

	if (timeout) {
		return 0;
	} else {
		return -ETIMEDOUT;
	}
}

/* Check the BUSY bit is cleared and WE bit is set */
static int cros_flash_npcx_wait_ready_and_we(const struct device *dev)
{
	int wait_period = 10; /* 10 us period t0 check status register */
	int timeout = (10 * USEC_PER_SEC) / wait_period; /* 10 seconds */

	do {
		uint8_t reg;

		cros_flash_npcx_get_status_reg(dev, SPI_NOR_CMD_RDSR, &reg);
		if ((reg & SPI_NOR_WIP_BIT) == 0 &&
		    (reg & SPI_NOR_WEL_BIT) != 0)
			break;
		k_usleep(wait_period);
	} while (--timeout); /* Wait for busy bit clear */

	if (timeout) {
		return 0;
	} else {
		return -ETIMEDOUT;
	}
}

static int cros_flash_npcx_set_write_enable(const struct device *dev)
{
	int ret;
	uint8_t opcode = SPI_NOR_CMD_WREN;
	struct cros_flash_npcx_data *data = DRV_DATA(dev);

	struct spi_buf spi_buf = {
		.buf = &opcode,
		.len = 1,
	};

	const struct spi_buf_set tx_set = {
		.buffers = &spi_buf,
		.count = 1,
	};

	/* Wait for previous operation to complete */
	ret = cros_flash_npcx_wait_ready(dev);
	if (ret != 0)
		return ret;

	/* Write enable command */
	ret = spi_transceive(data->spi_ctrl_dev, &spi_cfg, &tx_set, NULL);
	if (ret != 0)
		return ret;

	/* Wait for flash is not busy */
	return  cros_flash_npcx_wait_ready_and_we(dev);
}

static int cros_flash_npcx_set_status_reg(const struct device *dev, char *data)
{
	uint8_t  opcode = SPI_NOR_CMD_WRSR;
	int ret = 0;
	struct cros_flash_npcx_data *dev_data = DRV_DATA(dev);

	struct spi_buf spi_buf[2] = {
		[0] = {
			.buf = &opcode,
			.len = 1,
		},
		[1] = {
			.buf = data,
			.len = 2,
		}
	};

	const struct spi_buf_set tx_set = {
		.buffers = spi_buf,
		.count = 2,
	};

	if (data == 0) {
		return -EINVAL;
	}

	/* Enable write */
	ret = cros_flash_npcx_set_write_enable(dev);
	if (ret != 0)
		return ret;

	ret = spi_transceive(dev_data->spi_ctrl_dev, &spi_cfg, &tx_set, NULL);
	if (ret != 0)
		return ret;
	return cros_flash_npcx_wait_ready(dev);
}

static int cros_flash_npcx_write_protection_set(const struct device *dev,
						bool enable)
{
	int ret = 0;

	/* Write protection can be cleared only by core domain reset */
	if (!enable) {
		LOG_ERR("WP can be disabled only via core domain reset ");
		return -ENOTSUP;
	}
	ret = npcx_pinctrl_flash_write_protect_set();

	return ret;
}

static int cros_flash_npcx_write_protection_is_set(const struct device *dev)
{
	return npcx_pinctrl_flash_write_protect_is_set();
}

static int cros_flash_npcx_uma_lock(const struct device *dev, bool enable)
{
	struct cros_flash_npcx_data *data = DRV_DATA(dev);

	if (enable) {
		spi_cfg.operation |= SPI_LOCK_ON;
	} else {
		spi_cfg.operation &= ~SPI_LOCK_ON;
	}

	return spi_transceive(data->spi_ctrl_dev, &spi_cfg, NULL, NULL);
}

static int flash_get_status1(const struct device *dev)
{
	uint8_t reg;

	if (all_protected)
		return saved_sr1;

	/* Lock physical flash operations */
	crec_flash_lock_mapped_storage(1);

	cros_flash_npcx_get_status_reg(dev, SPI_NOR_CMD_RDSR, &reg);

	/* Unlock physical flash operations */
	crec_flash_lock_mapped_storage(0);

	return reg;
}

static int flash_get_status2(const struct device *dev)
{
	uint8_t reg;

	if (all_protected)
		return saved_sr1;

	/* Lock physical flash operations */
	crec_flash_lock_mapped_storage(1);

	cros_flash_npcx_get_status_reg(dev, SPI_NOR_CMD_RDSR2, &reg);

	/* Unlock physical flash operations */
	crec_flash_lock_mapped_storage(0);

	return reg;
}

static int flash_write_status_reg(const struct device *dev, uint8_t *data)
{
	return cros_flash_npcx_set_status_reg(dev, data);
}

static int is_int_flash_protected(const struct device *dev)
{
	return cros_flash_npcx_write_protection_is_set(dev);
}

static void flash_protect_int_flash(const struct device *dev, int enable)
{
	/*
	 * Please notice the type of WP_IF bit is R/W1S. Once it's set,
	 * only rebooting EC can clear it.
	 */
	if (enable)
		cros_flash_npcx_write_protection_set(dev, enable);
}

static void flash_uma_lock(const struct device *dev, int enable)
{
	if (enable && !all_protected) {
		/*
		 * Store SR1 / SR2 for later use since we're about to lock
		 * out all access (including read access) to these regs.
		 */
		saved_sr1 = flash_get_status1(dev);
		saved_sr2 = flash_get_status2(dev);
	}

	cros_flash_npcx_uma_lock(dev, enable);
	all_protected = enable;
}

static int flash_set_status_for_prot(const struct device *dev, int reg1,
				     int reg2)
{
	uint8_t regs[2];

	/*
	 * Writing SR regs will fail if our UMA lock is enabled. If WP
	 * is deasserted then remove the lock and allow the write.
	 */
	if (all_protected) {
		if (is_int_flash_protected(dev))
			return EC_ERROR_ACCESS_DENIED;

		if (crec_flash_get_protect() & EC_FLASH_PROTECT_GPIO_ASSERTED)
			return EC_ERROR_ACCESS_DENIED;
		flash_uma_lock(dev, 0);
	}

	/*
	 * If WP# is active and ec doesn't protect the status registers of
	 * internal spi-flash, protect it now before setting them.
	 */
#ifdef CONFIG_WP_ACTIVE_HIGH
	flash_protect_int_flash(dev, gpio_get_level(GPIO_WP));
#else
	flash_protect_int_flash(dev, !gpio_get_level(GPIO_WP_L));
#endif /*_CONFIG_WP_ACTIVE_HIGH_*/

	/* Lock physical flash operations */
	crec_flash_lock_mapped_storage(1);

	regs[0] = reg1;
	regs[1] = reg2;
	flash_write_status_reg(dev, regs);

	/* Unlock physical flash operations */
	crec_flash_lock_mapped_storage(0);

	spi_flash_reg_to_protect(reg1, reg2, &addr_prot_start,
				 &addr_prot_length);

	return EC_SUCCESS;
}

static int flash_check_prot_reg(const struct device *dev, unsigned int offset,
				unsigned int bytes)
{
	unsigned int start;
	unsigned int len;
	uint8_t sr1, sr2;
	int rv = EC_SUCCESS;

	/*
	 * If WP# is active and ec doesn't protect the status registers of
	 * internal spi-flash, protect it now.
	 */
#ifdef CONFIG_WP_ACTIVE_HIGH
	flash_protect_int_flash(dev, gpio_get_level(GPIO_WP));
#else
	flash_protect_int_flash(dev, !gpio_get_level(GPIO_WP_L));
#endif /* CONFIG_WP_ACTIVE_HIGH */

	sr1 = flash_get_status1(dev);
	sr2 = flash_get_status2(dev);

	/* Invalid value */
	if (offset + bytes > CONFIG_FLASH_SIZE_BYTES)
		return EC_ERROR_INVAL;

	/* Compute current protect range */
	rv = spi_flash_reg_to_protect(sr1, sr2, &start, &len);
	if (rv)
		return rv;

	/* Check if ranges overlap */
	if (MAX(start, offset) < MIN(start + len, offset + bytes))
		return EC_ERROR_ACCESS_DENIED;

	return EC_SUCCESS;
}

static int flash_write_prot_reg(const struct device *dev, unsigned int offset,
				unsigned int bytes, int hw_protect)
{
	int rv;
	uint8_t sr1 = flash_get_status1(dev);
	uint8_t sr2 = flash_get_status2(dev);

	/* Invalid values */
	if (offset + bytes > CONFIG_FLASH_SIZE_BYTES)
		return EC_ERROR_INVAL;

	/* Compute desired protect range */
	rv = spi_flash_protect_to_reg(offset, bytes, &sr1, &sr2);
	if (rv)
		return rv;

	if (hw_protect)
		sr1 |= SPI_FLASH_SR1_SRP0;

	return flash_set_status_for_prot(dev, sr1, sr2);
}

static int flash_check_prot_range(unsigned int offset, unsigned int bytes)
{
	/* Invalid value */
	if (offset + bytes > CONFIG_FLASH_SIZE_BYTES)
		return EC_ERROR_INVAL;

	/* Check if ranges overlap */
	if (MAX(addr_prot_start, offset) <
	    MIN(addr_prot_start + addr_prot_length, offset + bytes))
		return EC_ERROR_ACCESS_DENIED;

	return EC_SUCCESS;
}

/* cros ec flash api functions */
static int cros_flash_npcx_init(const struct device *dev)
{
	/*
	 * Protect status registers of internal spi-flash if WP# is active
	 * during ec initialization.
	 */
#ifdef CONFIG_WP_ACTIVE_HIGH
	flash_protect_int_flash(dev, gpio_get_level(GPIO_WP));
#else
	flash_protect_int_flash(dev, !gpio_get_level(GPIO_WP_L));
#endif /*CONFIG_WP_ACTIVE_HIGH */

	/* Initialize UMA to unlocked */
	flash_uma_lock(dev, 0);

	return 0;
}

/* TODO(b/205175314): Migrate cros-flash driver to Zephyr flash driver) */
static int cros_flash_npcx_read(const struct device *dev, int offset, int size,
				char *dst_data)
{
	struct cros_flash_npcx_data *data = DRV_DATA(dev);

	return flash_read(data->flash_dev, offset, dst_data, size);
}

static int cros_flash_npcx_write(const struct device *dev, int offset, int size,
				 const char *src_data)
{
	int ret = 0;
	struct cros_flash_npcx_data *data = DRV_DATA(dev);

	/* check protection */
	if (all_protected)
		return EC_ERROR_ACCESS_DENIED;

	/* check protection */
	if (flash_check_prot_range(offset, size))
		return EC_ERROR_ACCESS_DENIED;

	/* Invalid data pointer? */
	if (src_data == 0) {
		return -EINVAL;
	}

	ret = flash_write(data->flash_dev, offset, src_data, size);

	return ret;
}

static int cros_flash_npcx_erase(const struct device *dev, int offset, int size)
{
	int ret = 0;
	struct cros_flash_npcx_data *data = DRV_DATA(dev);

	/* check protection */
	if (all_protected)
		return EC_ERROR_ACCESS_DENIED;

	/* check protection */
	if (flash_check_prot_range(offset, size))
		return EC_ERROR_ACCESS_DENIED;

	/* address must be aligned to erase size */
	if ((offset % CONFIG_FLASH_ERASE_SIZE) != 0) {
		return -EINVAL;
	}

	/* Erase size must be a non-zero multiple of sectors */
	if ((size == 0) || (size % CONFIG_FLASH_ERASE_SIZE) != 0) {
		return -EINVAL;
	}

	ret = flash_erase(data->flash_dev, offset, size);

	return ret;
}

static int cros_flash_npcx_get_protect(const struct device *dev, int bank)
{
	uint32_t addr = bank * CONFIG_FLASH_BANK_SIZE;

	return flash_check_prot_reg(dev, addr, CONFIG_FLASH_BANK_SIZE);
}

static uint32_t cros_flash_npcx_get_protect_flags(const struct device *dev)
{
	uint32_t flags = 0;

	/* Check if WP region is protected in status register */
	if (flash_check_prot_reg(dev, WP_BANK_OFFSET * CONFIG_FLASH_BANK_SIZE,
				 WP_BANK_COUNT * CONFIG_FLASH_BANK_SIZE))
		flags |= EC_FLASH_PROTECT_RO_AT_BOOT;

	/*
	 * TODO: If status register protects a range, but SRP0 is not set,
	 * flags should indicate EC_FLASH_PROTECT_ERROR_INCONSISTENT.
	 */
	if (flag_prot_inconsistent)
		flags |= EC_FLASH_PROTECT_ERROR_INCONSISTENT;

	/* Read all-protected state from our shadow copy */
	if (all_protected)
		flags |= EC_FLASH_PROTECT_ALL_NOW;

	return flags;
}

static int cros_flash_npcx_protect_at_boot(const struct device *dev,
					   uint32_t new_flags)
{
	int ret;

	if ((new_flags & (EC_FLASH_PROTECT_RO_AT_BOOT |
			  EC_FLASH_PROTECT_ALL_AT_BOOT)) == 0) {
		/* Clear protection bits in status register */
		return flash_set_status_for_prot(dev, 0, 0);
	}

	ret = flash_write_prot_reg(dev, CONFIG_WP_STORAGE_OFF,
				   CONFIG_WP_STORAGE_SIZE, 1);

	/*
	 * Set UMA_LOCK bit for locking all UMA transaction.
	 * But we still can read directly from flash mapping address
	 */
	if (new_flags & EC_FLASH_PROTECT_ALL_AT_BOOT)
		flash_uma_lock(dev, 1);

	return ret;
}

static int cros_flash_npcx_protect_now(const struct device *dev, int all)
{
	if (all) {
		/*
		 * Set UMA_LOCK bit for locking all UMA transaction.
		 * But we still can read directly from flash mapping address
		 */
		flash_uma_lock(dev, 1);
	} else {
		/* TODO: Implement RO "now" protection */
	}

	return EC_SUCCESS;
}

static int cros_flash_npcx_get_jedec_id(const struct device *dev,
					uint8_t *manufacturer,
					uint16_t *device)
{
	int ret;
	uint8_t jedec_id[3];
	struct cros_flash_npcx_data *data = DRV_DATA(dev);

	/* Lock physical flash operations */
	crec_flash_lock_mapped_storage(1);

	ret = flash_read_jedec_id(data->flash_dev, jedec_id);
	if (ret == 0) {
		*manufacturer = jedec_id[0];
		*device = (jedec_id[1] << 8) | jedec_id[2];
	}

	/* Unlock physical flash operations */
	crec_flash_lock_mapped_storage(0);

	return ret;
}

static int cros_flash_npcx_get_status(const struct device *dev,
		uint8_t *sr1, uint8_t *sr2)
{
	*sr1 = flash_get_status1(dev);
	*sr2 = flash_get_status2(dev);

	return EC_SUCCESS;
}

/* cros ec flash driver registration */
static const struct cros_flash_driver_api cros_flash_npcx_driver_api = {
	.init = cros_flash_npcx_init,
	.physical_read = cros_flash_npcx_read,
	.physical_write = cros_flash_npcx_write,
	.physical_erase = cros_flash_npcx_erase,
	.physical_get_protect = cros_flash_npcx_get_protect,
	.physical_get_protect_flags = cros_flash_npcx_get_protect_flags,
	.physical_protect_at_boot = cros_flash_npcx_protect_at_boot,
	.physical_protect_now = cros_flash_npcx_protect_now,
	.physical_get_jedec_id = cros_flash_npcx_get_jedec_id,
	.physical_get_status = cros_flash_npcx_get_status,
};

static int flash_npcx_init(const struct device *dev)
{
	struct cros_flash_npcx_data *data = DRV_DATA(dev);

	data->flash_dev = DEVICE_DT_GET(FLASH_DEV);
	if (!device_is_ready(data->flash_dev)) {
		LOG_ERR("%s device not ready", data->flash_dev->name);
		return -ENODEV;
	}

	data->spi_ctrl_dev = DEVICE_DT_GET(SPI_CONTROLLER_DEV);
	if (!device_is_ready(data->spi_ctrl_dev)) {
		LOG_ERR("%s device not ready", data->spi_ctrl_dev->name);
		return -ENODEV;
	}

	return EC_SUCCESS;
}

#if CONFIG_CROS_FLASH_NPCX_INIT_PRIORITY <= CONFIG_SPI_NOR_INIT_PRIORITY
#error "CONFIG_CROS_FLASH_NPCX_INIT_PRIORITY must be greater than" \
	"CONFIG_SPI_NOR_INIT_PRIORITY."
#endif
static struct cros_flash_npcx_data cros_flash_data;
DEVICE_DT_INST_DEFINE(0, flash_npcx_init, NULL, &cros_flash_data, NULL,
		      POST_KERNEL, CONFIG_CROS_FLASH_NPCX_INIT_PRIORITY,
		      &cros_flash_npcx_driver_api);
