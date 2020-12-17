output: command.o	
	gcc command.o -o output

command.o: command.c
	gcc -c command.c

clean:
	rm *.o output

