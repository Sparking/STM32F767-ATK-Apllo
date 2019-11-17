bsp_path := $(PROJECT_ROOT)/board/bsp/$(BOARD)

_src_files := base/stm32f7xx_hal_msp.c base/stm32f7xx_it.c base/sys.c base/iic.c base/gpio.c \
              base/random_number_generator.c \
              driver/stm32f767-atk-apllo-iic.c \
              driver/stm32f767-atk-apllo-led.c \
              driver/stm32f767-atk-apllo-lcd.c \
              driver/stm32f767-atk-apllo-sdram.c
_include_path := base/include driver/include

CPPFLAGS += $(addprefix -I$(bsp_path)/, $(_include_path))
SOURCE_FILES_LIST += $(addprefix $(bsp_path)/, $(_src_files))
