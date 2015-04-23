#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<regex.h>
#include<math.h>

#define BUFSIZE 1024
#define MAXLINES 150
#define BUFFER 150
#define PARAM 200
#define CONTAINS 500



/* REGEX */
int match_query(char *str,char *pat,int flagse);
int get_substr(char *str,char *pat,int n,char **mem,int flags);

int hello();
int header_allow(char *hdr,char **sigs);
char ** parseSignatubres(char *f);
int req_allow(char *hdr,char **sigs);

/* anomaly */
#define DELIMITER "@@@###"

struct url_params{
	int max_parameters;
	double max_val;
	double min_val;
	double avg;
	int count;
	int char_count;
	double sd;
	char input_type;
	// double number_of_paramters;
	char *name;
	float valid_input[100];
	char *valid_chars[100];
};
typedef struct url_params parameters;
struct page{
	parameters *param[2048];
	// char* client_ip;
	// char* local_ip;
	char* protocol[5];
	double time_served[2048];
	char name[1000];
	double keep_alive_requests[2048];
	char* status[4];
	double bytes_sent[2048];
	double bytes_received[2048];
	double bytes_transferred[2048];
	int used;
};

void populate_min_max_avg(char* ,parameters*);
void get_parameter_obj(char* ,int );

typedef struct page Page;

Page page_arr[1000];

typedef enum {max,avg,char_val,sd} mode;


void add_details_to_page(char* ,char* ,char* );
void initialize_pages();
void create_profile();
void store_data();
void test_page_content();
void get_lhs_and_rhs(char* ,char* ,char* );
float standard_deviation(float[],float,int);
int is_url_valid(char*,mode);
int get_page_index(char*);
int get_param_index(int,char*);

void * my_malloc(size_t sz);
