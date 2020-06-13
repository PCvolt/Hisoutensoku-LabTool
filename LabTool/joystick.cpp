#include "stdafx.h"
#include "labTool_manager.h"
#include "joystick.h"

int Joystick::CreateDIObject()
{
	Joystick::result = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&lpDIObject, NULL);
	if (FAILED(result))
	{
		return 1;
	}
}

/* KEYBOARD */
int Joystick::getDIKeyboard()
{
	result = lpDIObject->CreateDevice(GUID_SysKeyboard, &lpDIKeyboard, NULL);
	if (FAILED(result)) { Finalize(); std::cout << "b" << std::endl; return 2; }
	result = lpDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result)) { Finalize(); std::cout << "b" << std::endl; return 3; }
	result = lpDIKeyboard->SetCooperativeLevel(LabToolManager::getSokuHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result)) { Finalize(); std::cout << "b" << std::endl; return 4; }

	if (lpDIKeyboard)
	{
		lpDIKeyboard->Acquire();
	}
}

int Joystick::getKeyboardInputs()
{
	char buffer[256];
	result = lpDIKeyboard->GetDeviceState(sizeof(buffer), (LPVOID)&buffer);

	if (FAILED(result))
	{
		// Possible errors: DIERR_INPUTLOST, DIERR_INVALIDPARAM, DIERR_NOTACQUIRED, DIERR_NOTINITIALIZED, E_PENDING
		return 7;
	}

	// Keystate handling
	if (KEYDOWN(buffer, DIK_A))
	{
		std::cout << "a" << std::endl;
	}
}

/* JOYPAD */
// Enumeration callback
BOOL CALLBACK Joystick::CreateDeviceCallback(LPCDIDEVICEINSTANCE instance, LPVOID reference) {
	return ((Joystick*)reference)->CreateDeviceCallback_impl(instance);
}

BOOL Joystick::CreateDeviceCallback_impl(LPCDIDEVICEINSTANCE instance) {
	HRESULT result;
	result = lpDIObject->CreateDevice(instance->guidInstance, &lpDIJoypad, NULL);

	if (FAILED(result)) {
		return DIENUM_CONTINUE;
	}

	return DIENUM_STOP;
}

int Joystick::getDIJoypad() {
	result = lpDIObject->EnumDevices(DI8DEVCLASS_GAMECTRL, CreateDeviceCallback, (LPVOID)this, DIEDFL_ATTACHEDONLY);

	if (FAILED(result)) {
		// Possible errors: DIERR_INVALIDPARAM, DIERR_NOTINITIALIZED
		Finalize();
		return 5;
	}

	result = lpDIJoypad->SetDataFormat(&c_dfDIJoystick2);

	if (FAILED(result))
	{
		// Possible errors: DIERR_ACQUIRED, DIERR_INVALIDPARAM, DIERR_NOTINITIALIZED
		Finalize();
		return 6;
	}

	if (lpDIJoypad)
	{
		lpDIJoypad->Acquire();
	}
}

int Joystick::getJoypadInputs()
{
	DIJOYSTATE2 state;
	result = lpDIJoypad->GetDeviceState(sizeof(state), (LPVOID)&state);

	if (FAILED(result))
	{
		// Possible errors: DIERR_INPUTLOST, DIERR_INVALIDPARAM, DIERR_NOTACQUIRED, DIERR_NOTINITIALIZED, E_PENDING
		return 8;
	}

	if (state.rgbButtons[0])
	{
		std::cout << "a" << std::endl;
	}
}

void Joystick::Finalize(void)
{
	if (lpDIObject)
	{
		// Finalisation du périphérique
		if (lpDIKeyboard)
		{
			lpDIKeyboard->Unacquire();
			lpDIKeyboard->Release();
			lpDIKeyboard = NULL;
		}

		if (lpDIJoypad)
		{
			lpDIJoypad->Unacquire();
			lpDIJoypad->Release();
			lpDIJoypad = NULL;
		}

		// Finalisation de DirectInput
		lpDIObject->Release();
		lpDIObject = NULL;
	}
}