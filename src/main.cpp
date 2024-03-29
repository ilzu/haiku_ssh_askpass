/*
 * Copyright 2024, Ilmari "ilzu" Siiteri <ilzu.siiteri@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */


#include "SAPApplication.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int retcode = 1;
const char* request;
SAPApplication::AskPassHint hint;

void PrintUsage(){
	printf("ssh_askpass is not meant to be run as user, but by ssh command wanting to authenticate.\n\nProgram takes one argument and displays it as the request, and returns the password for program that asked it.\n");
}

int main(int argc, char* argv[]){
	if(argc > 1){
		request = argv[1];
		if(strcmp(request, "--help") == 0){
			PrintUsage();
			return 0;
		}
	}
	hint = SAPApplication::HINT_DEFAULT;
	char* askpass_hint;
	if((askpass_hint = getenv("SSH_ASKPASS_PROMPT")) != NULL){
		if(strcmp(askpass_hint, "none") == 0){
			hint = SAPApplication::HINT_NONE;
		} else if(strcmp(askpass_hint, "confirm") == 0){
			hint = SAPApplication::HINT_CONFIRM;
		}
	}
	SAPApplication app;
	app.Run();
	return retcode;
}
