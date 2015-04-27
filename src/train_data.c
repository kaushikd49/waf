#include "/home/kaushik/waf/src/waf.h"

/* static int gen_train_pr=0; */
/* static int gen_profile=0; */

void print_profile(){
  /* if(gen_train_pr==0) */
  /*   gen_train_pr=1; */
  /* else */
  /*   return ; */
	int page_index =0;	
	for(page_index=0;page_index<100;page_index++){
		if(page_arr_index[page_index] ==0)
			break;
		page* curr_page = &page_arr[page_index];
		/* fprintf(stderr,"----------------------------------------------------------------------------\n"); */
		/* fprintf(stderr,"Details for Page = %s \t and Max Params = %d\n",curr_page->pageName,curr_page->maxParams); */
		/* fprintf(stderr,"|ParamName\t\tAverage\t\tStd\t\t\tCharset\t\t\n"); */
		// fprintf(stderr,"----------\n");
		int param_index = 0;
		for(param_index =0;param_index < 100;param_index++){
			if(curr_page->param_arr_index[param_index]==0)
				break;

			param *url_param = &(curr_page->url_params[param_index]);
			url_param->avg = (float)url_param->total/url_param->count;
			float sd = standard_deviation(url_param->sd_array,url_param->avg);
			url_param->sd = sd;
			char arr[3];
			arr[0] = url_param->csets[0] == 1?'N':' ';
			arr[1] = url_param->csets[1] == 1?'S':' ';
			arr[2] = url_param->csets[2] == 1?'~':' ';
			
			/* fprintf(stderr,"|%s\t\t%f\t\t%f\t\t%s\n",url_param->paramName,url_param->avg,url_param->sd,arr); */
			
			
		}
		/* fprintf(stderr,"---------------------------------------------------------------------------\n"); */
	}
	fflush(stderr);
}

void store_data(char* line){
  /* if(gen_train_pr==0)
  /*   gen_train_pr=1; */
  /* else */
  /*   return; */
	// fprintf(stderr,"[store_data] line = %s and\n",line);
	char* token = my_malloc(10000);
	int index = 0;
	char* place_holder = my_malloc(sizeof(char));

	token = strtok_r(line,DELIMITER,&place_holder);
	
	char *status,*url,*name,*protocol;
	while(token != NULL){
		if(index == 3){
			name = my_malloc(strlen(token)*sizeof(char));
			strcpy(name,token);
			/* fprintf(stderr,"Name = %s\n",name); */
			/* fflush(stderr); */
		}

		if(index == 4){
			url = my_malloc(strlen(token)*sizeof(char));
			strcpy(url,token);
			/* fprintf(stderr,"[store_data] Url = %s\n",url); */
			/* fflush(stderr); */
		}
		
		token = strtok_r(NULL,DELIMITER,&place_holder);
		index += 1;
	}
	int isMatch=match_query(url,".*=.*",REG_EXTENDED);
	if(!isMatch){
	  /* fprintf(stderr,"invalid  url = %s\n",url); */
	  /* fflush(stderr); */
	    return 0;
	}
	/* fprintf(stderr,"[store_data] name = %s and url = %s\n",name,url); */
	/* fflush(stderr); */
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
	// fprintf(stderr,"[add_details_to_page] index = %d and url = %s\n",index,url);

}


void construct_params(int page_index,char* url){
	page *current_page = &page_arr[page_index];
	char *p = my_malloc(strlen(url)*sizeof(char));
	strcpy(p,url);
	//fprintf(stderr,"[get_parameter_obj] Original url = %s\n",url);

	if(*p == '?')
		p++;

	//fprintf(stderr,"[get_parameter_obj] Value = %s\n",p);

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
		
		// fprintf(stderr,"[construct_params] lhs = %s and rhs = %s\n",lhs,rhs);
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
	//fprintf(stderr,"[get_parameter_obj] Original url = %s\n",url);

	if(*p == '?')
		p++;

	//fprintf(stderr,"[get_parameter_obj] Value = %s\n",p);

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

int get_existing_page_index(char* name){
	
	int page_index = -1;
	int i=0;
	for(i=0;i<100;i++){
		if(page_arr_index[i] == 0){
			continue;
		}else{
			// fprintf(stderr,"%s \n",name);
			if(strcmp(page_arr[i].pageName,name) == 0){
				page_index = i;
				break;
			}
		}
		
	}
	return page_index;
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
			// fprintf(stderr,"%s \n",name);
			if(strcmp(page_arr[page_index].pageName,name) == 0){
				break;
			}
		}
		
	}
	// fprintf(stderr,"Page index = %d for %s \n",page_index,name);
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
	// fprintf(stderr,"Param index = %d for %s\n",param_index,name);
	page_arr[page_index].param_arr_index[param_index] = 1;
	return param_index;
}

float standard_deviation(int* data,float mean)
{
	int index=0;
	float sum_deviation = 0;
	// fprintf(stderr,"[standard_deviation] Count = %d\n",n);
	// fprintf(stderr,"[standard_deviation] Mean = %f\n",mean);
	for(index=0;index<100;index++){

		int value = data[index];
		if(value == 0)
			 continue;

		// fprintf(stderr,"[standard_deviation]index = %d , value = %d\n",index,value);
		while(value >0){

		// fprintf(stderr,"data  = %f \n",data[i]);
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
  char* backup = my_malloc(1000);
	strcpy(backup,line);
	char* name = NULL;
	char* url = NULL;
	char* token = my_malloc(10000);
	int index = 0;
	char* place_holder = my_malloc(sizeof(char));
	// fprintf(stderr,"Line = %s\n",line);	

	token = strtok_r(line,"?",&place_holder);
	
	while(token != NULL){
		if(name == NULL){
			name = my_malloc(strlen(token)*sizeof(char));
			strcpy(name,token);
			// //fprintf(stderr,"Name = %s\n",name);
		}else{
			url = my_malloc(strlen(token)*sizeof(char));
			strcpy(url,token);
			// //fprintf(stderr,"[store_data] Url = %s\n",url);
		}
		
		token = strtok_r(NULL,"?",&place_holder);
		index += 1;
	}
	
	/* fprintf(stderr,"[is_url_valid] name = %s and url = %s\n",name,url); */
	/* fflush(stderr); */
	if(url == NULL || name == NULL){	  
		strcpy(line,backup);
		/* fprintf(stderr,"hello ####\n"); */
		/* fflush(stderr); */
		return 0;
	}


	int page_index = get_existing_page_index(name);

	if(page_index == -1){	  
		strcpy(line,backup);
		/* fprintf(stderr,"hello\n"); */
		/* fflush(stderr); */
		return 0;
	}
	int maxParams = get_max_params(url);
	if(page_arr[page_index].maxParams < maxParams && m == max){
		fprintf(stderr,"The %s file has max params = %d and that is exceeded by request with params = %d",name,page_arr[page_index].maxParams,maxParams);
		fflush(stderr);
		return -1;
	}

	page *current_page = &page_arr[page_index];
	char *p = my_malloc(strlen(url)*sizeof(char));
	strcpy(p,url);
	/* fprintf(stderr,"[get_parameter_obj] Original url = %s\n",url); */

/* 	fprintf(stderr,"[get_parameter_obj] Value = %s\n",p); */
/* fflush(stderr); */
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
		/* fprintf(stderr,"[construct_params] lhs = %s and rhs = %s\n",lhs,rhs); */
/* fflush(stderr); */
		int param_index = get_param_index(lhs,page_index);

		
		int len = strlen(rhs);
		param url_param= current_page->url_params[param_index];
		if(m == avg){
			if(url_param.avg < len){
				fprintf(stderr,"[URL Dropped]The average length of  parameter = %s is %f , the present parameter length is %d\n",lhs,url_param.avg,len);
				fflush(stderr);
				
				strcpy(backup,line);
				return -1;
			}
		}else if(m == sd){
			float value = 3*url_param.sd + url_param.avg;
			if(value < len){
				fprintf(stderr,"[URL Dropped] The (3*SD+Avg) length of  parameter = %s is %f , the present parameter length is %d\n",lhs,value,len);
				fflush(stderr);
				strcpy(backup,line);
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
					fprintf(stderr,"[URL Dropped] parameter = %s doesnot accept number as input parameter.\n",lhs);
				}
				if(i==1){
					fprintf(stderr,"[URL Dropped] parameter = %s doesnot accept string as input parameter.\n",lhs);
				}
				if(i==2){
					fprintf(stderr,"[URL Dropped] parameter = %s doesnot accept special charachters as input parameter.\n",lhs);
				}
				strcpy(backup,line);
				fflush(stderr);
				return -1;
			}
			
			
		}

	token1 = strtok_r(NULL,"&",&place_holder2);

	
	}
	strcpy(backup,line);
return 0;
	// fprintf(stderr," backup = %s  and line = %s",backup,line);
	
}
	
void * my_malloc(size_t sz){
    return malloc(2048*sizeof(char));
}	

	

