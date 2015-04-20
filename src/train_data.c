#include "waf.h"

int main(){

    char *sig_file_path="/home/karthik/waf/src/docs/signatures.sig";
    char *req="GET /?fahsdf=dsafasd HTTP/1.1";
    fprintf(stderr,"therequest:%s\n",req);
    /* const char * headerstr = apr_table_get(r->headers_in, "User-Agent"); */
    const char * headerstr = "Mozilla/5.0 (X11; Linux x86_64; rv:12.0) Gecko/20100101 Firefox/21.0";
    char **sigs=parseSignatures(sig_file_path);
    
    /* int i=0; */
	/* for(;sigs[i];i++){ */
	/*     fprintf(stderr, "sigs is:%s\n",sigs[i]); */
	/* } */
/* header filter */
    char buffer[BUFSIZE];
    strncpy(buffer,"User-Agent###",BUFSIZE);
    strncat(buffer,headerstr,BUFSIZE);
    /* fprintf(stderr,"->->%s\n",buffer); */
    if(header_allow(buffer,sigs)==0){ /* case insensitive */
	fprintf(stderr,"header allowed\n");
    }
    else{
	/* printf("header not allowed\n"); */
	fprintf(stderr,"header not allowed\n");
    }
/* request filter */
    if(req_allow(req,sigs)==0){
	fprintf(stderr,"req allowed\n");
    }
    else{
	fprintf(stderr,"req not allowed\n");
    }
    
    initialize_pages();
    // char *s = my_malloc(3000);
    FILE *fp;

	fp = fopen("/home/karthik/waf/src/access_waf.log","r");
	if(fp == NULL){
	    fprintf(stderr,"Couldnot open the file. EXITING\n");
	    exit(0);
	}
	// const char delim[10] = DELIMITER;
	char* line = NULL;
	ssize_t read;
	ssize_t len = 0;


	while((read = getline(&line,&len,fp)) != -1){
		store_data(line);
		// //printf("------\n");
	}
	// // free(s);
	char url[100] = "/index.html?foo=bars&k1=21&k2=2";
	printf("My char set = %s\n",url);
	mode m = max;
	printf("Testing Max\n");
	int drop = is_url_valid(url,m);
	if(drop != 0){
		printf("Url is dropped!!!\n");
	}else{
		printf("Url Not dropped\n");
	}
	printf("--------------------------------\n");
	printf("Testing Avg\n");
	drop = is_url_valid(url,avg);
	if(drop != 0){
		printf("Url is dropped!!!\n");
	}else{
		printf("Url Not dropped\n");
	}
	printf("Testing SD \n");
	drop = is_url_valid(url,sd);
	if(drop != 0){
		printf("Url is dropped!!!\n");
	}else{
		printf("Url Not dropped\n");
	}
	printf("--------------------------------\n");
	drop = is_url_valid(url,char_val);
	if(drop != 0){
		printf("Url is dropped!!!\n");
	}else{
		printf("Url Not dropped\n");
	}
	//test_page_content();
	fclose(fp);
}

void test_page_content(){
	int i=0;
	int count;
	Page page_var;
	for(i=0;i<1000;i++){
		page_var = page_arr[i];
		if(page_var.used == 1){

			printf("Name = %s\n",page_var.name);
			for(count=0;count<5;count++){
				if(page_var.protocol[count] != NULL){
					printf("Protocol = %s\n",page_var.protocol[count]);
				}
			}
			int j = 0;
			for(j=0;j<2048;j++){
				if(page_var.param[j]){
					parameters *p;
					p = page_var.param[j];
					printf("Printing Url Parameters for this \n");

					printf("Name %s\n",p->name);
					printf("Max Value %f\n",p->max_val);
					printf("Min value %f\n",p->min_val);
					printf("Avg value %f\n",p->avg);
					printf("Count %d\n",p->count);
					printf("Valid Integer inputs\n");
					int i;
					for(i=0;i<p->count;i++){
						printf("%f , ",p->valid_input[i]);
					}
					printf("\n");

					printf("Valid Char inputs\n");
					for(i=0;i<p->char_count;i++){
						printf("%s , ",p->valid_chars[i]);
					}
					printf("\n");
					printf("Sd %f\n",p->sd);
					printf("\n\n-------xxxxxxx--------\n\n");
				}

			}
			printf("--------------------------------------------------\n");

		}

	}
}

void store_data(char* line){
	char* token = my_malloc(10000);
	int index = 0;
	char* place_holder = my_malloc(sizeof(char));

	token = strtok_r(line,DELIMITER,&place_holder);

	char *status,*url,*name,*protocol;
	while(token != NULL){
		if(index == 11){
			status = my_malloc(strlen(token)*sizeof(char));
			strcpy(status,token);
			// //printf("[store_data] Status = %s\n",status);
		}

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

		if(index == 5){
			protocol = my_malloc(strlen(token)*sizeof(char));
			strcpy(protocol,token);
			// //printf("[store_data] Protocol = %s\n",protocol);
			// //printf("[After Protocol change]Index accessed = %d and url = %s\n",index,url);

		}
		// //printf("Index accessed = %d and url = %s\n",index,url);
		token = strtok_r(NULL,DELIMITER,&place_holder);
		index += 1;
	}

	add_details_to_page(protocol,name,url);
	// free(status);
	// free(protocol);
	// free(name);
	// free(url);

	// // free(token);
}

void get_parameter_obj(char* url,int page_index){
	//printf("[get_parameter_obj] page index = %d and url = %s\n",page_index,url);
	// //printf("Url = %s\n",url);
	Page* page_var;
	page_var = &page_arr[page_index];

	int param_index = 0;
	int j;

	char* place_holder = my_malloc(sizeof(char));

	char *p = my_malloc(strlen(url)*sizeof(char));
	strcpy(p,url);
	//printf("[get_parameter_obj] Original url = %s\n",url);

	if(*p == '?')
		p++;

	//printf("[get_parameter_obj] Value = %s\n",p);

	char* token1 = my_malloc(strlen(url)*sizeof(char));
	token1 = strtok_r(p,"&",&place_holder);

	while(token1 != NULL){
		//printf("[get_parameter_obj] Token = ~~ %s\n",token1);

		if(strstr(token1,"=") != NULL){
			//printf("[get_parameter_obj] Token = %s\n",token1);
			char* lhs = my_malloc(strlen(token1)*sizeof(char));
			char* rhs = my_malloc(strlen(token1)*sizeof(char));

			char* temp_token = my_malloc(strlen(url)*sizeof(char));
			// temp_token = strcpy(temp,"=");
			strcpy(temp_token,token1);
			// temp_token = strtok(NULL,"=");
			// strcpy(rhs,temp_token);

			get_lhs_and_rhs(lhs,rhs,temp_token);
			//printf("[get_parameter_obj] lhs is %s  rhs is %s\n",lhs,rhs);
			for(j=0;j<2048;j++){
				if(page_var->param[j] != NULL &&  strcmp(page_var->param[j]->name,lhs)==0){
					parameters *p;
					p = page_var->param[j];

					populate_min_max_avg(rhs,p);
					//printf("Token already exists this%s\n",token1);

					break;

				}
				if(page_var->param[j] == NULL){
					//printf("Enters here atleast !!\n");
					page_var->param[j] = my_malloc(sizeof(parameters));
					parameters *param_ptr;
					param_ptr = page_var->param[j];

					param_ptr->max_val = 0;
					param_ptr->min_val = 0;
					param_ptr->count = 0;
					param_ptr->char_count = 0;
					param_ptr->avg = 0;
					param_ptr->sd = 0;

					param_ptr->name = my_malloc(strlen(lhs)*sizeof(char));
					// //printf("New token added%s\n",token1);

					populate_min_max_avg(rhs,param_ptr);
					// //printf("Min Max populated \n");
					strcpy(param_ptr->name,lhs);	
					break;
				}

			}


		}

		token1 = strtok_r(NULL,"&",&place_holder);
		//printf("[get_parameter_obj] Next Token = ~~ %s\n",token1);

	}

	// free(token1);
	// free(place_holder);
	// // free(p);
}

void populate_min_max_avg(char* input,parameters* p){

	int count = p->count;
	float maxVal=p->max_val;
	float minVal=p->min_val;
	float total = p->avg * count;
	char* place_holder = my_malloc(sizeof(char));

	// //printf("Value changed = %s\n",s);

	//printf("[populate_min_max_avg] Input = %s\n",input);
	char* new_input = my_malloc((strlen(input)+1)*sizeof(char));
	strcpy(new_input,input);

	// new_input[strlen(input)] = ',';
	//printf("[populate_min_max_avg] New Input = %s\n",new_input);

	char* token = my_malloc((strlen(input)+1)*sizeof(char));
	token = strtok_r(new_input,",",&place_holder);




	while(token != NULL){
		float x = atof(token);

		if(count == 0){
			maxVal = x;
			minVal = x;
			total = x;
		}else{
			maxVal = x>maxVal?x:maxVal;
			minVal = x<minVal?x:minVal;
			total += x;
		}

		if (x != 0.0){
			//printf("Valid float\n");
			p->valid_input[count%100] = x;
			count += 1;
		}else{
			p->valid_chars[p->char_count] = my_malloc(sizeof(token)*sizeof(char));
			strcpy(p->valid_chars[p->char_count],token);
			p->char_count += 1;
			//printf("not Valid float %s\n",token);
		}

		// //printf("Float value = %f\n",x);
		// //printf("Token = %s\n",token);
		token = strtok_r(NULL,",",&place_holder);


	}


	//printf("Standard deviation = %f\n",p->sd);

	float avg = total/count;
	//printf("Max =%f\n",maxVal);
	//printf("Min = %f \n",minVal);
	//printf("Avg = %f \n",avg);
	p->count = count;
	p->max_val = maxVal;
	p->min_val = minVal;
	p->avg = avg;
	p->sd = standard_deviation(p->valid_input,avg,count);
	// free(place_holder);	

}

void add_details_to_page(char* protocol,char* name,char* url){
	// //printf("[add_details_to_page] url = %s\n",url);

	// //printf("protocol = %s , name = %s m url = %s\n",protocol,name,url);
	Page* page_var;
	int index;

	index = get_page_index(name);
	// //printf("Index returned = %d\n",index);
	page_var = &page_arr[index];
	strcpy(page_var->name,name);
	// //printf("Name copied%s\n",page_var->name);
	if(page_var->used == 0){
		// //printf("Page is not used\n");
		page_var->protocol[0] = my_malloc(strlen(protocol));
		strcpy(page_var->protocol[0],protocol);
	}else{
		// //printf("Page already there\n");
		int count;
		for(count=0;count<5;count++){
			// //printf("Value returned is = %s\n",page_var->protocol[count]);
			if(page_var->protocol[count]!= NULL && strcmp(protocol,page_var->protocol[count]) == 0){
				break;
			}
			if(page_var->protocol[count]== NULL){
				page_var->protocol[count] = my_malloc(strlen(protocol));
				strcpy(page_var->protocol[count],protocol);
				break;
			}
		}
	}
	get_parameter_obj(url,index);
	page_var->used = 1;
}

void initialize_pages(){
	int i,j;

	for(i=0;i<1000;i++){
		page_arr[i].used = 0;

		for(j=0;j<5;j++){
			page_arr[i].protocol[j] = NULL;
		}
		for(j=0;j<2048;j++){
			page_arr[i].param[j] = NULL;
		}
		for(j=0;j<2048;j++){
			page_arr[i].time_served[j] = 0.0;
		}
		for(j=0;j<2048;j++){
			page_arr[i].keep_alive_requests[j] = 0.0;
		}
		for(j=0;j<2048;j++){
			page_arr[i].bytes_sent[j] = 0.0;
		}
		for(j=0;j<2048;j++){
			page_arr[i].bytes_received[j] = 0.0;
		}
		for(j=0;j<2048;j++){
			page_arr[i].bytes_transferred[j] = 0.0;
		}
	}

}

void get_lhs_and_rhs(char* lhs,char* rhs,char* temp){
	int l = 0;
	char* start_lhs = lhs;
	char* start_rhs = rhs;
	// //printf("Entering populate_Fields = %s\n",temp);
	while(*temp != '\0'){
		if(*temp == '='){
			l =1;
			temp++;	
		}
		if(l == 0){
			*lhs = *temp;
			lhs++;
		}else{
			*rhs = *temp;
			rhs++;
		}
		temp++;
	}
	lhs = start_lhs;
	rhs = start_rhs;
	// //printf("LHS O  = %s and RHS O  = %s\n",start_lhs,start_rhs);
}

int number_of_values(char* p){
	int count = 0;
	while(*p){
		if(*p == ',')
			count ++;
		p++;
	}
	return count;
}

float standard_deviation(float data[],float mean, int n)
{
	int i=0;
	float sum_deviation = 0;
	// printf("[standard_deviation] Count = %d\n",n);
	// printf("[standard_deviation] Mean = %f\n",mean);
	for(i=0; i<n;++i){
		// printf("data  = %f \n",data[i]);
		sum_deviation+=(data[i]-mean)*(data[i]-mean);
	}

	return sqrt(sum_deviation/n);           
}
int is_url_valid(char *url, mode m){

	char backup[2048];// = my_malloc(sizeof(url)*sizeof(char));
	fprintf(stderr,"Value of url = %s\n",url);
	strcpy(backup,url);

	char *name = NULL;
	char *params = NULL;	
	char* token = my_malloc(2000*sizeof(char));
	char* place_holder = my_malloc(2000);	
	token = strtok_r(url,"?",&place_holder);
	fprintf(stderr,"1 Token = %s\n",token);
	while(token != NULL){
		if(name == NULL){
		    fprintf(stderr,"3 name token -> %s\n",token);
		    name = my_malloc(strlen(token)*sizeof(char)*100);
		    fprintf(stderr,"4 hello 1\n");
		    strcpy(name,token);
		    fprintf(stderr,"5 hello 2\n");		    
		}else{
		    fprintf(stderr,"6 param token -> %s\n",token);
		    params = my_malloc(strlen(token)*sizeof(char));
		    strcpy(params,token);
		}
		token = strtok_r(NULL,"?",&place_holder);
		fprintf(stderr,"7Token = %s\n",token);
	}

	fprintf(stderr,"Value of params = %s\n",params);
	int page_index=get_page_index(name);
	Page page_var = page_arr[page_index];
	fprintf(stderr,"Index val = %d\n",page_index);
	fprintf(stderr,"Value of name = %s\n",page_var.name);

	char* new_place_holder = my_malloc(sizeof(char));
	char* param_token = my_malloc(sizeof(params)*sizeof(char));
	param_token = strtok_r(params,"&",&new_place_holder);
	int invalid = 0;
	while(param_token != NULL && invalid == 0){
		char *key = NULL;
		char *value = NULL;
		char* value_token = my_malloc(sizeof(param_token)*sizeof(char));
		char *key_ph = my_malloc(sizeof(char));
		value_token = strtok_r(param_token,"=",&key_ph);
		while(value_token != NULL){
			if(key == NULL){
				key = my_malloc(sizeof(value_token));
				strcpy(key,value_token);
				fprintf(stderr,"Key == %s\n",key);
			}else if(value == NULL){
				value = my_malloc(sizeof(value_token));
				strcpy(value,value_token);
				fprintf(stderr,"Value == %s\n",value);
			}
			value_token = strtok_r(NULL,"=",&key_ph);
		}
		int param_index = get_param_index(page_index,key);
		fprintf(stderr,"Param index found = %d\n",param_index);
		if(param_index == -1){
			invalid = -1;
			break;
		}
		parameters *p = page_var.param[param_index];	
		if(m == max){
			fprintf(stderr,"1Comes here\n");
			float _value = atof(value);
			fprintf(stderr,"%f\n",_value);
			fprintf(stderr,"1Comes here %s\n",value);
			fprintf(stderr,"1Is %f grater than %f for key = %s\n",_value,p->max_val,key);
			if(_value > p->max_val){
				fprintf(stderr,"1Inavlid because of Max params\n");
				invalid = -1;
				break;
			}
		}else if(m == sd){
			float _value = atof(value);
			fprintf(stderr,"2Is %f grater than %f for key = %s\n",_value,p->sd,key);
			if(_value > 3*p->sd || _value < 3*p->sd){
				fprintf(stderr,"2Inavlid because of SD params\n");
				invalid = -1;
				break;
			}
			fprintf(stderr,"2sd mode selected \n");

		}else if(m == avg){
			float _value = atof(value);
			fprintf(stderr,"3Is %f grater than %f for key = %s\n",_value,p->avg,key);
			if(_value > p->avg){
				fprintf(stderr,"3 Invalid because of avg param\n");
				invalid = -1;
				break;
			}

		}else{
			float _value = atof(value);
			int count = p->count;
			int found =0;
			int i=0;
			for(i=0;i<count;i++){
				float f = p->valid_input[i];
				fprintf(stderr,"Valid float = %f \n",p->valid_input[i]);
				if(f == _value){
					found = 1;
					break;
				}
			}
			int char_count = p->char_count;
			for(i=0;i<char_count;i++){
				fprintf(stderr,"Valid chars = %s \n",p->valid_chars[i]);
				if(strcmp(p->valid_chars[i],value)==0){
					found = 1;
					break;
				}
			}
			invalid = found == 0?-1:0;
			fprintf(stderr,"4Invalid or Valid because of char set\n");
		}
		param_token = strtok_r(NULL,"&",&new_place_holder);
	}

	strcpy(url,backup);
//	// free(backup);
	// free(name);
	// free(params);
	// free(token);
	// free(place_holder);
	return invalid;
}


int get_param_index(int page_index,char* name){
	Page page_var = page_arr[page_index];
	int index = -1;
	int j =0;
	for(j=0;j<2048;j++){
		if(page_var.param[j]){
			parameters *p = page_var.param[j];
			if(strcmp(p->name,name) == 0){
				index = j;
				break;
			}
		}
	}
	return index;
}


int get_page_index(char *name){
	int index = -1;
	int i;
	for(i=0;i<1000;i++){
		index = i;
		if(page_arr[i].used == 0){
			break;
		}

		if(strcmp(name,page_arr[i].name) == 0){
			break;
		}

	}
	return index;

}


void * my_malloc(size_t sz){
    return malloc(20480*sizeof(char));
}
