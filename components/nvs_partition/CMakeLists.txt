idf_component_register(SRCS "nvs_partition.cc"
						INCLUDE_DIRS "include"
						REQUIRES nvs_flash
						PRIV_REQUIRES basics)
