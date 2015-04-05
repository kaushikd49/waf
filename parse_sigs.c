#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define BUFFER 150
#define MAXLINES 150
char ** parseSignatures(char *f){
    /* NULL marks end of sigs array. each element of sigs is pointer to char array where each char array is terminated by \0 */
    FILE *fp;
    char buff[BUFFER];
    fp=fopen(f,"r");
    if(!fp){
	printf("unable to open file\n");
	return NULL;
    }
    char **sigs=(char **)malloc(sizeof(char *)*MAXLINES);
    size_t i;
    int c=0;
    for(i=0;i<MAXLINES;i++){
	sigs[i]=NULL;		/* initialize array with NULL */
    }
    while(fgets(buff,150,fp)){
	/* some data has been read into buffer */
	i=strlen(buff)-1;
	if(buff[i]=='\n'){
	    buff[i]='\0';
	}
	/* printf("%s\n",buff); */
	sigs[c]=(char *)malloc(sizeof(char)*(strlen(buff)+1)); /* +1 for \0 */
	strcpy(sigs[c],buff);
	/* printf("%s,%d\n",sigs[c],strcmp(buff,sigs[c])); */
	c++;
    }
    /* read till EOF */
    fclose(fp);
    return sigs;
}
