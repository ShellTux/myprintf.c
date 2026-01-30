TARGETS = \
					$(TARGET_DIR)/my-printf \
					$(TARGET_DIR)/custom-printf

DEPS := src/my-printf.c
$(TARGET_DIR)/my-printf: $(DEPS:%.c=$(BUILD_DIR)/%.o)
	@mkdir --parents $(shell dirname $@)
	$(CC) -o $@ $^ $(LDFLAGS)

DEPS := src/custom-printf.c
$(TARGET_DIR)/custom-printf: $(DEPS:%.c=$(BUILD_DIR)/%.o)
	@mkdir --parents $(shell dirname $@)
	$(CC) -o $@ $^ $(LDFLAGS)
