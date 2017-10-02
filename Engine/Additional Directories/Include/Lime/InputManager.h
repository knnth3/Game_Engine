#pragma once
#include <unordered_map>
#include <glm\glm.hpp>
#include "Camera.h"
#include <glm\gtx\transform.hpp>

#if PLATFORM == OS_WINDOWS
#define DIRECTINPUT_VERSION 0x0800
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <dinput.h>
#endif


namespace Lime
{
	//Forward Decl
	class DX11App;

    class InputManager
    {
    public:
        DLL_EXPORT InputManager();
		DLL_EXPORT ~InputManager();
		DLL_EXPORT void Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);
        DLL_EXPORT void LoadCamera(std::shared_ptr<Camera>& camera);
        DLL_EXPORT glm::vec2 GetMouseCoords();
        DLL_EXPORT glm::vec3 GetMouse3DPosition();
        DLL_EXPORT glm::vec3 GetMouse3DPosition2();
        DLL_EXPORT bool KeyStatus(unsigned int keyID);
        DLL_EXPORT bool KeyPressed(unsigned int keyID);
	protected:
		friend class DX11App;
		void KeyDown(unsigned int keyID);
		void KeyUp(unsigned int keyID);
		void SetMouseCoords(short x, short y);
    private:
        glm::vec3 GetMouseCoordsWorldSpace();
        glm::vec2 NormalizeDeviceMouseCoords(glm::vec2& mousePos);
        glm::vec4 GetEyeSpaceCoords(glm::vec4 mouseCoords);
        glm::vec3 GetWorldSpaceCoords(glm::vec4 eyeCoords);

        std::unordered_map<unsigned int, bool> m_KeysDown;
        std::unordered_map<unsigned int, bool> m_KeysPressed;
        std::shared_ptr<Camera> m_Camera;
        glm::vec2 m_MouseCoords;
		uint16_t m_screenWidth, m_screenHeight;

#if PLATFORM == OS_WINDOWS
		IDirectInput8* m_directInput;
		IDirectInputDevice8* m_keyboard;
		IDirectInputDevice8* m_mouse;
		DIMOUSESTATE m_mouseState;
#endif

    };


}