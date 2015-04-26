#ifndef WAF_H
#define WAF_H
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
#define NUMBER 1
#define ALPHABET 2
#define SPL_SYMBOL 4

//New Code 



void store_data();

typedef enum {max,avg,charset,sd} mode;

//int getCharset(char);
int getAllCharset(char*, int*);
void * my_malloc(size_t);

#define DELIMITER "@@@###"

struct param{
	int total;
	int count;
	float avg;
	float sd;
	int csets[3];
	int sd_array[100];	
	char paramName[100];
};

typedef struct param param;

struct page{
	char pageName[100];
	int maxParams;
	param url_params[100];
	int param_arr_index[100];
};

typedef struct page page;

void construct_params(int,char*);
int is_url_valid(char*,mode);

page page_arr[100];

int page_arr_index[100];
int param_arr_index[100];

void add_details_to_page(char* ,char* );
void print_profile();
float standard_deviation(int* ,float);


//New Code ends



/* REGEX */
int match_query(char *str,char *pat,int flagse);
int get_substr(char *str,char *pat,int n,char **mem,int flags);

int hello();
int header_allow(char *hdr,char **sigs);
char ** parseSignatubres(char *f);
int req_allow(char *hdr,char **sigs);

/* anomaly */
#endif
