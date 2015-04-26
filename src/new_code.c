#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void store_data();

#define DELIMITER "@@@###"

struct param{
	int total;
	int count;
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

page page_arr[100];

int page_arr_index[100];
int param_arr_index[100];

void add_details_to_page(char* ,char* );
void print_profile();

int main(){
	printf("Hello world\n");
	FILE *fp;

	fp = fopen("access_waf.log","r");
	if(fp == NULL){
		//printf("Couldnot open the file. EXITING\n");
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
		// //printf("------\n");
	}
	print_profile();

	return 10;
}

void print_profile(){
	int page_index =0;	
	for(page_index=0;page_index<100;page_index++){
		if(page_arr_index[page_index] ==0)
			break;
		page curr_page = page_arr[page_index];
		printf("Profile for %s\n",curr_page.pageName);
		int param_index = 0;
		for(param_index =0;param_index < 100;param_index++){
			if(curr_page.param_arr_index[param_index]==0)
				break;

			param url_param = curr_page.url_params[param_index];
			printf("Parameter Name %s",url_param.paramName);

		}


	}
}

void store_data(char* line){
	char* token = malloc(10000);
	int index = 0;
	char* place_holder = malloc(sizeof(char));

	token = strtok_r(line,DELIMITER,&place_holder);

	char *status,*url,*name,*protocol;
	while(token != NULL){
		if(index == 3){
			name = malloc(strlen(token)*sizeof(char));
			strcpy(name,token);
			// //printf("Name = %s\n",name);
		}

		if(index == 4){
			url = malloc(strlen(token)*sizeof(char));
			strcpy(url,token);
			// //printf("[store_data] Url = %s\n",url);
		}
		
		token = strtok_r(NULL,DELIMITER,&place_holder);
		index += 1;
	}

	add_details_to_page(name,url);
	// free(status);
	// free(protocol);
	// free(name);
	// free(url);

	// free(token);
}

void add_details_to_page(char *name, char* url){
	int index = get_page_index(name);
	
	strcpy(page_arr[index].pageName,name);
	page_arr[index].maxParams = get_max_params(url);
	construct_params(index,url);


}


void construct_params(int page_index,char* url){
	page current_page = page_arr[page_index];
	char *p = malloc(strlen(url)*sizeof(char));
	strcpy(p,url);
	//printf("[get_parameter_obj] Original url = %s\n",url);

	if(*p == '?')
		p++;

	//printf("[get_parameter_obj] Value = %s\n",p);

	char* token1 = malloc(strlen(p)*sizeof(char));
	char* place_holder = malloc(sizeof(char));
	token1 = strtok_r(p,"&",&place_holder);
	int length = 1;
	while(token1 != NULL){
		length += 1;
		char *s = malloc(1000);
		strcpy(s,token1);
		char* place_holder2 = malloc(sizeof(char));

		char* token2 = malloc(strlen(token1)*sizeof(char));
		token2 = strtok_r(p,"=",&place_holder);
		char* lhs = NULL;
		char* rhs = NULL;


		while(token2 != NULL){
			if(lhs == NULL){
				lhs = malloc(strlen(token1)*sizeof(char));
				strcpy(lhs,token2);
			}else{
				rhs = malloc(strlen(token1)*sizeof(char));
				strcpy(rhs,token2);
			}

			token2 = strtok_r(NULL,"=",&place_holder);
		}
		
		int param_index = get_param_index(lhs,page_index);

		current_page.url_params[param_index].total += strlen(rhs);
		current_page.url_params[param_index].count += 1; 
		strcpy(current_page.url_params[param_index].paramName,lhs);
		current_page.url_params[param_index].sd_array[strlen(rhs)] += 1;
		token1 = strtok_r(NULL,"&",&place_holder);

	}
	
}


int get_max_params(char* url){
	char *p = malloc(strlen(url)*sizeof(char));
	strcpy(p,url);
	//printf("[get_parameter_obj] Original url = %s\n",url);

	if(*p == '?')
		p++;

	//printf("[get_parameter_obj] Value = %s\n",p);

	char* token1 = malloc(strlen(p)*sizeof(char));
	char* place_holder = malloc(sizeof(char));
	token1 = strtok_r(p,"&",&place_holder);
	int length = 1;
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
			page_arr[page_index] = new_page;
			page_arr_index[page_index] = 1;
			break;
		}else{
			page p = page_arr[page_index];
			if(strcmp(p.pageName,name) == 0){
				break;
			}
		}
		
	}
	return page_index;
}

int get_param_index(char* name,int page_index){
	page current_page = page_arr[page_index];
	int param_index = -1;
	int i=0;
	for(;i<100;i++){
		param_index = i;
		if(current_page.param_arr_index[param_index] != 0){
			param url_param;
			int j=0;
			for(j=0;j<100;j++){
				url_param.sd_array[j] = 0;
			}
			current_page.url_params[param_index] = url_param;
			break;
		}else{
			if(strcmp(current_page.url_params[param_index].paramName,name)==0){
				break;
			}
		}

	}
	
	current_page.param_arr_index[param_index] = 1;
	return param_index;
}
