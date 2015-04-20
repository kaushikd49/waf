/*
	Copyright 2002 Kevin O'Donnell

	Adapted for Linux User and Developer Magazine by Kunal Deo
	Copyright 2010 Kunal Deo

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "httpd.h"
#include "http_config.h"

#ifndef DEFAULT_MODlud_STRING
#define DEFAULT_MODlud_STRING "mod_lud: Default HTTP Request Alert."
#endif

#include "/home/karthik/waf/src/waf.h"



module AP_MODULE_DECLARE_DATA lud_module;


typedef struct {
	char *string;
} modlud_config;

static int mod_lud_method_handler (request_rec *r)
{
    modlud_config *s_cfg = ap_get_module_config(r->server->module_config, &lud_module);
    
	fprintf(stderr,"%s\n",s_cfg->string);
	char s[] = "adsad";
	//fprintf(stderr,"%s\n",r->the_request);
	char *sig_file_path="/home/karthik/waf/src/docs/signatures.sig";
	fprintf(stderr,"therequest:%s\n",r->the_request);
	const char * headerstr = apr_table_get(r->headers_in, "User-Agent");
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
        if(req_allow(r->the_request,sigs)==0){
	    fprintf(stderr,"req allowed\n");
	}
	else{
	    fprintf(stderr,"req not allowed\n");
	}
	
	char url[100] = "/index.html?foo=bars&k1=21&k2=2";
	mode m=max;
	int drop=is_url_valid(url,m);
	if(drop != 0){
		printf("Url is dropped!!!\n");
	}else{
		printf("Url Not dropped\n");
	}
	fflush(stderr);
	return DECLINED;
}

static void mod_lud_register_hooks (apr_pool_t *p)
{
	ap_hook_handler(mod_lud_method_handler, NULL, NULL, APR_HOOK_FIRST);
}

static const char *set_modlud_string(cmd_parms *parms, void *mconfig, const char *arg)
{
	modlud_config *s_cfg = ap_get_module_config(parms->server->module_config, &lud_module);

	s_cfg->string = (char *) arg;

	return NULL;
}

static const command_rec mod_lud_cmds[] =
{
	AP_INIT_TAKE1(
		"ModuleTutorialString",
		set_modlud_string,
		NULL,
		RSRC_CONF,
		"ModludString <string> -- the string to print to the error log for each HTTP request."
	),
	{NULL}
};

static void *create_modlud_config(apr_pool_t *p, server_rec *s)
{
	modlud_config *newcfg;

	newcfg = (modlud_config *) apr_pcalloc(p, sizeof(modlud_config));

	newcfg->string = DEFAULT_MODlud_STRING;

	return (void *) newcfg;
}

module AP_MODULE_DECLARE_DATA lud_module =
{
	STANDARD20_MODULE_STUFF,
	NULL,
	NULL,
	create_modlud_config,
	NULL,
	mod_lud_cmds,
	mod_lud_register_hooks,
};



int is_url_valid(char *url, mode m){
	
	char backup[2048];// = malloc(sizeof(url)*sizeof(char));
	strcpy(backup,url);

	char *name = NULL;
	char *params = NULL;
	char* token = malloc(sizeof(url)*sizeof(char));
	char* place_holder = malloc(sizeof(char));	
	token = strtok_r(url,"?",&place_holder);
	//printf("Value of url = %s\n",url);
	while(token != NULL){
		if(name == NULL){
			name = malloc(sizeof(token)*sizeof(char));
			strcpy(name,token);
		}else{
			params = malloc(sizeof(token)*sizeof(char));
			strcpy(params,token);
		}
		token = strtok_r(NULL,"?",&place_holder);
	}

	//printf("Value of params = %s\n",params);
	int page_index=get_page_index(name);
	Page page_var = page_arr[page_index];
	//printf("Index val = %d\n",page_index);
	//printf("Value of name = %s\n",page_var.name);

	char* new_place_holder = malloc(sizeof(char));
	char* param_token = malloc(sizeof(params)*sizeof(char));
	param_token = strtok_r(params,"&",&new_place_holder);
	int invalid = 0;
	while(param_token != NULL && invalid == 0){
		char *key = NULL;
		char *value = NULL;
		char* value_token = malloc(sizeof(param_token)*sizeof(char));
		char *key_ph = malloc(sizeof(char));
		value_token = strtok_r(param_token,"=",&key_ph);
		while(value_token != NULL){
			if(key == NULL){
				key = malloc(sizeof(value_token));
				strcpy(key,value_token);
				//printf("Key == %s\n",key);
			}else if(value == NULL){
				value = malloc(sizeof(value_token));
				strcpy(value,value_token);
				//printf("Value == %s\n",value);
			}
			value_token = strtok_r(NULL,"=",&key_ph);
		}
		int param_index = get_param_index(page_index,key);
		//printf("Param index found = %d\n",param_index);
		if(param_index == -1){
			invalid = -1;
			break;
		}
		parameters *p = page_var.param[param_index];	
		if(m == max){
			//printf("1Comes here\n");
			float _value = atof(value);
			//printf("%f\n",_value);
			//printf("1Comes here %s\n",value);
			//printf("1Is %f grater than %f for key = %s\n",_value,p->max_val,key);
			if(_value > p->max_val){
				//printf("1Inavlid because of Max params\n");
				invalid = -1;
				break;
			}
		}else if(m == sd){
			float _value = atof(value);
			//printf("2Is %f grater than %f for key = %s\n",_value,p->sd,key);
			if(_value > 3*p->sd || _value < 3*p->sd){
				//printf("2Inavlid because of SD params\n");
				invalid = -1;
				break;
			}
			//printf("2sd mode selected \n");

		}else if(m == avg){
			float _value = atof(value);
			//printf("3Is %f grater than %f for key = %s\n",_value,p->avg,key);
			if(_value > p->avg){
				//printf("3 Invalid because of avg param\n");
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
				//printf("Valid float = %f \n",p->valid_input[i]);
				if(f == _value){
					found = 1;
					break;
				}
			}
			int char_count = p->char_count;
			for(i=0;i<char_count;i++){
				//printf("Valid chars = %s \n",p->valid_chars[i]);
				if(strcmp(p->valid_chars[i],value)==0){
					found = 1;
					break;
				}
			}
			invalid = found == 0?-1:0;
			//printf("4Invalid or Valid because of char set\n");
		}
		param_token = strtok_r(NULL,"&",&new_place_holder);
	}

	strcpy(url,backup);
//	free(backup);
	free(name);
	free(params);
	free(token);
	free(place_holder);
	return invalid;
}
