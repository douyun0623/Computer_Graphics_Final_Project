#include "PlayerObject.h"

PlayerObject::PlayerObject()
{
	isWPressed = false;
	isAPressed = false;
	isSPressed = false;
	isDPressed = false;

	moveSpeed = 5.f;

	initilize();
}

PlayerObject::~PlayerObject()
{
}

void PlayerObject::initilize()
{
	// �ȱ׸��Ŵ�..
	setVAO(0, 0);
	setShader(0);
}

void PlayerObject::update(float elapsedTime)
{
	glm::vec3 dir(0.f);
	if (isWPressed)
		dir += getLook();
	if (isAPressed)
		dir += getRight();
	if (isSPressed)
		dir -= getLook();
	if (isDPressed)
		dir -= getRight();

	if (glm::length(dir) >= glm::epsilon<float>())	// 0���� Ŀ���Ѵ�
		move(dir, moveSpeed * elapsedTime);
}

void PlayerObject::draw() const
{
}

void PlayerObject::release()
{
}

void PlayerObject::keyboard(unsigned char key, bool isPressed)
{
	if (isPressed) {			// �������� ��
		switch (key) {
		case 'W':
		case 'w':
			isWPressed = true;
			break;
		case 'A':
		case 'a':
			isAPressed = true;
			break;
		case 'S':
		case 's':
			isSPressed = true;
			break;
		case 'D':
		case 'd':
			isDPressed = true;
			break;
		}
	}
	else {						// ������ ��
		switch (key) {
		case 'W':
		case 'w':
			isWPressed = false;
			break;
		case 'A':
		case 'a':
			isAPressed = false;
			break;
		case 'S':
		case 's':
			isSPressed = false;
			break;
		case 'D':
		case 'd':
			isDPressed = false;
			break;
		}
	}
}

void PlayerObject::mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			isLeftMousePressed = true;
			befMousePosX = x;
			befMousePosY = y;
		}
		else if (state == GLUT_UP)
			isLeftMousePressed = false;
	}
}

void PlayerObject::mouseMove(int x, int y)
{
	int moveXValue = x - befMousePosX;
	int moveYValue = y - befMousePosY;

	//if (moveXValue > 0) //  ����� �ð�...

	rotateY(float(-moveXValue / 3.f));

	befMousePosX = x;
	befMousePosY = y;
}
