/* opensn0w
 * An open-source jailbreaking utility.
 * Brought to you by rms, acfrazier & Maximus
 * Special thanks to iH8sn0w & MuscleNerd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include "sn0w.h"

#include "greenpois0n.h"

#ifdef _WIN32
#ifdef _GUI_ENABLE_
#include <windows.h>
#include <windowsx.h>
#include <wininet.h>
#if defined __MINGW_H
#define _WIN32_IE 0x0400
#endif
#include <commctrl.h>
#include <commdlg.h>
#include <fcntl.h>
#include <direct.h>
#include <stdio.h>

typedef enum _DFU_PHASES {
	DFU_PHASE_READY = 0,
	DFU_PHASE_POWER,
	DFU_PHASE_BOTH,
	DFU_PHASE_HOME,
	DFU_PHASE_NONE,
} DFU_PHASES;

#define DFU_TIMER_ID 1337
#endif
#endif

/* usage */

#define usage(x) \
	printf("Usage: %s [OPTIONS]\n" \
			"Jailbreak an iOS device, this includes the iPhone, iPad, iPod touch and\n" \
			"Apple TV 2G.\n\n" \
			"Currently supported devices are: %s\n" \
			"Not really supported devices are: %s\n" \
			"\n" \
			"Options:\n" \
			"   -a [boot-args]     Set device boot-args for boot.\n" \
			"   -A                 Set auto-boot. (Kick out of recovery.)\n" \
			"   -b bootlogo.img3   Use specified bootlogo img3 file during startup.\n" \
			"   -B                 Dump SecureROM to bootrom.bin (works on limera1n devices only.)\n" \
			"   -C [command]       Send command to device.\n" \
			"   -d                 Just pwn dfu mode.\n" \
			"   -D                 Dry run, still requires DFU mode.\n" \
			"   -g                 Use greenpois0n payload.\n" \
			"   -h                 Help.\n" \
			"   -i ipsw            Get necessary files from a remote IPSW.\n" \
			"   -I                 Apple TV 2G users, boot kernelcache on disk using iBoot with boot-args injected.\n" \
			"   -j                 Jailbreak.\n" \
			"   -k kernelcache     Boot using specified kernel.\n" \
			"   -n                 Do not display intro banner.\n" \
			"   -p plist           Use firmware plist\n" \
			"   -r ramdisk.dmg     Boot specified ramdisk.\n" \
			"   -R                 Just boot into pwned recovery mode.\n" \
			"   -S [file]          Send file to device.\n" \
			"   -s                 Start iRecovery recovery mode shell.\n" \
			"   -v                 Verbose mode. Useful for debugging.\n" \
			"   -X                 Download all files from plist.\n" \
			"   -Y                 Use the SHAtter exploit, but for god's sake its broken.\n" \
			"   -z                 Use raw image load payload and boot device. (Use on devices with corrupted Chip ID)\n" \
			"   -Z                 Use raw image load payload only.\n" \
			"\n" \
			"Exit status:\n" \
			"  0  if OK,\n" \
			" -1  if severe issues.\n" \
			"\n" \
			"Report %s bugs to rms@velocitylimitless.org\n" \
			"%s homepage: <http://www.opensn0w.com>\n" \
			"For complete documentation, see the UNIX manual.\n", \
			"opensn0w", \
			"s5l8930x, s5l8920x, s5l8922x", "s5l8720x, s5l8900x", "opensn0w", "opensn0w"); \
			exit(-1);

/* image names */

static const char *image_names[] = {
	"iBSS",
	"DeviceTree",
	"BatteryCharging1",
	"GlyphCharging",
	"BatteryCharging0",
	"iBoot",
	"BatteryLow0",
	"LLB",
	"iBEC",
	"KernelCache",
	"FileSystem",
	"AppleLogo",
	"UpdateRamdisk",
	"RestoreRamdisk",
	"GlyphPlugin",
	"Recovery",
	"BatteryLow1",
	"BatteryFull",
	"NeedService"
};

PartialZipProgressCallback callback = NULL;

#ifdef _WIN32

HINSTANCE currentInstance;

#ifdef _GUI_ENABLE_
char *szClassName = TEXT("WindowsApp");
HWND window = NULL;
HWND hMainMenu = NULL;
HWND nButton = NULL;
HWND eButton = NULL;
HWND hStatus0, hStatus1, hStatus2, hStatus3;
HWND title = NULL;
HWND group = NULL;
HWND copyright = NULL;
HWND progress = NULL;
HWND subtitle = NULL;
HWND reset = NULL;
HWND seconds = NULL;
HWND counter = NULL;
HWND first = NULL;
HWND second = NULL;
HWND third = NULL;
HWND fourth = NULL;
HWND enter = NULL;
HANDLE hJailbreakThread = NULL;
HANDLE hDraweButtonThread = NULL, hDrawnButtonThread = NULL;
HWND hPwnDfu, hExitRecovery, hBootTethered, hLeetHaxorMode, hTryAgain;
HWND hPwnDfuDescription, hExitRecoveryDescription, hBootTetheredDescription, hLeetHaxorModeDescription;
BOOL jbcomplete = FALSE;
INT DfuTimer = 0;
INT DfuPhase = 0;
INT DfuCountdown = 0;


LPCSTR DfuText[] = {
	"Let's start by turning off the device.",
	"Hold the sleep button.",
	"Hold the sleep and home buttons.",
	"Release sleep and hold the home button.",
	"Game over."
};

#endif
#endif


/* globals */

int opensn0w_debug_level = DBGFLTR_RELEASE;
bool verboseflag = false, dump_bootrom = false, raw_load =
    false, raw_load_exit = false;
int Img3DecryptLast = TRUE;
int UsingRamdisk = FALSE;
char *version = "UNKNOWN";
irecv_device_t device = NULL;
irecv_client_t client = NULL;
Dictionary *firmwarePatches, *patchDict, *info;
char *kernelcache = NULL, *bootlogo = NULL, *url = NULL, *plist =
    NULL, *ramdisk = NULL;
volatile int iboot = false, dry_run = false, gp_payload = false, nologo = false;
volatile int pwndfu = false, pwnrecovery = false, autoboot = false, download = false, use_shatter = false;
volatile bool jailbreaking = false;
int do_jailbreak = false;

#ifdef _WIN32
#ifdef _GUI_ENABLE_

#define CREATE_COLOR(x) (x << 8)

/*!
 * \fn void GuiSetGradient()
 * \brief Create backing gradient.
 */
void GuiSetGradient() {
	HDC gradientHDC = GetDC(window);
	TRIVERTEX vertex[2];
	GRADIENT_RECT gradientRect;
	
	vertex[0].x     = 0;
	vertex[0].y     = 0;
	vertex[0].Red   = 0xff00;
	vertex[0].Green = 0xff00;
	vertex[0].Blue  = 0xff00;
	vertex[0].Alpha = 0xff00;

	vertex[1].x     = 550 + GetSystemMetrics(SM_CXFIXEDFRAME);
	vertex[1].y     = 380 + GetSystemMetrics(SM_CYFIXEDFRAME);
	vertex[1].Red   = 0xff00;
	vertex[1].Green = 0xff00;
	vertex[1].Blue  = 0xff00;
	vertex[1].Alpha = 0x0000;
	
	gradientRect.UpperLeft = 0;
	gradientRect.LowerRight = 1;

	GradientFill(gradientHDC, vertex, 2, &gradientRect, 1, 1);
	
	vertex[0].x     = 0;
	vertex[0].y     = 380 + GetSystemMetrics(SM_CYFIXEDFRAME);
	vertex[0].Red   = 0xff00;
	vertex[0].Green = 0xff00;
	vertex[0].Blue  = 0xff00;
	vertex[0].Alpha = 0xff00;

	vertex[1].x     = 550 + GetSystemMetrics(SM_CXFIXEDFRAME);
	vertex[1].y     = 500 + GetSystemMetrics(SM_CYFIXEDFRAME);
	vertex[1].Red   = 0xdb00;
	vertex[1].Green = 0xdb00;
	vertex[1].Blue  = 0xdb00;
	vertex[1].Alpha = 0x0000;
	
	GradientFill(gradientHDC, vertex, 2, &gradientRect, 1, 1);
};

/*!
 * \fn int is_compatible(void)
 * \brief Check if device is in DFU mode.
 */
int is_compatible(void)
{
	irecv_get_device(client, &device);
	if (device == NULL) {
		DPRINT("Null device, continuing to probe.\n");
		return FALSE;
	}
	DPRINT("Identified device as %s\n", device->product);
	return TRUE;
}

/*!
 * \fn int irecv_event_cb_t_callback(irecv_client_t client, const irecv_event_t * event)
 * \brief Callback function for irecovery progress.
 *
 * \param client Device
 * \param event iRecovery Event
 */
int irecv_event_cb_t_callback(irecv_client_t client,
			      const irecv_event_t * event)
{
	SendMessage(progress, PBM_SETPOS, (int)event->progress, 0);
	return 0;
}

/*!
 * \fn void pzip_progress_callback(ZipInfo * info, CDFile * file, size_t progressamt)
 * \brief Callback function for libpartial progress.
 *
 * \param info zip information
 * \param file file information
 * \param progressamt Progress event
 */
void pzip_progress_callback(ZipInfo * info,
						    CDFile * file,
						    size_t progressamt) {
	SendMessage(progress, PBM_SETPOS, (int)progressamt, 0);
	return;
}

/*!
 * \fn DWORD win32_jailbreak(LPVOID lpThreadParameter)
 * \brief Windows jailbreak routine.
 *
 * \param lpThreadParameter Unused
 */
DWORD win32_jailbreak(LPVOID lpThreadParameter)
{
	OPENFILENAME ofn;
	char szFile[260];
	HWND hwnd = window;

	if(!autoboot) {
		while(!jailbreaking) {
			GuiUpdateJailbreakStatus();
		}
	}
	
	KillTimer(window, DfuTimer);
	
	if(!autoboot) {
		SendMessage(title, WM_SETTEXT, 0, (LPARAM) TEXT("let's jailbreak iOS"));
		SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("We identified your device as an:"));
		SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) device->colloquial_name);
		SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
		SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT("Please select a firmware bundle."));
		ShowWindow(hTryAgain, SW_HIDE);
		ShowWindow(hStatus0, SW_SHOW);
		ShowWindow(hStatus1, SW_SHOW);
		ShowWindow(hStatus2, SW_SHOW);
		ShowWindow(hStatus3, SW_SHOW);
		InvalidateRect(window, NULL, TRUE);
	} else {
		SendMessage(title, WM_SETTEXT, 0, (LPARAM) TEXT("fixing recovery loop"));
		SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Please wait..."));
		SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
		SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
		SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
		ShowWindow(hTryAgain, SW_HIDE);
		ShowWindow(hStatus0, SW_SHOW);
		ShowWindow(hStatus1, SW_SHOW);
		ShowWindow(hStatus2, SW_SHOW);
		ShowWindow(hStatus3, SW_SHOW);
		InvalidateRect(window, NULL, TRUE);
	}
	
	ZeroMemory(&ofn, sizeof(ofn));
	
	if (!autoboot || !pwndfu || !gp_payload) {
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "Property List\0*.plist\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		DPRINT("%d %d\n", autoboot, pwndfu);

		if(!autoboot && !pwndfu)
			GetOpenFileName(&ofn);
		plist = szFile;
	}
	
	callback = pzip_progress_callback;

	jailbreak();
	return 0;
}

/*!
 * \fn VOID GuiCenterWindow(HWND Window)
 * \brief Center window in screen.
 *
 * \param Window hwnd for current Window.
 */
VOID GuiCenterWindow(HWND Window)
{
	RECT ScreenRectangle;
	INT ScreenWidth;
	INT ScreenHeight;
	INT MetricWidth;
	INT MetricHeight;
	INT x, y;

	GetWindowRect(Window, &ScreenRectangle);
	ScreenWidth = ScreenRectangle.right - ScreenRectangle.left;
	ScreenHeight = ScreenRectangle.bottom - ScreenRectangle.top;

	MetricWidth = GetSystemMetrics(SM_CXSCREEN);
	MetricHeight = GetSystemMetrics(SM_CYSCREEN);

	x = (MetricWidth - ScreenWidth) >> 1;
	y = (MetricHeight - ScreenHeight) >> 1;

	MoveWindow(Window, x, y, ScreenWidth, ScreenHeight, FALSE);

	return;
}

HDC hdcMem = NULL;

/*!
 * \fn VOID GuiBoldifyLabel(HWND label, BOOL bold)
 * \brief Make text bold
 *
 * \param label hwnd for current label.
 * \param bold Enable/disable bold state.
 */
VOID GuiBoldifyLabel(HWND label, BOOL bold)
{
	SendMessage(label, WM_SETFONT,
		    (WPARAM) CreateFont(14, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL,
					FALSE, FALSE, FALSE, ANSI_CHARSET,
					OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE,
					TEXT("Segoe UI Light")), TRUE);
}

/*!
 * \fn VOID GuiUpdateDFUStatusText(VOID)
 * \brief Change DFU status text
 */
VOID GuiUpdateDFUStatusText(VOID)
{
	char text[255];
	
	if(DfuCountdown == 1) {
		snprintf(text, 255, "%d second remaining.", DfuCountdown);
	} else {
		snprintf(text, 255, "%d seconds remaining.", DfuCountdown);
	}
	
	ShowWindow(hTryAgain, SW_SHOW);
	InvalidateRect(window, NULL, TRUE);
	SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) text);
	SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) DfuText[DfuPhase]);
	
}

/*!
 * \fn VOID GuiToggleDFUTimers(BOOL show)
 * \brief Toggle DFU timer text.
 *
 * \param show Show/hide text.
 */
VOID GuiToggleDFUTimers(BOOL show)
{

	if (DfuTimer)
		KillTimer(window, DfuTimer);

	DfuTimer = 0;

	if (show) {
		ShowWindow(first, SW_SHOW);
		ShowWindow(second, SW_SHOW);
		ShowWindow(third, SW_SHOW);
		ShowWindow(fourth, SW_SHOW);
		ShowWindow(counter, SW_HIDE);
		ShowWindow(reset, SW_HIDE);
		ShowWindow(seconds, SW_HIDE);
		ShowWindow(enter, SW_HIDE);

		UpdateWindow(window);
		UpdateWindow(group);

		DfuCountdown = 4;
		DfuPhase = DFU_PHASE_READY;
		DfuTimer = SetTimer(window, DFU_TIMER_ID, 1000, NULL);
		GuiUpdateDFUStatusText();
	} else {
		DfuPhase = DFU_PHASE_NONE;

		ShowWindow(enter, SW_SHOW);
		ShowWindow(first, SW_HIDE);
		ShowWindow(second, SW_HIDE);
		ShowWindow(third, SW_HIDE);
		ShowWindow(fourth, SW_HIDE);
		ShowWindow(counter, SW_HIDE);
		ShowWindow(reset, SW_HIDE);
		ShowWindow(seconds, SW_HIDE);
	}
}

/*!
 * \fn BOOL DeviceInDFU(VOID)
 * \brief Check if device is in DFU.
 */
BOOL DeviceInDFU(VOID)
{
	return poll_device(DFU) == 0 && is_compatible() == TRUE;
}

/*!
 * \fn BOOL GuiUpdateJailbreakStatus(VOID)
 * \brief Update GUI jailbreak status.
 */
BOOL GuiUpdateJailbreakStatus(VOID)
{
	BOOL dfu = DeviceInDFU();

	if (dfu == true) {
		jailbreaking = true;
	} else {
		jailbreaking = false;
	}

	return dfu;
}

/*!
 * \fn VOID PerformJailbreak(VOID)
 * \brief Callee subroutine for jailbreak thread start.
 */
VOID PerformJailbreak(VOID)
{
	DWORD dwGenericThread;
	EnableWindow(progress, TRUE);
	ShowWindow(nButton, SW_HIDE);
	ShowWindow(eButton, SW_HIDE);
	ShowWindow(hPwnDfu, SW_HIDE);
	ShowWindow(hExitRecovery, SW_HIDE);
	ShowWindow(hBootTethered, SW_HIDE);
	ShowWindow(hLeetHaxorMode, SW_HIDE);
	ShowWindow(hPwnDfuDescription, SW_HIDE);
	ShowWindow(hExitRecoveryDescription, SW_HIDE);
	ShowWindow(hBootTetheredDescription, SW_HIDE);
	ShowWindow(hLeetHaxorModeDescription, SW_HIDE);
	ShowWindow(hMainMenu, SW_HIDE);
	InvalidateRect(window, NULL, TRUE);

	if (DfuTimer)
		KillTimer(window, DfuTimer);
		
	DfuCountdown = 4;
	DfuPhase = DFU_PHASE_READY;
	DfuTimer = SetTimer(window, DFU_TIMER_ID, 1000, NULL);

	if(autoboot == false) {
		SendMessage(title, WM_SETTEXT, 0, (LPARAM) TEXT("let's get ready"));

		ShowWindow(hStatus0, SW_SHOW);
		ShowWindow(hStatus1, SW_SHOW);
		GuiUpdateDFUStatusText();
	}
	InvalidateRect(window, NULL, TRUE);
	
	
	hJailbreakThread =
	    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) win32_jailbreak,
			 NULL, 0, &dwGenericThread);
}

/*!
 * \fn LRESULT CALLBACK GuiWindowProcedure(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
 * \brief Our window procedure.
 *
 * \param hWnd Current window.
 * \param uMessage Window message.
 * \param wParam Word parameter.
 * \param lParam Long parameter.
 */
LRESULT CALLBACK GuiWindowProcedure(HWND hWnd, UINT uMessage, WPARAM wParam,
				    LPARAM lParam)
{
	switch (uMessage) {
	case WM_CREATE:{
			HDC hDC = GetDC(window);
			hdcMem = CreateCompatibleDC(hDC);
			return 0;
		}
	case WM_DRAWITEM:{
			TCHAR Buffer[512];
			LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT)lParam;
			HFONT hFont;
			
			ZeroMemory(Buffer, 512);

			if(LOWORD(wParam) != 7)
				SetDCBrushColor(lpDIS->hDC, RGB(0,143,255));
			else
				SetDCBrushColor(lpDIS->hDC, RGB(52,111,0));
				
			SelectObject(lpDIS->hDC, GetStockObject(DC_BRUSH));
			RoundRect(lpDIS->hDC, lpDIS->rcItem.left, lpDIS->rcItem.top,
				lpDIS->rcItem.right, lpDIS->rcItem.bottom, 0, 0);
			
			if(lpDIS->itemState & ODS_SELECTED)
				DrawEdge(lpDIS->hDC, &lpDIS->rcItem,EDGE_RAISED,BF_ADJUST|BF_RECT|BF_FLAT); 
			else
				DrawEdge(lpDIS->hDC, &lpDIS->rcItem,EDGE_RAISED,BF_ADJUST|BF_RECT|BF_FLAT|BF_MONO); 
				
			GetWindowText(lpDIS->hwndItem, Buffer, 512);
			
			hFont = CreateFont(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE,
					FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
					CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE,
					TEXT("Segoe UI"));
			
			SelectObject(lpDIS->hDC, hFont);

			SetBkMode(lpDIS->hDC, TRANSPARENT);
			SetTextColor(lpDIS->hDC, RGB(255,255,255));
			
			DrawTextEx(lpDIS->hDC, Buffer, -1, &lpDIS->rcItem, DT_SINGLELINE | DT_VCENTER | DT_WORDBREAK | DT_CENTER, NULL);
			
			return TRUE;
		}
		break;
	case WM_ERASEBKGND:
		GuiSetGradient();
		return 0;
	case WM_PAINT: 
		GuiSetGradient();
		return DefWindowProc(hWnd, uMessage, wParam, lParam);
	case WM_CTLCOLORSTATIC: {
		HDC hdcStatic = (HDC) wParam;  
		
		if(GetDlgCtrlID((HWND)lParam) == 1337) {
			SetTextColor(hdcStatic, RGB(52,111,0));     
			SetBkMode (hdcStatic, TRANSPARENT); 
			return (LRESULT)GetStockObject(NULL_BRUSH); 
		}
		
		SetTextColor(hdcStatic, RGB(0,0,0));     
		SetBkMode (hdcStatic, TRANSPARENT); 
 
		return (LRESULT)GetStockObject(NULL_BRUSH); 
		}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == 1) {
			do_jailbreak = true;
			PerformJailbreak();
		} else if (LOWORD(wParam) == 4) {
			MessageBox(hWnd,
				   "By rms, acfrazier, sbingner and Maximus,\n"
				   "Special thanks to MuscleNerd and iH8sn0w.\n"
				   "\n"
				   "Parts of this program are from planetbeing's xpwn, and Chronic-Dev's doctors and syringe."
				   "\n\n"
				   "Brand new graphical user interface by Hurrian!"
				   "\n\n"
				   "This program is free software and is licensed under the GNU GPLv3, if you have paid for this program, you have been fooled!\n",
				   "Credits", 64);
		} else if (LOWORD(wParam) == 5) {
			ShowWindow(nButton, SW_HIDE);
			ShowWindow(eButton, SW_HIDE);
			ShowWindow(hPwnDfu, SW_SHOW);
			ShowWindow(hExitRecovery, SW_SHOW);
			ShowWindow(hBootTethered, SW_SHOW);
			ShowWindow(hLeetHaxorMode, SW_SHOW);
			ShowWindow(hPwnDfuDescription, SW_SHOW);
			ShowWindow(hExitRecoveryDescription, SW_SHOW);
			ShowWindow(hBootTetheredDescription, SW_SHOW);
			ShowWindow(hLeetHaxorModeDescription, SW_SHOW);
			ShowWindow(hMainMenu, SW_SHOW);
			ShowWindow(hTryAgain, SW_HIDE);
			InvalidateRect(window, NULL, TRUE);
		} else if (LOWORD(wParam) == 6) {
			ShowWindow(nButton, SW_SHOW);
			ShowWindow(eButton, SW_SHOW);
			ShowWindow(hPwnDfu, SW_HIDE);
			ShowWindow(hExitRecovery, SW_HIDE);
			ShowWindow(hBootTethered, SW_HIDE);
			ShowWindow(hLeetHaxorMode, SW_HIDE);
			ShowWindow(hPwnDfuDescription, SW_HIDE);
			ShowWindow(hExitRecoveryDescription, SW_HIDE);
			ShowWindow(hBootTetheredDescription, SW_HIDE);
			ShowWindow(hLeetHaxorModeDescription, SW_HIDE);
			ShowWindow(hMainMenu, SW_HIDE);
			ShowWindow(hTryAgain, SW_HIDE);
			InvalidateRect(window, NULL, TRUE);
		} else if (LOWORD(wParam) == 7) {
			if (DfuTimer)
				KillTimer(window, DfuTimer);
		
			DfuCountdown = 4;
			DfuPhase = DFU_PHASE_READY;
			DfuTimer = SetTimer(window, DFU_TIMER_ID, 1000, NULL);
		} else if (LOWORD(wParam) == 1000) {
			pwndfu = true;
			PerformJailbreak();
		} else if (LOWORD(wParam) == 1001) {
			autoboot = true;
			PerformJailbreak();
		} else if (LOWORD(wParam) == 1002) {
			PerformJailbreak();
		} else if (LOWORD(wParam) == 1003) {
			MessageBox(hWnd,
				   "This function is unimplemented!", "Whoops.", 64);
		}
		
	case WM_TIMER:
		if (jailbreaking == FALSE) {
			ShowWindow(counter, SW_SHOW);
			ShowWindow(seconds, SW_SHOW);
			ShowWindow(reset, SW_SHOW);
		}
		if (wParam == DFU_TIMER_ID) {
			DfuCountdown -= 1;
			
			if (DfuCountdown <= 0) {
				if (DfuPhase == DFU_PHASE_HOME
				    || DfuPhase == DFU_PHASE_NONE) {
					DfuPhase = DFU_PHASE_NONE;
					GuiToggleDFUTimers(FALSE);
				} else {
					DfuPhase += 1;
					if (DfuPhase == DFU_PHASE_POWER) {
						DfuCountdown = 2;
					} else if (DfuPhase == DFU_PHASE_BOTH) {
						DfuCountdown = 10;
					} else if (DfuPhase == DFU_PHASE_HOME) {
						DfuCountdown = 15;
					}
				}
			}

			GuiUpdateDFUStatusText();
		}
	default:
		return DefWindowProc(hWnd, uMessage, wParam, lParam);
	}
	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

/*!
 * \fn BOOL MessageLoop(BOOL blocking)
 * \brief Check for incoming blocking messages.
 *
 * \param blocking Blocking message boolean value.
 */
BOOL MessageLoop(BOOL blocking)
{
	MSG messages;

	if (!(blocking ?
	      GetMessage(&messages, NULL, 0, 0) :
	      PeekMessage(&messages, NULL, 0, 0, PM_REMOVE)
	    ))
		return FALSE;

	TranslateMessage(&messages);
	DispatchMessage(&messages);

	return TRUE;
}

/*!
 * \fn INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, INT nFunsterStil) {
 * \brief Windows GUI subroutine.
 *
 * \param hInstance Current module handle.
 * \param hPrevInstance Unused.
 * \param lpszArgument Current module arguments.
 * \param nFunsterStil Window command.
 */
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpszArgument, INT nFunsterStil)
{
	WNDCLASSEX wc;
	INITCOMMONCONTROLSEX icex;
	COLORREF bkColor, fgColor;
#ifdef MSVC_VER
	int ConsoleHandle;
	long StdoutHandle;
	FILE *stdoutNew;
	
	AllocConsole();
	StdoutHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	ConsoleHandle = _open_osfhandle(StdoutHandle, _O_TEXT);
	stdoutNew = _fdopen(ConsoleHandle, "w");
	*stdout = *stdoutNew;
	*stderr = *stdoutNew;
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	
#endif

	currentInstance = hInstance;

	/* configure xpwn endian */
	switch (endian()) {
	case ENDIAN_BIG:
		endianness = IS_BIG_ENDIAN;
		break;
	case ENDIAN_LITTLE:
		endianness = IS_LITTLE_ENDIAN;
		break;
	default:
		break;
	}
	printf("Running on %s.\n\n", endian_to_string(endian()));

	printf("version: %s\n", __SN0W_VERSION_FULL__);

	opensn0w_debug_level = DBGFLTR_MISC;
	
	/* construct gdiplus */
	GuiGdiPlusConstructor();

	/* initialize window class */

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = GuiWindowProcedure;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(1));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szClassName;
	wc.hIconSm =
	    (HICON) LoadImage(GetModuleHandle(NULL), TEXT("ID"), IMAGE_ICON, 16,
			      16, 0);
	if (!RegisterClassEx(&wc))
		return GetLastError();

	/* kill iTunes */

	if (is_process_running(L"iTunes.exe") == TRUE
	    || is_process_running(L"iTunesHelper.exe") == TRUE) {
		int c =
		    MessageBox(NULL,
			       (LPCSTR)
			       "iTunes is currently running or its helper process is running.\n\n"
			       "You will need to close iTunes to use opensn0w, would you like to kill it?",
			       (LPCSTR) "Kill iTunes?",
			       MB_YESNO | MB_ICONWARNING);
		switch (c) {
		case IDNO:
			MessageBox(NULL,
				   (LPCSTR)
				   "Please kill iTunes before using opensn0w!\n",
				   (LPCSTR) "Fatal Error", 0 | MB_ICONSTOP);
			exit(-1);
			break;
		case IDYES:
			system("taskkill /f /im iTunes.exe");
			system("taskkill /f /im iTunesHelper.exe");
		default:
			system("taskkill /f /im iTunes.exe");
			system("taskkill /f /im iTunesHelper.exe");
		}
	}

	/* initialize common controls */

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_PROGRESS_CLASS;
	InitCommonControlsEx(&icex);

	/* main */

	window =
	    CreateWindowEx(0, szClassName, TEXT("opensn0w for Windows"),
			   WS_OVERLAPPED | WS_SYSMENU | SS_OWNERDRAW ,
			   CW_USEDEFAULT, CW_USEDEFAULT,
			   550 + GetSystemMetrics(SM_CXFIXEDFRAME),
			   500 + GetSystemMetrics(SM_CYFIXEDFRAME) +
			   GetSystemMetrics(SM_CYCAPTION), HWND_DESKTOP, NULL,
			   hInstance, NULL);

	/* title */

	title =
	    CreateWindowEx(0, TEXT("STATIC"), TEXT("what would you like to do?"),
			   WS_VISIBLE | WS_CHILD | WS_EX_TRANSPARENT, 20, 30, 480, 60,
			   window, (HMENU)1337, NULL, NULL);
	
	SendMessage(title, WM_SETFONT,
		    (WPARAM) CreateFont(48, 0, 0, 0, FW_DONTCARE, FALSE,
					FALSE, FALSE, ANSI_CHARSET,
					OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE,
					TEXT("Segoe UI Light")), TRUE);

	/* subtitle */

	copyright =
	    CreateWindowEx(0, TEXT("STATIC"), "opensn0w - " "version "__SN0W_VERSION__ " - by vlo and friends",
			   WS_VISIBLE | WS_CHILD | SS_NOTIFY, 10, 460, 400, 40,
			   window, (HMENU) 4, NULL, NULL);
	SendMessage(copyright, WM_SETFONT,
		    (WPARAM) CreateFont(24, 0, 0, 0, FW_DONTCARE, FALSE, FALSE,
					FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
					CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE,
					TEXT("Segoe UI Light")), TRUE);
					
	/* progress */
	
	fgColor = RGB(0,143,255);
	bkColor = RGB(162,214,255);
	
	progress =
	    CreateWindowEx(0, PROGRESS_CLASS, NULL,
			   WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 0, 440, 550 + GetSystemMetrics(SM_CXFIXEDFRAME),
			   8, window, NULL, NULL, NULL);
	SendMessage(progress, PBM_SETPOS, 0, 0);
	SendMessage(progress, PBM_SETBARCOLOR, 0, fgColor);
	SendMessage(progress, PBM_SETBKCOLOR, 0, bkColor);
	EnableWindow(progress, FALSE);

	/* jailbreak */
	nButton =
	    CreateWindowEx(0, TEXT("BUTTON"), TEXT("jailbreak"),
			   BS_ICON | BS_FLAT | BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, 30 - GetSystemMetrics(SM_CXFIXEDFRAME), 165, 230,
			   150, window, (HMENU) 1, NULL, NULL);
	SendMessage(nButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)GuiGetIconForPng(L"artwork/jailbreak.png"));
	SendMessage(nButton, WM_SETFONT,
				(WPARAM) CreateFont(48, 0, 0, 0, FW_DONTCARE, FALSE, FALSE,
					FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
					CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE,
					TEXT("Segoe UI Light")), FALSE);
			
	eButton =
	    CreateWindowEx(0, TEXT("BUTTON"), TEXT("extras"),
			   BS_ICON | BS_FLAT | BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, 290 - GetSystemMetrics(SM_CXFIXEDFRAME), 165, 230,
			   150, window, (HMENU)5, NULL, NULL);
	SendMessage(eButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)GuiGetIconForPng(L"artwork/extras.png"));
	SendMessage(eButton, WM_SETFONT,
				(WPARAM) CreateFont(48, 0, 0, 0, FW_DONTCARE, FALSE, FALSE,
					FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
					CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE,
					TEXT("Segoe UI Light")), FALSE);

	/* advanced options pane */

	hPwnDfu =
	    CreateWindowEx(0, TEXT("BUTTON"), TEXT("Pwn DFU"),
			   BS_OWNERDRAW | BS_FLAT | BS_FLAT | BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, 30 - GetSystemMetrics(SM_CXFIXEDFRAME), 130, 150,
			   40, window, (HMENU)1000, NULL, NULL);
	SendMessage(hPwnDfu, WM_SETFONT,
				(WPARAM) CreateFont(24, 0, 0, 0, FW_DONTCARE, FALSE, FALSE,
					FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
					CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE,
					TEXT("Segoe UI Light")), FALSE);

	hPwnDfuDescription =
		CreateWindowEx(0, TEXT("STATIC"), TEXT("Prepare for restoring custom IPSWs."),
			   WS_VISIBLE | WS_CHILD | WS_EX_TRANSPARENT, 30 + 170 - GetSystemMetrics(SM_CXFIXEDFRAME), 137, 350,
			   40, window, NULL, NULL, NULL);
	SendMessage(hPwnDfuDescription, WM_SETFONT,
				(WPARAM) CreateFont(24, 0, 0, 0, FW_DONTCARE, FALSE, FALSE,
					FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
					CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE,
					TEXT("Segoe UI Light")), FALSE);	

	hExitRecovery =
	    CreateWindowEx(0, TEXT("BUTTON"), TEXT("Exit Recovery"),
			   BS_OWNERDRAW | BS_FLAT | BS_FLAT | BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, 30 - GetSystemMetrics(SM_CXFIXEDFRAME), 190, 150,
			   40, window, (HMENU)1001, NULL, NULL);
	SendMessage(hExitRecovery, WM_SETFONT,
				(WPARAM) CreateFont(24, 0, 0, 0, FW_DONTCARE, FALSE, FALSE,
					FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
					CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE,
					TEXT("Segoe UI Light")), FALSE);
		
	hExitRecoveryDescription =
		CreateWindowEx(0, TEXT("STATIC"), TEXT("Force device to exit Recovery Mode."),
			   WS_VISIBLE | WS_CHILD | WS_EX_TRANSPARENT, 30 + 170 - GetSystemMetrics(SM_CXFIXEDFRAME), 197, 350,
			   40, window, NULL, NULL, NULL);
	SendMessage(hExitRecoveryDescription, WM_SETFONT,
				(WPARAM) CreateFont(24, 0, 0, 0, FW_DONTCARE, FALSE, FALSE,
					FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
					CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE,
					TEXT("Segoe UI Light")), FALSE);	
		
	hBootTethered =
	    CreateWindowEx(0, TEXT("BUTTON"), TEXT("Boot Tethered"),
			   BS_OWNERDRAW | BS_FLAT | BS_FLAT | BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, 30 - GetSystemMetrics(SM_CXFIXEDFRAME), 250, 150,
			   40, window, (HMENU)1002, NULL, NULL);
	SendMessage(hBootTethered, WM_SETFONT,
				(WPARAM) CreateFont(24, 0, 0, 0, FW_DONTCARE, FALSE, FALSE,
					FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
					CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE,
					TEXT("Segoe UI Light")), FALSE);
					
	hBootTetheredDescription =
		CreateWindowEx(0, TEXT("STATIC"), TEXT("Just boot tethered right now."),
			   WS_VISIBLE | WS_CHILD | WS_EX_TRANSPARENT, 30 + 170 - GetSystemMetrics(SM_CXFIXEDFRAME), 257, 350,
			   40, window, NULL, NULL, NULL);
	SendMessage(hBootTetheredDescription, WM_SETFONT,
				(WPARAM) CreateFont(24, 0, 0, 0, FW_DONTCARE, FALSE, FALSE,
					FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
					CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE,
					TEXT("Segoe UI Light")), FALSE);	

	hLeetHaxorMode =
	    CreateWindowEx(0, TEXT("BUTTON"), TEXT("1337 h4x0r mode"),
			   BS_OWNERDRAW | BS_FLAT | BS_FLAT | BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, 30 - GetSystemMetrics(SM_CXFIXEDFRAME), 310, 150,
			   40, window, (HMENU)1003, NULL, NULL);
	SendMessage(hLeetHaxorMode, WM_SETFONT,
				(WPARAM) CreateFont(24, 0, 0, 0, FW_DONTCARE, FALSE, FALSE,
					FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
					CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE,
					TEXT("Segoe UI Light")), FALSE);	

	hLeetHaxorModeDescription =
		CreateWindowEx(0, TEXT("STATIC"), TEXT("Enable uber leet h4x0r mode."),
			   WS_VISIBLE | WS_CHILD | WS_EX_TRANSPARENT, 30 + 170 - GetSystemMetrics(SM_CXFIXEDFRAME), 317, 350,
			   40, window, NULL, NULL, NULL);
	SendMessage(hLeetHaxorModeDescription, WM_SETFONT,
				(WPARAM) CreateFont(24, 0, 0, 0, FW_DONTCARE, FALSE, FALSE,
					FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
					CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE,
					TEXT("Segoe UI Light")), FALSE);	

	hMainMenu =
	    CreateWindowEx(0, TEXT("BUTTON"), TEXT("Main Menu"),
			   BS_OWNERDRAW | BS_FLAT | BS_FLAT | BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, 370 - GetSystemMetrics(SM_CXFIXEDFRAME), 375, 150,
			   40, window, (HMENU)6, NULL, NULL);
	SendMessage(hMainMenu, WM_SETFONT,
				(WPARAM) CreateFont(24, 0, 0, 0, FW_DONTCARE, FALSE, FALSE,
					FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
					CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE,
					TEXT("Segoe UI Light")), FALSE);	


	hTryAgain =
	    CreateWindowEx(0, TEXT("BUTTON"), TEXT("Reset Timer"),
			   BS_OWNERDRAW | BS_FLAT | BS_FLAT | BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, 370 - GetSystemMetrics(SM_CXFIXEDFRAME), 375, 150,
			   40, window, (HMENU)7, NULL, NULL);
	SendMessage(hTryAgain, WM_SETFONT,
				(WPARAM) CreateFont(24, 0, 0, 0, FW_DONTCARE, FALSE, FALSE,
					FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
					CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE,
					TEXT("Segoe UI Light")), FALSE);	
	/* description lines */
	
	hStatus0 =
	    CreateWindowEx(0, TEXT("STATIC"), TEXT("heading line 0"),
			   SS_CENTER | WS_VISIBLE | WS_CHILD | WS_EX_TRANSPARENT, 0, 140, 550, 60,
			   window, NULL, NULL, NULL);
	
	SendMessage(hStatus0, WM_SETFONT,
		    (WPARAM) CreateFont(48, 0, 0, 0, FW_DONTCARE, FALSE,
					FALSE, FALSE, ANSI_CHARSET,
					OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE,
					TEXT("Segoe UI Light")), TRUE);

	hStatus1 =
	    CreateWindowEx(0, TEXT("STATIC"), TEXT("status line 1"),
			   SS_CENTER | WS_VISIBLE | WS_CHILD | WS_EX_TRANSPARENT, 0, 200, 550, 40,
			   window, NULL, NULL, NULL);
	
	SendMessage(hStatus1, WM_SETFONT,
		    (WPARAM) CreateFont(36, 0, 0, 0, FW_DONTCARE, FALSE,
					FALSE, FALSE, ANSI_CHARSET,
					OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE,
					TEXT("Segoe UI Light")), TRUE);
					
	hStatus2 =
	    CreateWindowEx(0, TEXT("STATIC"), TEXT("status line 2"),
			   SS_CENTER | WS_VISIBLE | WS_CHILD | WS_EX_TRANSPARENT, 0, 240, 550, 40,
			   window, NULL, NULL, NULL);
	
	SendMessage(hStatus2, WM_SETFONT,
		    (WPARAM) CreateFont(36, 0, 0, 0, FW_DONTCARE, FALSE,
					FALSE, FALSE, ANSI_CHARSET,
					OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE,
					TEXT("Segoe UI Light")), TRUE);
					
	hStatus3 =
	    CreateWindowEx(0, TEXT("STATIC"), TEXT("status line 3"),
			   SS_CENTER | WS_VISIBLE | WS_CHILD | WS_EX_TRANSPARENT, 0, 280, 550, 40,
			   window, NULL, NULL, NULL);
	
	SendMessage(hStatus3, WM_SETFONT,
		    (WPARAM) CreateFont(36, 0, 0, 0, FW_DONTCARE, FALSE,
					FALSE, FALSE, ANSI_CHARSET,
					OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE,
					TEXT("Segoe UI Light")), TRUE);
					
	ShowWindow(hPwnDfu, SW_HIDE);
	ShowWindow(hExitRecovery, SW_HIDE);
	ShowWindow(hBootTethered, SW_HIDE);
	ShowWindow(hLeetHaxorMode, SW_HIDE);
	ShowWindow(hPwnDfuDescription, SW_HIDE);
	ShowWindow(hExitRecoveryDescription, SW_HIDE);
	ShowWindow(hBootTetheredDescription, SW_HIDE);
	ShowWindow(hLeetHaxorModeDescription, SW_HIDE);
	ShowWindow(hMainMenu, SW_HIDE);
	ShowWindow(hTryAgain, SW_HIDE);
	ShowWindow(hStatus0, SW_HIDE);
	ShowWindow(hStatus1, SW_HIDE);
	ShowWindow(hStatus2, SW_HIDE);
	ShowWindow(hStatus3, SW_HIDE);
					
	GuiToggleDFUTimers(FALSE);
	GuiCenterWindow(window);
	
	ShowWindow(window, nFunsterStil);

	while (MessageLoop(TRUE)) ;

	GuiGdiPlusDeconstructor();
	return 0;
}

#endif
#endif

/*!
 * \fn void boot_args_process(char *args)
 * \brief Process iPhone boot arguments for iBoot injection.
 *
 * \param args Arguments for iBoot
 */

void boot_args_process(char *args)
{
	char buffer[39];

	if (strlen(args) > 39) {
		printf
		    ("Boot-args is too long. Please shorten to 39 characters or less.\n");
		exit(-1);
	}

	/* pad */
	snprintf(buffer, 39, "%-39s", args);

	DPRINT("Booting with boot-args \"%s\"\n", buffer);

	memcpy(iBEC_bootargs.patched, buffer, 39);
}

/*!
 * \fn bool file_exists(const char *fileName)
 * \brief Check if file exists.
 *
 * \param fileName filename.
 */

bool file_exists(const char *fileName)
{
	struct stat buf;
	return !stat(fileName, &buf);
}

/*!
 * \fn char *mode_to_string(int mode)
 * \brief Required \a mode to string.
 *
 * \param mode Required mode (DFU/iBoot)
 */

char *mode_to_string(int mode)
{
	switch (mode) {
	case DFU:
		return "DFU";
	case RECOVERYMODE:
		return "Recovery/iBoot";
	default:
		return "UNKNOWN";
	}
}

/*!
 * \fn int poll_device(int mode)
 * \brief Poll device for specified \a mode.
 *
 * \param mode Required mode (DFU/iBoot).
 */

int poll_device(int mode)
{
	irecv_error_t err;
	static int ___try;	/* actually a msvc keyword, __try */

	err = irecv_open(&client);
	if (err != IRECV_E_SUCCESS) {
		STATUS("Connect the device in %s mode. [%u]\r",
		       mode_to_string(mode), ___try);
		fflush(stdout);
		___try++;
		return 1;
	}

	switch (client->mode) {
	case kDfuMode2:
	case kDfuMode:
		if (mode == DFU) {
			return 0;
		}
	case kRecoveryMode2:
		if (mode == RECOVERYMODE) {
			return 0;
		}
	default:
		STATUS("Connect the device in %s mode. [%u]\r",
		       mode_to_string(mode), ___try);
		irecv_close(client);
		___try++;
		return 1;
	}

	return 0;
}

/*!
 * \fn int send_command(char *name)
 * \brief Send \a name as an iBoot command to device.
 *
 * \param name iBoot command.
 */

int send_command(char *name)
{
	irecv_error_t err;

	if(nologo == false) {
		printf("opensn0w, an open source jailbreaking program.\n"
		       "version: " __SN0W_VERSION_FULL__ "\n\n"
		       "Compiled on: " __DATE__ " " __TIME__ "\n\n");
		print_configuration();
	}

	DPRINT("Initializing libirecovery...\n");
	irecv_init();

#ifndef __APPLE__
	irecv_set_debug_level(3);
#endif

	while (1) {
		err = irecv_open(&client);
		if (err != IRECV_E_SUCCESS) {
			STATUS("Connect the device. err %d\n", err);
			sleep(1);
		} else if (err == IRECV_E_SUCCESS) {
			break;
		}
	};

	if (!client) {
		FATAL("Null client!\n");
	}

	STATUS("[*] Sending command \"%s\" to iBoot...\n", name);
	irecv_send_command(client, name);

	exit(err);
	return err;
}

/*!
 * \fn int send_filename(char *name)
 * \brief Send \a name as file to device.
 *
 * \param name iBoot file.
 */

int send_file(char *name)
{
	irecv_error_t err;

	if(nologo == false) {
		printf("opensn0w, an open source jailbreaking program.\n"
		       "version: " __SN0W_VERSION_FULL__ "\n\n"
		       "Compiled on: " __DATE__ " " __TIME__ "\n\n");
		print_configuration();
	}


	DPRINT("Initializing libirecovery...\n");
	irecv_init();

#ifndef __APPLE__
	irecv_set_debug_level(3);
#endif

	while (1) {
		err = irecv_open(&client);
		if (err != IRECV_E_SUCCESS) {
			STATUS("Connect the device. err %d\n", err);
			sleep(1);
		} else if (err == IRECV_E_SUCCESS) {
			break;
		}
	};

	if (!client) {
		FATAL("Null client!\n");
	}

	if (client->mode != kDfuMode) {
		err = irecv_send_file(client, name, 0);
	} else {
		err = irecv_send_file(client, name, 1);
	}

	if (err != IRECV_E_SUCCESS) {
		FATAL("%s\n", irecv_strerror(err));
	}

	exit(err);
	return err;
}

/*!
 * \fn int fetch_image(const char *path, const char *output)
 * \brief Fetch a specified image from the Internet.
 *
 * \param path Filename in zip.
 * \param output Output filename.
 */

int fetch_image(const char *path, const char *output)
{
	DPRINT("Fetching %s...\n", path);
	STATUS("[*] Fetching %s...\n", path);
	if (download_file_from_zip(device->url, path, output, callback) != 0) {
		ERR("Unable to fetch %s\n", path);
		return -1;
	}

	return 0;
}

/*!
 * \fn size_t writeData(void *ptr, size_t size, size_t mem, FILE * stream)
 * \brief Wrapper for fwrite.
 *
 * \param ptr Buffer for data.
 * \param size Size of written data.
 * \param mem Number of members of written data.
 * \param stream File descriptor structure.
 */

size_t writeData(void *ptr, size_t size, size_t mem, FILE * stream)
{
	size_t written;
	written = fwrite(ptr, size, mem, stream);
	return written;
}

/*!
 * \fn int download_ALL_the_files()
 * \brief Download ALL the files
 */
int download_ALL_the_files(firmware Firmware)
{
	char path[255];
	struct stat buf;
	char *buffer;
	char *filename;
	firmware_item item;
	int i;

	for (i = 0; i < NEEDSERVICE; i++) {
 begin:
		item = Firmware.item[i];
		if (item.name != NULL) {

			filename = strrchr(item.name, '/');

			if (filename == NULL)
				filename = item.name;
			else
				filename++;

			if (strstr(filename, "dmg")) {
				i++;
				goto begin;
			}

			buffer =
			    malloc(strlen(filename) + 10 + strlen(version));
			if (!buffer) {
				ERR("Cannot allocate memory\n");
				return -1;
			}
			memset(buffer, 0,
			       strlen(filename) + 10 + strlen(version));

			snprintf(buffer,
				 strlen(filename) + 10 + strlen(version),
				 "%s_%s", filename, version);

			DPRINT("Checking if %s already exists\n", buffer);

			memset(path, 0, 255);

			if (stat(buffer, &buf) != 0) {
				if (fetch_image(item.name, buffer) < 0) {
					ERR("Unable to download image\n");
					goto begin;
				}
			}

			free(buffer);
		}
	}
	return 0;
}

/*!
 * \fn int upload_image(firmware_item item, int mode, int patch, int userprovided)
 * \brief Upload image to device based on \a item and \a patch it if necessary.
 *
 * \param item Firmware item to be uploaded.
 * \param mode Notify DFU mode if upload should be finished or not.
 * \param patch Patch file.
 * \param userprovided User provided.
 */

int upload_image(firmware_item item, int mode, int patch, int userprovided)
{
	char path[255];
	struct stat buf;
	irecv_error_t error = IRECV_E_SUCCESS;
	char *buffer;
	char *filename = strrchr(item.name, '/');
	
	if(!item.name) {
		DPRINT("Failing upload of image as filename is NULL.\n");
		return -1;
	}

	filename = strrchr(item.name, '/');

	if (filename == NULL)
		filename = item.name;
	else
		filename++;

	buffer = malloc(strlen(filename) + 10 + strlen(version));
	if (!buffer) {
		ERR("Cannot allocate memory\n");
		return -1;
	}
	memset(buffer, 0, strlen(filename) + 10 + strlen(version));

	snprintf(buffer, strlen(filename) + 10 + strlen(version), "%s_%s",
		 filename, version);

	if (userprovided)
		snprintf(buffer, strlen(filename) + 10 + strlen(version), "%s",
			 filename);

	DPRINT("Checking if %s already exists\n", buffer);

	memset(path, 0, 255);

	if (stat(buffer, &buf) != 0) {
#ifdef _WIN32
#ifdef _GUI_ENABLE_
		SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Downloading from Apple..."));
		SendMessage(progress, PBM_SETPOS, 0, 0);
		SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) " ");
		InvalidateRect(window, NULL, TRUE);
#endif
#endif
		if (fetch_image(item.name, buffer) < 0) {
			ERR("Unable to upload DFU image\n");
			return -1;
		}
	}

#ifndef _EMBEDDED_ENABLE_
	if (patch && !userprovided)
		patch_file(buffer);
#else
#warning Make sure you have your pre-pwned files before (both .dec and .pwn!)
#endif

#ifdef _WIN32
#ifdef _GUI_ENABLE_
		SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Patching..."));
		SendMessage(progress, PBM_SETPOS, 0, 0);
		SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) " ");
		InvalidateRect(window, NULL, TRUE);
#endif
#endif

	if (patch && !userprovided) {
		snprintf(buffer, strlen(filename) + 10 + strlen(version),
			 "%s_%s.pwn", filename, version);
	}

	if (raw_load == true && strcasestr(item.name, "iBSS")) {
		snprintf(buffer, strlen(filename) + 10 + strlen(version),
			 "%s_%s.dec", filename, version);
	}

	DPRINT("Uploading %s to device\n", buffer);
	
#ifdef _WIN32
#ifdef _GUI_ENABLE_
		SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Uploading..."));
		SendMessage(progress, PBM_SETPOS, 0, 0);
		SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) " ");
		InvalidateRect(window, NULL, TRUE);
#endif
#endif

	if(!dry_run) {
		if (client->mode != kDfuMode)
			error = irecv_send_file(client, buffer, 0);
		else
			error = irecv_send_file(client, buffer, 1);
	}

	if (error != IRECV_E_SUCCESS) {
		ERR("%s\n", irecv_strerror(error));
		return -1;
	}
	free(buffer);
	return 0;
}

/*!
 * \fn void print_configuration(void)
 * \brief Print system configuration.
 */

void print_configuration(void)
{
	printf("Running on %s.\n\n", endian_to_string(endian()));
}

/*
 * win32 support, from wine
 */

#ifdef _WIN32

#include <psapi.h>

struct pid_close_info {
	DWORD pid;
	BOOL found;
};

static BOOL CALLBACK pid_enum_proc(HWND hwnd, LPARAM lParam)
{
	struct pid_close_info *info = (struct pid_close_info *)lParam;
	DWORD hwnd_pid;

	GetWindowThreadProcessId(hwnd, &hwnd_pid);

	if (hwnd_pid == info->pid) {
		//PostMessageW(hwnd, WM_CLOSE, 0, 0);
		info->found = TRUE;
	}

	return TRUE;
}

static DWORD *enumerate_processes(DWORD * list_count)
{
	DWORD *pid_list, alloc_bytes = 1024 * sizeof(*pid_list), needed_bytes;

	pid_list = HeapAlloc(GetProcessHeap(), 0, alloc_bytes);
	if (!pid_list)
		return NULL;

	for (;;) {
		DWORD *realloc_list;

		if (!EnumProcesses(pid_list, alloc_bytes, &needed_bytes)) {
			HeapFree(GetProcessHeap(), 0, pid_list);
			return NULL;
		}

		/* EnumProcesses can't signal an insufficient buffer condition, so the
		 * only way to possibly determine whether a larger buffer is required
		 * is to see whether the written number of bytes is the same as the
		 * buffer size. If so, the buffer will be reallocated to twice the
		 * size. */
		if (alloc_bytes != needed_bytes)
			break;

		alloc_bytes *= 2;
		realloc_list =
		    HeapReAlloc(GetProcessHeap(), 0, pid_list, alloc_bytes);
		if (!realloc_list) {
			HeapFree(GetProcessHeap(), 0, pid_list);
			return NULL;
		}
		pid_list = realloc_list;
	}

	*list_count = needed_bytes / sizeof(*pid_list);
	return pid_list;
}

static BOOL get_process_name_from_pid(DWORD pid, WCHAR * buf, DWORD chars)
{
	HANDLE process;
	HMODULE module;
	DWORD required_size;

	process =
	    OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE,
			pid);
	if (!process)
		return FALSE;

	if (!EnumProcessModules
	    (process, &module, sizeof(module), &required_size)) {
		CloseHandle(process);
		return FALSE;
	}

	if (!GetModuleBaseNameW(process, module, buf, chars)) {
		CloseHandle(process);
		return FALSE;
	}

	CloseHandle(process);
	return TRUE;
}

bool is_process_running(WCHAR * process_name)
{
	DWORD *pid_list, pid_list_size;
	DWORD index;
	BOOL found_process = FALSE;
	pid_list = enumerate_processes(&pid_list_size);

	if (!pid_list) {
		return FALSE;
	}

	for (index = 0; index < pid_list_size; index++) {
		WCHAR process_name2[MAX_PATH];

		if (get_process_name_from_pid
		    (pid_list[index], process_name2, MAX_PATH) == TRUE
		    && !wcscasecmp(process_name2, process_name)) {
			struct pid_close_info info = { pid_list[index] };

			found_process = TRUE;
			EnumWindows(pid_enum_proc, (LPARAM) & info);
		}

	}
	return found_process;
}

#endif

void jailbreak()
{
	int i;
	char *processedname;
	int userprovided = 0;
	irecv_error_t err = IRECV_E_SUCCESS;
	AbstractFile *plistFile;
	Dictionary *bundle;
	firmware Firmware;
	Dictionary *temporaryDict;
	StringValue *urlKey = NULL;

	jailbreaking = true;

	/* kill iTunes */
#ifdef _WIN32
#ifndef _GUI_ENABLE_
	if (is_process_running(L"iTunes.exe") == TRUE
	    || is_process_running(L"iTunesHelper.exe") == TRUE) {
		char c;
		STATUS
		    ("[!] iTunes is currently running or its helper process is running.\n");
		STATUS
		    ("[!] WARNING: You will need to close iTunes to use opensn0w, would you like to kill it? [Y/n] ");
		c = tolower(getchar());
		switch (c) {
		case 'n':
			FATAL("Please kill iTunes before using opensn0w!\n");
			break;
		case 'y':
			system("taskkill /f /im iTunes.exe");
			system("taskkill /f /im iTunesHelper.exe");
		default:
			system("taskkill /f /im iTunes.exe");
			system("taskkill /f /im iTunesHelper.exe");
		}
	}
#else
	if (is_process_running(L"iTunes.exe") == TRUE
	    || is_process_running(L"iTunesHelper.exe") == TRUE) {
		int c =
		    MessageBox(NULL,
			       (LPCSTR)
			       "iTunes is currently running or its helper process is running.\n\n"
			       "You will need to close iTunes to use opensn0w, would you like to kill it?",
			       (LPCSTR) "Kill iTunes?",
			       MB_YESNO | MB_ICONWARNING);
		switch (c) {
		case IDNO:
			MessageBox(NULL,
				   (LPCSTR)
				   "Please kill iTunes before using opensn0w!\n",
				   (LPCSTR) "Fatal Error", 0 | MB_ICONSTOP);
			exit(-1);
			break;
		case IDYES:
			system("taskkill /f /im iTunes.exe");
			system("taskkill /f /im iTunesHelper.exe");
		default:
			system("taskkill /f /im iTunes.exe");
			system("taskkill /f /im iTunesHelper.exe");
		}
	}
#endif
#endif

	/* Do stuff */
	if (autoboot) {
		DPRINT("Initializing libirecovery\n");
		irecv_init();

#ifndef _NDEBUG_
		irecv_set_debug_level(3);
#endif

		/* Poll for DFU mode */
		while (poll_device(RECOVERYMODE)) {
			sleep(1);
		}

#ifdef _WIN32
#ifdef _GUI_ENABLE_
		irecv_event_subscribe(client, IRECV_PROGRESS,
				      irecv_event_cb_t_callback, NULL);
#endif
#endif

		/* Check the device */
		err = irecv_get_device(client, &device);
		if (device == NULL || device->index == DEVICE_UNKNOWN) {
#ifdef _WIN32
			STATUS
			    ("[!] Make sure your device is in the appropriate mode, and that iTunes is installed and closed!\n");
#endif
			FATAL("Bad device. errno %d\n", err);
		}
		STATUS("[*] Device found.\n");

		DPRINT("Device found: name: %s, processor s5l%dxsi\n",
		       device->product, device->chip_id);
		DPRINT("iBoot information: %s\n", client->serial);
#ifdef _WIN32
#ifdef _GUI_ENABLE_
		SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Setting environment variables..."));
		SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
		SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
		SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
		SendMessage(progress, PBM_SETPOS, 0, 0);
		InvalidateRect(window, NULL, TRUE);
#endif
#endif

		STATUS("[*] Fixing recovery loop...\n");
		irecv_send_command(client, "setenv auto-boot true");
		irecv_send_command(client, "saveenv");
#ifndef _WIN32
		client = irecv_reconnect(client, 2);
#else
		client = irecv_reconnect(client, 5);
#endif
#ifdef _WIN32
#ifdef _GUI_ENABLE_
		SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Rebooting..."));
		SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
		SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
		SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
		SendMessage(progress, PBM_SETPOS, 0, 0);
		InvalidateRect(window, NULL, TRUE);
#endif
#endif
		irecv_send_command(client, "reboot");
#ifdef _WIN32
#ifdef _GUI_ENABLE_
		SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Done!"));
		SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
		SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT("Your device has now left recovery mode."));
		SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
		SendMessage(progress, PBM_SETPOS, 0, 0);
		InvalidateRect(window, NULL, TRUE);
		return;
#endif
#endif

		STATUS("[*] Operation completed.\n");
#ifndef _GUI_ENABLE_
		exit(0);
#endif
	}

	if (dump_bootrom || raw_load_exit || gp_payload) {
		/* i know, hacky */
		goto actually_do_stuff;
	}

	if ((plistFile =
	     createAbstractFileFromFile(fopen(plist, "rb"))) != NULL) {
		plist = (char *)malloc(plistFile->getLength(plistFile));
		plistFile->read(plistFile, plist,
				plistFile->getLength(plistFile));
		plistFile->close(plistFile);
		info = createRoot(plist);
	} else if ((pwndfu == false) &&
		   (plistFile =
		    createAbstractFileFromFile(fopen(plist, "rb"))) == NULL) {
		usage();
		FATAL("plist must be specified in this mode!\n\n");
	}

	/* Initialize Firmware structure */
	memset(&Firmware, 0, sizeof(firmware));
	Firmware.items = sizeof(image_names) / sizeof(char *);
	Firmware.item = malloc(Firmware.items * sizeof(firmware_item));

	if (Firmware.item == NULL) {
		FATAL("Unable to allocate memory for decryption keys!\n");
	}

	memset(Firmware.item, 0, Firmware.items * sizeof(firmware_item));

	bundle = (Dictionary *) getValueByKey(info, "FirmwareKeys");
	if (bundle != NULL) {
		for (i = 0; i < Firmware.items; i++) {
			Dictionary *entry = (Dictionary *) getValueByKey(bundle,
									 image_names
									 [i]);
			if (entry != NULL) {
				StringValue *key = NULL, *iv = NULL, *name =
				    NULL, *vfkey = NULL;

				key =
				    (StringValue *) getValueByKey(entry, "Key");
				iv = (StringValue *) getValueByKey(entry, "IV");
				name =
				    (StringValue *) getValueByKey(entry,
								  "FileName");
				vfkey =
				    (StringValue *) getValueByKey(entry,
								  "VFDecryptKey");

				if (key)
					Firmware.item[i].key = key->value;

				if (iv)
					Firmware.item[i].iv = iv->value;

				if (name)
					Firmware.item[i].name = name->value;

				if (vfkey)
					Firmware.item[i].vfkey = vfkey->value;

				DPRINT("[plist] (%s %s %s %s) [%s %d]\n",
				       Firmware.item[i].key,
				       Firmware.item[i].iv,
				       Firmware.item[i].name,
				       Firmware.item[i].vfkey,
				       image_names[i], i);

			}
		}
	}

 actually_do_stuff:
	/* to be done */
	DPRINT("Initializing libirecovery\n");
	irecv_init();

#ifndef __APPLE__
	irecv_set_debug_level(3);
#endif

	/* Poll for DFU mode */
	while (poll_device(DFU)) {
		sleep(1);
	}

#ifdef _WIN32
#ifdef _GUI_ENABLE_
	irecv_event_subscribe(client, IRECV_PROGRESS, irecv_event_cb_t_callback,
			      NULL);
#endif
#endif

#ifdef _GUI_ENABLE_
	SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Preparing..."));
	SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(progress, PBM_SETPOS, 0, 0);
	InvalidateRect(window, NULL, TRUE);
#endif
	/* Check the device */
	if (strcmp(client->serial, "89000000000001")) {
		err = irecv_get_device(client, &device);
		if (device == NULL || device->index == DEVICE_UNKNOWN) {
#ifdef _WIN32
			STATUS
			    ("[!] Make sure your device is in DFU mode, and that iTunes is installed and closed!\n");
#endif
			FATAL("Bad device. errno %d\n", err);
		}
		STATUS("[*] Device found.\n");
		DPRINT("Device found: name: %s, processor s5l%dxsi\n",
		       device->product, device->chip_id);
		DPRINT("iBoot information: %s\n", client->serial);

		if (device->chip_id == 8900) {
			FATAL
			    ("It looks like your device is in WTF mode already! Get into DFU mode.\n");
		}

	} else {
		DPRINT("Using 8900 device descriptor.\n");
		STATUS("[*] S5L8900XRB device found.\n");

		device = malloc(sizeof(irecv_device));
		memset(device, 0, sizeof(irecv_device));
		device->chip_id = 8900;
		device->product = "s5l8900xall";
	}

	temporaryDict =
	    (Dictionary *) getValueByKey(info, "FirmwareInfo");

	if (temporaryDict != NULL)
		urlKey = (StringValue *) getValueByKey(temporaryDict, "URL");
		
		
	if (urlKey != NULL) {
		char *p = NULL, dup[256];
		int len;

		memset(dup, 0, 256);

		if(!gp_payload)
			device->url = urlKey->value;
		if (temporaryDict != NULL)
			urlKey =
			    (StringValue *) getValueByKey(temporaryDict, "URL");
		if (urlKey != NULL)
			p = urlKey->value;
		if (!p)
			goto out;

		p = strstr(p, device->product);
		if (!p)
			goto out;

		len = strlen(p);
		if (len <= 0)
			goto out;

		strncpy(dup, p, len - sizeof("Restore.ipsw"));
		version = strdup(dup);

	}
 out:
	if (url) {
		char *p = NULL, dup[256];
		int len;

		memset(dup, 0, 256);
		processedname = malloc(strlen(url) + sizeof("file://"));
		if (!processedname) {
			printf("Could not allocate memory\n");
			exit(-1);
		}
		memset(processedname, 0, strlen(url) + sizeof("file://"));
		snprintf(processedname, strlen(url) + sizeof("file://"),
			 "file://%s", url);
		device->url = processedname;
		p = processedname;
		
		p = strstr(p, device->product);
		if (!p)
			goto out;

		len = strlen(p);
		if (len <= 0)
			goto out;

		strncpy(dup, p, len - sizeof("Restore.ipsw"));
		version = strdup(dup);
	}

	if (download) {
		download_ALL_the_files(Firmware);
		exit(0);
	}

	STATUS("[*] Exploiting bootrom...\n");
#ifdef _WIN32
#ifdef _GUI_ENABLE_
	SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Exploiting boot ROM..."));
	SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(progress, PBM_SETPOS, 0, 0);
	InvalidateRect(window, NULL, TRUE);
#endif
#endif
	if(!dry_run) {
	/* What jailbreak exploit is this thing capable of? */
	if ((device->chip_id == 8930 || device->chip_id == 8922
	    || device->chip_id == 8920) && !use_shatter) {
		DPRINT
		    ("This device is compatible with the limera1n exploit. Sending.\n");
		err = limera1n();
		if (err) {
			FATAL("Error during limera1ning.\n");
		}
		if (pwndfu == true) {
			printf
			    ("bootrom is owned. feel free to restore custom ipsws.\n");
#ifdef _GUI_ENABLE_
#ifdef _WIN32
			SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Done!"));
			SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
			SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT("Feel free to restore custom IPSWs in iTunes."));
			SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
			SendMessage(progress, PBM_SETPOS, 0, 0);
			InvalidateRect(window, NULL, TRUE);
			return;
#endif
#else
			exit(0);
#endif
		}
	} else if (device->chip_id == 8720 && !use_shatter) {
		DPRINT
		    ("This device is compatible with the steaks4uce exploit. Sending.\n");
		err = steaks4uce();
		if (err) {
			FATAL("Error during steaks4uceing.\n");
		}
		if (pwndfu == true) {
			printf
			    ("bootrom is owned. feel free to restore custom ipsws.\n");
#ifdef _GUI_ENABLE_
#ifdef _WIN32
			SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Done!"));
			SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
			SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT("Feel free to restore custom IPSWs in iTunes."));
			SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
			SendMessage(progress, PBM_SETPOS, 0, 0);
			InvalidateRect(window, NULL, TRUE);
			return;
#endif
#else
			exit(0);
#endif
		}
	} else if (device->chip_id == 8900 && !use_shatter) {
		DPRINT
		    ("This device is compatible with the pwnage2 exploit. Sending.\n");
		err = pwnage2();
		if (err) {
			ERR("Error during pwnage2ing.\n");
		}
		if (pwndfu == true) {
			printf
			    ("bootrom is owned. feel free to restore custom ipsws.\n");
#ifdef _GUI_ENABLE_
#ifdef _WIN32
			SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Done!"));
			SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
			SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT("Feel free to restore custom IPSWs in iTunes."));
			SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
			SendMessage(progress, PBM_SETPOS, 0, 0);
			InvalidateRect(window, NULL, TRUE);
			return;
#endif
#else
			exit(0);
#endif
		}
	} else if(use_shatter) {
		DPRINT("Using SHAtter because you told me to.\n");
		raw_load = true;
		err = shatter();
		if (err) {
			ERR("Error during shattering.\n");
		}
	} else {
		FATAL("Support for the S5L%dX isn't done yet.\n",
		      device->chip_id);
	}
	}
	/* We are owned now! */
	DPRINT("Bootrom is pwned now! :D\n");

	/* upload iBSS */
	if (ramdisk)
		UsingRamdisk = TRUE;

	if (gp_payload) {
#ifdef _GUI_ENABLE_
		SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Uploading stage zero..."));
		SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) TEXT("(iBSS-greenpois0n)"));
		SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
		SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
		SendMessage(progress, PBM_SETPOS, 0, 0);
		InvalidateRect(window, NULL, TRUE);
#endif
		STATUS("[*] Uploading stage zero - greenpois0n payload (iBSS)...\n");
		greenpois0n_inject();
		exit(-1);
	}
#ifdef _GUI_ENABLE_
	SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Uploading stage zero..."));
	SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) TEXT("(iBSS)"));
	SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(progress, PBM_SETPOS, 0, 0);
	InvalidateRect(window, NULL, TRUE);
#endif

	STATUS("[*] Uploading stage zero (iBSS)...\n");
	upload_image(Firmware.item[IBSS], 0, 1, 0);
#ifdef _WIN32
	client = irecv_reconnect(client, 10);
#else
	client = irecv_reconnect(client, 5);
#endif

#ifdef _GUI_ENABLE
	SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Uploading stage one..."));
	SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) TEXT("(iBEC)"));
	SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(progress, PBM_SETPOS, 0, 0);
	InvalidateRect(window, NULL, TRUE);
#endif

	STATUS("[*] Uploading stage one (iBEC)...\n");
	upload_image(Firmware.item[IBEC], 0, 1, 0);
#ifdef _WIN32
	client = irecv_reconnect(client, 10);
#else
	client = irecv_reconnect(client, 10);
#endif

	if(iboot == true)  {
#ifdef _GUI_ENABLE_
		SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Uploading stage two..."));
		SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) TEXT("(iBoot)"));
		SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
		SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
		SendMessage(progress, PBM_SETPOS, 0, 0);
		InvalidateRect(window, NULL, TRUE);
#endif
		STATUS("[*] Uploading stage two (iBoot)...\n");
		upload_image(Firmware.item[IBOOT], 0, 1, 0);
#ifdef _WIN32
		client = irecv_reconnect(client, 10);
#else
		client = irecv_reconnect(client, 10);
#endif
		irecv_send_command(client, "go");
		exit(0);
	}

#ifdef _GUI_ENABLE_
	SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Waiting..."));
	SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) TEXT("Waiting for USB reset."));
	SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(progress, PBM_SETPOS, 0, 0);
	InvalidateRect(window, NULL, TRUE);
#endif

	STATUS("[*] Waiting for reset...\n");
#ifdef _WIN32
	client = irecv_reconnect(client, 5);
#else
	client = irecv_reconnect(client, 2);
#endif
	irecv_reset(client);
	irecv_set_interface(client, 0, 0);
	irecv_set_interface(client, 1, 1);

	if (pwnrecovery) {
		FATAL
		    ("Device has a pwned iBEC uploaded. Do whatever you want \n");
	}

	/* upload logo */
#ifdef _GUI_ENABLE_
	SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Uploading image..."));
	SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) TEXT("(boot logo)"));
	SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(progress, PBM_SETPOS, 0, 0);
	InvalidateRect(window, NULL, TRUE);
#endif
	STATUS("[*] Uploading boot logo...\n");
	if (bootlogo) {
		userprovided = 1;
		Firmware.item[APPLELOGO].name = bootlogo;
	}

	upload_image(Firmware.item[APPLELOGO], 2, 0, userprovided);
	userprovided = 0;
	irecv_send_command(client, "setpicture 0");
	irecv_send_command(client, "bgcolor 0 0 0");
	irecv_send_command(client, "bgcolor 192 192 192");
#ifdef _WIN32
	client = irecv_reconnect(client, 5);
#else
	client = irecv_reconnect(client, 2);
#endif

	if(do_jailbreak)
		make_ramdisk(Firmware.item[RESTORERAMDISK]);

	/* upload ramdisk */
	if (ramdisk) {
#ifdef _GUI_ENABLE_
		SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Uploading image..."));
		SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) TEXT("(RAM disk)"));
		SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
		SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
		SendMessage(progress, PBM_SETPOS, 0, 0);
		InvalidateRect(window, NULL, TRUE);
#endif

		STATUS("[*] Uploading ramdisk...\n");
		Firmware.item[RESTORERAMDISK].name = ramdisk;
		upload_image(Firmware.item[RESTORERAMDISK], 4, 0, 1);
		irecv_reset(client);
		sleep(5);
		irecv_close(client);
		irecv_exit();

		DPRINT("Reinitializing libirecovery.\n");

//		STATUS
//		    ("[!] HACK-O-RAMA WARNING: TO GET THIS WORKING, YOU MUST REMOVE DEVICE WHEN IT TIMES OUT ON INTERFACE RESET.\n");
		sleep(5);
		irecv_init();

		while (poll_device(RECOVERYMODE)) {
			sleep(1);
		}

		DPRINT("sending ramdisk command\n");
		irecv_send_command(client, "ramdisk");

		irecv_reset_counters(client);

	}

	/* upload devicetree */
#ifdef _GUI_ENABLE_
	SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Uploading image..."));
	SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) TEXT("(Device Tree)"));
	SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(progress, PBM_SETPOS, 0, 0);
	InvalidateRect(window, NULL, TRUE);
#endif

	STATUS("[*] Uploading device tree...\n");
	upload_image(Firmware.item[DEVICETREE], 1, 1, 0);
#ifdef _WIN32
	client = irecv_reconnect(client, 5);
#else
	client = irecv_reconnect(client, 2);
#endif

	irecv_send_command(client, "devicetree");
#ifdef _WIN32
	client = irecv_reconnect(client, 5);
#else
	client = irecv_reconnect(client, 2);
#endif

	/* upload kernel */
#ifdef _GUI_ENABLE_
	SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Uploading Image..."));
	SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) TEXT("(kernelcache)"));
	SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(progress, PBM_SETPOS, 0, 0);
	InvalidateRect(window, NULL, TRUE);
#endif
	STATUS("[*] Uploading kernel...\n");
	if (kernelcache) {
		Firmware.item[KERNELCACHE].name = kernelcache;
		userprovided = 1;
	}
	upload_image(Firmware.item[KERNELCACHE], 3, 1, userprovided);
	userprovided = 0;
#ifdef _WIN32
	client = irecv_reconnect(client, 5);
#else
	client = irecv_reconnect(client, 2);
#endif

	/* BootX */
#ifdef _GUI_ENABLE_
	SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Booting device!"));
	SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(progress, PBM_SETPOS, 0, 0);
	InvalidateRect(window, NULL, TRUE);
#endif

	STATUS("[*] Booting.\n");
	DPRINT("Booting kernel.\n");
	irecv_send_command(client, "bootx");

#ifdef _GUI_ENABLE_
	SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Congratulations!"));
	SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) TEXT(" "));
	SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) TEXT("You are finished with the boot process."));
	SendMessage(hStatus3, WM_SETTEXT, 0, (LPARAM) TEXT("Feel free to remove the device now."));
	SendMessage(progress, PBM_SETPOS, 0, 0);
	InvalidateRect(window, NULL, TRUE);
#endif

}

int fetch_dfu_image(const char* type, const char* output) {
	char name[64];
	char path[255];

	memset(name, '\0', 64);
	memset(path, '\0', 255);
	snprintf(name, 63, "%s.%s.RELEASE.dfu", type, device->model);
	snprintf(path, 254, "Firmware/dfu/%s", name);

	DPRINT("Preparing to fetch DFU image from Apple's servers\n");
	if (fetch_image(path, output) < 0) {
		DPRINT("Unable to fetch DFU image from Apple's servers\n");
		return -1;
	}

	return 0;
}

/*!
 * \fn void greenpois0n_inject(void)
 * \brief gp injection
 */


int upload_firmware_payload(const char* type) {
	int size = 0;
	const unsigned char* payload = NULL;
	irecv_error_t error = IRECV_E_SUCCESS;

	switch (device->index) {
	case DEVICE_APPLETV2:
		if (!strcmp(type, "iBSS")) {
			payload = iBSS_k66ap;
			size = sizeof(iBSS_k66ap);
			DPRINT("Loaded payload for iBSS on k66ap\n0");
		}
		break;

	case DEVICE_IPAD1G:
		if (!strcmp(type, "iBSS")) {
			payload = iBSS_k48ap;
			size = sizeof(iBSS_k48ap);
			DPRINT("Loaded payload for iBSS on k48ap\n0");
		}
		break;

	case DEVICE_IPHONE3GS:
		if (!strcmp(type, "iBSS")) {
			payload = iBSS_n88ap;
			size = sizeof(iBSS_n88ap);
			DPRINT("Loaded payload for iBSS on n88ap\n");
		}
		break;

	case DEVICE_IPHONE4:
		if (!strcmp(type, "iBSS")) {
			payload = iBSS_n90ap;
			size = sizeof(iBSS_n90ap);
			DPRINT("Loaded payload for iBSS on n90ap\n");
		}
		break;

	case DEVICE_IPOD2G:
		if (!strcmp(type, "iBSS")) {
			payload = iBSS_n72ap;
			size = sizeof(iBSS_n72ap);
			DPRINT("Loaded payload for iBSS on n72ap\n");
		}
		break;

	case DEVICE_IPOD3G:
		if (!strcmp(type, "iBSS")) {
			payload = iBSS_n18ap;
			size = sizeof(iBSS_n18ap);
			DPRINT("Loaded payload for iBSS on n18ap\n");
		}
		break;

	case DEVICE_IPOD4G:
		if (!strcmp(type, "iBSS")) {
			payload = iBSS_n81ap;
			size = sizeof(iBSS_n81ap);
			DPRINT("Loaded payload for iBSS on n81ap\n");
		}
		break;
	}

	if (payload == NULL) {
		ERR("Unable to upload firmware payload\n");
		return -1;
	}

	DPRINT("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		ERR("Unable to upload firmware payload\n");
		DPRINT("%s\n", irecv_strerror(error));
		return -1;
	}

	DPRINT("Uploading %s payload\n", type);
	error = irecv_send_buffer(client, (unsigned char*) payload, size, 1);
	if (error != IRECV_E_SUCCESS) {
		ERR("Unable to upload %s payload\n", type);
		return -1;
	}

	return 0;
}

void greenpois0n_inject(void) {
	char image[255];
	struct stat buf;
	irecv_error_t error = IRECV_E_SUCCESS;

	char type[] = "iBSS";

	memset(image, '\0', 255);
	snprintf(image, 254, "%s.%s", type, device->model);

	DPRINT("Checking if %s already exists\n", image);
	if (stat(image, &buf) != 0) {
		if (fetch_dfu_image(type, image) < 0) {
			ERR("Unable to upload DFU image\n");
			return;
		}
	}

	if (client->mode != kDfuMode) {
		DPRINT("Resetting device counters\n");
		error = irecv_reset_counters(client);
		if (error != IRECV_E_SUCCESS) {
			ERR("%s\n", irecv_strerror(error));
			return;
		}
	}

	DPRINT("Uploading %s to device\n", image);
	error = irecv_send_file(client, image, 1);
	if (error != IRECV_E_SUCCESS) {
		ERR("%s\n", irecv_strerror(error));
		return;
	}

#ifdef _WIN32
	client = irecv_reconnect(client, 10);
#else
	client = irecv_reconnect(client, 10);
#endif

	upload_firmware_payload("iBSS");

#ifdef _WIN32
	client = irecv_reconnect(client, 10);
#else
	client = irecv_reconnect(client, 10);
#endif
	irecv_send_command(client, "go");

	return;
}


#ifndef _GUI_ENABLE_

/*!
 * \fn int main(int argc, char **argv)
 * \brief Main opensn0w routine!
 *
 * \param argc Argument count.
 * \param argv Argument variables.      
 */
#ifdef MSVC_VER
int __cdecl main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	int c;
	/* configure xpwn endian */
	switch (endian()) {
	case ENDIAN_BIG:
		endianness = IS_BIG_ENDIAN;
		break;
	case ENDIAN_LITTLE:
		endianness = IS_LITTLE_ENDIAN;
		break;
	default:
		break;
	}
	/* set up signals */
#ifndef _WIN32
	struct sigaction sigact;

	sigact.sa_sigaction = critical_error_handler;
	sigact.sa_flags = SA_RESTART | SA_SIGINFO;
	if (sigaction(SIGSEGV, &sigact, (struct sigaction *)NULL) != 0) {
		FATAL("Error setting signal handler for %d (%s)\n", SIGSEGV,
		      strsignal(SIGSEGV));
	}
#endif

	opterr = 0;

	while ((c = getopt(argc, argv, "DIYvZndAghBzjsXp:Rb:i:k:S:C:r:a:")) != -1) {
		switch (c) {
		case 'I':
			iboot = true;
			break;
		case 'n':
			nologo = true;
			break;
		case 'g':
			gp_payload = true;
			break;
		case 'j':
			do_jailbreak = true;
			break;
		case 'D':
			dry_run = true;
			break;
		case 'Y':
			use_shatter = true;
			break;
		case 'Z':
			raw_load_exit = true;
			break;
		case 'B':
			dump_bootrom = true;
			break;
		case 'A':
			autoboot = true;
			break;
		case 'v':
			opensn0w_debug_level = DBGFLTR_MISC;
			break;
		case 'R':
			pwnrecovery = true;
			break;
		case 'z':
			raw_load = true;
			break;
		case 'a':
			boot_args_process(optarg);
			break;
		case 'X':
			download = true;
			break;
		case 'd':
			pwndfu = true;
			break;
		case 'h':
			usage();
			break;
		case 'p':
			if (!file_exists(optarg)) {
				printf("Cannot open plist file '%s'\n", optarg);
				return -1;
			}
			plist = optarg;
			break;
		case 'k':
			if (!file_exists(optarg)) {
				printf("Cannot open kernelcache file '%s'\n",
				       optarg);
				return -1;
			}
			kernelcache = optarg;
			break;
		case 'i':
			url = optarg;
			break;
		case 'b':
			if (!file_exists(optarg)) {
				printf("Cannot open bootlogo file '%s'\n",
				       optarg);
				return -1;
			}
			bootlogo = optarg;
			break;
		case 'S':
			if (!file_exists(optarg)) {
				printf("Cannot open file '%s'\n", optarg);
				return -1;
			}
			send_file(optarg);
			break;
		case 'C':
			send_command(optarg);
			break;
		case 's':
			irecovery_shell_initialize();
			break;
		case 'r':
			if (!file_exists(optarg)) {
				printf("Cannot open ramdisk file '%s'\n",
				       optarg);
				return -1;
			}
			ramdisk = optarg;
			break;
		default:
			usage();
			break;
		}
	}

	if(nologo == false) {
		printf("opensn0w, an open source jailbreaking program.\n"
		       "version: " __SN0W_VERSION_FULL__ "\n\n"
		       "Compiled on: " __DATE__ " " __TIME__ "\n\n");
		print_configuration();
	}

	jailbreak();

	return 0;
}

#endif
