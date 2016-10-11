#include "CubeMapMaterial.h"
#include "Texture.h"
#include "Vertex.h"

CubeMapMaterial::CubeMapMaterial()
{
	m_CubeTexture = 0;
}

CubeMapMaterial::~CubeMapMaterial()
{
}

void CubeMapMaterial::destory()
{
	if (m_CubeTexture)
	{
		glDeleteTextures(1, &m_CubeTexture);
	}
}

void CubeMapMaterial::bind()
{
	glDepthMask(GL_FALSE);
	glUseProgram(m_ShaderProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeTexture);

	GLint vertexPosLocation = glGetAttribLocation(m_ShaderProgram, "vertexPosition");
	glBindAttribLocation(m_ShaderProgram, vertexPosLocation, "vertexPosition");

	glEnableVertexAttribArray(vertexPosLocation);
	glVertexAttribPointer(vertexPosLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
}

void CubeMapMaterial::unbind()
{
	glDepthMask(GL_TRUE);
}

void CubeMapMaterial::loadCubeTexture
(const std::string& PosXFilename,
const std::string& NegXFilename,
const std::string& PosYFilename,
const std::string& NegYFilename,
const std::string& PosZFilename,
const std::string& NegZFilename)
{
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &m_CubeTexture);

	glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);

	loadCubeMapSide(PosXFilename, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	loadCubeMapSide(NegXFilename, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);

	loadCubeMapSide(PosZFilename, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	loadCubeMapSide(NegZFilename, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

	loadCubeMapSide(PosYFilename, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	loadCubeMapSide(NegYFilename, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
}

GLuint CubeMapMaterial::getCubeTexture()
{
	return m_CubeTexture;
}