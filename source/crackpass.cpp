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
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>

// C++ Std Libs
#include <map> 
#include <string>
#include <iostream> 
#include <iterator> 
#include <vector>
#include "salty.h"
#include "SubstringIterator.h"


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

// int total_rows = 65536;
int total_rows = 512;

vector<char> alphanumeric = {
	'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 
	'm', 'n', 'o', 'p', 'q', 'r',
	's', 't', 'u', 'v', 'w', 'x', 
	'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9'
};

vector<string> words = {
	"addition", "reflective", "torpid", "dust", "superb",
	"substance", "need", "share", "dear", "opposite",
	"credit", "vessel", "fuzzy", "puzzling", "race",
	"plastic", "summer", "war", "unpack", "level",
	"collar", "point", "volcano", "nonstop", "coal",
	"name", "request", "greasy", "adventurous", "nonchalant",
	"noise", "switch", "yell", "income", "songs",
	"beg", "cannon", "bathe", "ratty", "nimble",
	"morning", "scrape", "offer", "steep", "jar",
	"travel", "signal", "number", "tap", "scared"
};



double g_time_in_secs = 0;
double g_processor_frequency = 1600000000.0; // processing speed for BG/Q
unsigned long long g_start_cycles = 0;
unsigned long long g_end_cycles = 0;

int mpi_myrank;
int mpi_ranks;
MPI_Datatype mpi_hash_match;
MPI_Datatype mpi_acct_row;

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

	hash_match() : send_rank(-1), matched(0), userid(-1) {}
};

struct acct_row {
	int user_id;
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

void mpi_read_db(const char *filename, acct_row *db_rows, int rows, int rank);
void init_acct_row_type();
void print_acct_row(acct_row &hm);

void init_act_hashes(map<string, int> &act_hashes) {
	act_hashes.insert(pair<string, int>("77ecb9c86001b69287f0fc210869d4db3c013067230a87f6fbf96adf65cc4030", 1));
	act_hashes.insert(pair<string, int>("a1b48c37ac21232a6ef80baae25e080309c6b64330722bc06d61d9045045a37b", 2));
	act_hashes.insert(pair<string, int>("ef6d4e20906b5aa18241941b23ff3e95ba7dda12758667db93c3f4e3b6410e8c", 3));
	act_hashes.insert(pair<string, int>("e2e191cc90b5e4805e7137941a7b227779d3e11213b554af2d96055fe4b27f4d", 4));
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
	printf("(Rank: %d, Matched: %d, uid: %d, hashed: %s, plain: %s)\n", hm.send_rank, hm.matched, hm.userid, hm.hashed_pwd, hm.plain_pwd);
}

void send_hash_matches(hash_match *hm, int num, int r_rank);
void recv_hash_matches(hash_match *hm, int num, int s_rank);
void check_hash_match();

vector<int> pivots {0, 1, 2, 3}; // to be replaced by args
vector<int> saltLengths {0, 1, 2, 3}; // to be replaced by args

/***************************************************************************/
/* Function: Main **********************************************************/
/***************************************************************************/

int main(int argc, char *argv[]) {
	// Example MPI startup and using CLCG4 RNG
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_ranks);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_myrank);
	init_hash_match_type();
	init_acct_row_type();

	// Parse command-line args
	// struct prog_params params = parse_args(argc, argv);

	// Print rank configuration
	if (mpi_myrank == 0) {
		printf("Ranks: %d\n", mpi_ranks);
		// print_params(params);
	}

	// Read portion of database table into memory
	int rows_per_rank = total_rows / mpi_ranks;
	acct_row file_rows[rows_per_rank];
	mpi_read_db("db.txt", file_rows, rows_per_rank, mpi_myrank);
	print_acct_row(file_rows[0]); // Sanity check

	// Create lookup table mapping hash -> userid
	map<string, int> act_hashes;
	init_act_hashes(act_hashes);

	hash_match rank_match_buf_out[mpi_ranks];
	hash_match rank_match_buf_in[mpi_ranks];

	SubstringIterator si("hashes");
	for (int t = 0; t < 10; t++) {


		// Check for hash-match all in buff_in

		// Generate new hash
		string hashed_pwd = si.nextSubstring();

		// Copy buff_in to buff_out
		for (int i = 0; i < mpi_ranks; i++) {
			rank_match_buf_out[i] = rank_match_buf_in[i];
		}

		// Make new hash match in buff_out for this rank
		hash_match hm;
		strcpy(hm.hashed_pwd, hashed_pwd.c_str());
		rank_match_buf_out[mpi_myrank] = hm;

		if (mpi_myrank == 0) 
			printf("Rank: %d, row: %d, hash: %s\n", mpi_myrank, rank_match_buf_out[0].userid, rank_match_buf_out[0].hashed_pwd);

		// Sync buffer
		// printf("Rank %d send to rank %d\n", mpi_myrank, mod(mpi_myrank + 1, mpi_ranks));
		send_hash_matches(rank_match_buf_out, mpi_ranks, mod(mpi_myrank + 1, mpi_ranks));
		recv_hash_matches(rank_match_buf_in, mpi_ranks, mod(mpi_myrank - 1, mpi_ranks));
		MPI_Barrier(MPI_COMM_WORLD);

		if (mpi_myrank == 1)
			printf("Rank: %d, row: %d, hash: %s\n", mpi_myrank, rank_match_buf_in[0].userid, rank_match_buf_in[0].hashed_pwd);

	}
	
    /* Primary execution:
	*   for word in words:
	*		for substrings of word
	*       	for salt-len in salt-lens
	*           	for salt in salts:
	*					for pivot in pivots
	*						make password
	*						salt password
	*						hash password
	*               		check hash match
	*/

    // SubstringIterator si;
	// // words
    // for (vector<string>::iterator itr = words.begin(); itr != words.end(); itr++) {
    //     si.initSubstringIterator(*itr, 2, 7);
	// 	// substrings
    //     while (si.hasNext()) {
    //         string pt_password = si.nextSubstring();
    //         // Salt len
    //         for(vector<int>::iterator salt = saltLengths.begin(); salt != saltLengths.end(); salt++) {
    //             vector<string> saltVec = generateSalts(alphanumeric, *salt);
    //             // salts
    //             for(vector<string>::iterator currSalt = saltVec.begin(); currSalt != saltVec.end(); currSalt++) {
    //                 // pivots
    //                 for(vector<int>::iterator piv = pivots.begin(); piv != pivots.end(); piv++) {
    //                     if((unsigned)*piv <= currSalt->length()) {
    //                         string salted = applySalt(pt_password, *currSalt, *salt, *piv);
    //                         string hashed = applyHash(salted); 


    //                         // Check hash match against local db
	// 						if (act_hashes.count(hashed) > 0) {
    //                         	cout << hashed << " " << pt_password << " " << salted << " " << *currSalt << " userid: " << act_hashes[hashed] << '\n';
	// 						}

	// 						// Send hash match against other db

	// 						// Recv hash match against local db
	// 						// then fwd
    //                     }
    //                 }
    //             }
    //         }
    //     }   
    // }

	// if (mpi_myrank == 0) {
	// 	hash_match hm;
	// 	hm.send_rank = 0;
	// 	hm.matched = 0;
	// 	strcpy(hm.hashed_pwd, "abcd");
	// 	strcpy(hm.plain_pwd, "hey");
	// 	hm.userid = -1;
	// 	send_msg(hm, 1);
	// }
	// if (mpi_myrank == 1) {
	// 	hash_match hm;
	// 	recv_msg(hm, 0);
	// 	cout << "Rank " << mpi_myrank << " recieved!" << endl;
	// 	print_hash_match(hm);
	// }

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

void send_hash_matches(hash_match *hm, int num, int r_rank) {
    MPI_Request req1;
    MPI_Isend(hm, num, mpi_hash_match, r_rank, 0, MPI_COMM_WORLD, &req1);
}

void recv_hash_matches(hash_match *hm, int num, int s_rank) {
	MPI_Request req1;
	MPI_Status status1;
	MPI_Irecv(hm, num, mpi_hash_match, s_rank, 0, MPI_COMM_WORLD, &req1);
	MPI_Wait(&req1, &status1);
}

void mpi_read_db(const char *filename, acct_row *db_rows, int rows, int rank) {
	MPI_Status status;
    MPI_File fh;
    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDWR | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);
	int offset = rank * rows * 114;
    MPI_File_read_at(fh, offset, db_rows, rows, mpi_acct_row, &status);

    MPI_File_close(&fh);
}

void init_acct_row_type() {
	int nitems = 3;
	int blocklengths[3] = {1, 100, 10};
	MPI_Datatype types[3] = {MPI_INT, MPI_CHAR, MPI_CHAR};
	MPI_Aint offsets[3];
	offsets[0] = offsetof(acct_row, user_id);
	offsets[1] = offsetof(acct_row, hashed_pwd);
	offsets[2] = offsetof(acct_row, plain_pwd);
	MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_acct_row);
	MPI_Type_commit(&mpi_acct_row);
}

void print_acct_row(acct_row &hm) {
	printf("(%d, %s, %s)\n", hm.user_id, hm.hashed_pwd, hm.plain_pwd);
}

// void mpi_write_db(char *filename, int **uni, int rows, int cols, int rank) {
//     MPI_Status status;
//     MPI_File fh;
//     MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDWR | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);

//     for (int i = 0; i < rows; i++) {
//         int global_row_ind = i + (rank * (rows));
//         int offset = (global_row_ind * cols * 4);
//         // printf("Rank %d writing row %d with offset %d.\n", rank, i, offset);
//         MPI_File_write_at(fh, offset, uni[i], cols, MPI_INT, &status);
//     }

//     MPI_File_close(&fh);
// }

// void mpi_read_db(char *filename, int **uni, int rows int rank) {
//     MPI_Status status;
//     MPI_File fh;
//     MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDWR | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);

//     for (int i = 0; i < rows; i++) {
//         int global_row_ind = i + (rank * (rows));
//         int offset = (global_row_ind * cols * 4);
//         // printf("Rank %d writing row %d with offset %d.\n", rank, i, offset);
//         MPI_File_write_at(fh, offset, uni[i], cols, MPI_INT, &status);
//     }

//     MPI_File_close(&fh);
// }