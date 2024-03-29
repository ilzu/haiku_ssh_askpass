/*
 * Copyright 2024, Ilmari "ilzu" Siiteri <ilzu.siiteri@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include <LayoutBuilder.h>
#include <TextControl.h>
#include <Button.h>
#include <CheckBox.h>
#include <StringView.h>
#include <Alert.h>
#include <Catalog.h>
#include <KeyStore.h>
#include <Key.h>
#include <stdio.h>
#include <stdlib.h>

#include "SAPWindow.h"
#include "SAPApplication.h"

extern int retcode;
extern const char* request;

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "MainWindow"

SAPWindow::SAPWindow() : BWindow(BRect(0, 0, 0, 0), B_TRANSLATE("SSH Authentication request"), B_TITLED_WINDOW, B_AUTO_UPDATE_SIZE_LIMITS | B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_QUIT_ON_WINDOW_CLOSE | B_SAME_POSITION_IN_ALL_WORKSPACES | B_CLOSE_ON_ESCAPE, B_ALL_WORKSPACES){
	BString parentStr;
	BString executable;
	int32 parentPid;
	status_t status;
	status = GetParentProcess(&parentPid, &parentStr, &executable);
	BString pidStr;
	pidStr << parentPid;
	BString processStr(B_TRANSLATE("Process (%pid%) \"%parent_process_cmd%\" requests authentication with following string:"));
	processStr.ReplaceAll("%parent_process_cmd%", parentStr);
	processStr.ReplaceAll("%pid%", pidStr);

	BStringView* parentView = new BStringView("parent", processStr);
	passwdView = new BTextControl(B_TRANSLATE("Please enter your SSH passphrase"), "", new BMessage(MSG_AUTH));
	if(request != NULL){
		passwdView->SetLabel(request);
	}
	passwdView->TextView()->HideTyping(true);
	BButton* cancel = new BButton(B_TRANSLATE("Cancel"), new BMessage(B_CANCEL));
	BButton* authenticate = new BButton(B_TRANSLATE("Authenticate"), new BMessage(MSG_AUTH));
	authenticate->MakeDefault(true);
	useKeystore = new BCheckBox(B_TRANSLATE("Save to keystore"), new BMessage(MSG_SAVE_TO_KEYSTORE));
	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.SetInsets(B_USE_WINDOW_INSETS)
		.Add(parentView)
		.Add(passwdView->CreateLabelLayoutItem())
		.Add(passwdView->CreateTextViewLayoutItem())
		.AddGroup(B_HORIZONTAL)
			.Add(useKeystore)
			.AddGlue()
			.Add(cancel)
			.Add(authenticate)
		.End()
	.End();

	CenterOnScreen();
	passwdView->TextView()->MakeFocus(true);

}

SAPWindow::~SAPWindow(){
}

void SAPWindow::MessageReceived(BMessage* msg){
	switch(msg->what){
		case B_CANCEL:
			Lock();
			Quit();
			break;
		case MSG_AUTH:
			if(useKeystore->Value() == B_CONTROL_ON){
				status_t status;
				((SAPApplication*)be_app)->SetBoolSetting(SAPApplication::USE_KEYSTORE, true);
				BKeyStore keyStore;
				BPasswordKey key;
				BString requestInfo;
				status = GetRequestInfo(&requestInfo);
				if(status == B_OK){
					key.SetIdentifier(requestInfo);
				} else {
					key.SetIdentifier("ssh passphrase");
				}
				int32 parentPid;
				BString processInfo;
				BString executable;
				status = GetParentProcess(&parentPid, &processInfo, &executable);
				if(status == B_OK){
					key.SetSecondaryIdentifier(executable);
				}
				key.SetPassword(passwdView->Text());

				BString keyring;
				uint32 cookie = 0;
				bool found = false;
				status = B_OK;
				while(status == B_OK){
					status = keyStore.GetNextKeyring(cookie, keyring);
					if(keyring == "ssh") found = true;
				}
				if(!found){
					status = keyStore.AddKeyring("ssh");
				}

				keyStore.AddKey("ssh", key);
			}
			printf("%s", passwdView->Text());
			retcode = 0;
			Lock();
			Quit();
			break;
		case MSG_SAVE_TO_KEYSTORE:
			{
				if(((SAPApplication*)be_app)->GetBoolSetting(SAPApplication::DONT_WARN_KEYSTORE) == false && useKeystore->Value() == B_CONTROL_ON){
					BAlert* alert = new BAlert(B_TRANSLATE("Security warning!"), B_TRANSLATE("Haiku keystore is currently not secure. Anyone with access to your computer might get this password as clear text. This option is added for future and as a proof of concept.\n\nAre you sure you want to save the password to keystore?"), B_TRANSLATE("Save, and don't warn again"), B_TRANSLATE("Save"), B_TRANSLATE("Cancel"), B_WIDTH_AS_USUAL, B_WARNING_ALERT);
					int32 reply = alert->Go();
					switch(reply){
						default:
							case 0:
								((SAPApplication*)be_app)->SetBoolSetting(SAPApplication::DONT_WARN_KEYSTORE, true);
							case 1:
								break;
							case 2:
								useKeystore->SetValue(B_CONTROL_OFF);
							break;
					}
				}
			}
			break;
		default:
			BWindow::MessageReceived(msg);
	}
}
