
CC = gcc
CFLAGS = -Wall -lX11
TARGET = nobar
INSTALLDIR = /usr/local/bin

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

.PHONY: clean
clean:
	rm -f $(TARGET)

.PHONY: install
install: $(TARGET)
	sudo cp $(TARGET) $(INSTALLDIR)

.PHONY: uninstall
uninstall:
	sudo rm -f $(INSTALLDIR)/$(TARGET)
