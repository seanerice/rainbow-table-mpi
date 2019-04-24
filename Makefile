all:
	/usr/local/mpich-3.2/bin/mpicc -I. -Wall -O3 source/crackpass.cpp -lstdc++ -o crackpass
	# gcc -I. -Wall -O3 -c clcg4.c -o clcg4.o
# b: clcg4.h clcg4.c assignment4-5.c
	# gcc -I. -Wall -O5 -c clcg4.c -o clcg4.o
	# module load xl && mpixlc -I. -O5 assignment4-5.c clcg4.o -o assignment4-5.xl -lpthread