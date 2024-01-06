.PHONY: all clean debug release examples test

EXE:=base64

SRCDIR:=src
RELDIR:=build/release
DBGDIR:=build/debug

SRCS:=$(wildcard $(SRCDIR)/*.c)
OBJS:=$(patsubst %.c,%.o,$(notdir $(SRCS)))


ifeq ($(MAKELEVEL),0)
	CFLAGS:=-std=c99 -Wall -Werror=vla
	CPPFLAGS:=
endif

release: BUILD_DIR=$(RELDIR)
release: CFLAGS+=-Werror -O3
release: CPPFLAGS+=-DNDEBUG

debug: BUILD_DIR=$(DBGDIR)
debug: CFLAGS+=-g

export

all: examples debug release

clean:
	@rm -rf build
	@$(MAKE) -C example_code clean

examples:
	@$(MAKE) -C example_code

debug release:
	echo $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(MAKE) $(BUILD_DIR)/$(EXE)

$(addprefix $(BUILD_DIR)/,$(OBJS)): $(BUILD_DIR)/%.o: $(SRCDIR)/%.c | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $^

$(BUILD_DIR)/$(EXE): $(addprefix $(BUILD_DIR)/,$(OBJS)) | $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

test: release
	for test in tests/*; do "$$test"; done
