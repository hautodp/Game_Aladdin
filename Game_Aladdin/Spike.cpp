#include "Spike.h"

void Spike::Render()
{
	if (!isHitted)
	{
		if ((animations[0]->currentFrame > 0 && name == "spike1") || (animations[0]->currentFrame < 5 && name == "spike2"))
		{
			SetBound();
		}
		else
		{
			width = 0;
			height = 0;
		}
		animations[0]->RenderRock(x, y, 255);

	}
}

void Spike::SetHit()
{
	isHitted = true;
}

void Spike::SetBound()
{

	width = 0;
	height = 0;
}

void Spike::GetBoundingBox(float &l, float &t, float &r, float &b)
{
	l = x;
	t = y;
	r = x;
	b = y;
}