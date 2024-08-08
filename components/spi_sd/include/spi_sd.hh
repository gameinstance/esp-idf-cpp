/* Copyright (C) 2024  Bogdan-Gabriel Alecu  (GameInstance.com)
 *
 * esp-idf-cpp - ESP-IDF C++ wrappers.
 *
 * This library is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ESP_IO_SPI_SD
#define ESP_IO_SPI_SD

#include <string>
#include <basics/error.hh>
#include "esp_vfs_fat.h"
#include "spi_bus.hh"


namespace esp {
namespace io {

/**
 * @name spi_sd
 *
 * @brief An SPI SD-card mounting component.
 */


struct spi_sd_config {
	spi_bus_config spi_bus;
	uint8_t cs_pin;
	uint8_t cd_pin = GPIO_NUM_NC;
	const char * mount_point = "/sd";
	uint8_t max_files = 5;
	bool format_if_empty = false;
	bool disk_status_check_enable = true;
};

struct spi_sd_deps {
	sdmmc_host_t &host;
};


class spi_sd {
public:
	using config_type = spi_sd_config;
	using deps_type = spi_sd_deps;

	class error_type: public basics::error {
	public:
		explicit error_type(const char *what_arg = "") noexcept
			: basics::error{std::string{"esp::io::spi_sd: "}.append(what_arg).c_str()} {}
	};

	spi_sd(const config_type &config, deps_type &deps)
		: _config{config}, _card{nullptr}
	{
		esp_vfs_fat_sdmmc_mount_config_t mount_config = {
			.format_if_mount_failed = _config.format_if_empty,
			.max_files = _config.max_files,
			.allocation_unit_size = 512,
			.disk_status_check_enable = _config.disk_status_check_enable
		};

		// initializes the slot without card detect (CD) and write protect (WP) signals
		sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
		slot_config.gpio_cs = (gpio_num_t)_config.cs_pin;
// 		slot_config.gpio_cd = (gpio_num_t)_config.cd_pin;
		slot_config.host_id = _config.spi_bus.host_device;

		esp_err_t ret = esp_vfs_fat_sdspi_mount(_config.mount_point, &deps.host, &slot_config,
																				&mount_config, &_card);
		if (ret != ESP_OK) {
			if (ret == ESP_ERR_INVALID_STATE)
				throw error_type{"already mounted"};
			if (ret == ESP_ERR_NO_MEM)
				throw error_type{"failed to allocate memory"};
			if (ret == ESP_FAIL)
				throw error_type{"failed to mount filesystem"};

			throw error_type{"failed to initialize card"};
		}
	}

	~spi_sd()
	{
		esp_vfs_fat_sdcard_unmount(_config.mount_point, _card);
	}

	sdmmc_card_t *card() const
	{
		return _card;
	}

private:
	const config_type &_config;
	sdmmc_card_t *_card;
};


};  // namespace io
};  // namespace esp

#endif // ESP_IO_SPI_SD
