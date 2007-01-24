/*
 * Copyright (C) 2007 Nikolay Pultsin <geometer@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "ZLWin32Dialog.h"
#include "ZLWin32DialogContent.h"
//#include "ZLWin32Util.h"

ZLWin32Dialog::ZLWin32Dialog(const std::string &name) : myTitle(name) {
	myTab = new ZLWin32DialogContent();
	//myDialog = createWin32Dialog(name.c_str());
}

ZLWin32Dialog::~ZLWin32Dialog() {
	//destroyWin32Dialog(myDialog);
}

void ZLWin32Dialog::addButton(const std::string &text, bool accept) {
	myButtons.push_back(ButtonInfo(text, accept));
	//std::string buttonText = gtkString(text);
	//gtk_dialog_add_button(myDialog, buttonText.c_str(), accept ? GTK_RESPONSE_ACCEPT : GTK_RESPONSE_REJECT);
}

static BOOL CALLBACK DialogProc(HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_COMMAND:
			switch (wParam) {
				case IDOK:
				case IDCANCEL:
					EndDialog(hDialog, true);
					return true;
			}
	}
	return false;
}

static LPWORD lpwAlign(LPWORD p) {
	ULONG ul = (ULONG)p;
	ul += 3;
	ul >>= 2;
	ul <<= 2;
	return (LPWORD)ul; 
}

static int nCopyAnsiToWideChar(LPWORD lpWCStr, LPSTR lpAnsiln) {
	int cchAnsi = lstrlen(lpAnsiln);
	return MultiByteToWideChar(GetACP(), MB_PRECOMPOSED, lpAnsiln, cchAnsi, (LPWSTR)lpWCStr, cchAnsi) + 1;
}

static void DlgTemplate(PWORD &p, DWORD style, int items, int x, int y, int cx, int cy, LPSTR txt) {
	*p++ = LOWORD(style);
	*p++ = HIWORD(style);
	*p++ = 0;
	*p++ = 0;
	*p++ = items;
	*p++ = x;
	*p++ = y;
	*p++ = cx;
	*p++ = cy;
	*p++ = 0;
	*p++ = 0;
	int nchar = nCopyAnsiToWideChar(p, TEXT(txt));
	p += nchar;
	p = lpwAlign((LPWORD)p);
}

static void DlgItemTemplate(PWORD &p, DWORD style, int x, int y, int cx, int cy, WORD id, LPSTR classname, LPSTR txt) {
	*p++ = LOWORD(style);
	*p++ = HIWORD(style);
	*p++ = 0;
	*p++ = 0;
	*p++ = x;
	*p++ = y;
	*p++ = cx;
	*p++ = cy;
	*p++ = id;
	int nchar = nCopyAnsiToWideChar(p, TEXT(classname));
	p += nchar;
	nchar = nCopyAnsiToWideChar(p, TEXT(txt));
	p += nchar;
	*p++ = 0;
	p = lpwAlign((LPWORD)p);
}


bool ZLWin32Dialog::run() {
	WORD *pTemplate = (PWORD)LocalAlloc(LPTR, 2000);
	WORD *p = pTemplate;

	const int buttonNumber = myButtons.size();
	DlgTemplate(p, DS_3DLOOK | DS_CENTER | DS_MODALFRAME | WS_POPUPWINDOW | WS_CAPTION, buttonNumber, 20, 20, 20 + 60 * buttonNumber, 120, (char*)myTitle.c_str());
	for (int i = 0; i < buttonNumber; ++i) {
		DWORD style = (i == 0) ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON;
		style = style | WS_VISIBLE | WS_CHILD | WS_TABSTOP;
		DlgItemTemplate(p, style, 20 + 60 * i, 80, 40, 20, IDOK, (LPSTR)"button", (LPSTR)myButtons[i].first.c_str());
	}

	int code = DialogBoxIndirect(GetModuleHandle(0), (DLGTEMPLATE*)pTemplate, 0, DialogProc);
	LocalFree(LocalHandle(pTemplate));
	return code;
}