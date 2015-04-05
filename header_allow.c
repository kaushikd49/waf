#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<regex.h>
#define BUFSIZE 1024

int header_allow(char *hdr,char **sigs){
    /* returns 0 on not-allowed */
    /* returns 1 on allowed */
    /* assumes input header sting is of form */
    /* <header field>:<text> */
    /* Eg:   User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:12.0) Gecko/20100101 Firefox/21.0 */
    /* max size of input hdr string is 1024 bytes */
    char buf[BUFSIZE];
    strncpy(buf,hdr,BUFSIZE);
    char *token;
    token=strtok(buf,":");
    int i;
    char rx_string[BUFSIZE],msgbuf[100],inr_msgbuf[100];
    regex_t *regex,*inr_regex,*fnl_regex;
    regex=(regex_t *)malloc(sizeof(regex_t));
    inr_regex=(regex_t *)malloc(sizeof(regex_t));
    fnl_regex=(regex_t *)malloc(sizeof(regex_t));
    strncpy(rx_string,"",BUFSIZE);
    strncat(rx_string,token,BUFSIZE);
    printf("regex is %s,len is %d\n",rx_string,(int)strlen(rx_string));
    int reti; /* = regcomp(regex,rx_string,REG_ICASE); */
    int inr_reti,fnl_reti;
    int isfound=0;
    /* if(reti){			/\* retrns non zero for failure *\/ */
    /* 	fprintf(stderr,"could not compile regex\n"); */
    /* 	regfree(regex); */
    /* 	return 0; */
    /* } */
    for(i=0;sigs[i] && !isfound;i++,regfree(inr_regex),regfree(fnl_regex),regfree(regex)){

	/* Function: void regfree (regex_t *compiled)

    Preliminary: | MT-Safe | AS-Unsafe heap | AC-Unsafe mem | See POSIX Safety Concepts.

    Calling regfree frees all the storage that *compiled points to. This includes various internal fields of the regex_t structure that aren’t documented in this manual.

    regfree does not free the object *compiled itself. 

You should always free the space in a regex_t structure with regfree before using the structure to compile another regular expression.

When regcomp or regexec reports an error, you can use the function regerror to turn it into an error message string.  */


	/* regcomp structure->do regexec->regfree->regcomp structure */

    	//strncpy(buf,sig[i],strlen(sig[i])+1); /* clear buf */
	reti = regcomp(regex,rx_string,REG_ICASE);
	if(reti){			/* retrns non zero for failure */
	    fprintf(stderr,"could not compile regex\n");
	    regfree(regex);
	    return 0;
	}
    	reti = regexec(regex,sigs[i],0,NULL,0);
    	if(!reti){
    	    printf("sigs match\n");
	    //regfree(&regex);
    	    /* header field matched */
	    /* now check if the header contains the "contains" value from the signatures */
	    inr_reti = regcomp(inr_regex,"\"(.+)\"",REG_ICASE|REG_EXTENDED);
	    if(inr_reti){			/* retrns non zero for failure */
		fprintf(stderr,"could not compile inner regex\n");
		continue;
	    }
	    size_t ngroups=2;
	    regmatch_t *groups=malloc(ngroups*sizeof(regmatch_t));
	    if(!(inr_reti=regexec(inr_regex,sigs[i],ngroups,groups,0))){
	    
		
		/* no need of buf and token any more. since we have already constructed the regexp and we can use that in every iteration */
		/* clear buf */
		strncpy(buf,"",BUFSIZE);
		/* rm_eo points to the byte after the end match. eo-so gives the size of group */
		/* copy the group contents into the buf */
		strncpy(buf,sigs[i]+groups[1].rm_so,groups[1].rm_eo-groups[1].rm_so);
		/* since eo-so gives length of group. using this put \0 at the end of buf */
		buf[groups[1].rm_eo-groups[1].rm_so]='\0';
		/* matches complete regexp */
		/* printf("start %c -> end %c\n",sigs[i][groups[0].rm_so],sigs[i][groups[0].rm_eo-1]); */
		/* matches the first group of the regexp */
		printf("start %c -> end %c -> %s\n",sigs[i][groups[1].rm_so],sigs[i][groups[1].rm_eo-1],buf);
		/* now check if group present in header */
		/* construct pattern */
		fnl_reti = regcomp(fnl_regex,buf,REG_ICASE|REG_EXTENDED);
		if(fnl_reti){
		    fprintf(stderr,"could not compile final regex\n");
		    continue;
		}
		if(!(fnl_reti=regexec(fnl_regex,hdr,0,NULL,0))){
		    /* signature fragment is contained in hdr string */
		    isfound=1;
		}
		else if(fnl_reti==REG_NOMATCH){
		    
		    printf("No final match\n");
		}
		else{
		    regerror(fnl_reti,fnl_regex,msgbuf,sizeof(msgbuf));
		    fprintf(stderr,"final regex match failed: %s\n",msgbuf);
		}

	    }
	    else if(inr_reti == REG_NOMATCH){
		printf("No inner match\n");
	    }
	    else{
		regerror(inr_reti,inr_regex,inr_msgbuf,sizeof(inr_msgbuf));
		fprintf(stderr,"inner regex match failed: %s\n",inr_msgbuf);
	    }
	    free(groups);
    	}
    	else if(reti == REG_NOMATCH){
    	    printf("No match\n");
    	}
    	else{
    	    regerror(reti,regex,msgbuf,sizeof(msgbuf));
    	    fprintf(stderr,"regex match failed: %s\n",msgbuf);
    	}
    }
    /* regfree(inr_regex); */
    /* regfree(fnl_regex); */
    /* regfree(regex); */
    free(regex);
    free(inr_regex);
    free(fnl_regex);
    return isfound;
    
}
