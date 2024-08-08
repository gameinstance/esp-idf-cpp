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

#ifndef ESP_STORAGE_NVS
#define ESP_STORAGE_NVS

#include <string>
#include <basics/error.hh>
#include "nvs_flash.h"

namespace esp {
namespace storage {


/**
* @name nvs_partition
*
* @brief NVS flash partition component.
*/


class nvs_partition {
public:
	class error_type: public basics::error {
	public:
		explicit error_type(const char *what_arg = "") noexcept
			: basics::error{std::string{"esp::storage::nvs_partition: "}.append(what_arg).c_str()} {}
	};

	nvs_partition()
	{
		auto ret = nvs_flash_init();
		if (ret == ESP_ERR_NOT_FOUND)
			throw error_type{"'nvs' partition missing"};

		if ((ret == ESP_ERR_NVS_NO_FREE_PAGES) || (ret == ESP_ERR_NVS_NEW_VERSION_FOUND)) {
			if (nvs_flash_erase() != ESP_OK)
				throw error_type{"failed erasing 'nvs' partition"};

			ret = nvs_flash_init();
		}

		if (ret != ESP_OK)
			throw error_type{};
	}
	~nvs_partition()
	{
		nvs_flash_deinit();
	}
};


};  // namespace storage
};  // namespace esp

#endif // ESP_STORAGE_NVS
