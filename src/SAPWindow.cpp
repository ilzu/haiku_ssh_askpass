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
	int32 parentPid;
	status_t status;
	status = GetParentProcess(&parentPid, &parentStr);
	BString processStr(B_TRANSLATE("Process ("));
	processStr << parentPid << ") \"" << parentStr << "\" " << B_TRANSLATE("requests authentication with following string:");
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
				int parentPid;
				BString processInfo;
				status = GetParentProcess(&parentPid, &processInfo);
				if(status == B_OK){
					key.SetSecondaryIdentifier(processInfo);
				}
				key.SetPassword(passwdView->Text());
				keyStore.AddKey(key);
			}
			printf("%s", passwdView->Text());
			retcode = 0;
			Lock();
			Quit();
			break;
		case MSG_SAVE_TO_KEYSTORE:
			{
				if(((SAPApplication*)be_app)->GetBoolSetting(SAPApplication::DONT_WARN_KEYSTORE) == false && useKeystore->Value() == B_CONTROL_ON){
					BAlert* alert = new BAlert(B_TRANSLATE("Security warning!"), B_TRANSLATE("Haiku keystore is currently not secure. Anyone with access to your compputer might get this password as clear text. This option is added for future and as a proof of concept. Are you sure you want to save the password to keystore?"), B_TRANSLATE("Save, and don't warn again"), B_TRANSLATE("Save"), B_TRANSLATE("Cancel"), B_WIDTH_AS_USUAL, B_WARNING_ALERT);
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
