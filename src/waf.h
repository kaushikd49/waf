#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<regex.h>


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
