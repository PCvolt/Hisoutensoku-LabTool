#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <initguid.h>
#include <shlwapi.h>
// dinput8.lib dxguid.lib

#define KEYDOWN( buffer, key ) ( buffer[key] & 0x80 ) 

class Joystick
{
private:
	HINSTANCE			 instance;
	HRESULT				 result;
	LPDIRECTINPUT8		 lpDIObject = NULL;		// pointer to direct input object
	LPDIRECTINPUTDEVICE8 lpDIKeyboard = NULL;	// pointer to keyboard device
	LPDIRECTINPUTDEVICE8 lpDIJoypad = NULL;


public:
	int CreateDIObject();

	int getDIKeyboard();
	int getKeyboardInputs();

	static BOOL CALLBACK CreateDeviceCallback(LPCDIDEVICEINSTANCE instance, LPVOID reference);
	BOOL CreateDeviceCallback_impl(LPCDIDEVICEINSTANCE instance);
	int getDIJoypad();
	int getJoypadInputs();

	void Finalize(void);
};
