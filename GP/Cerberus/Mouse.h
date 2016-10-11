#ifndef Mouse_h
#define Mouse_h

class Mouse
{
public:
	Mouse();
	~Mouse();

	void update();

	void setMousePosition(int absX, int absY, int relX, int relY);

	void setMouseButtonDown(int buttonIndex);
	void setMouseButtonUp(int buttonIndex);

	bool isButtonDown(int buttonIndex);
	bool isButtonUp(int buttonIndex);

	int getRelativeMouseX()
	{
		return m_RelMouseX;
	};

	int getRelativeMouseY()
	{
		return m_RelMouseY;
	};

	const static int DeadzoneNeg = -5;
	const static int DeadzonePos = 5;
private:
	static const int MAX_NO_OF_BUTTONS = 3;
	int m_AbsMouseX;
	int m_AbsMouseY;
	int m_RelMouseX;
	int m_RelMouseY;
	bool m_MouseButtonsDown[MAX_NO_OF_BUTTONS];
	bool m_MouseButtonsUp[MAX_NO_OF_BUTTONS];
};

#endif
