#include "Input.h"


Input::Input()
{
	m_Mouse = NULL;
	m_Keyboard = NULL;

}

Input::~Input()
{

}

void Input::destroy()
{

	if (m_Mouse)
	{
		delete m_Mouse;
		m_Mouse = NULL;
	}
	if (m_Keyboard)
	{
		delete m_Keyboard;
		m_Keyboard = NULL;
	}
}

void Input::update()
{
	m_Keyboard->update();
	m_Mouse->update();
}

bool Input::init()
{
	m_Keyboard = new Keyboard();
	m_Mouse = new Mouse();

	return true;
}