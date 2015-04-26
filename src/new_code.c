#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define NUMBER 1
#define ALPHABET 2
#define SPL_SYMBOL 4

void store_data();

typedef enum {max,avg,charset,sd} mode;

int getCharset(char);
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

int main(){
	printf("Hello world\n");
	FILE *fp;

	fp = fopen("access_waf.log","r");
	if(fp == NULL){
		printf("Couldnot open the file. EXITING\n");
		exit(0);
	}
	// const char delim[10] = DELIMITER;
	char* line = NULL;
	ssize_t read;
	ssize_t len = 0;
	int i = 0;
	for(i=0;i<100;i++){
		page_arr_index[i] = 0;
	}

	while((read = getline(&line,&len,fp)) != -1){
		store_data(line);
		printf("------\n");
	}
	print_profile();

	 char url[100] = "/index.html?foo=bars&k1=21,s&k2=2";
	 char* backup = my_malloc(strlen(url)*sizeof(char));
	 strcpy(backup,url);

	 mode m = max;
	 printf("Testing Max\n");
	 int drop = is_url_valid(url,m);
	 if(drop != 0){
	 	printf("Url is dropped!!!\n");
	 }else{
	 	printf("Url Not dropped\n");
	 }
	
	 strcpy(url,backup);
	 m = avg;
	 printf("Testing Average\n");
	 drop = is_url_valid(url,m);
	 if(drop != 0){
	 	printf("Url is dropped!!!\n");
	 }else{
	 	printf("Url Not dropped\n");
	 }
	
	 strcpy(url,backup);
	 m = charset;
	 printf("Testing Char Set\n");
	 drop = is_url_valid(url,m);
	 if(drop != 0){
	 	printf("Url is dropped!!!\n");
	 }else{
	 	printf("Url Not dropped\n");
	 }

	 strcpy(url,backup);
	 m = sd;
	 printf("Testing Average\n");
	 drop = is_url_valid(url,m);
	 if(drop != 0){
	 	printf("Url is dropped!!!\n");
	 }else{
	 	printf("Url Not dropped\n");
	 }
	


	return 10;
}

void print_profile(){
	int page_index =0;	
	for(page_index=0;page_index<100;page_index++){
		if(page_arr_index[page_index] ==0)
			break;
		page curr_page = page_arr[page_index];
		printf("----------------------------------------------------------------------------\n");
		printf("Details for Page = %s \t and Max Params = %d\n",curr_page.pageName,curr_page.maxParams);
		printf("|ParamName\t\tAverage\t\tStd\t\t\tCharset\t\t\n");
		// printf("----------\n");
		int param_index = 0;
		for(param_index =0;param_index < 100;param_index++){
			if(curr_page.param_arr_index[param_index]==0)
				break;


			

			param *url_param = &curr_page.url_params[param_index];
			url_param->avg = (float)url_param->total/url_param->count;
			float sd = standard_deviation(url_param->sd_array,url_param->avg);
			url_param->sd = sd;
			char arr[3];
			arr[0] = url_param->csets[0] == 1?'N':' ';
			arr[1] = url_param->csets[1] == 1?'S':' ';
			arr[2] = url_param->csets[2] == 1?'~':' ';
			
			printf("|%s\t\t%f\t\t%f\t\t%s\n",url_param->paramName,url_param->avg,url_param->sd,arr);
			
			
		}
		printf("---------------------------------------------------------------------------\n");
	}
}

void store_data(char* line){
	printf("[store_data] line = %s and\n",line);
	char* token = my_malloc(10000);
	int index = 0;
	char* place_holder = my_malloc(sizeof(char));

	token = strtok_r(line,DELIMITER,&place_holder);

	char *status,*url,*name,*protocol;
	while(token != NULL){
		if(index == 3){
			name = my_malloc(strlen(token)*sizeof(char));
			strcpy(name,token);
			// //printf("Name = %s\n",name);
		}

		if(index == 4){
			url = my_malloc(strlen(token)*sizeof(char));
			strcpy(url,token);
			// //printf("[store_data] Url = %s\n",url);
		}
		
		token = strtok_r(NULL,DELIMITER,&place_holder);
		index += 1;
	}
	printf("[store_data] name = %s and url = %s\n",name,url);
	add_details_to_page(name,url);
	// free(status);
	// free(protocol);
	// free(name);
	// free(url);

	// free(token);
}

void add_details_to_page(char *name, char* url){
	int index = get_page_index(name);
	// strcpy(page_arr[index].pageName,name);
	page_arr[index].maxParams = get_max_params(url);
	construct_params(index,url);
	// printf("[add_details_to_page] index = %d and url = %s\n",index,url);

}


void construct_params(int page_index,char* url){
	page *current_page = &page_arr[page_index];
	char *p = my_malloc(strlen(url)*sizeof(char));
	strcpy(p,url);
	//printf("[get_parameter_obj] Original url = %s\n",url);

	if(*p == '?')
		p++;

	//printf("[get_parameter_obj] Value = %s\n",p);

	char* token1 = my_malloc(strlen(p)*sizeof(char));
	char* place_holder = my_malloc(sizeof(char));
	token1 = strtok_r(p,"&",&place_holder);
	int length = 1;
	while(token1 != NULL){
		length += 1;
		char *s = my_malloc(1000);
		strcpy(s,token1);
		char* place_holder2 = my_malloc(sizeof(char));

		char* token2 = my_malloc(strlen(token1)*sizeof(char));
		token2 = strtok_r(s,"=",&place_holder2);
		char* lhs = NULL;
		char* rhs = NULL;


		while(token2 != NULL){
			if(lhs == NULL){
				lhs = my_malloc(strlen(token2)*sizeof(char));
				strcpy(lhs,token2);
			}else{
				rhs = my_malloc(strlen(token2)*sizeof(char));
				strcpy(rhs,token2);
			}

			token2 = strtok_r(NULL,"=",&place_holder2);
		}
		
		char* t_rhs = my_malloc(1000);
		strcpy(t_rhs,rhs);
		// int len = 0;
		
		// printf("[construct_params] lhs = %s and rhs = %s\n",lhs,rhs);
		int param_index = get_param_index(lhs,page_index);


		current_page->url_params[param_index].sd_array[strlen(rhs)] += 1;
		current_page->url_params[param_index].count += 1;

		current_page->url_params[param_index].total += strlen(rhs);
		getAllCharset(rhs,current_page->url_params[param_index].csets);
		// strcpy(current_page.url_params[param_index].paramName,lhs);
		
		token1 = strtok_r(NULL,"&",&place_holder);

	}
	
}


int get_max_params(char* url){
	char *p = my_malloc(strlen(url)*sizeof(char));
	strcpy(p,url);
	//printf("[get_parameter_obj] Original url = %s\n",url);

	if(*p == '?')
		p++;

	//printf("[get_parameter_obj] Value = %s\n",p);

	char* token1 = my_malloc(strlen(p)*sizeof(char));
	char* place_holder = my_malloc(sizeof(char));
	token1 = strtok_r(p,"&",&place_holder);
	int length = 0;
	while(token1 != NULL){
		length += 1;
		token1 = strtok_r(NULL,"&",&place_holder);

	}
	return length;
}

int get_page_index(char* name){
	int page_index = -1;
	int i=0;
	for(i=0;i<100;i++){
		page_index = i;
		if(page_arr_index[page_index] == 0){
			page new_page;
			int j;
			for(j=0;j<100;j++){
				new_page.param_arr_index[j] = 0;
			}
			// page_arr[page_index] = new_page;
			strcpy(page_arr[page_index].pageName,name);
			page_arr_index[page_index] = 1;
			break;
		}else{
			page_arr[page_index];
			// printf("%s \n",name);
			if(strcmp(page_arr[page_index].pageName,name) == 0){
				break;
			}
		}
		
	}
	// printf("Page index = %d for %s \n",page_index,name);
	return page_index;
}

int get_param_index(char* name,int page_index){
	page_arr[page_index];
	int param_index = -1;
	int i=0;
	for(;i<100;i++){
		param_index = i;
		if(page_arr[page_index].param_arr_index[param_index] == 0){
			int j=0;
			for(j=0;j<100;j++){
				page_arr[page_index].url_params[param_index].sd_array[j] = 0;
			}
			strcpy(page_arr[page_index].url_params[param_index].paramName,name);
			// current_page->url_params[param_index] = url_param;
			break;
		}else{

			if(strcmp(page_arr[page_index].url_params[param_index].paramName,name)==0){
				break;
			}
		}

	}
	// printf("Param index = %d for %s\n",param_index,name);
	page_arr[page_index].param_arr_index[param_index] = 1;
	return param_index;
}

float standard_deviation(int* data,float mean)
{
	int index=0;
	float sum_deviation = 0;
	// printf("[standard_deviation] Count = %d\n",n);
	// printf("[standard_deviation] Mean = %f\n",mean);
	for(index=0;index<100;index++){

		int value = data[index];
		if(value == 0)
			 continue;

		// printf("[standard_deviation]index = %d , value = %d\n",index,value);
		while(value >0){

		// printf("data  = %f \n",data[i]);
			sum_deviation+=(index-mean)*(index-mean);
			value --;
		}
	}  

	return 1.0;
	return sqrtf(sum_deviation/100);           
}

int getCharset(char c) {
	if (c >= 48 && c <= 57)
		return NUMBER;
	else if ((c >= 65 && c <= 90) || c >= 97 && c <= 122)
		return ALPHABET;
	else
		return SPL_SYMBOL;
}

int getAllCharset(char *str, int* charsetList) {
	int i = 0;
	
	int charSetsCovered = 0; // lower 3 bits represent charsets present
	while (str[i]) {
		int charSet = getCharset(str[i]);
		charSetsCovered |= charSet;

		if (charSetsCovered == 7)
			break;
		i++;
	}

	int j = 0;
	int myarr[] = {1,2,4};
	while (j < 3) {
		if (charSetsCovered & myarr[j] ) {
			charsetList[j] = 1;
		} else {
			charsetList[j] = 0;
		}
		j++;
	}

}
int is_url_valid(char* line,mode m){
	char* name = NULL;
	char* url = NULL;
	char* token = my_malloc(10000);
	int index = 0;
	char* place_holder = my_malloc(sizeof(char));
	printf("Line = %s\n",line);	

	token = strtok_r(line,"?",&place_holder);
	
	while(token != NULL){
		if(name == NULL){
			name = my_malloc(strlen(token)*sizeof(char));
			strcpy(name,token);
			// //printf("Name = %s\n",name);
		}else{
			url = my_malloc(strlen(token)*sizeof(char));
			strcpy(url,token);
			// //printf("[store_data] Url = %s\n",url);
		}
		
		token = strtok_r(NULL,"?",&place_holder);
		index += 1;
	}
	
	printf("[is_url_valid] name = %s and url = %s\n",name,url);

	int page_index = get_page_index(name);

	int maxParams = get_max_params(url);
	if(page_arr[page_index].maxParams < maxParams && m == max){
		printf("The %s file has max params = %d and that is exceeded by request with params = %d",name,page_arr[page_index].maxParams,maxParams);
		return -1;
	}




	page *current_page = &page_arr[page_index];
	char *p = my_malloc(strlen(url)*sizeof(char));
	strcpy(p,url);
	//printf("[get_parameter_obj] Original url = %s\n",url);

	//printf("[get_parameter_obj] Value = %s\n",p);

	char* token1 = my_malloc(strlen(p)*sizeof(char));
	char* place_holder2 = my_malloc(sizeof(char));
	token1 = strtok_r(p,"&",&place_holder2);
	int length = 1;
	while(token1 != NULL){
		length += 1;
		char *s = my_malloc(1000);
		strcpy(s,token1);
		char* place_holder3 = my_malloc(sizeof(char));

		char* token2 = my_malloc(strlen(token1)*sizeof(char));
		token2 = strtok_r(s,"=",&place_holder3);
		char* lhs = NULL;
		char* rhs = NULL;


		while(token2 != NULL){
			if(lhs == NULL){
				lhs = my_malloc(strlen(token2)*sizeof(char));
				strcpy(lhs,token2);
			}else{
				rhs = my_malloc(strlen(token2)*sizeof(char));
				strcpy(rhs,token2);
			}

			token2 = strtok_r(NULL,"=",&place_holder3);
		}
		
		char* t_rhs = my_malloc(1000);
		strcpy(t_rhs,rhs);
		// printf("[construct_params] lhs = %s and rhs = %s\n",lhs,rhs);
		int param_index = get_param_index(lhs,page_index);

		
		int len = strlen(rhs);
		param url_param= current_page->url_params[param_index];
		if(m == avg){
			if(url_param.avg < len){
				printf("[URL Dropped]The average length of  parameter = %s is %f , the present parameter length is %d\n",lhs,url_param.avg,len);
				return -1;
			}
		}else if(m == sd){
			float value = 3*url_param.sd + url_param.avg;
			if(value < len){
				printf("[URL Dropped] The (3*SD+Avg) length of  parameter = %s is %f , the present parameter length is %d\n",lhs,value,len);
				return -1;
			}

		}else if(m == charset){	
			int *c = url_param.csets;
			int o[3];
			getAllCharset(rhs,o);
			int i =0;
			int false = 0;
			for(i=0;i<2;i++){
				if(c[i] != o[i]){
					false = 1;
					break;
				}
			}

			if(false == 1){
				if(i == 0){
					printf("[URL Dropped] parameter = %s doesnot accept number as input parameter.\n",lhs);
				}
				if(i==1){
					printf("[URL Dropped] parameter = %s doesnot accept string as input parameter.\n",lhs);
				}
				if(i==2){
					printf("[URL Dropped] parameter = %s doesnot accept special charachters as input parameter.\n",lhs);
				}
				return -1;
			}
			
			
		}

	token1 = strtok_r(NULL,"&",&place_holder2);

	
	}
	// printf(" backup = %s  and line = %s",backup,line);
	
}
	
void * my_malloc(size_t sz){
    return malloc(2048*sizeof(char));
}	

	



