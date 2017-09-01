#include "InputManager.h"
#include <iostream>

namespace Lime
{

    InputManager::InputManager()
    {
        m_MouseCoords = glm::vec2(0.0f, 0.0f);
        m_Camera = nullptr;
    }
    bool InputManager::Update()
    {
        bool isRunning = true;
        SDL_Event evnt;
        glm::vec2 mouserel(0, 0);
        while (SDL_PollEvent(&evnt))
        {
            switch (evnt.type)
            {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_MOUSEMOTION:
                SetMouseCoords((float)evnt.motion.x, (float)evnt.motion.y);
                break;
            case SDL_KEYUP:
                KeyUp(evnt.key.keysym.sym);
                break;
            case SDL_KEYDOWN:
                KeyDown(evnt.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                KeyDown(evnt.button.button);
                break;
            case SDL_MOUSEBUTTONUP:
                KeyUp(evnt.button.button);
                break;
            }
        }
        return isRunning;
    }

    void InputManager::LoadCamera(std::shared_ptr<Camera>& camera)
    {
        m_Camera = camera;
    }

    glm::vec2 InputManager::GetMouseCoords()
    {
        return m_MouseCoords;
    }

    glm::vec3 InputManager::GetMouseCoordsWorldSpace()
    {
        glm::vec3 worldCoords;
        if (m_Camera)
        {
            glm::vec2 mousePos = GetMouseCoords();
            glm::vec2 normalized = NormalizeDeviceMouseCoords(mousePos);
            glm::vec4 clipCoords = glm::vec4(normalized, -1.0f, 1.0f);
            glm::vec4 eyeCoords = GetEyeSpaceCoords(clipCoords);
            worldCoords = GetWorldSpaceCoords(eyeCoords);
        }
        return worldCoords;
    }

    glm::vec3 InputManager::GetMouse3DPosition()
    {
        glm::vec3 origin = m_Camera->GetPosition();
        glm::vec3 mousePosition = GetMouseCoordsWorldSpace();
        glm::vec3 planeNormal = glm::vec3(0, 1, 0);
        float farDistance = m_Camera->GetFarPlane() / 2;
        float DotPt1 = glm::dot(origin, planeNormal);
        float DotPt2 = glm::dot(mousePosition, planeNormal);
        float distanceToPlane = 0.0f - (DotPt1 / DotPt2);
        glm::vec3 mouse3Dposition = origin + (mousePosition * distanceToPlane);
        return mouse3Dposition;
    }

    glm::vec3 InputManager::GetMouse3DPosition2()
    {
        glm::vec3 cameraPos = m_Camera->GetPosition();
        glm::vec3 mousePos = GetMouseCoordsWorldSpace();
        float t = -cameraPos.y / mousePos.y;
        glm::vec3 mousePosition = cameraPos + (mousePos * t);
        return mousePosition;

    }

    void InputManager::SetMouseCoords(float x, float y)
    {
        m_MouseCoords.x = x;
        m_MouseCoords.y = y;
    }

    glm::vec2 InputManager::NormalizeDeviceMouseCoords(glm::vec2 & mousePos)
    {
        glm::vec2 mousePosition;
        if (m_Camera)
        {
            mousePosition.x = (2.0f * mousePos.x) / m_Camera->GetWindowWidth() - 1.0f;
            mousePosition.y = 1.0f - (2.0f * mousePos.y) / m_Camera->GetWindowHeight();
        }
        return mousePosition;
    }

    glm::vec4 InputManager::GetEyeSpaceCoords(glm::vec4 mouseCoords)
    {
        glm::vec4 EyeSpaceCoords;
        if (m_Camera)
        {
            EyeSpaceCoords = glm::inverse(m_Camera->GetProjectiomMatrix()) * mouseCoords;
            EyeSpaceCoords = glm::vec4(EyeSpaceCoords.x, EyeSpaceCoords.y, -1.0f, 0.0f);
        }
        return EyeSpaceCoords;
    }

    glm::vec3 InputManager::GetWorldSpaceCoords(glm::vec4 eyeCoords)
    {
        glm::vec3 worldCoords;
        if (m_Camera)
        {
            glm::vec4 mouseCoords = glm::inverse(m_Camera->GetViewMatrix()) * eyeCoords;
            worldCoords = glm::vec3(mouseCoords.x, mouseCoords.y, mouseCoords.z);
            worldCoords = glm::normalize(worldCoords);
        }
        return worldCoords;
    }

    void InputManager::KeyUp(unsigned int keyID)
    {
        if (m_KeysDown[keyID] == true)
            m_KeysPressed[keyID] = true;

        m_KeysDown[keyID] = false;
    }

    void InputManager::KeyDown(unsigned int keyID)
    {
        m_KeysDown[keyID] = true;
    }

    bool InputManager::KeyStatus(unsigned int keyID)
    {
        auto iterator = m_KeysDown.find(keyID);
        if (iterator != m_KeysDown.end())
        {
            return iterator->second;
        }
        return false;
    }
    DLL_EXPORT bool InputManager::KeyPressed(unsigned int keyID)
    {
        bool bWasPressed = m_KeysPressed[keyID];
        if(bWasPressed)
            m_KeysPressed[keyID] = false;

        return bWasPressed;
    }
}