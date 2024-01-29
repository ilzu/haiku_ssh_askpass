/*
 * Copyright 2024, Ilmari "ilzu" Siiteri <ilzu.siiteri@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include <FindDirectory.h>
#include <Path.h>
#include <File.h>
#include <KeyStore.h>

#include <stdio.h>

#include "SAPApplication.h"
#include "SAPWindow.h"

extern const char* request;
extern int retcode;

SAPApplication::SAPApplication() : BApplication("application/x-vnd.haiku-ssh-askpass"){
	status_t status;
	window = NULL;
	LoadSettings();
	if(GetBoolSetting(USE_KEYSTORE) == true){
		BString requestInfo;
		GetRequestInfo(&requestInfo);
		BPasswordKey key;
		key.SetIdentifier(requestInfo);
		BKeyStore keyStore;
		status = keyStore.GetKey(B_KEY_TYPE_PASSWORD, requestInfo, key);
		if(status == B_OK){
			printf("%s", key.Password());
			retcode = 0;
			return;
		}
	}
	window = new SAPWindow();
}

SAPApplication::~SAPApplication(){
}

void SAPApplication::ReadyToRun(){
	if(window){
		window->Show();
	} else {
		Lock();
		Quit();
	}
}

void SAPApplication::MessageReceived(BMessage* msg){
	switch(msg->what){
		default:
			BApplication::MessageReceived(msg);
	}
}

bool SAPApplication::QuitRequested(){
	SaveSettings();
	return true;
}

void SAPApplication::SetBoolSetting(Setting setting, bool value){
	while(this->IsLocked()){
		snooze(100);
	}
	this->Lock();

	status_t status;
	bool temp;
	switch(setting){
		case USE_KEYSTORE:
			status = settings->FindBool("UseKeystore", &temp);
			if(status == B_NAME_NOT_FOUND){
				status = settings->AddBool("UseKeystore", value);
			} else {
				status = settings->ReplaceBool("UseKeystore", value);
			}
			break;
		case DONT_WARN_KEYSTORE:
			status = settings->FindBool("DontWarnKeystore", &temp);
			if(status == B_NAME_NOT_FOUND){
				status = settings->AddBool("DontWarnKeystore", value);
			} else {
				status = settings->ReplaceBool("DontWarnKeystore", value);
			}
			break;

		defaut:
			break;
	}
	status = SaveSettings();
	this->Unlock();
}

bool SAPApplication::GetBoolSetting(SAPApplication::Setting setting){
	status_t status;
	bool value = false;
	switch(setting){
		case USE_KEYSTORE:
			status = settings->FindBool("UseKeystore", &value);
			break;
		case DONT_WARN_KEYSTORE:
			status = settings->FindBool("DontWarnKeystore", &value);
			break;
		default:
			break;
	}
	return value;
}


status_t SAPApplication::LoadSettings(){
	status_t status;
	BPath settingsPath;
	settings = new BMessage();
	status = find_directory(B_USER_SETTINGS_DIRECTORY, &settingsPath);
	if(status != B_OK) return status;
	status = settingsPath.Append("ssh/ssh_askpass_settings");;
	if(status != B_OK) return status;
	BFile settingsFile(settingsPath.Path(), B_READ_ONLY);
	status = settingsFile.InitCheck();
	if(status != B_OK) return status;
	status = settings->Unflatten(&settingsFile);
	settingsFile.Unset();
	return status;
}

status_t SAPApplication::SaveSettings(){
	status_t status;
	BPath settingsPath;
	status = find_directory(B_USER_SETTINGS_DIRECTORY, &settingsPath);
	if(status != B_OK) return status;
	status = settingsPath.Append("ssh/ssh_askpass_settings");
	if(status != B_OK) return status;
	BFile settingsFile(settingsPath.Path(), B_WRITE_ONLY | B_CREATE_FILE);
	status = settingsFile.InitCheck();
	if(status != B_OK) return status;
	status = settings->Flatten(&settingsFile);
	settingsFile.Unset();
	return status;
}

status_t GetRequestInfo(BString* info){
	if(request != NULL){
		BString requestStr(request);
		int32 first = 0, last = 0;
		// path to private key
		first = requestStr.FindFirst('/');
		last = requestStr.FindLast(':');
		if(first != B_ERROR && last != B_ERROR){
			requestStr.Remove(last, requestStr.Length() - last);
			requestStr.Remove(0, first);
			info->Adopt(requestStr);
			return B_OK;
		}

		// username@host
		first = requestStr.FindFirst('(');
		last = requestStr.FindLast(')');
		if(first != B_ERROR && last != B_ERROR){
			requestStr.Remove(last, requestStr.Length() - last);
			requestStr.Remove(0, first);
			info->Adopt(requestStr);
			return B_OK;
		}
	}
	return B_ERROR;
}



