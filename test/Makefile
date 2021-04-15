test_suite: test1 test2 test3 test4 test5
test1: gc.o hashmap.o hashset.o test1.c
	cc gc.o hashmap.o hashset.o test1.c -o test1
test2: gc.o hashmap.o hashset.o test2.c
	cc gc.o hashmap.o hashset.o test2.c -o test2
test3: gc.o hashmap.o hashset.o test3.c
	cc gc.o hashmap.o hashset.o test3.c -o test3
test4: gc.o hashmap.o hashset.o test4.c
	cc gc.o hashmap.o hashset.o test4.c -o test4
test5: gc.o hashmap.o hashset.o test5.c
	cc gc.o hashmap.o hashset.o test5.c -o test5
gc.o: ../src/gc.c
	cc -c ../src/gc.c
hashmap.o: ../src/hashmap.c
	cc -c ../src/hashmap.c
hashset.o: ../src/hashset.c
	cc -c ..src/hashset.c