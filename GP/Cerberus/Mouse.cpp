#include "Mouse.h"

Mouse::Mouse()
{
	m_AbsMouseX = 0;
	m_AbsMouseY = 0;
	m_RelMouseX = 0;
	m_RelMouseY = 0;
	for (int i = 0; i<MAX_NO_OF_BUTTONS; i++)
	{
		m_MouseButtonsDown[i] = false;
		m_MouseButtonsUp[i] = false;
	}
}

Mouse::~Mouse()
{

}

void Mouse::update()
{
	m_AbsMouseX = 0;
	m_AbsMouseY = 0;
	m_RelMouseX = 0;
	m_RelMouseY = 0;
	for (int i = 0; i<MAX_NO_OF_BUTTONS; i++)
	{
		m_MouseButtonsDown[i] = false;
		m_MouseButtonsUp[i] = false;
	}
}

void Mouse::setMousePosition(int absX, int absY, int relX, int relY)
{
	m_AbsMouseX = absX;
	m_AbsMouseY = absY;
	m_RelMouseX = relX;
	m_RelMouseY = relY;
}

void Mouse::setMouseButtonDown(int buttonIndex)
{
	m_MouseButtonsDown[buttonIndex] = true;
	m_MouseButtonsUp[buttonIndex] = false;
}

void Mouse::setMouseButtonUp(int buttonIndex)
{
	m_MouseButtonsDown[buttonIndex] = false;
	m_MouseButtonsUp[buttonIndex] = true;
}

bool Mouse::isButtonDown(int buttonIndex)
{
	return m_MouseButtonsDown[buttonIndex];
}

bool Mouse::isButtonUp(int buttonIndex)
{
	return m_MouseButtonsUp[buttonIndex];
}