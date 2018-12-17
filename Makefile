CC = gcc
OS = $(shell uname)
CFLAGS  = -I. -Wall -Wextra -Werror -g

TARGET = term

all : $(TARGET)

$(TARGET) : main.c term.c
	$(CC) main.c term.c $(CFLAGS) -o $(TARGET)
	@echo "Successfully built $(TARGET) for $(OS)"

install : $(TARGET)
	install -v $(TARGET) /usr/local/bin/

clean :
	rm $(TARGET)

