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
    char *hdr="User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:12.0) Gecko/20100101 Firefox/21.0";
    if(header_allow(hdr,sigs)){
	printf("allowed\n");
    }
    else{
	printf("not allowed\n");
    }
    char *req="User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:12.0) Gecko/20100101 Firefox/21.0";
    if(header_allow(req,sigs)){
	printf("allowed\n");
    }
    else{
	printf("not allowed\n");
    }
    int i=0;
    for(;sigs[i];i++){
	free(sigs[i]);
    }
    free(sigs);
    return 0;
}
