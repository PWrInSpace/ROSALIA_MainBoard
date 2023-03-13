idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ""
                    REQUIRES driver lora console fatfs)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
