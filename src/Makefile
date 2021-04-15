gc: hashmap.o hashset.o gc.o
	cc hashmap.o hashset.o gc.o -o gc
	rm -f *.o *~ 
hashmap.o: hashmap.c hashmap.h
	cc -c hashmap.c
hashset.o: hashset.c hashset.h hashmap.c hashmap.h
	cc -c hashset.c
gc.o: gc.c gc.h hashset.c hashset.h hashmap.c hashmap.h
	cc -c gc.c