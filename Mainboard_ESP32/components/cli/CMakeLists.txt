idf_component_register(SRC_DIRS "."
                    INCLUDE_DIRS "."
                    REQUIRES cmock sys console)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")