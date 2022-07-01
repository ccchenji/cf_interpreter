CC = gcc
obj = main.o cf_interpreter.o
exe = main.exe
$(exe):$(obj)
	$(CC)  -o main $(obj) 
	make clean
main.o:main.c 
	$(CC) -c main.c
cf_interpreter.o: cf_interpreter.c cf_interpreter.h

.PHONY:clean 
clean:
	-del $(obj)

