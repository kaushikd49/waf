#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<regex.h>
#define BUFSIZE 1024
#define PARAM 100
#define CONTAINS 500

int match_query(char *str,char *pat,int flags){
    /* return 0 if not found */
    /* return 1 if found/match */
    regex_t regex;
    int isFound=0;
    int reti=regcomp(&regex,pat,flags);
    char msgbuf[BUFSIZE];
    if(reti){
	fprintf(stderr,"could not compile regex\n");
	regfree(&regex);
	return isFound;
    }
    reti = regexec(&regex,str,0,NULL,0);
    if(!reti){
	isFound=1;
    }
    else if(reti==REG_NOMATCH){
	/* printf("No match\n") */;
    }
    else{
	regerror(reti,&regex,msgbuf,sizeof(msgbuf));
	fprintf(stderr,"final regex match failed: %s\n",msgbuf);
    }
    regfree(&regex);
    return isFound;
}

int get_substr(char *str,char *pat,int n,char **mem,int flags){
    size_t ngroups=n+1;
    regmatch_t *groups=(regmatch_t *)malloc(ngroups*sizeof(regmatch_t));
    memset(groups,0,ngroups*sizeof(regmatch_t));
    regex_t regex;
    int reti=regcomp(&regex,pat,flags);
    char msgbuf[BUFSIZE];
    int isFound=0;
    if(reti){
	fprintf(stderr,"could not compile substr regex\n");
	regfree(&regex);
	free(groups);
	return isFound;
    }
    reti = regexec(&regex,str,ngroups,groups,0);
    size_t i=0;
    if(!reti){
	for(i=0;i<ngroups-1;i++){
	    /* already cleared mem entries */

	    /* how do i detect, if there was no match and later set isFound=0*/
	    if(groups[i+1].rm_so!=-1){
		strncpy(mem[i],str+groups[i+1].rm_so,groups[i+1].rm_eo-groups[i+1].rm_so);
		mem[i][groups[i+1].rm_eo-groups[i+1].rm_so]='\0';
		isFound=1;
	    }
	}
    }
    else if(reti==REG_NOMATCH){
	/* printf("No match\n") */;
    }
    else{
	regerror(reti,&regex,msgbuf,sizeof(msgbuf));
	fprintf(stderr,"get_substr regex match failed: %s\n",msgbuf);
    }
    regfree(&regex);
    free(groups);
    return isFound;
}


int req_allow(char *hdr,char **sigs){
    /* returns 0 on not-allowed */
    /* returns 1 on allowed */

    char *pat_get="REQUEST_METHOD:GET";
    char *pat_post="REQUEST_METHOD:POST";
    /* check if get req */
    //char *pat_get="^GET";
    int isGetReq=match_query(hdr,"^GET",REG_EXTENDED);

    /* check if it post req */
    //char *pat_post="^POST";
    int isPostReq=match_query(hdr,"^POST",REG_EXTENDED);

    /* if not either post or get return allowed */
    if(!isGetReq && !isPostReq)
	return 1;		/* allowed */

    /* it is either a get req or post */
    /* now we must check for conditions in sigs */
    int i=0,isMatch,isFound=0;
    /* char *pat_get="REQUEST_METHOD:GET"; */
    /* char *pat_post="REQUEST_METHOD:POST"; */
    char *pat;
    char *pat_substr="PARAMETER:(.+),CONTAINS:\"(.+)\"$";
    char temp[BUFSIZE];
    char *pat_substr_val;
    /* if get req */
    if(isGetReq)
	pat=pat_get;
    else
	pat=pat_post;
    for(i=0;sigs[i];i++){
	/* for every sig which has REQUEST_METHOD: GET/POST*/
	isMatch=match_query(sigs[i],pat,REG_EXTENDED);
	if(!isMatch){
	    /* sig doesnt have pat */
	    continue;
	}
	else{
	    /* sig has pat */
	    /* sig is get req filter pattern */
	    /* get value of PARAMTER and value of CONTAINS */
	    char param[PARAM],contains[CONTAINS];
	    /* clear contents */
	    memset(param,0,PARAM);
	    memset(contains,0,CONTAINS);
	    char *mem[2];
	    mem[0]=param;mem[1]=contains;
	    /* 1 is expected num of substrings */
	    /* mem is space for subtring for func to write into */
	    int ret=get_substr(sigs[i],pat_substr,2,mem,REG_EXTENDED);
	    /* now we have params and contains values */
	    if(!ret){
		/* no match in get_substr */
		/* some error in sigs. because this is should always match */
		fprintf(stderr,"sigs did not match predef pattern\n");
		continue;
	    }
	    if(strcmp(param,"*")==0){
		/* if param is \* then do a whole string search for contains */
		int isMatch=match_query(hdr,contains,REG_EXTENDED);
		if(!isMatch){
		    /* no match */
		    continue;
		}
		else{
		    /* match */
		    isFound=1;
		    break;
		}
	    }
	    else{
		/* this param is partucular key */
		/* get value of param from req if it exists*/
		char val[CONTAINS];
		/* clear contents */
		memset(val,0,CONTAINS);
		/* 1 is expected num of substrings */
		/* mem is space for subtring for func to write into */
		char *mem[1];
		mem[0]=val;
		/* construct regex pattern to look for param in hdr */
		memset(temp,'\0',BUFSIZE);
		strncpy(temp,"\\?.*",BUFSIZE);
		strncat(temp,param,BUFSIZE);
		strncat(temp,"=([^&]+)",BUFSIZE);
		pat_substr_val=temp;
		//char *pat_substr_val="\?.*"+param+"=([^ &]+)"; /* verified */
		int ret=get_substr(hdr,pat_substr_val,1,mem,REG_EXTENDED);
		if(!ret){
		    /* no substring matched */
		    continue;
		}
		else{
		    /* we found the param in hdr */
		    /* now check if val is present in contains */
		    /* contains is the pattern */
		    int isMatch=match_query(val,contains,REG_EXTENDED);
		    if(isMatch){
			/* val is contained in contains */
			/* reject this */
			isFound=1;
			break;
		    }
		}
	    }
	}
    }
    return isFound;
}


