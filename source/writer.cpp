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
int last_user_id = 0;

int mpi_myrank;
int mpi_ranks;
MPI_Datatype mpi_acct_row;

struct acct_row {
	int user_id;
	char hashed_pwd[100];
	char plain_pwd[10];
};

/***************************************************************************/
/* Function Decs ***********************************************************/
/***************************************************************************/

void init_acct_row_type();
void print_acct_row(acct_row &hm);

vector<acct_row> gen_rows(vector<string> words, int num, int salt_len, int pivot);
void init_act_hashes(map<string, int> &act_hashes);

void mpi_write_db(const char *filename, acct_row *db_rows, int rows, int rank);
void mpi_read_db(const char *filename, acct_row *db_rows, int rows, int rank);

int clamp(int a, int lo, int hi);

/***************************************************************************/
/* Function: Main **********************************************************/
/***************************************************************************/

int main(int argc, char *argv[]) {
	// Example MPI startup and using CLCG4 RNG
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_ranks);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_myrank);
	init_acct_row_type();

	// Print rank and thread configuration
	if (mpi_myrank == 0) {
		printf("Ranks: %d\n", mpi_ranks);

		// Create dictionary
		map<string, int> act_hashes;
		init_act_hashes(act_hashes);

		vector<acct_row> db_rows = gen_rows(words, 500, 3, 1);
		acct_row rows[500];
		int i = 0;
		for (vector<acct_row>::iterator itr = db_rows.begin(); itr != db_rows.end(); itr++) {
			rows[i] = *itr;
			i++;
		}

		mpi_write_db("db.txt", rows, 500, 0);

		acct_row file_rows[500];
		print_acct_row(file_rows[0]);
		mpi_read_db("db.txt", file_rows, 500, 0);
		print_acct_row(file_rows[0]);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}


void mpi_write_db(const char *filename, acct_row *db_rows, int rows, int rank) {
    MPI_Status status;
    MPI_File fh;
    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDWR | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);

    MPI_File_write_at(fh, 0, db_rows, rows, mpi_acct_row, &status);

    MPI_File_close(&fh);
}

void mpi_read_db(const char *filename, acct_row *db_rows, int rows, int rank) {
	MPI_Status status;
    MPI_File fh;
    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDWR | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);

    MPI_File_read_at(fh, 0, db_rows, rows, mpi_acct_row, &status);

    MPI_File_close(&fh);
}

int clamp(int a, int lo, int hi) {
	if (a < lo) return lo;
	if (a > hi) return hi;
	return a;
}

void init_act_hashes(map<string, int> &act_hashes) {
	act_hashes.insert(pair<string, int>("77ecb9c86001b69287f0fc210869d4db3c013067230a87f6fbf96adf65cc4030", 1));
	act_hashes.insert(pair<string, int>("a1b48c37ac21232a6ef80baae25e080309c6b64330722bc06d61d9045045a37b", 2));
	act_hashes.insert(pair<string, int>("ef6d4e20906b5aa18241941b23ff3e95ba7dda12758667db93c3f4e3b6410e8c", 3));
	act_hashes.insert(pair<string, int>("e2e191cc90b5e4805e7137941a7b227779d3e11213b554af2d96055fe4b27f4d", 4));
}

void print_acct_row(acct_row &hm) {
	cout << "(";
	cout << hm.user_id << ", ";
	cout << hm.hashed_pwd << ", ";
	cout << hm.plain_pwd << ")" << endl << endl;
}

vector<acct_row> gen_rows(vector<string> words, int num, int salt_len, int pivot) {
	vector<vector<char>> salts = prod(alphanumeric, salt_len);
	vector<acct_row> rows;

	for (int n = 0; n < num; n++){
		// Generate random salt
		int s = rand() % salts.size();
		string salt = "";
		for (vector<char>::iterator itr = salts[s].begin(); itr != salts[s].end(); itr++) {
			salt += *itr;
		}

		// Pick random word
		int w = rand() % words.size();
		string word = words[w];

		// Pick random substring
		int len = clamp(rand() % 5 + 2, 2, word.size());
		int w_start = 0;
		if (word.size() - len > 0) {
			w_start = rand() % (word.size() - len);
		}

		// Salt and hash password
		string pwd = word.substr(w_start, len);
		string salted_pwd = applySalt(pwd, salt, salt_len, pivot);
		string hashed_pwd = applyHash(salted_pwd);

		// Populate new row
		acct_row row;
		row.user_id = last_user_id + 1;
		strcpy(row.hashed_pwd, hashed_pwd.c_str());
		strcpy(row.plain_pwd, pwd.c_str());
		rows.push_back(row);

		// Increment global userid
		last_user_id++;
		
	}

	return rows;
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
