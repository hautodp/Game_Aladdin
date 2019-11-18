﻿#include <algorithm>
#include "debug.h"
#include "Rock.h"
#include "Aladdin.h"
#include "Game.h"
#include "Item.h"
#include "Zombie.h"
//#include "Ground.h"
#include "Pillar.h"
#include "CheckPoint.h"
#include "Soldier.h"
#include "Bullet.h"
#include "Spike.h"
#include "Rope.h"

//#define SCREEN_WIDTH 640
//#define SCREEN_HEIGHT 480

int Aladdin::score = 0;
int Aladdin::heartsAmount = 5;
/*
	Calculate potential collisions with the list of colliable objects

	coObjects: the list of colliable objects
	coEvents: list of potential collisions
*/
void Aladdin::CalcPotentialCollisions(
	vector<LPGAMEOBJECT> *coObjects,
	vector<LPCOLLISIONEVENT> &coEvents)
{
	bool isCollideWithCheckBox = false;

	for (UINT i = 0; i < coObjects->size(); i++)
	{
		// Aladdin se khong va cham voi nhung vat sau:
		if (!dynamic_cast<Pillar *>(coObjects->at(i)))
		{
			LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));

			if (e->t > 0 && e->t <= 1.0f)
				coEvents.push_back(e);
			else
				delete e;
		}

		/*if (!dynamic_cast<Rock *>(coObjects->at(i)))
		{
			LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));

			if (e->t > 0 && e->t <= 1.0f)
				coEvents.push_back(e);
			else
				delete e;
		}*/
	}

	std::sort(coEvents.begin(), coEvents.end(), CCollisionEvent::compare);
}

void Aladdin::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	// Calculate dx, dy 
	CGameObject::Update(dt);

	// Has completed attack animation
	if (isAttack == true && GetTickCount() - attackTime >= SIMON_TIMER_ATTACK)
	{
		isAttack = false;
		if (isExitSit)
		{
			isSit = false;
			y -= SIMON_SIT_TO_STAND;
			isExitSit = false;
		}
	}
	if (isThrow == true && GetTickCount() - attackTime >= SIMON_TIMER_ATTACK)
	{
		isThrow = false;
		if (isExitSit)
		{
			isSit = false;
			y -= SIMON_SIT_TO_STAND;
			isExitSit = false;
		}
	}

	// Simple fall down
	vy += SIMON_GRAVITY * dt;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state != SIMON_STATE_DIE || state != SIMON_STATE_HURT)
		CalcPotentialCollisions(coObjects, coEvents);

	// turn off isOnCheckStair


	// Set idle state
	if (!isSit && !isMoving && !isJump && !isAttack && !isHurt)
	{
		SetState(SIMON_STATE_IDLE);
	}

	// reset untouchable timer if untouchable time has passed
	if (untouchable == 1 && GetTickCount() - untouchable_start > SIMON_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}


	// Handle Aladdin go over screen camera
	float leftCorner = CGame::GetInstance()->mCamera->getX();
	float rightCorner = leftCorner + SCREEN_WIDTH - SIMON_STAND_BBOX_WIDTH;
	// Left corner
	if (x < leftCorner)
	{
		x = 0;
	}
	// Right corner
	else if (x > rightCorner)
	{
		x = rightCorner - 16;
	}

	if (level == 1 && (y >= 365 && y <= 1024)) {
		if (x > 1365 && x <= 1470)x = 1365;
		if (x > 1480 && x <= 1490) x = 1480;
	}
	if (level == 1 && ((x >= 140 && x < 180 && y>800)
		|| (x >= 440 && x < 480 && y>864)
		|| (x >= 740 && x < 780 && y>337)
		|| (x >= 1080 && x < 1120 && y>832)))
	{
		StartUntouchable();
	}
	else {
		untouchable_start = 0;
		untouchable = 0;
	}

	// No collision occured, proceed normally
	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);
		bool willHurt = false;
		// block 
		x += min_tx * dx + nx * 0.4f;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
		y += min_ty * dy + ny * 0.4f;

		/*
		 * Handle collision here
		 */
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<Zombie *>(e->obj))
			{
				Zombie *zombie = dynamic_cast<Zombie *>(e->obj);
				if (zombie->GetState() != GUARDIAN_STATE_DIE && untouchable == 0) {
					// Đặt hướng hurt
					if (e->nx > 0)
						this->nx = -1;
					else if (e->nx < 0)
						this->nx = 1;

					SetState(SIMON_STATE_HURT);
					willHurt = true;
					StartUntouchable();
				}
			}
			else if (dynamic_cast<Soldier *>(e->obj))
			{
				Soldier *zombie = dynamic_cast<Soldier *>(e->obj);
				if (zombie->GetState() != SOLDIER_STATE_DIE) {
					StartUntouchable();
					SetState(SIMON_STATE_UNTOUCHABLE);
				}
			}
			else if (dynamic_cast<Bullet *>(e->obj))
			{
				Bullet *zombie = dynamic_cast<Bullet *>(e->obj);
				//if (zombie->GetState() != SOLDIER_STATE_DIE) {
				//	StartUntouchable();
				//	//SetState(SIMON_STATE_DIE);
				//}
				StartUntouchable();
			}
			else if (dynamic_cast<Rock*>(e->obj))
			{
				// Da cham dat
				// Khong va cham theo phuong ngang
				if (isJump && e->nx == 0 && e->ny < 0)
					isJump = false;
				if (isHurt)
					isHurt = false;
				// Xét va chạm cứng
				if (nx != 0) vx = 0;
				if (ny != 0) vy = 0;
			}
			else if (dynamic_cast<Rope*>(e->obj))
			{
				// Da cham dat
				// Khong va cham theo phuong ngang
				y -= 2;
				ny = -1;
				vy = -0.5;
				/*vy= -1.0;*/
			}
			//else if (dynamic_cast<Spike*>(e->obj))
			//{
			//	// Da cham dat
			//	// Khong va cham theo phuong ngang
			//	if (isJump && e->nx == 0 && e->ny < 0)
			//		isJump = false;

			//	// Xét va chạm cứng
			//	if (nx != 0) vx = 0;
			//	if (ny != 0) vy = 0;
			//}
			else if (dynamic_cast<Ground *>(e->obj) && !willHurt)
			{
				// Da cham dat
				// Khong va cham theo phuong ngang
				if (y<= e->obj->y) {
					if (isJump && e->nx == 0 && e->ny < 0)
						isJump = false;
				if (isHurt)
					isHurt = false;
					// Xét va chạm cứng
					if (nx != 0) vx = 0;
					if (ny != 0) vy = 0;
					dynamic_cast<Ground *>(e->obj)->checkCollision = true;
				}
				else{
					dynamic_cast<Ground *>(e->obj)->checkCollision = false;
				}
			}
			else if (dynamic_cast<CheckPoint *>(e->obj))
			{
				CheckPoint *checkpoint = dynamic_cast<CheckPoint *>(e->obj);
				int type = checkpoint->GetType();

				switch (type)
				{
				case CHECKPOINT_BONUS:
					isBonus = true;
					checkpoint->SetType(CHECKPOINT_BONUS);
					break;
				case CHECKPOINT_LEVELUP:
					if (!isLevelUp)
					{
						x -= nx * 1.0f;
						isLevelUp = true;
						onCheckPointTime = GetTickCount();
					}
					break;
				}
			}
			else if (dynamic_cast<Item *>(e->obj))
			{
				Item *item = dynamic_cast<Item *>(e->obj);
				item->SetEaten();

				int type = item->GetType();
				switch (type)
				{
				case ITEM_HEART:
					IncHeart(5);
					break;
				case ITEM_KNIFE:
					SetCurrentWeapon(ITEM_KNIFE);
					break;
				case ITEM_MONEY:
					IncScore(1000);
					break;
				}
			}
		}
	}
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void Aladdin::Render()
{
	int ani = -1, aniWhip = -1;

	if (state == SIMON_STATE_DIE)
		ani = SIMON_ANI_DIE;
	else
	{
		if (isHurt)
		{
			if (nx > 0)
				ani = SIMON_ANI_HURT_RIGHT;
			else if (nx < 0)
				ani = SIMON_ANI_HURT_LEFT;
		}
		else
			if (isAttack)
			{
				if (nx > 0)
				{
					if (isSit)
					{
						ani = SIMON_ANI_SIT_ATTACK_RIGHT;
					}
					else
					{
						ani = SIMON_ANI_ATTACK_RIGHT;
					}
				}
				else
				{
					if (isSit)
					{
						ani = SIMON_ANI_SIT_ATTACK_LEFT;
					}
					else
					{
						ani = SIMON_ANI_ATTACK_LEFT;
					}
				}
			}
			else if (isThrow)
			{
				if (nx > 0)
				{
					if (isSit)
					{
						ani = SIMON_ANI_SIT_THROW_RIGHT;
					}
					else
					{
						ani = SIMON_ANI_THROW_RIGHT;
					}
				}
				else
				{
					if (isSit)
					{
						ani = SIMON_ANI_SIT_THROW_LEFT;
					}
					else
					{
						ani = SIMON_ANI_THROW_LEFT;
					}
				}
			}
			else if (isJump)
			{
				if (nx > 0)
					ani = SIMON_ANI_JUMP_RIGHT;
				else
					ani = SIMON_ANI_JUMP_LEFT;
			}
			else if (isDoubleJump) {
				ani = SIMON_ANI_DOUBLE_JUMP;
				isJump = false;
			}
		// Right direction
			else if (nx > 0)
			{
				switch (state)
				{
				case SIMON_STATE_SIT:
					if (isThrow)
						ani = SIMON_ANI_SIT_THROW_RIGHT;
					else
						if (isAttack)
							ani = SIMON_ANI_SIT_ATTACK_RIGHT;
						else
							ani = SIMON_ANI_SIT_RIGHT;
					break;
				case SIMON_STATE_WALK:
					ani = SIMON_ANI_WALKING_RIGHT;
					break;
				case SIMON_STATE_DASHING:
					ani = SIMON_ANI_DASHING_RIGHT;
					break;
				case SIMON_STATE_IDLE:
					ani = SIMON_ANI_IDLE_RIGHT;
					break;
				}
			}
		// Left direction
			else if (nx < 0)
			{
				switch (state)
				{
				case SIMON_STATE_ONCHECKSTAIR:
					ani = SIMON_ANI_WALKING_LEFT;
					break;
				case SIMON_STATE_SIT:
					if (isThrow)
						ani = SIMON_ANI_SIT_THROW_LEFT;
					else
						if (isAttack)
							ani = SIMON_ANI_SIT_ATTACK_LEFT;
						else
							ani = SIMON_ANI_SIT_LEFT;
					break;
				case SIMON_STATE_WALK:
					ani = SIMON_ANI_WALKING_LEFT;
					break;
				case SIMON_STATE_DASHING:
					ani = SIMON_ANI_DASHING_LEFT;
					break;
				case SIMON_STATE_IDLE:
					ani = SIMON_ANI_IDLE_LEFT;
					break;
				}
			}
	}

	int alpha = 255;
	if (untouchable) alpha = 0;

	if (ani != -1)
	{
		switch (ani)
		{
		case SIMON_ANI_ATTACK_RIGHT:
		case SIMON_ANI_SIT_ATTACK_RIGHT:
			animations[ani]->Render(x - 16, y, alpha);
			break;
		case SIMON_ANI_HITUP_RLADDER:
			animations[ani]->Render(x - 18, y, alpha);
			break;
		case SIMON_ANI_ATTACK_LEFT:
		case SIMON_ANI_SIT_ATTACK_LEFT:
			animations[ani]->Render(x - 12, y, alpha);
			break;
		default:
			animations[ani]->Render(x, y, alpha);
			break;
		}
	}

	RenderBoundingBox();
}

void Aladdin::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case SIMON_STATE_DIE:
		isDead = true;
		vy = -SIMON_DIE_DEFLECT_SPEED;
		break;
	case SIMON_STATE_HURT:
		isHurt = true;

		vy = -SIMON_DIE_DEFLECT_SPEED;
		if (nx > 0)
		{

			vx = -SIMON_DIE_DEFLECT_SPEED;
		}
		else if (nx < 0)
		{
			vx = SIMON_DIE_DEFLECT_SPEED;
		}

		isAttack = false;
		isJump = false;
		isMoving = false;
		isOnStair = false;
		isOnCheckStairDown = false;
		isOnCheckStairUp = false;
		isSit = false;
		isExitSit = false;
		break;
	case SIMON_STATE_SIT:
		//isOnCheckStair = false;
		isSit = true;
		vx = 0;
		break;
	case SIMON_STATE_WALK:
		//isOnCheckStair = false;
		isMoving = true;
		if (nx == 1.0f)
		{
			vx = SIMON_WALKING_SPEED;
		}
		else if (nx == -1.0f)
		{
			vx = -SIMON_WALKING_SPEED;
		}
		break;
	case SIMON_STATE_IDLE:
		//isOnCheckStair = false;
		vx = 0;
		isMoving = false;
		break;
	}
}

void Aladdin::SetAction(int action)
{
	switch (action)
	{
	case SIMON_ACTION_ATTACK:
		// Ngat tat ca trang thai (tru ngoi)
		// Neu dang nhay toi thi de nguyen van toc, neu khong thi dung lai va danh
		isAttack = true;
		if (!isJump)
			vx = 0;
		isJump = false;
		isMoving = false;
		attackTime = GetTickCount();
		break;
		// Don gian la cho nhay, khong ngat bat ki trang thai nao
		// Them van toc nhay
	case SIMON_ACTION_THROW:
		isThrow = true;
		if (!isJump)
			vx = 0;
		isJump = false;
		isMoving = false;
		attackTime = GetTickCount();
		break;
	case SIMON_ACTION_JUMP:
		isJump = true;
		vy = -SIMON_JUMP_SPEED_Y;
		break;
	}
}

void Aladdin::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	left = x;
	top = y;
	right = x + SIMON_STAND_BBOX_WIDTH;
	bottom = y + SIMON_STAND_BBOX_HEIGHT;

}


