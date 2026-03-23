CC      = gcc
CFLAGS  = -O2 -Wall -Wextra -Wpedantic
LDFLAGS = -lm
TARGET  = alu81a-rgb
PREFIX  ?= /usr/local
DATADIR ?= $(PREFIX)/share

# Shell completion dirs — derived from PREFIX, but each can be overridden
FISH_COMP_DIR ?= $(DATADIR)/fish/vendor_completions.d
ZSH_COMP_DIR  ?= $(DATADIR)/zsh/site-functions
# Ask bash-completion where it wants files; fall back to standard location
BASH_COMP_DIR ?= $(shell pkg-config --variable=completionsdir bash-completion 2>/dev/null \
                         || echo $(DATADIR)/bash-completion/completions)

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

install: $(TARGET)
	install -Dm755 $(TARGET)                     $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	install -Dm644 alu81a-rgb.rules              $(DESTDIR)/etc/udev/rules.d/99-alu81a-rgb.rules
	install -Dm644 completions/alu81a-rgb.fish   $(DESTDIR)$(FISH_COMP_DIR)/$(TARGET).fish
	install -Dm644 completions/alu81a-rgb.bash   $(DESTDIR)$(BASH_COMP_DIR)/$(TARGET)
	install -Dm644 completions/alu81a-rgb.zsh    $(DESTDIR)$(ZSH_COMP_DIR)/_$(TARGET)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	rm -f $(DESTDIR)/etc/udev/rules.d/99-alu81a-rgb.rules
	rm -f $(DESTDIR)$(FISH_COMP_DIR)/$(TARGET).fish
	rm -f $(DESTDIR)$(BASH_COMP_DIR)/$(TARGET)
	rm -f $(DESTDIR)$(ZSH_COMP_DIR)/_$(TARGET)

clean:
	rm -f $(TARGET)
