#include "Weapon.h"

void Weapon::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (!isActivate)
	{
		if (GetTickCount() - firstCast > WEAPON_ACTIVATE_TIME)
		{
			float x = aladdin->x;
			float y = aladdin->y;
			
			nx = aladdin->nx;

			if (nx > 0)
			{
				this->max_x = x + max_width;
			}
			else if (nx < 0)
			{
				this->max_x = x - max_width;
			}

			SetPosition(x, y - 10);
			appearTime = GetTickCount();

			isActivate = true;
		}
	}
}

void Weapon::Render()
{
	if (isActivate)
		Item::Render();
}
