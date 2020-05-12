build: shamil.c
	$(CC) shamil.c -o shamil -Wall -Wextra -pedantic -lm

clean:
	rm -rf shamil
