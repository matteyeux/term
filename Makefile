CC = gcc
OS = $(shell uname)
TARGET = term

all : $(TARGET)

$(TARGET) :
	$(CC) term.c -o $(TARGET)
	@echo "Successfully built $(TARGET) for $(OS)"

install : $(TARGET)
	cp $(TARGET) /usr/local/bin/ 
clean : 
	rm $(TARGET)

