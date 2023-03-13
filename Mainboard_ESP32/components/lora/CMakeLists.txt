idf_component_register(SRCS "lora.c" "lora_esp32_config.c"
                    INCLUDE_DIRS "."
                    REQUIRES cmock driver soc esp_rom lora)

