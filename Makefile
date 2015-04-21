FLAGS = -Wall -std=c99


mycloud: csapp util
	gcc $(FLAGS) -pthread csapp.o util.o mycloud.c -o mycloud

util:
	gcc $(FLAGS) util.c -c

csapp:
	gcc csapp.c -c

clean:
	rm *.o
	rm mycloud
