#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<regex.h>
#define BUFSIZE 1024
#define PARAM 200
#define CONTAINS 500

int match_query(char *str,char *pat,int flagse);

int get_substr(char *str,char *pat,int n,char **mem,int flags);

int header_allow(char *hdr,char **sigs){
    /* returns 0 on not-allowed */
    /* returns 1 on allowed */
    
    /* assumes input header sting is of form */
    /* <header field>:<text> */
    /* Eg:   User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:12.0) Gecko/20100101 Firefox/21.0 */
    /* max size of input hdr string is 1024 bytes */

    int isMatch;
    int isFound=0;
    char hdr_field[PARAM],hdr_value[CONTAINS];
    /* clear contains and field */
    memset(hdr_field,0,PARAM);
    memset(hdr_value,0,CONTAINS);
    char *mem[2];
    mem[0]=hdr_field;
    mem[1]=hdr_value;
    /* split header string into field:value */
    isMatch=get_substr(hdr,"^(.+)###(.+)$",2,mem,REG_EXTENDED);
    if(!isMatch){
	fprintf(stderr,"input header string is not being split by the header split regex...fatal\n");
	return 0;		/* returning not found on error ??? */
    }
    
    /* now we have heder field in hdr_field and value in hdr_value */

    int i;
    char contains[CONTAINS],temp_hdr_field[PARAM];
    for(i=0;sigs[i];i++){

	/* TODO: how about splitting signature into heder and contains and then fo processing?? */
	
	memset(temp_hdr_field,0,PARAM);
	strncpy(temp_hdr_field,"^HEADER:",PARAM);
	strncat(temp_hdr_field,hdr_field,PARAM);
	strncat(temp_hdr_field,",",PARAM);
	
	/* check if hdr_field-> (^<field-name>:) is in signature string */
	int isMatch = match_query(sigs[i],temp_hdr_field,REG_ICASE);
	if(!isMatch){
	    /* sigs[i] does not have hdr_field */
	    continue;
	}
	/* header field matched */
	/* now get the "contains" value from the signatures */	
	/* clear contains */
	memset(contains,0,CONTAINS);
	char *mem[1];		/* hidden from outer mem */
	mem[0]=contains;
	
	isMatch=get_substr(sigs[i],"CONTAINS:\"(.+)\"",1,mem,REG_EXTENDED);
	if(!isMatch){
	    /* no match in get_substr */
	    /* some error in sigs. because this is should always match */
	    fprintf(stderr,"sigs did not match predef pattern\n");
	    continue;
	}
	/* now we have the value of CONTAINS field in contains array*/
	/* check if contains is present in header field's value */
	
	isMatch=match_query(hdr_value,contains,REG_ICASE);
	if(!isMatch){
	    /* "contains" not in header field's value*/
	    continue;
	}
	/* "contains" in header field's value */
	isFound=1;
	break;
    }
    return isFound;
    
}
