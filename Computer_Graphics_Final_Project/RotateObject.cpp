#include "RotateObject.h"

#include <iostream>

RotateObject::RotateObject()
{
	rotateSpeed = (rand() % 2 + 4) * 45.f;	// 45 ~ 180��
}

RotateObject::~RotateObject()
{
}

void RotateObject::initilize()
{
}

void RotateObject::update(float elapsedTime)
{
	rotateY(rotateSpeed * elapsedTime);		// �ʴ� rotateSpeed��ŭ ȸ��
}

void RotateObject::draw() const
{
	// �𵨺�ȯ���
	GLint modelLoc = glGetUniformLocation(shader, "modelTransform");
	if (modelLoc < 0)
		std::cout << "modelLoc ã�� ����\n";
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(worldTransform));

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

void RotateObject::release()
{
}
