objects=init.o injector.o elf.o
flags=-g
init:$(objects)
	arm-none-linux-gnueabi-gcc -static -o  init  $(flags) $(objects)

init.o:init.c global.h injector.h elf.h
	arm-none-linux-gnueabi-gcc -c $(flags) init.c -o init.o
injector.o:injector.c injector.h global.h
	arm-none-linux-gnueabi-gcc -c $(flags) injector.c -o injector.o
elf.o:elf.c elf.h injector.h global.h
	arm-none-linux-gnueabi-gcc -c $(flags) elf.c -o elf.o
clean:
	rm -rf init $(objects)
