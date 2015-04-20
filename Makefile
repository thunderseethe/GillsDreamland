
mycloud: csapp
	gcc -pthread csapp.o mycloud.c -o mycloud -std=c99

csapp:
	gcc -Wall csapp.c -c

clean:
	rm *.o
	rm mycloud
