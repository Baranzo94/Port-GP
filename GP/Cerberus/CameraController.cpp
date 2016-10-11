#include <iostream>

#include "CameraController.h"
#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "Timer.h"
#include "Input.h"

#include <glm/glm.hpp>
using glm::vec3;

CameraController::CameraController()
{
	m_AttachedCamera = NULL;
	m_ForwardSpeed = 2;
	m_StrafeSpeed = 1;
	m_LookSpeed = 0.1f;
	m_Type = "CameraController";
	m_Name = m_Type;
}

CameraController::~CameraController()
{

}

void CameraController::update()
{
	
	DeltaTime = Timer::getTimer().getDeltaTime();
	if (m_AttachedCamera)
	{
		//Using standard FPS mouse & keyboard controls
		vec3 currentPos = m_AttachedCamera->getParent()->getTransform()->getPosition();
		vec3 currentRot = m_AttachedCamera->getParent()->getTransform()->getRotation();
		vec3 currentLook = m_AttachedCamera->getLookAt();

		//get the mouse values and directly move the yaw & pitch of the camera(the x and y of the camera rotation)

		//use these to caculate the look at
		int mouseX = Input::getInput().getMouse()->getRelativeMouseX();
		int mouseY = Input::getInput().getMouse()->getRelativeMouseY();

		currentRot.x += mouseX*Timer::getTimer().getDeltaTime()*m_LookSpeed;
		currentRot.y += mouseY*Timer::getTimer().getDeltaTime()*m_LookSpeed;

		vec3 direction(cos(currentRot.y) * sin(currentRot.x),
			sin(currentRot.y),
			cos(currentRot.y) * cos(currentRot.x));

		vec3 right = vec3(sin(currentRot.x - 3.14f / 2.0f), 0, cos(currentRot.y - 3.14 / 2.0f));

		glm::vec3 up = glm::cross(direction, right);

		//Grab input
		if (Input::getInput().getKeyboard()->isKeyDown(SDLK_w))
		{
			currentPos.z += direction.z*m_ForwardSpeed*Timer::getTimer().getDeltaTime();
		}
		else if (Input::getInput().getKeyboard()->isKeyDown(SDLK_s))
		{
			currentPos.z -= direction.z*m_ForwardSpeed*Timer::getTimer().getDeltaTime();
		}

		if (Input::getInput().getKeyboard()->isKeyDown(SDLK_a))
		{
			currentPos += right*m_StrafeSpeed*Timer::getTimer().getDeltaTime();
		}
		else if (Input::getInput().getKeyboard()->isKeyDown(SDLK_d))
		{
			currentPos -= right*m_StrafeSpeed*Timer::getTimer().getDeltaTime();
		}

		if (Input::getInput().getKeyboard()->isKeyDown(SDLK_q))
		{
			currentPos.x = 0.0f;
			currentPos.y = 0.0f;
			currentPos.z = 8.0f;
			
			m_AttachedCamera->setLook(0.0f, 0.0f, 0.0f);


		}

		m_AttachedCamera->getParent()->getTransform()->setPosition(currentPos.x, currentPos.y, currentPos.z);
		m_AttachedCamera->getParent()->getTransform()->setRotation(currentRot.x, currentRot.y, currentRot.z);
		m_AttachedCamera->setUp(up.x, up.y, up.z);
		m_AttachedCamera->setLook(currentPos.x + direction.x, currentPos.y + direction.y, currentPos.z + direction.z);

	}
}

void CameraController::setCamera(Camera * cam)
{
	m_AttachedCamera = cam;
}