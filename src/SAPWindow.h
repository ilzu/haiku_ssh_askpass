/*
 * Copyright 2024, Ilmari "ilzu" Siiteri <ilzu.siiteri@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#ifndef SAP_WINDOW_H_
#define SAP_WINDOW_H_


#include <Window.h>

class BTextControl;
class BCheckBox;

class SAPWindow : public BWindow{
	public:
		SAPWindow();
		~SAPWindow();
		void MessageReceived(BMessage* msg);
		enum{
			MSG_AUTH = 'Auth',
			MSG_SAVE_TO_KEYSTORE = 'SKey',
		};

	private:
		BTextControl* passwdView;
		BCheckBox* useKeystore;


};


#endif // SAP_WINDOW_H_
