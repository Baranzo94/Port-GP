#ifndef Component_h
#define Component_h

#include <string>

class GameObject;

class Component
{
public:
	Component();
	virtual ~Component();

	virtual void init();

	virtual void update();

	virtual void render();

	virtual void destroy();

	const std::string& getType();

	bool isActive();

	void toggleActive();

	void setParent(GameObject * object);

	const std::string& getName();
	GameObject * getParent();
	void setName(const std::string& name);
protected:
	std::string m_Type;
	std::string m_Name;
	bool m_Active;
	GameObject * m_Parent;
private:
};

#endif
