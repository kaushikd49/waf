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
    
    fprintf(stderr,"hello\n");
	fprintf(stderr,"%s\n",s_cfg->string);
	char s[] = "adsad";
	//fprintf(stderr,"%s\n",r->the_request);
	char *sig_file_path="/home/karthik/waf/src/docs/signatures.sig";
	fprintf(stderr,"therequest:%s\n",r->the_request);
	const char * headerstr = apr_table_get(r->headers_in, "User-Agent");
	char **sigs=parseSignatures(sig_file_path);
	int isAllowed=1;
	char hdr_field[PARAM], buffer[BUFSIZE];
	memset(hdr_field,0,PARAM);
	int i=0,isMatch=0;
	char *mem[1];
	mem[0]=hdr_field;
	for(i=0;sigs[i] && isAllowed;i++){
	    /* HEADER:(.+),CONTAINS */
	    isMatch=get_substr(sigs[i],"HEADER:(.+),CONTAINS",1,mem,REG_EXTENDED);
	    if(!isMatch){
		memset(hdr_field,0,PARAM);
		continue;
	    }
	    /* assume hdr_field has header field name */
	    headerstr = apr_table_get(r->headers_in,hdr_field);
	    if(headerstr==NULL){
		fprintf(stderr,"%s header field not present\n",hdr_field);
		memset(hdr_field,0,PARAM);
		continue;
	    }
	    /* header filter */
	    memset(buffer,0,BUFSIZE);
	    strncpy(buffer,hdr_field,BUFSIZE);
	    strncat(buffer,"###",BUFSIZE);
	    strncat(buffer,headerstr,BUFSIZE);
	    fprintf(stderr,"->->%s\n",buffer);
	    if(header_allow(buffer,sigs)==0){ /* case insensitive */
		fprintf(stderr,"header allowed\n");
	    }
	    else{
		/* printf("header not allowed\n"); */
		/* set isALlowed to 0 */
		isAllowed=0;
		fprintf(stderr,"header not allowed\n");
	    }
	}


/* request filter */
	if(isAllowed){
	    if(req_allow(r->the_request,sigs)==0){
		fprintf(stderr,"req allowed\n");
	    }
	    else{
		/* set isAllowed to 0 */
		isAllowed=0;
		fprintf(stderr,"req not allowed\n");
	    }
	}
	
	if(isAllowed){
	    
	    initialize_pages();
	    // char *s = malloc(3000);
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
	    
	    /* char url[100] = "/index.html?foo=bars&k1=21&k2=2"; */
	    
	    char *temp=my_malloc(sizeof(char)*2000);
	    char *url=strtok_r(r->the_request," ",&temp);
	    url=strtok_r(NULL," ",&temp);
	    fprintf(stderr,"url passed is ->%s\n",url);
	    
	    /* fprintf(stderr,"url -> %s\n",strtok_r(url,"?",&temp)); */
	    
	    /* fprintf(stderr,"url -> %s\n",strtok_r(NULL,"?",&temp)); */
	    char *y1=my_malloc(1000);
	    strcpy(y1,url);
	    mode m=max;
	    int drop=is_url_valid(y1,m);
	    if(drop != 0){
		isAllowed=0;
		fprintf(stderr,"MAX->Url is dropped!!!\n");
	    }else{
		fprintf(stderr,"MAX->Url Not dropped\n");
	    }
	    if(isAllowed){
		y1=my_malloc(1000);
		strcpy(y1,url);
		m=avg;
		drop=is_url_valid(y1,m);
		if(drop != 0){
		    isAllowed=0;
		    fprintf(stderr,"AVG->Url is dropped!!!\n");
		}else{
		    fprintf(stderr,"AVG->Url Not dropped\n");
		}
	    }
	    
	    if(isAllowed){
		y1=my_malloc(1000);
		strcpy(y1,url);
		m=char_val;
		drop=is_url_valid(y1,m);
		if(drop != 0){
		    isAllowed=0;
		    fprintf(stderr,"CHAR_VAL->Url is dropped!!!\n");
		}else{
		    fprintf(stderr,"CHAR_VAL->Url Not dropped\n");
		}
	    }
	    
	    if(isAllowed){
		y1=my_malloc(1000);
		strcpy(y1,url);
		m=sd;
		drop=is_url_valid(y1,m);
		if(drop != 0){
		    isAllowed=0;
		    fprintf(stderr,"SD->Url is dropped!!!\n");
		}else{
		    fprintf(stderr,"SD->Url Not dropped\n");
		}
	    }
	    
	}
	if(isAllowed)
	    fprintf(stderr,"ALLOWED\n");
	else
	    fprintf(stderr,"NOT ALLOWED\n");
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
