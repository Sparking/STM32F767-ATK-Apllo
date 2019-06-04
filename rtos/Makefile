rtos_path = $(PROJECT_ROOT)/rtos/FreeRTOS/Source
_src_files := list.c queue.c tasks.c timers.c portable/GCC/ARM_CM7/r0p1/port.c portable/MemMang/heap_4.c
#_src_files += portable/Common/mpu_wrappers.c
_include_path := include portable/GCC/ARM_CM7/r0p1

CPPFLAGS += $(addprefix -I$(rtos_path)/, $(_include_path))
SOURCE_FILES_LIST += $(addprefix $(rtos_path)/, $(_src_files))

