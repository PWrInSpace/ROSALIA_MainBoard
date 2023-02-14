idf_component_register(SRCS "lora.c" "uart_interface.c"
                    INCLUDE_DIRS "."
                    REQUIRES cmock driver freertos soc)

