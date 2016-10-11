
#include "Keyboard.h"

Keyboard::Keyboard()
{
	for (int i = 0; i<MAX_NO_KEYS; i++)
	{
		keysDown[i] = false;
		keysUp[i] = false;
	}
}

Keyboard::~Keyboard()
{

}

void Keyboard::update()
{
	for (int i = 0; i<MAX_NO_KEYS; i++)
	{
		keysDown[i] = false;
		keysUp[i] = false;
	}

}

void Keyboard::setKeyDown(short index)
{
	keysDown[index] = true;
}

void Keyboard::setKeyUp(short index)
{
	keysUp[index] = true;
}

bool Keyboard::isKeyDown(short index)
{
	return keysDown[index];
}

bool Keyboard::isKeyUp(short index)
{
	return keysUp[index];
}