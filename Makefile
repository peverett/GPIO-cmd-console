.SECONDARY: %.o
.PHONY: all clean

ifeq ($(OS),Windows_NT)
    CC=GCC
endif

all: parser_example

parser_example: parser_example.c simple_command_parser.c simple_command_parser.h

clean:
	-rm *.o
	-rm parser_example.exe
