CC = gcc
OS = $(shell uname)
TARGET = term

all : $(TARGET)

$(TARGET) : term.c
	$(CC) term.c -g -o $(TARGET)
	@echo "Successfully built $(TARGET) for $(OS)"

install : $(TARGET)
	install -v $(TARGET) /usr/local/bin/
clean : 
	rm $(TARGET)

