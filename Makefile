include config.mk

.PHONY: all always clean build run run-hud

all: always build

always:
	@mkdir -p $(BUILD_DIR)

clean:
	@test -n "$(BUILD_DIR)" && test "$(BUILD_DIR)" != "/" && rm -rf $(BUILD_DIR)/*

build:
	@$(MAKE) $(BINARY) -j$(shell nproc) --output-sync=target --no-print-directory
	@echo "Done!"


# Compile

SOURCES := $(shell find $(SRC_DIR) -type f \( -name "*.c" \))
OBJECTS := $(patsubst %, $(BUILD_DIR)/obj/%.o, $(SOURCES))
-include $(OBJECTS:.o=.d)

$(BINARY): $(OBJECTS)
	@mkdir -p $(@D)
	@echo "Linking $(subst $(SOURCE_DIR)/,,$@)"
	@$(C) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/obj/%.o: %
	@mkdir -p $(@D)
	@echo "Compiling $(subst $(SOURCE_DIR)/,,$<)"
	@$(C) $(CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@


# Run

run:
	@$(BINARY)