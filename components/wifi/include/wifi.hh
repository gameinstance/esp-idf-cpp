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

#ifndef ESP_IO_WIFI
#define ESP_IO_WIFI

#include <iostream>
#include <string>
#include <basics/error.hh>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"

namespace esp {
namespace io {

/**
* @name wifi station
*
* @brief WiFi station component.
*
* @todo  Relocate most components.
*/


class event_group {
public:
	class error_type: public basics::error {
	public:
		explicit error_type(const char *what_arg = "") noexcept
			: basics::error{std::string{"esp::io::event_group: "}.append(what_arg).c_str()} {}
	};

	event_group()
		: _base{xEventGroupCreate()}
	{
		if (_base == nullptr)
			throw error_type{"creation failed"};
	}
	event_group(const event_group&) = delete;
	event_group(event_group&& other) = delete;

	event_group& operator=(const event_group&) = delete;
	event_group& operator=(event_group&& other) = delete;

	~event_group()
	{
		vEventGroupDelete(_base);
	}

	EventGroupHandle_t get()
	{
		return _base;
	}

private:
	EventGroupHandle_t _base;
};

class esp_event_loop_default {
public:
	class error_type: public basics::error {
	public:
		explicit error_type(const char *what_arg = "") noexcept
			: basics::error{std::string{"esp::io::esp_event_loop_default: "}.append(what_arg).c_str()} {}
	};

	esp_event_loop_default()
	{
		if (esp_event_loop_create_default() != ESP_OK)
			throw error_type{"creation failed"};
	}
	esp_event_loop_default(const esp_event_loop_default&) = delete;
	esp_event_loop_default(esp_event_loop_default&& other) = delete;

	esp_event_loop_default& operator=(const esp_event_loop_default&) = delete;
	esp_event_loop_default& operator=(esp_event_loop_default&& other) = delete;

	~esp_event_loop_default()
	{
		esp_event_loop_delete_default();
	}
};


class esp_netif {
public:
	class error_type: public basics::error {
	public:
		explicit error_type(const char *what_arg = "") noexcept
			: basics::error{std::string{"esp::io::esp_netif: "}.append(what_arg).c_str()} {}
	};

	esp_netif()
	{
		if (esp_netif_init() != ESP_OK)
			throw error_type{"creation failed"};
	}
	esp_netif(const esp_netif&) = delete;
	esp_netif(esp_netif&& other) = delete;

	esp_netif& operator=(const esp_netif&) = delete;
	esp_netif& operator=(esp_netif&& other) = delete;

	~esp_netif()
	{
		esp_netif_deinit();
	}
};

class esp_netif_default_wifi_sta {
public:
	esp_netif_default_wifi_sta()
		: _base{esp_netif_create_default_wifi_sta()}  // WARNING: on init error this API aborts
	{
	}
	esp_netif_default_wifi_sta(const esp_netif_default_wifi_sta&) = delete;
	esp_netif_default_wifi_sta(esp_netif_default_wifi_sta&& other) = delete;

	esp_netif_default_wifi_sta& operator=(const esp_netif_default_wifi_sta&) = delete;
	esp_netif_default_wifi_sta& operator=(esp_netif_default_wifi_sta&& other) = delete;

	~esp_netif_default_wifi_sta()
	{
		esp_netif_destroy_default_wifi(_base);
	}

private:
	esp_netif_t *_base;
};


class esp_wifi {
public:
	class error_type: public basics::error {
	public:
		explicit error_type(const char *what_arg = "") noexcept
			: basics::error{std::string{"esp::io::esp_wifi: "}.append(what_arg).c_str()} {}
	};

	esp_wifi()
	{
		_config = WIFI_INIT_CONFIG_DEFAULT();
		if (esp_wifi_init(&_config) != ESP_OK)
			throw error_type{"creation failed"};
	}
	esp_wifi(const esp_wifi&) = delete;
	esp_wifi(esp_wifi&& other) = delete;

	esp_wifi& operator=(const esp_wifi&) = delete;
	esp_wifi& operator=(esp_wifi&& other) = delete;

	~esp_wifi()
	{
		esp_wifi_deinit();
	}

private:
	wifi_init_config_t _config;
};

class esp_wifi_station {
public:
	class error_type: public basics::error {
	public:
		explicit error_type(const char *what_arg = "") noexcept
			: basics::error{std::string{"esp::io::esp_wifi_station: "}.append(what_arg).c_str()} {}
	};

	esp_wifi_station(const uint8_t *ssid, const uint8_t *pass)
	{
		if (esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK)
			throw error_type{"set mode failed"};

		wifi_config_t wifi_config = {
			.sta = {
				.ssid = "",      // WARNING: need to copy data into this uint8_t array; see below
				.password = "",  // WARNING: need to copy data into this uint8_t array; see below
				.scan_method = WIFI_FAST_SCAN,
				.bssid_set = false,
				.bssid = "",
				.channel = 0,
				.listen_interval = 0,
				.sort_method = WIFI_CONNECT_AP_BY_SECURITY,
				.threshold = wifi_scan_threshold_t{
					.rssi = 0,
					.authmode = WIFI_AUTH_WPA2_PSK,
				},
				.pmf_cfg = wifi_pmf_config_t{
					.capable = false,
					.required = false,
				},
				.rm_enabled = 0,
				.btm_enabled = 0,
				.mbo_enabled = 0,
				.ft_enabled = 0,
				.owe_enabled = 0,
				.transition_disable = 0,
				.reserved = 0,
				.sae_pwe_h2e = WPA3_SAE_PWE_UNSPECIFIED /*ESP_WIFI_SAE_MODE*/,
				.sae_pk_mode = WPA3_SAE_PK_MODE_AUTOMATIC,
				.failure_retry_cnt = 2,
				.he_dcm_set = 0,
				.he_dcm_max_constellation_tx = 3,
				.he_dcm_max_constellation_rx = 3,
				.he_mcs9_enabled = 0,
				.he_su_beamformee_disabled = 0,
				.he_trig_su_bmforming_feedback_disabled = 0,
				.he_trig_mu_bmforming_partial_feedback_disabled = 0,
				.he_trig_cqi_feedback_disabled = 0,
				.he_reserved = 0,
				.sae_h2e_identifier = "",
			}
		};
		std::memcpy(wifi_config.sta.ssid, ssid, 32);
		std::memcpy(wifi_config.sta.password, pass, 64);

		if (esp_wifi_set_config(WIFI_IF_STA, &wifi_config) != ESP_OK)
			throw error_type{"configuration failed"};

		if (esp_wifi_start() != ESP_OK)
			throw error_type{"start failed"};
	}
	esp_wifi_station(const esp_wifi_station&) = delete;
	esp_wifi_station(esp_wifi_station&& other) = delete;

	esp_wifi_station& operator=(const esp_wifi_station&) = delete;
	esp_wifi_station& operator=(esp_wifi_station&& other) = delete;

	~esp_wifi_station()
	{
		esp_wifi_stop();
	}
};


class esp_wifi_event_handler {
public:
	class error_type: public basics::error {
	public:
		explicit error_type(const char *what_arg = "") noexcept
			: basics::error{std::string{"esp::io::esp_wifi_event_handler: "}.append(what_arg).c_str()} {}
	};

	esp_wifi_event_handler(esp_event_handler_t event_handler, void *event_arg)
		: _instance{}
	{
		if (esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
													event_handler, event_arg, &_instance) != ESP_OK)
			throw error_type{"creation failed"};
	}
	esp_wifi_event_handler(const esp_wifi_event_handler&) = delete;
	esp_wifi_event_handler(esp_wifi_event_handler&& other) = delete;

	esp_wifi_event_handler& operator=(const esp_wifi_event_handler&) = delete;
	esp_wifi_event_handler& operator=(esp_wifi_event_handler&& other) = delete;

	~esp_wifi_event_handler()
	{
		esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, _instance);
	}

private:
	esp_event_handler_instance_t _instance;
};

class esp_connection_event_handler {
public:
	class error_type: public basics::error {
	public:
		explicit error_type(const char *what_arg = "") noexcept
			: basics::error{std::string{"esp::io::esp_connection_event_handler: "}.append(what_arg).c_str()} {}
	};

	esp_connection_event_handler(esp_event_handler_t event_handler, void *event_arg)
		: _instance{}
	{
		if (esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
				event_handler, event_arg, &_instance) != ESP_OK)
			throw error_type{"creation failed"};
	}
	esp_connection_event_handler(const esp_connection_event_handler&) = delete;
	esp_connection_event_handler(esp_connection_event_handler&& other) = delete;

	esp_connection_event_handler& operator=(const esp_connection_event_handler&) = delete;
	esp_connection_event_handler& operator=(esp_connection_event_handler&& other) = delete;

	~esp_connection_event_handler()
	{
		esp_event_handler_instance_unregister(WIFI_EVENT, IP_EVENT_STA_GOT_IP, _instance);
	}

private:
	esp_event_handler_instance_t _instance;
};


class wifi_sta {
public:
	class error_type: public basics::error {
	public:
		explicit error_type(const char *what_arg = "") noexcept
			: basics::error{std::string{"esp::io::wifi_sta: "}.append(what_arg).c_str()} {}
	};

	wifi_sta(const uint8_t *ssid, const uint8_t *pass, const size_t max_reconnect_count = 0)
		: _event_group{}, _state{_event_group.get(), 0, max_reconnect_count},
		  _esp_netif{}, _esp_event_loop_default{}, _esp_netif_default_wifi_sta{}, _esp_wifi{},
		  _esp_wifi_event_handler{&esp::io::wifi_sta::event_handler, &_state},
		  _esp_connection_event_handler{&esp::io::wifi_sta::event_handler, &_state},
		  _esp_wifi_station{ssid, pass}
	{
		EventBits_t bits = xEventGroupWaitBits(_event_group.get(), _wifi_connected_bit | _wifi_fail_bit,
																	pdFALSE, pdFALSE, portMAX_DELAY);

		if (bits & _wifi_connected_bit) {
			std::cout << "connected to SSID: " << ssid << std::endl;
		} else if (bits & _wifi_fail_bit) {
			std::cout << "failed to connect to SSID: " << ssid << std::endl;

			throw error_type{"connection failed"};
		} else {
			std::cout << "unexpected wifi connection event" << std::endl;

			throw error_type{"unexpected connection event"};
		}
	}
	wifi_sta(const wifi_sta&) = delete;
	wifi_sta(wifi_sta&& other) = delete;

	wifi_sta& operator=(const wifi_sta&) = delete;
	wifi_sta& operator=(wifi_sta&& other) = delete;

	~wifi_sta() = default;

	static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
	{
		_state_type *state = (_state_type *) arg;

		if ((event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_STA_START)) {
			esp_wifi_connect();
		} else if ((event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_STA_DISCONNECTED)) {
			if ((state->max_reconnect_count == 0)
										|| (state->reconnect_count < state->max_reconnect_count)) {
				printf("(re)trying to connect to the AP\n");
				esp_wifi_connect();

				++state->reconnect_count;
			} else {
				xEventGroupSetBits(state->event_group, _wifi_fail_bit);
			}
		} else if ((event_base == IP_EVENT) && (event_id == IP_EVENT_STA_GOT_IP)) {
			ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
			printf("got ip: " IPSTR "\n", IP2STR(&event->ip_info.ip));

			state->reconnect_count = 0;

			xEventGroupSetBits(state->event_group, _wifi_connected_bit);
		}
	}

private:
	struct _state_type {
		EventGroupHandle_t event_group;
		size_t reconnect_count;
		const size_t max_reconnect_count;
	};

	static const constexpr EventBits_t _wifi_connected_bit = 1 << 0;
	static const constexpr EventBits_t _wifi_fail_bit      = 1 << 1;

	event_group _event_group;
	_state_type _state;
	esp_netif _esp_netif;
	esp_event_loop_default _esp_event_loop_default;
	esp_netif_default_wifi_sta _esp_netif_default_wifi_sta;
	esp_wifi _esp_wifi;
	esp_wifi_event_handler _esp_wifi_event_handler;
	esp_connection_event_handler _esp_connection_event_handler;
	esp_wifi_station _esp_wifi_station;
};


};  // namespace io
};  // namespace esp

#endif // ESP_IO_WIFI
