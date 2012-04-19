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
#ifdef MSVC_VER
#include <objidl.h>		/* damn you Windows Driver Kit! */
#endif

#include <gdiplus.h>

ULONG_PTR gdiplusToken;

extern "C" HINSTANCE currentInstance;

extern "C" void GuiGdiPlusConstructor(void) {
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;

	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}	

extern "C" void GuiGdiPlusDeconstructor(void) {
	Gdiplus::GdiplusShutdown(gdiplusToken);
}

extern "C" HICON GuiGetIconForPng(const WCHAR *filename) {
	Gdiplus::Bitmap *pBitmap;
	HICON hIcon = NULL;

	pBitmap = Gdiplus::Bitmap::FromFile(filename);
	pBitmap->GetHICON(&hIcon);

	return hIcon;
}

extern "C" HICON GuiGetIconForResource(const WCHAR *bitmapName) {
	Gdiplus::Bitmap *pBitmap;
	HICON hIcon = NULL;

	pBitmap = Gdiplus::Bitmap::FromResource(currentInstance, bitmapName);
	pBitmap->GetHICON(&hIcon);

	return hIcon;
}


