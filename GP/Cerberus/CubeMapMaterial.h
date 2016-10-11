#ifndef CUBEMAPMATERIAL_H
#define CUBEMAPMATERIAL_H

#include "Material.h"

class CubeMapMaterial :public BaseMaterial
{
public:
	CubeMapMaterial();
	~CubeMapMaterial();

	void destory();
	void bind();
	void unbind();

	void loadCubeTexture(
		const std::string& PosXFilename,
		const std::string& NegXFilename,
		const std::string& PosYFilename,
		const std::string& NegYFilename,
		const std::string& PosZFilename,
		const std::string& NegZFilename);

	GLuint getCubeTexture();

private:

	GLuint m_CubeTexture;

};



#endif