#include<stdio.h>
#include<stdlib.h>

int header_allow(char *hdr,char **sigs);
char ** parseSignatures(char *f);
int req_allow(char *hdr,char **sigs);

int main(){
    /* test bed */
    char *sig_file_path="./docs/signatures.sig";
    char **sigs=parseSignatures(sig_file_path);
    /* example header string */
    /* header string should be of form */
    /* <header-field-name>###<header-field-value> */
    /* ### should be the delimiter */
    /* header field name must match exactly to sigs file entries */
    /* TEST CASE 1 */
    char *hdr="User-Agent### Mozilla/5.0 (X11; Linux x86_64; rv:12.0) Gecko/20100101 Firefox/21.0";
    if(header_allow(hdr,sigs)==0){ /* case insensitive */
	printf("header allowed\n");
    }
    else{
	printf("header not allowed\n");
    }

    /* TEST CASE 2 */
    char *hdr_2="Content-Type### application//x-www-form-urlencoded";
    if(header_allow(hdr_2,sigs)==0){ /* case insensitive */
	printf("header allowed\n");
    }
    else{
	printf("header not allowed\n");
    }

    /* TEST CASE 3 */
    char *hdr_3="User-Agent###<script>dsafgadg()</script>";
    if(header_allow(hdr_3,sigs)==0){ /* case insensitive */
	printf("header allowed\n");
    }
    else{
	printf("header not allowed\n");
    }

    /* TEST CASE 4 */
    char *hdr_4="User-Agent###bot";
    if(header_allow(hdr_4,sigs)==0){ /* case insensitive */
	printf("header allowed\n");
    }
    else{
	printf("header not allowed\n");
    }

    /* req body should not have any string at the end after the 
       last parameter's value. 
       Either put \0 right after last parameter's value in the string 
       before passing to req_allow or we can use a delimiter between 
       last param's value and HTTP/1.1. I would suggest first option(\0) 
       since int the URL specification a request string should not 
       have anything after the last kv pair, and all kv pairs are 
       seperated by &. so whitespace is counted as data */
    
    /* char *invalid_bod="GET /blah?foo=bar&k2=v2&k3=union HTTP/1.1"; */
    /* HTTP/1.1 is counted as part of k3's value */
    /* correct value */
    
    /* TEST CASE FOR PARAMETER <VALUE> AS WELL AS REQUEST METHOD */
    char *bod="POST /blah?foo=bar&k2=v2&k3=v3"; /* correct request url */
    if(req_allow(bod,sigs)==0){
	printf("req allowed\n");
    }
    else{
	printf("req not allowed\n");
    }
    /* TEST CASE FOR PARAMETER "*" */
    char *bod_allparams="GET /blah?foo=bar&k2=union all select&k3=v2"; /* correct request url */
    if(req_allow(bod_allparams,sigs)==0){
	printf("req allowed\n");
    }
    else{
	printf("req not allowed\n");
    }
    
    /* TEST CASE FOR MULTIPLE RUNS "*" */
    //char *bod_allparams="GET /blah?foo=bar&k2=union all select&k3=v2"; /* correct request url */
    /* int j,ct=0; */
    /* for(j=0;j<1000;j++){ */
    /* 	if(req_allow(bod_allparams,sigs)==0){ */
    /* 	    printf("req allowed\n"); */
    /* 	} */
    /* 	else{ */
    /* 	    printf("req not allowed\n"); */
    /* 	    ct++;	     */
    /* 	} */
    /* } */
    /* printf("count is %d\n",ct); */

    int i=0;
    for(;sigs[i];i++){
	free(sigs[i]);
    }
    free(sigs);
    return 0;
}
