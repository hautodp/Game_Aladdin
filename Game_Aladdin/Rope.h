#pragma once
#include "GameObject.h"
#include"define.h"


class Rope : public CGameObject
{
public:
	int height;
	virtual void Render();
	virtual void GetBoundingBox(float &l, float &t, float &r, float &b);
};

