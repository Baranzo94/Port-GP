#ifndef CameraController_h
#define CameraController_h

#include "Component.h"

class Camera;

class CameraController :public Component
{
public:
	CameraController();
	~CameraController();

	void update();

	void setCamera(Camera * cam);
	void moveForward();
	void moveBackward();
	void camRot();
	void mouseMoved();
	float DeltaTime;
private:
	float m_ForwardSpeed;
	float m_StrafeSpeed;
	float m_LookSpeed;
	bool m_MouseMouvement;
	Camera * m_AttachedCamera;

};


#endif
