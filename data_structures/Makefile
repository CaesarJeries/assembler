FLAGS=-Wall -ansi -pedantic -std=c99
GCC=gcc

hmap_test: hash_map_test.c hash_map.c hash_map.c
	$(GCC) $(FLAGS) $^ -o main

.PHONY: clean

clean:
	rm -f *.o *.so

