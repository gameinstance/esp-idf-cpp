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

#ifndef ESP_IO_SPI_BUS
#define ESP_IO_SPI_BUS

#include <string>
#include <basics/error.hh>
#include "driver/spi_common.h"


namespace esp {
namespace io {

/**
* @name spi_bus
*
* @brief An SPI bus component.
*/

// TODO: move this to an ESP32 specific definitions .hh file
namespace cpu_core_affinity {
	static const constexpr auto cpu_auto = esp_intr_cpu_affinity_t::ESP_INTR_CPU_AFFINITY_AUTO;
	static const constexpr auto cpu_core_0 = esp_intr_cpu_affinity_t::ESP_INTR_CPU_AFFINITY_0;
	static const constexpr auto cpu_core_1 = esp_intr_cpu_affinity_t::ESP_INTR_CPU_AFFINITY_1;
};


struct spi_bus_config {
	int8_t sck_pin;
	int8_t mosi_pin;         // DAT0
	int8_t miso_pin;         // DAT1
	int8_t quadwp_pin = -1;  // DAT2
	int8_t quadhd_pin = -1;  // DAT3
	size_t max_transfer_size = 4096;
	esp_intr_cpu_affinity_t core_affinity = cpu_core_affinity::cpu_auto;
	spi_host_device_t host_device = HSPI_HOST;
};


class spi_bus {
public:
	using config_type = spi_bus_config;

	class error_type: public basics::error {
	public:
		explicit error_type(const char *what_arg = "") noexcept
			: basics::error{std::string{"esp::io::spi_bus: "}.append(what_arg).c_str()} {}
	};

	explicit spi_bus(const config_type &config)
		: _config{config}
	{
		spi_bus_config_t spi_bus_config = {
			.mosi_io_num = _config.mosi_pin,      // DAT0 CMD
			.miso_io_num = _config.miso_pin,      // DAT1 DAT0
			.sclk_io_num = _config.sck_pin,       // SCK  CLK
			.quadwp_io_num = _config.quadwp_pin,  // DAT2
			.quadhd_io_num = _config.quadhd_pin,  // DAT3
			.data4_io_num = -1,
			.data5_io_num = -1,
			.data6_io_num = -1,
			.data7_io_num = -1,
			.max_transfer_sz = (int)_config.max_transfer_size,
			.flags{},
			.isr_cpu_id = _config.core_affinity,
			.intr_flags{}
		};

		esp_err_t ret = spi_bus_initialize(_config.host_device, &spi_bus_config, SPI_DMA_CH_AUTO);
		if (ret != ESP_OK)
			throw error_type{"esp::io::spi_bus init failed"};
	}
	spi_bus(const spi_bus&) = delete;
	spi_bus(spi_bus&& other) = delete;

	spi_bus& operator=(const spi_bus&) = delete;
	spi_bus& operator=(spi_bus&& other) = delete;

	~spi_bus()
	{
		spi_bus_free(_config.host_device);
	}

private:
	const config_type &_config;
};


};  // namespace io
};  // namespace esp

#endif // ESP_IO_SPI_BUS
