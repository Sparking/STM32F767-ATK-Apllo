# build time: 2019-06-03

BOARD := STM32F767-ATK-Apllo

# toolchains
CROSS_COMPILE = arm-none-eabi-
CC      := $(CROSS_COMPILE)gcc
AS      := $(CROSS_COMPILE)as
LD      := $(CROSS_COMPILE)ld
SIZE    := $(CROSS_COMPILE)size
OBJCOPY := $(CROSS_COMPILE)objcopy

CFLAGS  := -mthumb -mcpu=cortex-m7
CFLAGS  += -g -gdwarf-2
CFLAGS  += -mfpu=fpv5-d16 -mfloat-abi=hard
ASFLAGS := $(CFLAGS)

CFLAGS += -O0
CFLAGS += -u _printf_float
CFLAGS += -ffunction-sections -fdata-sections -ffp-contract=off
CFLAGS += -fno-strict-aliasing -fmessage-length=0
CFLAGS += --specs=nano.specs --specs=nosys.specs
CPPFLAGS := -Wall -Werror -std=gnu99

LDFLAGS := -Wl,-gc-section
LIBS :=

ifeq ("$(BOARD)", "STM32F767-ATK-Apllo")
CPPFLAGS += -DSTM32F7xx -DSTM32F767xx -D__FPU_USED=1U -DUNALIGNED_SUPPORT_DISABLE
CPPFLAGS += -DARM_MATH_CM7 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING -DARM_MATH_LOOPUNROLL
CPPFLAGS += -DUSE_HAL_DRIVER
endif


# all directories for project
PROJECT_ROOT        := .
SOURCE_FILES_LIST   :=
ASSEMBLY_FILES_LIST :=

MAP_FILE  := $(PROJECT_ROOT)/$(BOARD).map
LDFLAGS += -T$(PROJECT_ROOT)/scripts/$(BOARD).ld -Wl,-Map=$(MAP_FILE)

include $(PROJECT_ROOT)/board/Makefile
include $(PROJECT_ROOT)/rtos/Makefile
include $(PROJECT_ROOT)/src/Makefile
include $(PROJECT_ROOT)/lib/Makefile

c_objs := $(patsubst %.c, %.o, $(SOURCE_FILES_LIST))
s_objs := $(patsubst %.s, %.o, $(ASSEMBLY_FILES_LIST))
objs   := $(s_objs) $(c_objs)
depend_files := $(patsubst %.o, %.d, $(c_objs))
assembling_files := $(patsubst %.c, %.s, $(SOURCE_FILES_LIST))

output_bin := $(PROJECT_ROOT)/$(BOARD).bin
output_elf := $(PROJECT_ROOT)/$(BOARD).elf

define compile_c
@echo CC	$1
@$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -c -o $1 $2
endef

define compile_s
@echo AS	$1
@$(AS) $(ASFLAGS) -c -o $1 $2
endef

define link_all
@echo LD	$1
@$(CC) $(CFLAGS) $(LDFLAGS) -o $1 $2 $(LIBS)
endef

define objcopy_binary
@echo OBJCOPY	$1
@$(OBJCOPY) -O binary $2 $1
endef

.PHONY: all
all: $(output_bin)
	@$(SIZE) $(notdir $(output_elf))
$(output_elf): $(objs)
	$(call link_all, $@, $^)
$(output_bin): $(output_elf)
	$(call objcopy_binary, $@, $^)
-include $(depend_files)

$(c_objs): %.o: %.c
	$(call compile_c, $@, $<)
$(s_objs): %.o: %.s
	$(call compile_s, $@, $<)

.PHONY: assembly
assembly: $(assembling_files)
$(assembling_files): %.s: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -S -o $@ $<

.PHONY: clean
clean:
ifeq ($(OS), Windows_NT)
	@for %%I in ($(subst /,\,$(objs) $(output_bin) $(output_elf) $(output_hex) $(assembling_files) $(MAP_FILE) $(depend_files))) do if exist %%I del /f /q %%I
else
	@$(RM) $(objs)
	@$(RM) $(depend_files)
	@$(RM) $(MAP_FILE)
	@$(RM) $(output_hex)
	@$(RM) $(output_bin)
	@$(RM) $(output_elf)
	@$(RM) $(assembling_files)
endif

.PHONY: flash
flash: $(output_bin)
	st-flash write $< 0x08000000

.PHONY: debug
debug: $(output_bin)
	arm-none-eabi-gdb $(output_elf)
