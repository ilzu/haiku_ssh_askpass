/*
 * Copyright 2024, Ilmari "ilzu" Siiteri <ilzu.siiteri@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */


#include "SAPApplication.h"
#include <stdio.h>

int retcode = 1;
const char* request;

int main(int argc, char* argv[]){
	if(argc > 1){
		request = argv[1];
	}
	SAPApplication app;
	app.Run();
	return retcode;
}
