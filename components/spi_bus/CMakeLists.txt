idf_component_register(SRCS "spi_bus.cc"
                    INCLUDE_DIRS "include"
                    REQUIRES basics driver)

#target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++17)

#component_compile_options(-std=gnu++17)
