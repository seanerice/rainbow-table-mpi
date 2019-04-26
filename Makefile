all:
	/usr/local/mpich-3.2/bin/mpicc -I. -Wall source/crackpass.cpp source/salty.cpp source/SubstringIterator.cpp source/pwdhashiter.cpp -lstdc++ -std=c++11 -o crackpass
	/usr/local/mpich-3.2/bin/mpicc -I. -Wall source/writer.cpp source/salty.cpp -lstdc++ -std=c++11 -o writer
	# gcc -I. -Wall -O3 -c clcg4.c -o clcg4.o
# b: clcg4.h clcg4.c assignment4-5.c
	# gcc -I. -Wall -O5 -c clcg4.c -o clcg4.o
	# module load xl && mpixlc -I. -O5 assignment4-5.c clcg4.o -o assignment4-5.xl -lpthread