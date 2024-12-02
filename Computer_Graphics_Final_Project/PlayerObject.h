#pragma once

#include "GameObject.h"

class PlayerObject : public GameObject
{
	bool isWPressed;
	bool isAPressed;
	bool isSPressed;
	bool isDPressed;

	float moveSpeed;

	bool isLeftMousePressed;
	int befMousePosX;
	int befMousePosY;

public:
	PlayerObject();
	virtual ~PlayerObject();

	virtual void initilize() override;
	virtual void update(float elapsedTime) override;
	virtual void draw() const override;
	virtual void release() override;

	void keyboard(unsigned char key, bool isPressed);
	void mouse(int button, int state, int x, int y);
	void mouseMove(int x, int y);

};

