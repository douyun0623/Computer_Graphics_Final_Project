#pragma once

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

class GameObject
{
protected:		// 부모 클래스가 될 것이다..
	glm::mat4 worldTransform;

	GLuint shader;
	GLuint VAO;
	GLsizei vertexCount;

public:
	GameObject();
	virtual ~GameObject();

	virtual void initilize();
	virtual void update(float elapsedTime);
	virtual void draw() const;
	virtual void release();

	void setShader(GLuint shader);
	void setVAO(GLuint vao, GLsizei count);

	glm::vec3 getPosition() const;
	void setPosition(glm::vec3 position);
	void setPosition(float x, float y, float z);	// 오버로딩

	glm::vec3 getLook() const;
	glm::vec3 getRight() const;

	void rotateY(float degrees);

	void move(glm::vec3 dir, float value);
	void moveForward(float value);

};

