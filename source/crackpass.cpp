/***************************************************************************/
/* Password Cracker MPI ****************************************************/
/* Judy Fan             ****************************************************/
/* Stanislav Ondrus     ****************************************************/
/* Sean Rice            ****************************************************/

// Compile with:
// BGQ: make b
// Mastiff: make all
/***************************************************************************/

/***************************************************************************/
/* Includes ****************************************************************/
/***************************************************************************/

// C Std Libs
#include<stdio.h>
#include<stdlib.h>
#include <stddef.h>
#include<string.h>
#include<errno.h>
#include<math.h>
#include<unistd.h>

// C++ Std Libs
#include <map> 
#include <string>
#include <iostream> 
#include <iterator> 


#include<mpi.h>

using namespace std;

// #define BGQ 1 // when running BG/Q, comment out when testing on mastiff

#ifdef BGQ
#include<hwi/include/bqc/A2_inlines.h>
#else
#define GetTimeBase MPI_Wtime
#endif


/***************************************************************************/
/* Global Vars *************************************************************/
/***************************************************************************/

double g_time_in_secs = 0;
double g_processor_frequency = 1600000000.0; // processing speed for BG/Q
unsigned long long g_start_cycles = 0;
unsigned long long g_end_cycles = 0;

int mpi_myrank;
int mpi_ranks;
MPI_Datatype mpi_hash_match;

struct prog_params {
	int uni_rows;
	int uni_cols;
	int heatmap_rows;
	int heatmap_cols;
	int ticks;
	int threads;
	float threshold;
	bool write_universe;
	bool write_heatmap;
};

struct hash_match {
	int send_rank;
	int matched;
	int userid;
	char hashed_pwd[100];
	char plain_pwd[10];
};

/***************************************************************************/
/* Function Decs ***********************************************************/
/***************************************************************************/

// Parameter parsing
void print_params(struct prog_params params);
struct prog_params parse_args(int argc, char **argv);

// Alloc/free 2d array funcs
bool **alloc_bool_arr_2d(int rows, int cols);
int **alloc_int_arr_2d(int rows, int cols);
void free_bool_arr_2d(bool **uni, int rows);
void free_int_arr_2d(int **uni, int rows);

// Helper funcs
int mod(int a, int b);
void strreverse(char *begin, char *end);
void itoa(int value, char *str, int base);

void init_act_hashes(map<string, int> &act_hashes) {
	act_hashes.insert(pair<string, int>("abcd", 1));
	act_hashes.insert(pair<string, int>("efgh", 2));
	act_hashes.insert(pair<string, int>("ijkl", 3));
	act_hashes.insert(pair<string, int>("mnop", 4));
}

void init_hash_match_type() {
	int nitems = 5;
	int blocklengths[5] = {1, 1, 1, 100, 10};
	MPI_Datatype types[5] = {MPI_INT, MPI_INT, MPI_INT, MPI_CHAR, MPI_CHAR};
	MPI_Aint offsets[5];
	offsets[0] = offsetof(hash_match, send_rank);
	offsets[1] = offsetof(hash_match, matched);
	offsets[2] = offsetof(hash_match, userid);
	offsets[3] = offsetof(hash_match, hashed_pwd);
	offsets[4] = offsetof(hash_match, plain_pwd);
	MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_hash_match);
	MPI_Type_commit(&mpi_hash_match);
}

void print_hash_match(hash_match &hm) {
	cout << "Hash Match:" << endl;
	cout << hm.send_rank << endl;
	cout << hm.matched << endl;
	cout << hm.userid << endl;
	cout << hm.hashed_pwd << endl;
	cout << hm.plain_pwd << endl << endl;
}

void send_msg(hash_match &hm, int r_rank);
void recv_msg(hash_match &hm, int s_rank);
void check_hash_match();

/***************************************************************************/
/* Function: Main **********************************************************/
/***************************************************************************/

int main(int argc, char *argv[]) {
	// Example MPI startup and using CLCG4 RNG
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_ranks);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_myrank);
	init_hash_match_type();

	// Parse command-line args
	struct prog_params params = parse_args(argc, argv);

	// Print rank and thread configuration
	if (mpi_myrank == 0) {
		printf("Ranks: %d\n", mpi_ranks);
		print_params(params);
	}

	// Create dictionary
	map<string, int> act_hashes;
	init_act_hashes(act_hashes);
	
	// Print dictionary
	map<string, int>::iterator itr;
	for (itr = act_hashes.begin(); itr != act_hashes.end(); ++itr) {
		cout << itr->first << " " << itr->second << endl;
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if (mpi_myrank == 0) {
		hash_match hm;
		hm.send_rank = 0;
		hm.matched = 0;
		strcpy(hm.hashed_pwd, "abcd");
		strcpy(hm.plain_pwd, "hey");
		hm.userid = -1;
		send_msg(hm, 1);
	}
	if (mpi_myrank == 1) {
		hash_match hm;
		recv_msg(hm, 0);
		cout << "Rank " << mpi_myrank << " recieved!" << endl;
		print_hash_match(hm);
	}

	/* Primary execution:
	*   for words:
	*       for plaintext-pwds:
	*           for salted-pwds:
	*               check hash match
	*/

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}

/***************************************************************************/
/* Argument Parsing ********************************************************/
/***************************************************************************/

struct prog_params parse_args(int argc, char **argv) {
	struct prog_params params;
	params.threads = 1;
	params.uni_rows = 32768;
	params.uni_cols = 32768;
	params.heatmap_cols = 1024;
	params.heatmap_rows = 1024;
	params.ticks = 256;
	params.threshold = 0.50f;
	params.write_heatmap = false;
	params.write_universe = false;

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			if (strcmp("--threads", argv[i]) == 0) {
				params.threads = atoi(argv[i + 1]);
			}
			if (strcmp("--ticks", argv[i]) == 0) {
				params.ticks = atoi(argv[i + 1]);
			}
			if (strcmp("--uni-size", argv[i]) == 0) {
				params.uni_rows = atoi(argv[i + 1]);
				params.uni_cols = atoi(argv[i + 2]);
			}
			if (strcmp("--heatmap-size", argv[i]) == 0) {
				params.heatmap_rows = atoi(argv[i + 1]);
				params.heatmap_cols = atoi(argv[i + 2]);
			}
			if (strcmp("--write-uni", argv[i]) == 0) {
				params.write_universe = true;
			}
			if (strcmp("--write-heatmap", argv[i]) == 0) {
				params.write_heatmap = true;
			}
			if (strcmp("--threshold", argv[i]) == 0) {
				params.threshold = atof(argv[i + 1]);
			}
		}
	}

	return params;
}

void print_params(struct prog_params params) {
	printf("Threads per Rank: %d\n", params.threads);
	printf("Universe Size: %d by %d\n", params.uni_rows, params.uni_cols);
	printf("Heatmap Size: %d by %d\n", params.heatmap_rows, params.heatmap_cols);
	printf("Ticks: %d\n", params.ticks);
	printf("Threshold: %f\n", params.threshold);
}


/***************************************************************************/
/* Alloc/Free 2D Array Funcs ***********************************************/
/***************************************************************************/

int **alloc_int_arr_2d(int rows, int cols) {
	int **uni = (int **) calloc(rows, sizeof(int *));
	for (int i = 0; i < rows; i++) {
		uni[i] = (int *) calloc(cols, sizeof(int));
	}
	return uni;
}

void free_int_arr_2d(int **uni, int rows) {
	for (int i = 0; i < rows; i++) {
		free(uni[i]);
	}
	free(uni);
}

bool **alloc_bool_arr_2d(int rows, int cols) {
	bool **uni = (bool **) calloc(rows, sizeof(bool *));
	for (int i = 0; i < rows; i++) {
		uni[i] = (bool *) calloc(cols, sizeof(bool));
	}
	return uni;
}

void free_bool_arr_2d(bool **uni, int rows) {
	for (int i = 0; i < rows; i++) {
		free(uni[i]);
	}
	free(uni);
}


/***************************************************************************/
/* Helper Functions ********************************************************/
/***************************************************************************/

int mod(int a, int b) {
	// We can get the correct modulus for negative values using this function
	int r = a % b;
	return r < 0 ? r + b : r;
}

void strreverse(char *begin, char *end) {
	char aux;
	while (end > begin) {
		aux = *end, *end-- = *begin, *begin++ = aux;
	}
}

void itoa(int value, char *str, int base) {
	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char *wstr = str;
	int sign;

	// Validate base
	if (base < 2 || base > 35) {
		*wstr = '\0';
		return;
	}

	// Take care of sign
	if ((sign = value) < 0) value = -value;

	// Conversion. Number is reversed.
	do *wstr++ = num[value % base]; while (value /= base);
	if (sign < 0) *wstr++ = '-';
	*wstr = '\0';

	// Reverse string
	strreverse(str, wstr - 1);
}

void send_msg(hash_match &hm, int r_rank) {
    MPI_Request req1;
    MPI_Isend(&hm, 1, mpi_hash_match, 1, 0, MPI_COMM_WORLD, &req1);
}

void recv_msg(hash_match &hm, int s_rank) {
	MPI_Request req1;
	MPI_Status status1;
	MPI_Irecv(&hm, 1, mpi_hash_match, 0, 0, MPI_COMM_WORLD, &req1);
	MPI_Wait(&req1, &status1);
}