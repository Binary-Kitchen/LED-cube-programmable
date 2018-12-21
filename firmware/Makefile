TARGET = led_cube

MCU ?= atmega328p
F_OSC ?= 16000000

BUILD_DIR = ./build
SRC_DIRS = \
	./src \
	./ledcube

SRCS := \
	$(shell find $(SRC_DIRS) -name *.cpp -or -name *.s) \

INC_DIRS := \
	./inc \
	./ledcube\
	$(shell find $(SRC_DIRS) -type d) \

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CXX = avr-g++
AS = avr-as
OBJCOPY = avr-objcopy
AVRDUDE = avrdude
CPPFLAGS :=  -MMD -MP -O3 -mmcu=$(MCU)
CPPFLAGS += -fdata-sections -ffunction-sections -fno-exceptions -flto
CPPFLAGS += -Wall -Wextra
CPPFLAGS += $(INC_FLAGS)
CPPFLAGS += -DF_OSC=$(F_OSC) -DMCU=$(MCU) -DF_CPU=$(F_OSC) 
LDFLAGS = -Wl,--gc-sections

SIZECMD = avr-size --format=berkly $(BUILD_DIR)/$(TARGET).elf
all: $(BUILD_DIR)/$(TARGET).elf
	$(SIZECMD)

$(BUILD_DIR)/$(TARGET).elf: $(OBJS)
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# cpp source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) -c $< -o $@

-include $(DEPS)
MKDIR_P ?= mkdir -p

.PHONY: clean flash

flash: $(BUILD_DIR)/$(TARGET).hex
	$(SIZECMD)
	$(AVRDUDE) -p $(MCU) -P /dev/ttyUSB0 -c arduino -b 57600 -F -u -U flash:w:$^ -U lfuse:w:0xe2:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m 

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $^ $@

clean: 
	rm -r $(BUILD_DIR)
