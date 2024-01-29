/*
 * Copyright 2024, Ilmari "ilzu" Siiteri <ilzu.siiteri@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#ifndef SAP_APPLICATION_H_
#define SAP_APPLICATION_H_


#include <Application.h>

class SAPWindow;

class SAPApplication : public BApplication{
	public:
		typedef enum Setting{
			USE_KEYSTORE,
			DONT_WARN_KEYSTORE,
		} Setting;

	public:
		SAPApplication();
		~SAPApplication();
		void ReadyToRun();
		bool QuitRequested();
		void MessageReceived(BMessage* msg);
		void SetBoolSetting(Setting setting, bool value);
		bool GetBoolSetting(Setting setting);

	private:
		SAPWindow* window;
		BMessage* settings;
		status_t LoadSettings();
		status_t SaveSettings();
};

status_t GetRequestInfo(BString* info);

#endif // SAP_APPLICATION_H_
