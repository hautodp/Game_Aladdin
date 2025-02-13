﻿#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include "debug.h"
#include "Game.h"
#include "GameObject.h"
#include "Textures.h"
#include "Apple.h"
#include "Aladdin.h"
#include "Ground.h"
#include "define.h"
#include "Item.h"
#include "Pillar.h"
#include "Skeleton.h"
#include "Rock.h"
#include "Map.h"
#include "CheckPoint.h"
#include "Grid.h"
#include "Spike.h"
#include "Dumbbell.h"
#include "Rope.h"
#include "Bat.h"
#include "BG.h"
#include "BossJafar.h"
#include "BossSnake.h"
#include "Fire.h"
#include "FireBullet.h"
#include "JafarBullet.h"
#include "Bone.h"
#include "Stars.h"
#include "ItemApple.h"
#include "GenieFace.h"
#include "GenieJar.h"
#include "ItemSpend.h"
#include "ItemHeart.h"
#include "Sound.h"
static bool isJafarDead = false;
static bool isSnakeDead = false;
ItemApple *itemapple, *itemapple1, *itemapple2, *itemapple3, *itemapple4, *itemapple5;CGame *game;
Aladdin * aladdin;
Fire *fire, *fire1, *fire2, *fire3;
Item *item;
Effect *whipEffect;
Soldier *soldier;
Soldier *soldier1;
Map *mapg;
Apple *apple;
Skeleton *skeleton;
Skeleton *skeleton1;
BG* bg;
CSprite *sprite;
BossJafar *jafar;
BossSnake * snake;
Grid *grid;
bool is1FireAppear = false;
bool is2FireAppear = false;
bool is3FireAppear = false;
bool is4FireAppear = false;
vector<Cell*> currentCell;
static float preDieX;
static float preDieY;
bool lv1 = true;
bool lv2 = false;
bool isCrash = false;
bool isAppleAppear = false;
bool isSecondCrash = false;
bool isStartToEatApple = false;
bool boss = false;
static bool isAlive = true;
// check scene lv2->lv2_1
bool checkScene = false;
bool check1 = false;
bool check = false;
bool isDeadBossSnake = false;
bool checkEndLv2 = false;
bool lvEnd = false;
bool isDeadAladdin = false;
bool checkInfo = false;
vector<ObjectTile*> listObjectsInMap;
ListObject*lsObjs;

//
//check scene lv2_1 ->lv2_2
bool checkScene1 = false;
bool countLoadResourceLv2 = false;

DWORD timer; // load enemy lv2
DWORD timer2;//load enemy boss

DWORD gameTime = 999000;
CSprites * sprites = CSprites::GetInstance();
CAnimations * animations = CAnimations::GetInstance();
CTextures * textures = CTextures::GetInstance();

class CSampleKeyHander : public CKeyEventHandler
{
public:
	virtual void KeyState(BYTE *states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
};

CSampleKeyHander * keyHandler;

void CSampleKeyHander::OnKeyDown(int KeyCode)
{
	// Chet
	if (aladdin->GetState() == SIMON_STATE_DIE) return;

	// Nhay
	if (KeyCode == DIK_Z)
	{
		if (aladdin->isJump == false && aladdin->isSit == false && aladdin->isAttack == false && aladdin->isOnRope == true)
			aladdin->SetAction(SIMON_ACTION_JUMP_ONROPE);
		else
			if (aladdin->isJump == false && aladdin->isSit == false && aladdin->isAttack == false && aladdin->isOnRope == false)
				aladdin->SetAction(SIMON_ACTION_JUMP);
	}

	if (KeyCode == DIK_C)
	{
		
		if (aladdin->isAttack == false && !aladdin->isOnRope)
		{
			aladdin->vy = 0;
			aladdin->SetAction(SIMON_ACTION_ATTACK);
			Sound::getInstance()->loadSound(CUT_MUSIC, "cut");
			Sound::getInstance()->play("cut", false, 1);
			
		}
	}
	
		if (KeyCode == DIK_X && aladdin->GetNumberapples() > 0)
		{
			aladdin->SetCurrentWeapon(809);
			if (aladdin->isAttack == false && !aladdin->isOnRope) {
				aladdin->DescNumberApples();
				int nx = aladdin->nx;
				aladdin->SetAction(SIMON_ACTION_THROW);
				Sound::getInstance()->loadSound(THROW_MUSIC, "throw");
				Sound::getInstance()->play("throw", false, 1);
				switch (aladdin->currentWeapon)
				{
				case ITEM_KNIFE:
					apple = new Apple(aladdin, 2 * SCREEN_WIDTH / 3);
					if (nx > 0)
					{
						apple->SetSpeed(KNIFE_SPEED_X, KNIFE_SPEED_Y);
						apple->AddAnimation(WEAPON_ANI_KNIFE);
					}
					else if (nx < 0)
					{
						apple->SetSpeed(-KNIFE_SPEED_X, KNIFE_SPEED_Y);
						apple->AddAnimation(WEAPON_ANI_KNIFE);
					}
					//knife->SetType(ITEM_KNIFE);
					apple->SetPosition(aladdin->x, aladdin->y);
					apple->appearTime = GetTickCount();
					apple->firstCast = GetTickCount();
					//objects.push_back(knife);
					grid->AddObject(apple);
					break;
				}
			}
		}
}

void CSampleKeyHander::OnKeyUp(int KeyCode)
{
	// Chet
	if (aladdin->GetState() == SIMON_STATE_DIE) return;

	// Len xuong cau thang
	if (KeyCode == DIK_UP)
	{
		if (aladdin->isOnRope)
		{
			aladdin->SetState(SIMON_STATE_ONROPE);
		}
		else
			aladdin->SetState(SIMON_STATE_IDLE);

	}

	// Ngoi
	if (KeyCode == DIK_DOWN)
	{
		if (aladdin->isOnRope) {
			aladdin->SetState(SIMON_STATE_ONROPE);
		}
		else
			if (aladdin->isSit)
			{
				if (!aladdin->isAttack)
				{
					aladdin->isSit = false;
				}
				else
				{
					aladdin->isExitSit = true;
				}
			}
	}
	// Di bo
	else if (KeyCode == DIK_RIGHT || KeyCode == DIK_LEFT)
	{

		aladdin->isMoving = false;
		aladdin->vx = 0;
	}
}

void CSampleKeyHander::KeyState(BYTE *states)
{
	// Chet
	if (aladdin->GetState() == SIMON_STATE_DIE) return;

	// Len xuong cau thang
	if (game->IsKeyDown(DIK_UP))
	{
		if (aladdin->isOnRope)
		{
			aladdin->SetState(SIMON_STATE_ONROPE_CLIMB);
		}
	}

	// Ngoi
	if (game->IsKeyDown(DIK_DOWN))
	{
		if (aladdin->isOnRope) {
			aladdin->SetState(SIMON_STATE_ONROPE_CLIMB_DOWN);
		}
		else
			if (!aladdin->isAttack && !aladdin->isJump)
				aladdin->SetState(SIMON_STATE_SIT);
	}

	// Di bo
	if (game->IsKeyDown(DIK_RIGHT))
	{
		if (!aladdin->isSit && !aladdin->isAttack && !aladdin->isOnRope)
			aladdin->SetState(SIMON_STATE_WALK);
		if (!aladdin->isJump && !aladdin->isAttack)
			aladdin->nx = 1.0f;
	}
	else if (game->IsKeyDown(DIK_LEFT))
	{
		if (!aladdin->isSit && !aladdin->isAttack && !aladdin->isOnRope)
			aladdin->SetState(SIMON_STATE_WALK);
		if (!aladdin->isJump && !aladdin->isAttack)
			aladdin->nx = -1.0f;
	}

	// Neu khong co gi xay ra se dung im
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

/*
	Load all game resources
	In this example: load textures, sprites, animations and mario object

	TO-DO: Improve this function by loading texture,sprite,animation,object from file
*/
void LoadResources()
{
	textures->Add(ITEM_BG_INFO, L"Resources\\author.jpg", D3DCOLOR_XRGB(163, 73, 164));
	textures->Add(ITEM_BG_GAME_OVER, L"Resources\\game-over.jpg", D3DCOLOR_XRGB(163, 73, 164));
	textures->Add(ITEM_BG_NEXT_GAME, L"Resources\\Next.png", D3DCOLOR_XRGB(163, 73, 164));
	textures->Add(ID_TEX_PILLAR_1, L"Resources\\pillar_1.png", D3DCOLOR_XRGB(163, 73, 164));
	textures->Add(ID_TEX_PILLAR_2, L"Resources\\pillar_2.png", D3DCOLOR_XRGB(163, 73, 164));
	textures->Add(ID_TEX_PILLAR_3, L"Resources\\pillar_3.png", D3DCOLOR_XRGB(163, 73, 164));
	textures->Add(ID_TEX_PILLAR_4, L"Resources\\pillar_4.png", D3DCOLOR_XRGB(163, 73, 164));
	textures->Add(ID_TEX_GATE, L"Resources\\gate_exit.png", D3DCOLOR_XRGB(163, 73, 164));
	textures->Add(ITEM_BG_APPLE, L"Resources\\apple.png", D3DCOLOR_XRGB(163, 73, 164));
	textures->Add(ITEM_BG_SPEND, L"Resources\\spend.png", D3DCOLOR_XRGB(163, 73, 164));
	textures->Add(ALADDIN_LIFE, L"Resources\\aladdin_life.png", D3DCOLOR_XRGB(163, 73, 164));
	textures->Add(ITEM_BG_GOD_LIGHT, L"Resources\\god_light.png", D3DCOLOR_XRGB(163, 73, 164));
	textures->Add(ITEM_APPLE, L"Resources\\item-apple.png", D3DCOLOR_XRGB(163, 73, 164));
	textures->Add(ITEM_GENIEFACE, L"Resources\\GenieFace.png", D3DCOLOR_XRGB(163, 73, 164));
	textures->Add(ITEM_GENIEJAR, L"Resources\\genie-jar.png", D3DCOLOR_XRGB(163, 73, 164));
	textures->Add(ITEM_SPEND, L"Resources\\item-spend.png", D3DCOLOR_XRGB(163, 73, 164));
	textures->Add(ITEM_HEART, L"Resources\\item-heart.png", D3DCOLOR_XRGB(163, 73, 164));
	textures->Add(ID_TEX_ALADDIN_ATTACK, L"Resources\\aladdinattack.png", D3DCOLOR_XRGB(255, 0, 255));
	textures->Add(ID_TEX_ALADDIN_STAND, L"Resources\\aladdinstand.png", D3DCOLOR_XRGB(255, 0, 255));
	textures->Add(ID_TEX_ALADDIN_SIT, L"Resources\\aladdinsit.png", D3DCOLOR_XRGB(255, 0, 255));
	textures->Add(ID_TEX_ALADDIN_SIT_THROW, L"Resources\\aladdinsitthrow.png", D3DCOLOR_XRGB(255, 0, 255));
	textures->Add(ID_TEX_KNIFE, L"Resources\\aladdin.png", D3DCOLOR_XRGB(255, 0, 255));
	textures->Add(ID_TEX_ALLADIN, L"Resources\\aladdin.png", D3DCOLOR_XRGB(255, 0, 255));
	textures->Add(ID_TEX_BAT, L"Resources\\bat.png", D3DCOLOR_XRGB(255, 0, 255));
	textures->Add(ID_TEX_BOSS, L"Resources\\boss.png", D3DCOLOR_XRGB(255, 0, 255));
	textures->Add(ID_TEX_SKELETON, L"Resources\\skeleton.png", D3DCOLOR_XRGB(255, 0, 255));
	textures->Add(ID_TEX_ROCK, L"Resources\\rock.png", D3DCOLOR_XRGB(163, 73, 164));
	textures->Add(ID_TEX_TILESET, L"Resources\\tileset.png", D3DCOLOR_XRGB(255, 0, 255));
	textures->Add(ID_TEX_TILESET_2, L"Resources\\tileset1.png", D3DCOLOR_XRGB(255, 0, 255));
	textures->Add(ID_TEX_GUARDIAN, L"Resources\\enemy.png", D3DCOLOR_XRGB(255, 0, 255));
	textures->Add(ID_TEX_SWORD_ATTACK, L"Resources\\swordattack.png", D3DCOLOR_XRGB(255, 0, 255));
	textures->Add(ID_TEX_DUMBBELL, L"Resources\\dumbbell.png", D3DCOLOR_XRGB(163, 73, 164));
	textures->Add(ID_TEX_SPIKE, L"Resources\\spike.png", D3DCOLOR_XRGB(163, 73, 164));
	textures->Add(ID_TEX_SWORD_GUARDIAN, L"Resources\\sword.png", D3DCOLOR_XRGB(255, 0, 255));
	textures->Add(ID_TEX_SWORD_WALK, L"Resources\\swordwalk.png", D3DCOLOR_XRGB(255, 0, 255));
	LPDIRECT3DTEXTURE9 texSword = textures->Get(ID_TEX_SWORD_GUARDIAN);
	LPDIRECT3DTEXTURE9 texSwordAttack = textures->Get(ID_TEX_SWORD_ATTACK);
	LPDIRECT3DTEXTURE9 texSwordWalk = textures->Get(ID_TEX_SWORD_WALK);
	LPDIRECT3DTEXTURE9 texApple = textures->Get(ID_TEX_KNIFE);
	LPDIRECT3DTEXTURE9 texAladdin = textures->Get(ID_TEX_ALLADIN);
	LPDIRECT3DTEXTURE9 texBat = textures->Get(ID_TEX_BAT);
	LPDIRECT3DTEXTURE9 texSkeleton = textures->Get(ID_TEX_SKELETON);
	LPDIRECT3DTEXTURE9 texBoss = textures->Get(ID_TEX_BOSS);
	LPDIRECT3DTEXTURE9 texAladdinAttack = textures->Get(ID_TEX_ALADDIN_ATTACK);
	LPDIRECT3DTEXTURE9 texAladdinStand = textures->Get(ID_TEX_ALADDIN_STAND);
	LPDIRECT3DTEXTURE9 texAladdinSit = textures->Get(ID_TEX_ALADDIN_SIT);
	LPDIRECT3DTEXTURE9 texAladdinSitThrow = textures->Get(ID_TEX_ALADDIN_SIT_THROW);
	#pragma region Addsprite
	sprites->Add(10001, 109, 18, 148, 84, texAladdinStand);		// đứng im phải - 66

	sprites->Add(14002, 1186, 1215, 1229, 1275, texAladdin);		// đi phải
	sprites->Add(14003, 1239, 1216, 1280, 1275, texAladdin);
	sprites->Add(14004, 1290, 1215, 1333, 1275, texAladdin);
	sprites->Add(14005, 1340, 1215, 1394, 1275, texAladdin);
	sprites->Add(14006, 1397, 1215, 1449, 1275, texAladdin);
	sprites->Add(14007, 1451, 1215, 1496, 1275, texAladdin);
	sprites->Add(14008, 1505, 1215, 1547, 1275, texAladdin);
	sprites->Add(14009, 1560, 1215, 1593, 1275, texAladdin);
	sprites->Add(14010, 1608, 1215, 1657, 1275, texAladdin);
	sprites->Add(14011, 1730, 1215, 1788, 1275, texAladdin);
	sprites->Add(14012, 1797, 1215, 1844, 1275, texAladdin);

	sprites->Add(14040, 1132, 1360, 1160, 1447, texAladdin);		// Leo dây phải
	sprites->Add(14041, 1174, 1360, 1200, 1447, texAladdin);
	sprites->Add(14042, 1214, 1360, 1242, 1447, texAladdin);
	sprites->Add(14043, 1252, 1360, 1295, 1447, texAladdin);
	sprites->Add(14044, 1305, 1360, 1338, 1447, texAladdin);
	sprites->Add(14045, 1350, 1360, 1382, 1447, texAladdin);
	sprites->Add(14046, 1392, 1360, 1424, 1447, texAladdin);
	sprites->Add(14047, 1432, 1360, 1468, 1447, texAladdin);
	sprites->Add(14048, 1476, 1360, 1518, 1447, texAladdin);
	sprites->Add(14049, 1525, 1360, 1555, 1447, texAladdin);

	sprites->Add(10004, 1110, 61, 1156, 127, texAladdinAttack);		// đánh phải
	sprites->Add(10005, 1160, 61, 1212, 127, texAladdinAttack);
	sprites->Add(10006, 1220, 61, 1267, 127, texAladdinAttack);
	sprites->Add(10007, 1275, 61, 1352, 127, texAladdinAttack);
	sprites->Add(10008, 1365, 61, 1420, 127, texAladdinAttack);

	sprites->Add(10025, 610, 16, 658, 82, texAladdinSit);		// ngồi đánh phải			
	sprites->Add(10026, 662, 16, 734, 82, texAladdinSit);
	sprites->Add(10027, 738, 16, 832, 82, texAladdinSit);
	sprites->Add(10028, 843, 16, 931, 82, texAladdinSit);
	sprites->Add(10029, 938, 16, 1012, 82, texAladdinSit);


	sprites->Add(10018, 552, 16, 605, 82, texAladdinSit); // ngồi phải

	sprites->Add(12001, 1196, 820, 1248, 890, texAladdin); // nhảy phải
	sprites->Add(12002, 1258, 820, 1313, 890, texAladdin);
	sprites->Add(12003, 1321, 820, 1377, 890, texAladdin);
	sprites->Add(12004, 1506, 820, 1545, 890, texAladdin);
	sprites->Add(12005, 1681, 810, 1713, 907, texAladdin);

	sprites->Add(10054, 436, 67, 471, 130, texAladdin);//bị đánh từ bên trái

	sprites->Add(13001, 1124, 232, 1170, 290, texAladdin); //ném táo phải
	sprites->Add(13002, 1178, 232, 1217, 290, texAladdin);
	sprites->Add(13003, 1227, 232, 1270, 290, texAladdin);
	sprites->Add(13004, 1282, 232, 1330, 290, texAladdin);
	sprites->Add(13005, 1339, 232, 1382, 290, texAladdin);

	sprites->Add(13021, 436, 18, 478, 84, texAladdinSitThrow); // ngồi ném táo phải
	sprites->Add(13022, 484, 18, 528, 84, texAladdinSitThrow);
	sprites->Add(13023, 536, 18, 589, 84, texAladdinSitThrow);
	sprites->Add(13024, 594, 18, 676, 84, texAladdinSitThrow);
	sprites->Add(13025, 685, 18, 745, 84, texAladdinSitThrow);

	sprites->Add(14111, 1128, 2163, 1172, 2212, texAladdin); //bị thương bên phải
	sprites->Add(14112, 1178, 2163, 1234, 2212, texAladdin);
	sprites->Add(14113, 1241, 2163, 1300, 2212, texAladdin);
	sprites->Add(14114, 1310, 2163, 1389, 2212, texAladdin);
	sprites->Add(14115, 1402, 2163, 1464, 2212, texAladdin);
	sprites->Add(14116, 1474, 2163, 1530, 2212, texAladdin);

	LPDIRECT3DTEXTURE9 texAladdin2 = textures->Get(ID_TEX_ALLADIN);
	sprites->Add(10011, 66, 18, 105, 84, texAladdinStand);		// đứng im trái

	sprites->Add(14022, 1008, 1215, 1057, 1275, texAladdin2);		// đi trái
	sprites->Add(14023, 959, 1215, 1003, 1275, texAladdin2);
	sprites->Add(14024, 907, 1215, 952, 1275, texAladdin2);
	sprites->Add(14025, 848, 1215, 904, 1275, texAladdin2);
	sprites->Add(14026, 793, 1215, 846, 1275, texAladdin2);
	sprites->Add(14027, 742, 1215, 789, 1275, texAladdin2);
	sprites->Add(14028, 692, 1215, 736, 1275, texAladdin2);
	sprites->Add(14029, 643, 1215, 685, 1275, texAladdin2);
	sprites->Add(14030, 583, 1215, 634, 1275, texAladdin2);
	sprites->Add(14031, 518, 1215, 575, 1275, texAladdin2);
	sprites->Add(14032, 455, 1215, 512, 1275, texAladdin2);
	sprites->Add(14033, 455, 1215, 512, 1275, texAladdin2);

	sprites->Add(14050, 1080, 1360, 1111, 1447, texAladdin2);		// Leo dây trái
	sprites->Add(14051, 1040, 1360, 1070, 1447, texAladdin2);
	sprites->Add(14052, 998, 1360, 1028, 1447, texAladdin2);
	sprites->Add(14053, 946, 1360, 988, 1447, texAladdin2);
	sprites->Add(14054, 904, 1360, 938, 1447, texAladdin2);
	sprites->Add(14055, 860, 1360, 890, 1447, texAladdin2);
	sprites->Add(14056, 820, 1360, 850, 1447, texAladdin2);
	sprites->Add(14057, 775, 1360, 810, 1447, texAladdin2);
	sprites->Add(14058, 722, 1360, 765, 1447, texAladdin2);
	sprites->Add(14059, 687, 1360, 715, 1447, texAladdin2);

	sprites->Add(10014, 1056, 61, 1102, 127, texAladdinAttack);		// đánh trái				
	sprites->Add(10015, 1000, 61, 1054, 127, texAladdinAttack);
	sprites->Add(10016, 938, 61, 994, 127, texAladdinAttack);
	sprites->Add(10017, 862, 61, 936, 127, texAladdinAttack);
	sprites->Add(10009, 786, 61, 850, 127, texAladdinAttack);

	sprites->Add(10031, 436, 16, 482, 82, texAladdinSit);		// ngồi đánh trái				
	sprites->Add(10032, 360, 16, 430, 82, texAladdinSit);
	sprites->Add(10033, 262, 16, 353, 82, texAladdinSit);
	sprites->Add(10034, 165, 16, 250, 82, texAladdinSit);
	sprites->Add(10035, 82, 16, 155, 82, texAladdinSit);

	sprites->Add(10019, 490, 16, 540, 82, texAladdinSit); // ngồi trái

	sprites->Add(12011, 994, 820, 1047, 890, texAladdin);//nhảy trái
	sprites->Add(12012, 929, 820, 986, 890, texAladdin);
	sprites->Add(12013, 801, 820, 851, 890, texAladdin);
	sprites->Add(12014, 694, 820, 728, 890, texAladdin);
	sprites->Add(12015, 522, 820, 558, 890, texAladdin);

	sprites->Add(13011, 1070, 232, 1115, 290, texAladdin); // ném táo trái
	sprites->Add(13012, 1023, 232, 1067, 290, texAladdin);
	sprites->Add(13013, 973, 232, 1014, 290, texAladdin);
	sprites->Add(13014, 912, 232, 959, 290, texAladdin);
	sprites->Add(13015, 861, 232, 902, 290, texAladdin);

	sprites->Add(13031, 380, 18, 422, 84, texAladdinSitThrow); //ngồi ném táo trái
	sprites->Add(13032, 332, 18, 372, 84, texAladdinSitThrow);
	sprites->Add(13033, 272, 18, 322, 84, texAladdinSitThrow);
	sprites->Add(13034, 183, 18, 266, 84, texAladdinSitThrow);
	sprites->Add(13035, 115, 18, 175, 84, texAladdinSitThrow);

	sprites->Add(14121, 1070, 2163, 1113, 2212, texAladdin); //bị thương bên trái
	sprites->Add(14122, 1008, 2163, 1062, 2212, texAladdin);
	sprites->Add(14123, 940, 2163, 998, 2212, texAladdin);
	sprites->Add(14124, 852, 2163, 928, 2212, texAladdin);
	sprites->Add(14125, 778, 2163, 838, 2212, texAladdin);
	sprites->Add(14126, 712, 2163, 768, 2212, texAladdin);

	sprites->Add(10055, 12, 70, 41, 130, texAladdin2);//bị đánh từ bên phải

	sprites->Add(10099, 180, 237, 240, 264, texAladdin);		// chết 

	sprites->Add(10110, 106, 144, 149, 189, texAladdinSit);//nhay danh phai

	sprites->Add(10111, 59, 144, 102, 189, texAladdinSit); // nhay danh trai

	sprites->Add(10120, 282, 96, 302, 136, texAladdin); // double nhay 
	sprites->Add(10121, 305, 96, 327, 136, texAladdin);
	sprites->Add(10122, 327, 96, 349, 136, texAladdin);

	sprites->Add(10130, 237, 79, 276, 123, texAladdinSit);// dashing left
	sprites->Add(10131, 310, 79, 348, 123, texAladdinSit);// dashing right

	sprites->Add(10140, 233, 224, 258, 268, texAladdin); // knifeing right
	sprites->Add(10141, 202, 224, 227, 268, texAladdin);	//knifeing left
	sprites->Add(1200, 1482, 19, 1511, 71, texApple);	//khien
	#pragma endregion

	#pragma region ENEMY

	//Guardian Enemy
	LPDIRECT3DTEXTURE9 guardian = textures->Get(ID_TEX_GUARDIAN);
	sprites->Add(16006, 499, 16, 594, 87, texSwordAttack); // guardian tấn công phải
	sprites->Add(16005, 604, 16, 694, 87, texSwordAttack);
	sprites->Add(16004, 700, 16, 774, 87, texSwordAttack);
	sprites->Add(16003, 780, 16, 850, 87, texSwordAttack);
	sprites->Add(16002, 855, 16, 925, 87, texSwordAttack);
	sprites->Add(16001, 928, 16, 986, 87, texSwordAttack);
	sprites->Add(16007, 943, 630, 985, 702, guardian);
	sprites->Add(16008, 888, 630, 930, 702, guardian);
	sprites->Add(16009, 824, 630, 878, 702, guardian);
	sprites->Add(16010, 701, 630, 790, 702, guardian);
	sprites->Add(16110, 600, 630, 685, 702, guardian);

	sprites->Add(16011, 393, 16, 489, 87, texSwordAttack); // guardian tấn công trái
	sprites->Add(16012, 294, 16, 385, 87, texSwordAttack);
	sprites->Add(16013, 216, 16, 293, 87, texSwordAttack);
	sprites->Add(16014, 140, 16, 212, 87, texSwordAttack);
	sprites->Add(16015, 2, 16, 60, 87, texSwordAttack);
	sprites->Add(16016, 5, 630, 50, 702, guardian);
	sprites->Add(16017, 60, 630, 106, 702, guardian);
	sprites->Add(16018, 116, 630, 170, 702, guardian);
	sprites->Add(16019, 180, 630, 292, 702, guardian);
	sprites->Add(16020, 312, 630, 390, 702, guardian);

	sprites->Add(16031, 382, 797, 448, 860, guardian); //guardian bi thuong trai
	sprites->Add(16032, 290, 797, 363, 860, guardian);
	sprites->Add(16033, 210, 797, 280, 860, guardian);
	sprites->Add(16034, 138, 797, 202, 860, guardian);
	sprites->Add(16035, 72, 797, 134, 860, guardian);
	sprites->Add(16036, 6, 797, 60, 860, guardian);

	sprites->Add(16021, 545, 797, 610, 860, guardian); //guardian bi thuong phai
	sprites->Add(16022, 629, 797, 700, 860, guardian);
	sprites->Add(16023, 712, 797, 780, 860, guardian);
	sprites->Add(16024, 790, 797, 852, 860, guardian);
	sprites->Add(16025, 858, 797, 920, 860, guardian);
	sprites->Add(16026, 932, 797, 986, 860, guardian);

	sprites->Add(16041, 475, 15, 523, 85, texSword); //guardian đứng phải
	sprites->Add(16042, 528, 15, 582, 85, texSword);
	sprites->Add(16043, 585, 15, 644, 85, texSword);
	sprites->Add(16044, 646, 15, 705, 85, texSword);
	sprites->Add(16045, 707, 15, 748, 85, texSword);
	sprites->Add(16046, 750, 15, 800, 85, texSword);

	sprites->Add(16051, 380, 15, 428, 85, texSword); //guardian đứng trái
	sprites->Add(16052, 324, 15, 375, 85, texSword);
	sprites->Add(16053, 260, 15, 320, 85, texSword);
	sprites->Add(16054, 202, 15, 257, 85, texSword);
	sprites->Add(16055, 160, 15, 200, 85, texSword);
	sprites->Add(16056, 108, 15, 155, 85, texSword);

	sprites->Add(16061, 560, 21, 610, 92, texSwordWalk); // guardian di phai
	sprites->Add(16062, 614, 21, 660, 92, texSwordWalk);
	sprites->Add(16063, 666, 21, 709, 92, texSwordWalk);
	sprites->Add(16064, 712, 21, 764, 92, texSwordWalk);
	sprites->Add(16065, 768, 21, 820, 92, texSwordWalk);
	sprites->Add(16066, 824, 21, 880, 92, texSwordWalk);
	sprites->Add(16067, 885, 21, 938, 92, texSwordWalk);
	sprites->Add(16068, 942, 21, 991, 92, texSwordWalk);

	sprites->Add(16071, 382, 21, 430, 92, texSwordWalk); // guardian di trai
	sprites->Add(16072, 333, 21, 377, 92, texSwordWalk);
	sprites->Add(16073, 282, 21, 326, 92, texSwordWalk);
	sprites->Add(16074, 230, 21, 280, 92, texSwordWalk);
	sprites->Add(16075, 172, 21, 226, 92, texSwordWalk);
	sprites->Add(16076, 112, 21, 168, 92, texSwordWalk);
	sprites->Add(16077, 55, 21, 107, 92, texSwordWalk);
	sprites->Add(16078, 2, 21, 50, 92, texSwordWalk);
	//Soldier
	LPDIRECT3DTEXTURE9 soldierTex = textures->Get(ID_TEX_GUARDIAN); 
	sprites->Add(17001, 570, 92, 609, 156, soldierTex);	//solider danh phai
	sprites->Add(17002, 625, 92, 701, 156, soldierTex);
	sprites->Add(17003, 704, 92, 788, 156, soldierTex);
	sprites->Add(17004, 792, 92, 885, 156, soldierTex);
	sprites->Add(17005, 889, 92, 937, 156, soldierTex);
	sprites->Add(17006, 943, 92, 986, 156, soldierTex);

	sprites->Add(17011, 381, 92, 426, 156, soldierTex); //soldier danh trai
	sprites->Add(17012, 292, 92, 372, 156, soldierTex);
	sprites->Add(17013, 203, 92, 290, 156, soldierTex);
	sprites->Add(17014, 111, 92, 203, 156, soldierTex);
	sprites->Add(17015, 53, 92, 101, 156, soldierTex);
	sprites->Add(17016, 9, 92, 50, 156, soldierTex);

	sprites->Add(17021, 941, 166, 983, 233, soldierTex); // soldier đứng phải

	sprites->Add(17031, 6, 166, 52, 233, soldierTex); //soldier đứng trái

	sprites->Add(17041, 516, 162, 560, 234, soldierTex); //soldier bi danh phai
	sprites->Add(17042, 574, 162, 614, 234, soldierTex);
	sprites->Add(17043, 682, 162, 724, 234, soldierTex);
	sprites->Add(17044, 736, 162, 780, 234, soldierTex);
	sprites->Add(17045, 790, 162, 830, 234, soldierTex);
	sprites->Add(17046, 840, 162, 878, 234, soldierTex);
	sprites->Add(17047, 892, 162, 932, 234, soldierTex);

	sprites->Add(17051, 434, 162, 478, 234, soldierTex); //soldier bi danh trai
	sprites->Add(17052, 378, 162, 418, 234, soldierTex);
	sprites->Add(17053, 268, 162, 310, 234, soldierTex);
	sprites->Add(17054, 210, 162, 256, 234, soldierTex);
	sprites->Add(17055, 160, 162, 202, 234, soldierTex);
	sprites->Add(17056, 114, 162, 152, 234, soldierTex);
	sprites->Add(17057, 58, 162, 98, 234, soldierTex);

	sprites->Add(17061, 591, 7, 630, 76, soldierTex); //soldier di phai
	sprites->Add(17062, 636, 7, 685, 76, soldierTex);
	sprites->Add(17063, 690, 7, 735, 76, soldierTex);
	sprites->Add(17064, 740, 7, 782, 76, soldierTex);
	sprites->Add(17065, 784, 7, 830, 76, soldierTex);
	sprites->Add(17066, 836, 7, 886, 76, soldierTex);
	sprites->Add(17067, 894, 7, 940, 76, soldierTex);
	sprites->Add(17068, 945, 7, 992, 76, soldierTex);

	sprites->Add(17071, 362, 7, 398, 76, soldierTex); //soldier di trai
	sprites->Add(17072, 308, 7, 358, 76, soldierTex);
	sprites->Add(17073, 258, 7, 300, 76, soldierTex);
	sprites->Add(17074, 212, 7, 252, 76, soldierTex);
	sprites->Add(17075, 164, 7, 210, 76, soldierTex);
	sprites->Add(17076, 108, 7, 156, 76, soldierTex);
	sprites->Add(17077, 54, 7, 98, 76, soldierTex);
	sprites->Add(17078, 2, 7, 48, 76, soldierTex);
	//Bat enemy
	sprites->Add(17100, 4, 8, 18, 34, texBat); //bat đứng yên

	sprites->Add(17101, 8, 50, 34, 78, texBat); //bat tấn công
	sprites->Add(17102, 36, 50, 54, 78, texBat);
	sprites->Add(17103, 55, 50, 85, 78, texBat);
	sprites->Add(17104, 89, 57, 123, 86, texBat);
	sprites->Add(17105, 124, 64, 139, 94, texBat);
	sprites->Add(17106, 143, 50, 163, 78, texBat);
	sprites->Add(17107, 164, 60, 199, 88, texBat);

	sprites->Add(17201, 880, 124, 932, 204, texSkeleton); // skeleton đứng phải
	sprites->Add(17202, 943, 124, 995, 204, texSkeleton);
	sprites->Add(17203, 999, 124, 1062, 204, texSkeleton);
	sprites->Add(17204, 1072, 124, 1136, 204, texSkeleton);
	sprites->Add(17205, 1149, 124, 1222, 204, texSkeleton);
	sprites->Add(17206, 1248, 124, 1325, 204, texSkeleton);
	sprites->Add(17207, 1350, 124, 1440, 204, texSkeleton);
	sprites->Add(17208, 1460, 124, 1522, 204, texSkeleton);

	sprites->Add(17211, 806, 124, 856, 204, texSkeleton); //skeleton đứng trái
	sprites->Add(17212, 744, 124, 794, 204, texSkeleton);
	sprites->Add(17213, 674, 124, 736, 204, texSkeleton);
	sprites->Add(17214, 600, 124, 666, 204, texSkeleton);
	sprites->Add(17215, 514, 124, 586, 204, texSkeleton);
	sprites->Add(17216, 412, 124, 490, 204, texSkeleton);
	sprites->Add(17217, 294, 124, 386, 204, texSkeleton);
	sprites->Add(17218, 214, 124, 276, 204, texSkeleton);

	sprites->Add(17221, 32, 258, 48, 280, texSkeleton); //bone
	sprites->Add(17222, 52, 258, 66, 280, texSkeleton);
	sprites->Add(17223, 70, 258, 90, 280, texSkeleton);
	//boss jafar
	sprites->Add(17301, 869, 8, 935, 81, texBoss); //jafar dung phai

	sprites->Add(17311, 790, 8, 856, 81, texBoss); //jafar dung trai

	//jafar danh phai
	sprites->Add(17321, 942, 8, 1000, 81, texBoss);
	sprites->Add(17322, 1006, 8, 1064, 81, texBoss);
	sprites->Add(17323, 1069, 8, 1122, 81, texBoss);
	sprites->Add(17324, 1132, 8, 1200, 81, texBoss);
	sprites->Add(17325, 1206, 8, 1288, 81, texBoss);
	sprites->Add(17326, 1302, 8, 1388, 81, texBoss);

	sprites->Add(17331, 726, 8, 785, 81, texBoss); //jafar danh trai
	sprites->Add(17332, 664, 8, 720, 81, texBoss);
	sprites->Add(17333, 605, 8, 656, 81, texBoss);
	sprites->Add(17334, 526, 8, 592, 81, texBoss);
	sprites->Add(17335, 436, 8, 520, 81, texBoss);
	sprites->Add(17336, 258, 8, 328, 81, texBoss);

	sprites->Add(17341, 942, 90, 1001, 161, texBoss); //jafar bi thuong phai
	sprites->Add(17342, 1008, 90, 1068, 161, texBoss);
	sprites->Add(17343, 1072, 90, 1138, 161, texBoss);
	sprites->Add(17344, 1278, 90, 1338, 161, texBoss);

	sprites->Add(17351, 722, 90, 782, 161, texBoss); //jafar bi thuong trai
	sprites->Add(17352, 658, 90, 716, 161, texBoss);
	sprites->Add(17353, 585, 90, 654, 161, texBoss);
	sprites->Add(17354, 386, 90, 450, 161, texBoss);

#pragma region Boss Snake
	sprites->Add(17401, 869, 290, 940, 376, texBoss); //snake tan cong phai
	sprites->Add(17402, 950, 290, 1015, 376, texBoss);
	sprites->Add(17403, 1020, 290, 1088, 376, texBoss);
	sprites->Add(17404, 1102, 290, 1172, 376, texBoss);
	sprites->Add(17405, 1182, 290, 1252, 376, texBoss);
	sprites->Add(17406, 1262, 290, 1330, 376, texBoss);
	sprites->Add(17407, 1340, 290, 1412, 376, texBoss);
	sprites->Add(17408, 1422, 290, 1492, 376, texBoss);
	sprites->Add(17409, 1505, 290, 1570, 376, texBoss);
	sprites->Add(17410, 1578, 290, 1640, 376, texBoss);
	sprites->Add(17411, 1648, 290, 1722, 376, texBoss);

	sprites->Add(17421, 785, 290, 856, 376, texBoss); //snake tan cong trai
	sprites->Add(17422, 710, 290, 775, 376, texBoss);
	sprites->Add(17423, 636, 290, 704, 376, texBoss);
	sprites->Add(17424, 550, 290, 624, 376, texBoss);
	sprites->Add(17425, 470, 290, 544, 376, texBoss);
	sprites->Add(17426, 392, 290, 465, 376, texBoss);
	sprites->Add(17427, 310, 290, 384, 376, texBoss);
	sprites->Add(17428, 232, 290, 306, 376, texBoss);
	sprites->Add(17429, 154, 290, 220, 376, texBoss);
	sprites->Add(17430, 82, 290, 150, 376, texBoss);
	sprites->Add(17431, 2, 290, 78, 376, texBoss);

	sprites->Add(17441, 920, 177, 968, 261, texBoss); //snake bi danh phai
	sprites->Add(17442, 984, 177, 1034, 261, texBoss);
	sprites->Add(17443, 1045, 177, 1115, 261, texBoss);
	sprites->Add(17444, 984, 177, 1034, 261, texBoss);
	sprites->Add(17445, 920, 177, 968, 261, texBoss);


	sprites->Add(17451, 758, 177, 806, 261, texBoss); //snake bi danh trai
	sprites->Add(17452, 690, 177, 742, 261, texBoss);
	sprites->Add(17453, 612, 177, 678, 261, texBoss);
	sprites->Add(17454, 690, 177, 742, 261, texBoss);
	sprites->Add(17455, 758, 177, 806, 261, texBoss);


	sprites->Add(17501, 870, 482, 944, 534, texBoss); //fire bullet right
	sprites->Add(17502, 958, 482, 1040, 534, texBoss);
	sprites->Add(17503, 1046, 482, 1146, 534, texBoss);
	sprites->Add(17504, 1160, 482, 1248, 534, texBoss);
	sprites->Add(17505, 1260, 482, 1340, 534, texBoss);
	sprites->Add(17506, 1350, 482, 1424, 534, texBoss);
	sprites->Add(17507, 1430, 482, 1496, 534, texBoss);
	sprites->Add(17508, 1506, 482, 1572, 534, texBoss);


	sprites->Add(17511, 780, 482, 854, 534, texBoss); //fire bullet left
	sprites->Add(17512, 688, 482, 768, 534, texBoss);
	sprites->Add(17513, 580, 482, 680, 534, texBoss);
	sprites->Add(17514, 478, 482, 568, 534, texBoss);
	sprites->Add(17515, 384, 482, 464, 534, texBoss);
	sprites->Add(17516, 302, 482, 376, 534, texBoss);
	sprites->Add(17517, 230, 482, 296, 534, texBoss);
	sprites->Add(17518, 154, 482, 220, 534, texBoss);


	sprites->Add(17601, 876, 386, 900, 439, texBoss); //fire
	sprites->Add(17602, 904, 386, 930, 439, texBoss);
	sprites->Add(17603, 940, 386, 966, 439, texBoss);
	sprites->Add(17604, 974, 386, 1000, 439, texBoss);
	sprites->Add(17605, 1012, 386, 1048, 439, texBoss);

	sprites->Add(17701, 1058, 422, 1089, 438, texBoss); //stars
	sprites->Add(17702, 1098, 422, 1126, 438, texBoss);
	sprites->Add(17703, 1130, 422, 1160, 438, texBoss); 

	sprites->Add(17711, 1166, 422, 1189, 438, texBoss); //broken stars
	sprites->Add(17712, 1195, 422, 1219, 438, texBoss);
	sprites->Add(17713, 1238, 422, 1262, 438, texBoss);
#pragma endregion

	LPDIRECT3DTEXTURE9 texMisc11 = textures->Get(ID_TEX_KNIFE);
	sprites->Add(40023, 0, 0, 32, 18, texMisc11);

	LPDIRECT3DTEXTURE9 texRock1 = textures->Get(ID_TEX_ROCK); // rock
	sprites->Add(40031, 0, 4, 30, 20, texRock1);
	sprites->Add(40032, 36, 4, 66, 20, texRock1);
	sprites->Add(40033, 72, 4, 105, 21, texRock1);
	sprites->Add(40034, 111, 4, 147, 24, texRock1);
	sprites->Add(40035, 154, 4, 194, 26, texRock1);

	

	LPDIRECT3DTEXTURE9 texGate = textures->Get(ID_TEX_GATE); //gate
	sprites->Add(410040, 0, 0, 78, 212, texGate);

	LPDIRECT3DTEXTURE9 texSpike = textures->Get(ID_TEX_SPIKE); // spike - cây chông
	sprites->Add(40041, 1, 2, 25, 24, texSpike);
	sprites->Add(40042, 29, 2, 52, 24, texSpike);
	sprites->Add(40043, 57, 2, 81, 30, texSpike);
	sprites->Add(40044, 88, 2, 122, 34, texSpike);
	sprites->Add(40045, 126, 2, 170, 35, texSpike);
	sprites->Add(40046, 173, 2, 222, 38, texSpike);

	LPDIRECT3DTEXTURE9 texDumbbell = textures->Get(ID_TEX_DUMBBELL); // dumbbell - quả tạ
	sprites->Add(40050, 3, 2, 19, 69, texDumbbell);
	sprites->Add(40051, 23, 2, 38, 69, texDumbbell);
	sprites->Add(40052, 43, 2, 58, 69, texDumbbell);
	sprites->Add(40053, 63, 2, 78, 69, texDumbbell);
	sprites->Add(40054, 83, 2, 98, 69, texDumbbell);
	sprites->Add(40055, 103, 2, 120, 69, texDumbbell);
	sprites->Add(40056, 124, 2, 143, 69, texDumbbell);
	sprites->Add(40057, 147, 2, 169, 69, texDumbbell);
	sprites->Add(40058, 173, 2, 201, 69, texDumbbell);
	sprites->Add(40059, 204, 2, 233, 69, texDumbbell);
	sprites->Add(40060, 237, 2, 269, 69, texDumbbell);
	sprites->Add(40061, 272, 2, 305, 69, texDumbbell);
	sprites->Add(40062, 308, 2, 342, 69, texDumbbell);
	sprites->Add(40063, 345, 2, 380, 69, texDumbbell);
	sprites->Add(40064, 383, 2, 416, 69, texDumbbell);


	LPDIRECT3DTEXTURE9 texItemApple = textures->Get(ITEM_APPLE); // item-apple
	sprites->Add(40070, 0, 0, 12, 13, texItemApple);

	LPDIRECT3DTEXTURE9 texGenieFace = textures->Get(ITEM_GENIEFACE); // genie-face
	sprites->Add(40071, 0, 0, 45, 53, texGenieFace);
	sprites->Add(40072, 56, 0, 103, 53, texGenieFace);

	LPDIRECT3DTEXTURE9 texGenieJar = textures->Get(ITEM_GENIEJAR); // geniejar
	sprites->Add(40073, 2, 2, 23, 36, texGenieJar);

	LPDIRECT3DTEXTURE9 texSpend = textures->Get(ITEM_SPEND); // item-spend
	sprites->Add(40074, 1, 2, 18, 20, texSpend);
	sprites->Add(40075, 23, 3, 41, 20, texSpend);
	sprites->Add(40076, 46, 1, 65, 20, texSpend);
	sprites->Add(40077, 72, 2, 101, 23, texSpend);
	sprites->Add(40078, 106, 2, 132, 20, texSpend);
	sprites->Add(40079, 136, 4, 154, 20, texSpend);
	sprites->Add(40080, 159, 4, 177, 20, texSpend);
	sprites->Add(40081, 188, 4, 205, 20, texSpend);

	LPDIRECT3DTEXTURE9 texHeart = textures->Get(ITEM_HEART); // item-heart
	sprites->Add(40082, 1, 1, 20, 26, texHeart);
	sprites->Add(40083, 23, 3, 41, 27, texHeart);
	sprites->Add(40084, 44, 5, 61, 27, texHeart);
	sprites->Add(40085, 63, 5, 81, 27, texHeart);
	sprites->Add(40086, 83, 3, 103, 27, texHeart);
	sprites->Add(40087, 105, 4, 123, 28, texHeart);
	sprites->Add(40088, 124, 6, 141, 28, texHeart);
	sprites->Add(40089, 143, 6, 161, 28, texHeart);

	LPANIMATION ani;

	#pragma region AladdinAnimation
	ani = new CAnimation(100);	//đứng phải
	ani->Add(10001);
	animations->Add(400, ani);

	ani = new CAnimation(100);	//đứng trái
	ani->Add(10011);
	animations->Add(401, ani);

	ani = new CAnimation(100);	//đi phải
	ani->Add(14002);
	ani->Add(14003);
	ani->Add(14004);
	ani->Add(14005);
	ani->Add(14006);
	ani->Add(14007);
	ani->Add(14008);
	ani->Add(14009);
	ani->Add(14010);
	ani->Add(14011);
	ani->Add(14012);
	animations->Add(500, ani);

	ani = new CAnimation(100);	//đi trái
	ani->Add(14022);
	ani->Add(14023);
	ani->Add(14024);
	ani->Add(14025);
	ani->Add(14026);
	ani->Add(14027);
	ani->Add(14028);
	ani->Add(14029);
	ani->Add(14030);
	ani->Add(14031);
	ani->Add(14032);
	ani->Add(14033);
	animations->Add(501, ani);

	ani = new CAnimation(180);	//Leo phải
	ani->Add(14040);
	ani->Add(14041);
	ani->Add(14042);
	ani->Add(14043);
	ani->Add(14044);
	ani->Add(14045);
	ani->Add(14046);
	ani->Add(14047);
	ani->Add(14048);
	ani->Add(14049);
	animations->Add(502, ani);

	ani = new CAnimation(180);	//Leo trái
	ani->Add(14050);
	ani->Add(14051);
	ani->Add(14052);
	ani->Add(14053);
	ani->Add(14054);
	ani->Add(14055);
	ani->Add(14056);
	ani->Add(14057);
	ani->Add(14058);
	ani->Add(14059);
	animations->Add(503, ani);

	ani = new CAnimation(80); //đánh phải
	ani->Add(10004);
	ani->Add(10005);
	ani->Add(10006);
	ani->Add(10007);
	ani->Add(10008);
	animations->Add(402, ani);

	ani = new CAnimation(80);//đánh trái
	ani->Add(10014);
	ani->Add(10015);
	ani->Add(10016);
	ani->Add(10017);
	ani->Add(10009);
	animations->Add(403, ani);

	ani = new CAnimation(100); //ngồi phải
	ani->Add(10018);
	animations->Add(404, ani);

	ani = new CAnimation(100);//ngồi trái
	ani->Add(10019);
	animations->Add(405, ani);

	ani = new CAnimation(100);//nhảy phải
	ani->Add(12001);
	ani->Add(12002);
	ani->Add(12003);
	ani->Add(12004);
	ani->Add(12005);
	animations->Add(406, ani);

	ani = new CAnimation(100);//nhảy trái
	ani->Add(12011);
	ani->Add(12012);
	ani->Add(12013);
	ani->Add(12014);
	ani->Add(12015);
	animations->Add(407, ani);

	ani = new CAnimation(150);//ngồi đánh phải
	ani->Add(10025);
	ani->Add(10026);
	ani->Add(10027);
	ani->Add(10028);
	ani->Add(10029);
	animations->Add(410, ani);

	ani = new CAnimation(150);//ngồi đánh trái
	ani->Add(10031);
	ani->Add(10032);
	ani->Add(10033);
	ani->Add(10034);
	ani->Add(10035);
	animations->Add(411, ani);

	ani = new CAnimation(150);//trên thang phải đánh phải
	ani->Add(10034);
	ani->Add(10035);
	ani->Add(10036);
	animations->Add(412, ani);

	ani = new CAnimation(150);//trên thang phải đánh trái
	ani->Add(10037);
	ani->Add(10038);
	ani->Add(10039);
	animations->Add(413, ani);

	ani = new CAnimation(150);//trên thang trái đánh phải
	ani->Add(10040);
	ani->Add(10041);
	ani->Add(10042);
	animations->Add(414, ani);

	ani = new CAnimation(150);//trên thang trái đánh trái
	ani->Add(10043);
	ani->Add(10044);
	ani->Add(10045);
	animations->Add(415, ani);


	ani = new CAnimation(100);//lên thang phải
	ani->Add(10046);
	ani->Add(10047);
	animations->Add(416, ani);

	ani = new CAnimation(100);//xuống thang phải
	ani->Add(10048);
	ani->Add(10049);
	animations->Add(417, ani);

	ani = new CAnimation(100);//lên thang trái
	ani->Add(10050);
	ani->Add(10051);
	animations->Add(418, ani);

	ani = new CAnimation(100);//xuống thang trái
	ani->Add(10052);
	ani->Add(10053);
	animations->Add(419, ani);

	ani = new CAnimation(100);//đứng im bên phải thang phải
	ani->Add(10047);
	animations->Add(420, ani);

	ani = new CAnimation(100);//đứng im bên phải thang trái
	ani->Add(10049);
	animations->Add(421, ani);

	ani = new CAnimation(100);//đứng im bên trái thang phải
	ani->Add(10051);
	animations->Add(422, ani);

	ani = new CAnimation(100);//đứng im bên trái thang phải
	ani->Add(10053);
	animations->Add(423, ani);

	ani = new CAnimation(100);//bị đánh từ bên phải
	ani->Add(14111);
	ani->Add(14112);
	ani->Add(14113);
	ani->Add(14114);
	ani->Add(14115);
	ani->Add(14116);
	animations->Add(424, ani);

	ani = new CAnimation(100);//bị đánh từ bên trái
	ani->Add(14121);
	ani->Add(14122);
	ani->Add(14123);
	ani->Add(14124);
	ani->Add(14125);
	ani->Add(14126);
	animations->Add(425, ani);

	ani = new CAnimation(100);	//chết	
	ani->Add(10099);
	animations->Add(599, ani);


	ani = new CAnimation(100); //nhay danh phai
	ani->Add(10110);
	animations->Add(1600, ani);

	ani = new CAnimation(100); //nhay danh trai
	ani->Add(10111);
	animations->Add(1601, ani);

	ani = new CAnimation(100); //double nhay
	ani->Add(10120);
	ani->Add(10121);
	ani->Add(10122);
	animations->Add(1602, ani);

	ani = new CAnimation(100); //dashing right
	ani->Add(10131);
	animations->Add(1603, ani);

	ani = new CAnimation(100); //dashing left
	ani->Add(10130);
	animations->Add(1604, ani);


	ani = new CAnimation(100); //knifeing right
	ani->Add(10140);
	animations->Add(1605, ani);

	ani = new CAnimation(100); //knifeing left
	ani->Add(10141);
	animations->Add(1606, ani);

	ani = new CAnimation(100);
	ani->Add(1200);
	animations->Add(1607, ani); //nem khien

	ani = new CAnimation(100);
	ani->Add(44444);
	animations->Add(1608, ani); //nem dan of zombie

	ani = new CAnimation(100);
	ani->Add(45555);
	animations->Add(1609, ani); //nem dan of soldier

	ani = new CAnimation(100); // ném táo phải
	ani->Add(13001);
	ani->Add(13002);
	ani->Add(13003);
	ani->Add(13004);
	ani->Add(13005);
	animations->Add(1610, ani);

	ani = new CAnimation(100); // ném táo trái
	ani->Add(13011);
	ani->Add(13012);
	ani->Add(13013);
	ani->Add(13014);
	ani->Add(13015);
	animations->Add(1611, ani);

	ani = new CAnimation(100); // ngồi ném táo phải
	ani->Add(13021);
	ani->Add(13022);
	ani->Add(13023);
	ani->Add(13024);
	ani->Add(13025);
	animations->Add(1612, ani);

	ani = new CAnimation(100); // ngồi ném táo trái
	ani->Add(13031);
	ani->Add(13032);
	ani->Add(13033);
	ani->Add(13034);
	ani->Add(13035);
	animations->Add(1613, ani);

	ani = new CAnimation(100);  // dung tren day phai
	ani->Add(14040);
	animations->Add(1614, ani);

	ani = new CAnimation(100);  // dung tren day trai
	ani->Add(14050);
	animations->Add(1615, ani);

	ani = new CAnimation(100);//Boss di phai
	ani->Add(41000);
	ani->Add(41001);
	ani->Add(41002);
	animations->Add(666, ani);

	ani = new CAnimation(100);//Boss di trai
	ani->Add(41003);
	ani->Add(41004);
	ani->Add(41005);
	animations->Add(667, ani);

	ani = new CAnimation(100);//dan bay
	ani->Add(40099);
	animations->Add(668, ani);

	ani = new CAnimation(100);//boss dung trai
	ani->Add(44010);
	animations->Add(670, ani);

	ani = new CAnimation(100);//boss dung phai
	ani->Add(44011);
	animations->Add(671, ani);

	ani = new CAnimation(100);//boss drop trai
	ani->Add(44020);
	animations->Add(672, ani);

	ani = new CAnimation(100);//boss drop phai
	ani->Add(44021);
	animations->Add(673, ani);
	#pragma endregion

	#pragma region ObjectAnimation

	ani = new CAnimation(100);	//Guardian tan cong phai
	ani->Add(16001);
	ani->Add(16002);
	ani->Add(16003);
	ani->Add(16004);
	ani->Add(16005);
	ani->Add(16006);
	ani->Add(16007);
	ani->Add(16008);
	ani->Add(16009);
	ani->Add(16010);
	ani->Add(16110);
	animations->Add(551, ani);

	ani = new CAnimation(100);	//Guardian tan cong trai
	ani->Add(16011);
	ani->Add(16012);
	ani->Add(16013);
	ani->Add(16014);
	ani->Add(16015);
	ani->Add(16016);
	ani->Add(16017);
	ani->Add(16018);
	ani->Add(16019);
	ani->Add(16020);
	animations->Add(552, ani);

	ani = new CAnimation(100);	//Guardian bi thuong phai
	ani->Add(16021);
	ani->Add(16022);
	ani->Add(16023);
	ani->Add(16024);
	ani->Add(16025);
	ani->Add(16026);
	animations->Add(553, ani);

	ani = new CAnimation(100);	//Guardian bi thuong trai
	ani->Add(16031);
	ani->Add(16032);
	ani->Add(16033);
	ani->Add(16034);
	ani->Add(16035);
	ani->Add(16036);
	animations->Add(554, ani);

	ani = new CAnimation(100);	//Guardian đứng phải
	ani->Add(16041);
	ani->Add(16042);
	ani->Add(16043);
	ani->Add(16044);
	ani->Add(16045);
	ani->Add(16046);
	animations->Add(555, ani);

	ani = new CAnimation(100);	//Guardian đứng trái
	ani->Add(16051);
	ani->Add(16052);
	ani->Add(16053);
	ani->Add(16054);
	ani->Add(16055);
	ani->Add(16056);
	animations->Add(556, ani);

	ani = new CAnimation(100);	//Guardian di  phai
	ani->Add(16061);
	ani->Add(16062);
	ani->Add(16063);
	ani->Add(16064);
	ani->Add(16065);
	ani->Add(16066);
	ani->Add(16067);
	ani->Add(16068);
	animations->Add(557, ani);

	ani = new CAnimation(100);	//Guardian di trai
	ani->Add(16071);
	ani->Add(16072);
	ani->Add(16073);
	ani->Add(16074);
	ani->Add(16075);
	ani->Add(16076);
	ani->Add(16077);
	ani->Add(16078);
	animations->Add(558, ani);

	ani = new CAnimation(100);	//soldier danh phai
	ani->Add(17001);
	ani->Add(17002);
	ani->Add(17003);
	ani->Add(17004);
	ani->Add(17005);
	ani->Add(17006);
	animations->Add(561, ani);

	ani = new CAnimation(100);	//soldier danh trai
	ani->Add(17011);
	ani->Add(17012);
	ani->Add(17013);
	ani->Add(17014);
	ani->Add(17015);
	ani->Add(17016);
	animations->Add(562, ani);

	ani = new CAnimation(100);	//soldier dung phai
	ani->Add(17021);
	animations->Add(563, ani);

	ani = new CAnimation(100);	//soldier dung trai
	ani->Add(17031);
	animations->Add(564, ani);

	ani = new CAnimation(150);	//soldier bi danh phai
	ani->Add(17041);
	ani->Add(17042);
	ani->Add(17043);
	ani->Add(17044);
	ani->Add(17045);
	ani->Add(17046);
	ani->Add(17047);
	animations->Add(565, ani);

	ani = new CAnimation(150);	//soldier bi danh trai
	ani->Add(17051);
	ani->Add(17052);
	ani->Add(17053);
	ani->Add(17054);
	ani->Add(17055);
	ani->Add(17056);
	ani->Add(17057);
	animations->Add(566, ani);

	ani = new CAnimation(150);	//soldier di phai
	ani->Add(17061);
	ani->Add(17062);
	ani->Add(17063);
	ani->Add(17064);
	ani->Add(17065);
	ani->Add(17066);
	ani->Add(17067);
	ani->Add(17068);
	animations->Add(567, ani);

	ani = new CAnimation(150);	//soldier di trai
	ani->Add(17071);
	ani->Add(17072);
	ani->Add(17073);
	ani->Add(17074);
	ani->Add(17075);
	ani->Add(17076);
	ani->Add(17077);
	ani->Add(17078);
	animations->Add(568, ani);

	ani = new CAnimation(100);	//bat đứng đợi
	ani->Add(17100);
	animations->Add(605, ani);

	ani = new CAnimation(100);	//bat tấn công
	ani->Add(17101);
	ani->Add(17102);
	ani->Add(17103);
	ani->Add(17104);
	ani->Add(17105);
	ani->Add(17106);
	ani->Add(17107);
	animations->Add(606, ani);

	ani = new CAnimation(100);	//skeleton dung phai
	ani->Add(17201);
	ani->Add(17202);
	ani->Add(17203);
	ani->Add(17204);
	ani->Add(17205);
	ani->Add(17206);
	ani->Add(17207);
	ani->Add(17208);
	animations->Add(607, ani);

	ani = new CAnimation(100);	//skeleton dung trai
	ani->Add(17211);
	ani->Add(17212);
	ani->Add(17213);
	ani->Add(17214);
	ani->Add(17215);
	ani->Add(17216);
	ani->Add(17217);
	ani->Add(17218);
	animations->Add(608, ani);

	ani = new CAnimation(100); //bone
	ani->Add(17221);
	ani->Add(17222);
	ani->Add(17223);
	animations->Add(700, ani);

	ani = new CAnimation(100); // jafar dung phai
	ani->Add(17301);
	animations->Add(609, ani);

	ani = new CAnimation(100); // jafar dung trai
	ani->Add(17311);
	animations->Add(610, ani);

	ani = new CAnimation(100); // jafar danh phai
	ani->Add(17321);
	ani->Add(17322);
	ani->Add(17323);
	ani->Add(17324);
	ani->Add(17325);
	ani->Add(17326);
	animations->Add(611, ani);


	ani = new CAnimation(100); // jafar danh trai
	ani->Add(17331);
	ani->Add(17332);
	ani->Add(17333);
	ani->Add(17334);
	ani->Add(17335);
	ani->Add(17336);
	animations->Add(612, ani);

	ani = new CAnimation(100); // jafar bi danh phai
	ani->Add(17341);
	ani->Add(17342);
	ani->Add(17343);
	ani->Add(17344);
	animations->Add(613, ani);

	ani = new CAnimation(100); // jafar bi danh trai
	ani->Add(17351);
	ani->Add(17352);
	ani->Add(17353);
	ani->Add(17354);
	animations->Add(614, ani);

	//snake
	ani = new CAnimation(100); // snake danh phai
	ani->Add(17401);
	ani->Add(17402);
	ani->Add(17403);
	ani->Add(17404);
	ani->Add(17405);
	ani->Add(17406);
	ani->Add(17407);
	ani->Add(17408);
	ani->Add(17409);
	ani->Add(17410);
	ani->Add(17411);
	animations->Add(615, ani);

	ani = new CAnimation(100); // snake danh trai
	ani->Add(17421);
	ani->Add(17422);
	ani->Add(17423);
	ani->Add(17424);
	ani->Add(17425);
	ani->Add(17426);
	ani->Add(17427);
	ani->Add(17428);
	ani->Add(17429);
	ani->Add(17430);
	ani->Add(17431);
	animations->Add(616, ani);

	ani = new CAnimation(100); // snake bi danh phai
	ani->Add(17441);
	ani->Add(17442);
	ani->Add(17443);
	ani->Add(17444);
	ani->Add(17445);
	animations->Add(617, ani);

	ani = new CAnimation(100); // snake bi danh trai
	ani->Add(17451);
	ani->Add(17452);
	ani->Add(17453);
	ani->Add(17454);
	ani->Add(17455);
	animations->Add(618, ani);

	ani = new CAnimation(100); // fire bullet right
	ani->Add(17501);
	ani->Add(17502);
	ani->Add(17503);
	ani->Add(17504);
	ani->Add(17505);
	ani->Add(17506);
	ani->Add(17507);
	ani->Add(17508);
	animations->Add(619, ani);

	ani = new CAnimation(100); // fire bullet left
	ani->Add(17511);
	ani->Add(17512);
	ani->Add(17513);
	ani->Add(17514);
	ani->Add(17515);
	ani->Add(17516);
	ani->Add(17517);
	ani->Add(17518);
	animations->Add(620, ani);

	ani = new CAnimation(100); // fire 
	ani->Add(17601);
	ani->Add(17602);
	ani->Add(17603);
	ani->Add(17604);
	ani->Add(17605);
	animations->Add(621, ani);

	ani = new CAnimation(100); // stars 
	ani->Add(17701);
	ani->Add(17702);
	ani->Add(17703);
	animations->Add(622, ani);


	ani = new CAnimation(100); // broken stars 
	ani->Add(17711);
	ani->Add(17712);
	ani->Add(17713);
	animations->Add(623, ani);

	ani = new CAnimation(100);	//đất
	ani->Add(20001);
	animations->Add(601, ani);

	ani = new CAnimation(0); //knife
	ani->Add(40023);
	animations->Add(809, ani);

	ani = new CAnimation(0); // bậc thang 1
	ani->Add(40031);
	ani->Add(40032);
	ani->Add(40033);
	ani->Add(40034);
	ani->Add(40035);
	animations->Add(990, ani);


	ani = new CAnimation(0); // bậc thang 2
	ani->Add(40035);
	ani->Add(40034);
	ani->Add(40033);
	ani->Add(40032);
	ani->Add(40031);
	animations->Add(991, ani);

	ani = new CAnimation(0); // spike 1
	ani->Add(40041);
	ani->Add(40042);
	ani->Add(40043);
	ani->Add(40044);
	ani->Add(40045);
	ani->Add(40046);
	animations->Add(992, ani);

	ani = new CAnimation(0); // spike 2
	ani->Add(40046);
	ani->Add(40045);
	ani->Add(40044);
	ani->Add(40043);
	ani->Add(40042);
	ani->Add(40041);
	animations->Add(993, ani);

	ani = new CAnimation(0); // dumbbell 1
	ani->Add(40050);
	ani->Add(40051);
	ani->Add(40052);
	ani->Add(40053);
	ani->Add(40054);
	ani->Add(40055);
	ani->Add(40056);
	ani->Add(40057);
	ani->Add(40058);
	ani->Add(40059);
	ani->Add(40060);
	ani->Add(40061);
	ani->Add(40062);
	ani->Add(40063);
	ani->Add(40064);
	ani->Add(40063);
	ani->Add(40062);
	ani->Add(40061);
	ani->Add(40060);
	ani->Add(40059);
	ani->Add(40058);
	ani->Add(40057);
	ani->Add(40056);
	ani->Add(40055);
	ani->Add(40054);
	ani->Add(40053);
	ani->Add(40052);
	ani->Add(40051);
	animations->Add(994, ani);

	ani = new CAnimation(0); // dumbbell 1
	ani->Add(40057);
	ani->Add(40058);
	ani->Add(40059);
	ani->Add(40060);
	ani->Add(40061);
	ani->Add(40062);
	ani->Add(40063);
	ani->Add(40064);
	ani->Add(40063);
	ani->Add(40062);
	ani->Add(40061);
	ani->Add(40060);
	ani->Add(40059);
	ani->Add(40058);
	ani->Add(40057);
	ani->Add(40056);
	ani->Add(40055);
	ani->Add(40054);
	ani->Add(40053);
	ani->Add(40052);
	ani->Add(40051);
	ani->Add(40050);
	ani->Add(40051);
	ani->Add(40052);
	ani->Add(40053);
	ani->Add(40054);
	ani->Add(40055);
	ani->Add(40056);
	animations->Add(995, ani);

	ani = new CAnimation(0); // dumbbell 1
	ani->Add(40064);
	ani->Add(40063);
	ani->Add(40062);
	ani->Add(40061);
	ani->Add(40060);
	ani->Add(40059);
	ani->Add(40058);
	ani->Add(40057);
	ani->Add(40056);
	ani->Add(40055);
	ani->Add(40054);
	ani->Add(40053);
	ani->Add(40052);
	ani->Add(40051);
	ani->Add(40050);
	ani->Add(40051);
	ani->Add(40052);
	ani->Add(40053);
	ani->Add(40054);
	ani->Add(40055);
	ani->Add(40056);
	ani->Add(40057);
	ani->Add(40058);
	ani->Add(40059);
	ani->Add(40060);
	ani->Add(40061);
	ani->Add(40062);
	ani->Add(40063);
	animations->Add(996, ani);


	ani = new CAnimation(0);// item-apple
	ani->Add(40070);
	animations->Add(2000, ani);

	ani = new CAnimation(200);// genieface
	ani->Add(40071);
	ani->Add(40072);
	animations->Add(2001, ani);


	ani = new CAnimation(0);// geniejar
	ani->Add(40073);
	animations->Add(2002, ani);

	ani = new CAnimation(100);// item-spend
	ani->Add(40074);
	ani->Add(40075);
	ani->Add(40076); 
	ani->Add(40077);
	ani->Add(40078);
	ani->Add(40079);
	ani->Add(40080);
	ani->Add(40081);
	animations->Add(2003, ani);

	ani = new CAnimation(100);// item-heart
	ani->Add(40082);
	ani->Add(40083);
	ani->Add(40084);
	ani->Add(40085);
	ani->Add(40086);
	ani->Add(40087);
	ani->Add(40088);
	ani->Add(40089);
	animations->Add(2004, ani);

	#pragma endregion

	#pragma region aladdin
	aladdin = new Aladdin();
	aladdin->AddAnimation(400);	// đứng phải	
	aladdin->AddAnimation(401);	//đứng trái	
	aladdin->AddAnimation(500);	// đi phải
	aladdin->AddAnimation(501);	//đi trái
	aladdin->AddAnimation(402);	//đánh phải 
	aladdin->AddAnimation(403);	//đánh trái
	aladdin->AddAnimation(404);	//ngồi phải
	aladdin->AddAnimation(405);	//ngồi trái
	aladdin->AddAnimation(406);	//nhảy phải 
	aladdin->AddAnimation(407);	//nhảy trái
	aladdin->AddAnimation(410);	//ngồi đánh phải 
	aladdin->AddAnimation(411);	//ngồi đánh trái
	aladdin->AddAnimation(412);	//trên thang phải đánh phải
	aladdin->AddAnimation(413);	//trên thang phải đánh trái
	aladdin->AddAnimation(414);	//trên thang trái đánh phải
	aladdin->AddAnimation(415);	//trên thang trái đánh trái
	aladdin->AddAnimation(416);	//lên thang phải
	aladdin->AddAnimation(417);	//xuống thang phải
	aladdin->AddAnimation(418);	//lên thang trái
	aladdin->AddAnimation(419);	//xuống thang trái
	aladdin->AddAnimation(420);	//đứng im bên phải thang phải
	aladdin->AddAnimation(421);	//đứng im bên phải thang trái
	aladdin->AddAnimation(422);	//đứng im bên trái thang phải
	aladdin->AddAnimation(423);	//đứng im bên trái thang trái
	aladdin->AddAnimation(424);	//bị đánh từ bên phải
	aladdin->AddAnimation(425);	//bị đánh từ bên trái
	aladdin->AddAnimation(599);	//chết
	aladdin->AddAnimation(1600); // nhay danh phai
	aladdin->AddAnimation(1601); // nhay danh trai
	aladdin->AddAnimation(1602); // double nhay
	aladdin->AddAnimation(1603); // dashing right
	aladdin->AddAnimation(1604); // dashing left
	aladdin->AddAnimation(1605);	//knifeing right
	aladdin->AddAnimation(1606);	//knifeing left
	aladdin->AddAnimation(1610); //ném táo phải
	aladdin->AddAnimation(1611); //ném táo trái
	aladdin->AddAnimation(1612);	//ngồi ném táo phải
	aladdin->AddAnimation(1613); // ngồi ném táo trái
	aladdin->AddAnimation(502);	//Leo dây phải
	aladdin->AddAnimation(503); // Leo dây trái
	aladdin->AddAnimation(1614);//dung day phai
	aladdin->AddAnimation(1615);//dung day trai
			//objects.push_back(aladdin);

		// khởi tạo grid
	grid->InitList(MAX_WIDTH_LV1, MAX_HEIGHT_LV1);
	aladdin->SetPosition(10, 850);
	aladdin->name = "aladdin";
	grid->AddObject(aladdin);

	#pragma endregion

	#pragma region Ground
		listObjectsInMap = lsObjs->getObjectsInFile("Resources\\objects.txt");
		grid->InitObjectsAtCell("Resources\\o_grid.txt");

		if (grid->listCells.size() > 0) {
			for (int j = 0; j < grid->listCells.size(); j++) {
				if (grid->listCells[j]->listIdObject.size() > 0) {
					for (int k = 0; k < grid->listCells[j]->listIdObject.size(); k++) {
						if (listObjectsInMap.size() > 0) {
							int p = grid->listCells[j]->listIdObject[k];
							for (int i = p-1; i < listObjectsInMap.size(); i++)
							{
								int l = listObjectsInMap[i]->id;
								if ((p) == l) {
									// Gắn đường đi - Ground
									Ground *ground = new Ground();
									if (listObjectsInMap[i]->name == "Ground")
									{
										ground->SetPosition(listObjectsInMap[i]->x, listObjectsInMap[i]->y);
										//ground->cellNumber = grid->listCells[j]->getId();
										//ground->id= listObjectsInMap[i]->id;
										/*grid->AddObject(ground);*/
										grid->listCells[j]->AddObject(ground);
									}

									// Gắn Item-apple
									ItemApple *itemAplle = new ItemApple();
									if (listObjectsInMap[i]->name == "Apple")
									{
										
										itemAplle->SetPosition(listObjectsInMap[i]->x, listObjectsInMap[i]->y);
										//ground->cellNumber = grid->listCells[j]->getId();
										//ground->id= listObjectsInMap[i]->id;
										
										itemAplle->AddAnimation(2000);
										grid->AddObject(itemAplle);
									}

									// Gắn GenieFace
									GenieFace *genieface = new GenieFace();
									if (listObjectsInMap[i]->name == "GenieFace")
									{

										genieface->SetPosition(listObjectsInMap[i]->x, listObjectsInMap[i]->y);
										//ground->cellNumber = grid->listCells[j]->getId();
										//ground->id= listObjectsInMap[i]->id;
										
										genieface->AddAnimation(2001);
										grid->AddObject(genieface);
									}

									// Gắn GenieJar
									GenieJar *geniejar = new GenieJar();
									if (listObjectsInMap[i]->name == "GenieJar")
									{

										geniejar->SetPosition(listObjectsInMap[i]->x, listObjectsInMap[i]->y);
										//ground->cellNumber = grid->listCells[j]->getId();
										//ground->id= listObjectsInMap[i]->id;										
										geniejar->AddAnimation(2002);
										grid->AddObject(geniejar);
									}

									// Gắn item-spend
									ItemSpend *itemSpend = new ItemSpend();
									if (listObjectsInMap[i]->name == "Spend")
									{

										itemSpend->SetPosition(listObjectsInMap[i]->x, listObjectsInMap[i]->y);
										//ground->cellNumber = grid->listCells[j]->getId();
										//ground->id= listObjectsInMap[i]->id;
										/*grid->AddObject(ground);*/
										itemSpend->AddAnimation(2003);
										/*grid->listCells[j]->AddObject(itemSpend);*/
										grid->AddObject(itemSpend);
									}

									// Gắn item-spend
									ItemHeart *itemHeart = new ItemHeart();
									if (listObjectsInMap[i]->name == "Heart")
									{

										itemHeart->SetPosition(listObjectsInMap[i]->x, listObjectsInMap[i]->y);
										//ground->cellNumber = grid->listCells[j]->getId();
										//ground->id= listObjectsInMap[i]->id;
										
										itemHeart->AddAnimation(2004);
										grid->AddObject(itemHeart);
									}

									Rope *rope = new Rope();
									if (listObjectsInMap[i]->name == "Rope")
									{
										rope->SetPositionAndHeight(listObjectsInMap[i]->x, listObjectsInMap[i]->y, listObjectsInMap[i]->height);
										/*grid->AddObject(rope);*/
										/*rope->cellNumber = grid->listCells[j]->getId();*/
										grid->listCells[j]->AddObject(rope);
									}

									// Gắn viên đá - Rock
									Rock*rock = new Rock();
									if (listObjectsInMap[i]->name == "Rock")
									{
										int id = listObjectsInMap[i]->id;
										if ((id == 160) || (id==161) || (id==163) || (id == 168) || (id == 265) || (id == 266) || (id == 470)
											|| (id == 353) || (id == 355) || (id == 357) || (id == 359) || (id == 361) || (id == 363)
											|| (id == 456) || (id == 458) || (id == 460) || (id == 462) || (id == 464) || (id == 466) || (id == 468))
										{
											rock->AddAnimation(990);
											rock->name = "rock1";
										}
										else if((id == 162)||(id==164)||(id==167) || (id == 262)||(id==263) || (id == 362)
											||(id==264) || (id == 352) || (id == 354) || (id == 356) || (id == 360)
											|| (id == 457) || (id == 459) || (id == 461) || (id == 463) || (id == 465) || (id == 467))
										{
											rock->AddAnimation(991);
											rock->name = "rock2";
										}

										rock->SetPosition(listObjectsInMap[i]->x, listObjectsInMap[i]->y);
										/*grid->AddObject(rock);*/
										//rock->id = id;
										//rock->cellNumber = grid->listCells[j]->getId();
										grid->listCells[j]->AddObject(rock);
									}

									// Gắn mũi nhọn - Spike
									Spike*spike = new Spike();
									if (listObjectsInMap[i]->name == "Spike")
									{
										int id = listObjectsInMap[i]->id;
										if (id == 153 || id == 154 ||id==165||id==166|| id==267 || id == 469) {
											spike->AddAnimation(992);
											spike->name = "spike1";
										}
										else if (id == 155 || id == 156|| id==219 || id==220 || id == 358
											|| id == 451 || id == 452 || id == 453 || id == 454 || id == 455) {
											spike->AddAnimation(993);
											spike->name = "spike2";
										}

										spike->SetPosition(listObjectsInMap[i]->x, listObjectsInMap[i]->y);
										/*grid->AddObject(spike);*/
										//spike->id = id;
										//spike->cellNumber = grid->listCells[j]->getId();
										grid->listCells[j]->AddObject(spike);
									}

									//// Gắn quả tạ - Dumbbell
									Dumbbell*dumbbell = new Dumbbell();
									if (listObjectsInMap[i]->name == "Dumbbell")
									{
										int id = listObjectsInMap[i]->id;
										if ((id == 157)|| (id == 215)|| (id == 217)) {
											dumbbell->AddAnimation(994);
											dumbbell->name = "dumbbell1";
										}
										else if ((id == 158) || (id == 216) || (id == 218)) {
											dumbbell->AddAnimation(995);
											dumbbell->name = "dumbbell2";
										}
										else if ((id == 159)) {
											dumbbell->AddAnimation(996);
											dumbbell->name = "dumbbell3";
										}

										dumbbell->SetPosition(listObjectsInMap[i]->x, listObjectsInMap[i]->y);
										/*grid->AddObject(dumbbell);*/
										//dumbbell->id = id;
										//dumbbell->cellNumber = grid->listCells[j]->getId();
										grid->listCells[j]->AddObject(dumbbell);
									}

									break;
								}
							}
						}
					}
				}
			}
		}

	#pragma endregion

	#pragma region Zombie
		Zombie *zombie = new Zombie();
		zombie->nx = -1;
		zombie->AddAnimation(551);
		zombie->AddAnimation(552);
		zombie->AddAnimation(553);
		zombie->AddAnimation(554);
		zombie->AddAnimation(555);
		zombie->AddAnimation(556);
		zombie->AddAnimation(557);
		zombie->AddAnimation(558);
		zombie->SetPosition(1250, 935);
		zombie->SetState(GUARDIAN_STATE_WALK);
		//objects.push_back(zombie);
		grid->AddObject(zombie);

		Zombie *zombie1 = new Zombie();
		zombie1->nx = -1;
		zombie1->AddAnimation(551);
		zombie1->AddAnimation(552);
		zombie1->AddAnimation(553);
		zombie1->AddAnimation(554);
		zombie1->AddAnimation(555);
		zombie1->AddAnimation(556);
		zombie1->AddAnimation(557);
		zombie1->AddAnimation(558);
		zombie1->SetPosition(1900, 155);
		zombie1->SetState(GUARDIAN_STATE_IDLE);
		//objects.push_back(zombie);
		grid->AddObject(zombie1);

		Zombie *zombie2 = new Zombie();
		zombie2->nx = 1;
		zombie2->AddAnimation(551);
		zombie2->AddAnimation(552);
		zombie2->AddAnimation(553);
		zombie2->AddAnimation(554);
		zombie2->AddAnimation(555);
		zombie2->AddAnimation(556);
		zombie2->AddAnimation(557);
		zombie2->AddAnimation(558);
		zombie2->SetPosition(440, 345);
		zombie2->SetState(GUARDIAN_STATE_IDLE);
		//objects.push_back(zombie);
		grid->AddObject(zombie2);

		Zombie *zombie3 = new Zombie();
		zombie3->nx = 1;
		zombie3->AddAnimation(551);
		zombie3->AddAnimation(552);
		zombie3->AddAnimation(553);
		zombie3->AddAnimation(554);
		zombie3->AddAnimation(555);
		zombie3->AddAnimation(556);
		zombie3->AddAnimation(557);
		zombie3->AddAnimation(558);
		zombie3->SetPosition(1720, 480);
		zombie3->SetState(GUARDIAN_STATE_IDLE);
		//objects.push_back(zombie);
		grid->AddObject(zombie3);


		Zombie *zombie4 = new Zombie();
		zombie4->nx = 1;
		zombie4->AddAnimation(551);
		zombie4->AddAnimation(552);
		zombie4->AddAnimation(553);
		zombie4->AddAnimation(554);
		zombie4->AddAnimation(555);
		zombie4->AddAnimation(556);
		zombie4->AddAnimation(557);
		zombie4->AddAnimation(558);
		zombie4->SetPosition(1550, 934);
		zombie4->SetState(GUARDIAN_STATE_WALK);
		//objects.push_back(zombie);
		grid->AddObject(zombie4);


#pragma endregion

	#pragma region Bat
		Bat *bat1 = new Bat();
		bat1->AddAnimation(605);
		bat1->AddAnimation(606);
		bat1->SetPosition(375, 828);
		bat1->SetState(BAT_STATE_WAIT);
		grid->AddObject(bat1);

		Bat *bat2 = new Bat();
		bat2->AddAnimation(605);
		bat2->AddAnimation(606);
		bat2->SetPosition(940, 765);
		bat2->SetState(BAT_STATE_WAIT);
		grid->AddObject(bat2);

		Bat *bat3 = new Bat();
		bat3->AddAnimation(605);
		bat3->AddAnimation(606);
		bat3->SetPosition(1372, 848);
		bat3->SetState(BAT_STATE_WAIT);
		grid->AddObject(bat3);
		#pragma endregion

	#pragma region Skeleton
			skeleton = new Skeleton();
			skeleton->nx = -1;
			skeleton->AddAnimation(607);
			skeleton->AddAnimation(608);
			skeleton->SetPosition(400, 930);
			skeleton->SetState(SKELETON_STATE_IDLE);
			grid->AddObject(skeleton);
			skeleton1 = new Skeleton();
			skeleton1->nx = -1;
			skeleton1->AddAnimation(607);
			skeleton1->AddAnimation(608);
			skeleton1->SetPosition(750, 750);
			skeleton1->SetState(SKELETON_STATE_IDLE);
			grid->AddObject(skeleton1);
	#pragma endregion
		jafar = new BossJafar();
		snake = new BossSnake();
		//fire = new Fire();
		//fire1 = new Fire();
	#pragma region Soldier
			soldier = new Soldier();
			soldier->nx = 1;
			soldier->AddAnimation(563);
			soldier->AddAnimation(564);
			soldier->AddAnimation(561);
			soldier->AddAnimation(562);
			soldier->AddAnimation(565);
			soldier->AddAnimation(566);
			soldier->AddAnimation(567);
			soldier->AddAnimation(568);
			soldier->SetPosition(10, 200);
			soldier->SetState(SOLDIER_STATE_IDLE);
			//objects.push_back(zombie1);
			grid->AddObject(soldier);

			Soldier *soldier1 = new Soldier();
			soldier1->nx = -1;
			soldier1->AddAnimation(563);
			soldier1->AddAnimation(564);
			soldier1->AddAnimation(561);
			soldier1->AddAnimation(562);
			soldier1->AddAnimation(565);
			soldier1->AddAnimation(566);
			soldier1->AddAnimation(567);
			soldier1->AddAnimation(568);
			soldier1->SetPosition(1550, 275);
			soldier1->SetState(SOLDIER_STATE_WALK);
			//objects.push_back(zombie1);
			grid->AddObject(soldier1);

	#pragma endregion

	#pragma region Fire
		//Fire *fire = new Fire();
		//fire->AddAnimation(621);
		//fire->SetPosition(100, 950);
		//fire->SetState(FIRE_STATE_BURNING);
		//grid->AddObject(fire);

#pragma endregion

 	#pragma region CheckPoint
	CheckPoint *checkPoint;
	checkPoint = new CheckPoint();
	checkPoint->SetType(CHECKPOINT_LEVELUP);
	checkPoint->SetPosition(2071, 62);
	//checkPoint->cellNumber = grid->listCells[3]->getId();
	grid->listCells[3]->AddObject(checkPoint);
	/*grid->AddObject(checkPoint);*/
#pragma endregion
	LPDIRECT3DDEVICE9 d3ddv = game->GetDirect3DDevice();
	bg = new BG();
	bg->Initialize(d3ddv);
}

void LoadResourceLv2()
{
	listObjectsInMap = lsObjs->getObjectsInFile("Resources\\objects_lv2.txt");
	grid->InitObjectsAtCell("Resources\\object_grid_lv2.txt");
	
	if (grid->listCells.size() > 0) {
		for (int j = 0; j < grid->listCells.size() > 0; j++) {
			if (grid->listCells[j]->listIdObject.size() > 0) {
				for (int k = 0; k < grid->listCells[j]->listIdObject.size(); k++) {
					if (listObjectsInMap.size() > 0) {
						for (int i = 0; i < listObjectsInMap.size(); i++)
						{
							int p = grid->listCells[j]->listIdObject[k];
							int l = listObjectsInMap[i]->id;
							if ((p) == l) {
								// Gắn đường đi - Ground
								Ground *ground = new Ground();
								if (listObjectsInMap[i]->name == "Ground")
								{
									ground->SetPosition(listObjectsInMap[i]->x, listObjectsInMap[i]->y);
									/*ground->cellNumber = grid->listCells[j]->getId();*/
									/*grid->AddObject(ground);*/
									grid->listCells[j]->AddObject(ground);
								}
								break;
							}
						}
					}
				}
			}
		}
	}
}
float Aladdin::XforGet = 0;
float Aladdin::YforGet = 0;
float Aladdin::VyfoGet = 0;
void Update(DWORD dt)
{
	float x, y;
	aladdin->GetPosition(x, y);
	Aladdin::XforGet = x;
	Aladdin::YforGet = y;
	Aladdin::VyfoGet = aladdin->vy;
	#pragma region Resource
		if (lv1 == true)
		{
			// Lên cấp
			if (aladdin->isLevelUp) {
				//aladdin->SetState(SIMON_STATE_WALK);

				grid->ReleaseList();
				/*delete bg;*/

				lv2 = true;
				lv1 = false;
				aladdin->isLevelUp = false;
				/*aladdin->isLevelUp = false;
				aladdin->SetState(SIMON_STATE_IDLE);*/

			}
			if (skeleton1->GetHP() <= 0 && isSecondCrash == false && lv2 == false) {
				Bone *bone = new Bone();
				bone->AddAnimation(700);
				bone->SetSpeed(BONE_SPEED_ATTACK_X, -BONE_SPEED_ATTACK_X);
				bone->SetState(BONE_STATE_ATTACK);
				bone->SetPosition(preDieX,preDieY);
				grid->AddObject(bone);

				Bone *bone1 = new Bone();
				bone1->AddAnimation(700);
				bone1->SetSpeed(0, -BONE_SPEED_ATTACK_X);
				bone1->SetState(BONE_STATE_ATTACK);
				bone1->SetPosition(preDieX, preDieY);
				grid->AddObject(bone1);

				Bone *bone2 = new Bone();
				bone2->AddAnimation(700);
				bone2->SetSpeed(-BONE_SPEED_ATTACK_X, -BONE_SPEED_ATTACK_Y);
				bone2->SetState(BONE_STATE_ATTACK);
				bone2->SetPosition(preDieX, preDieY);
				grid->AddObject(bone2);

				Bone *bone3 = new Bone();
				bone3->AddAnimation(700);
				bone3->SetSpeed(BONE_SPEED_ATTACK_X, 0);
				bone3->SetState(BONE_STATE_ATTACK);
				bone3->SetPosition(preDieX, preDieY);
				grid->AddObject(bone3);

				Bone *bone4 = new Bone();
				bone4->AddAnimation(700);
				bone4->SetSpeed(BONE_SPEED_ATTACK_X, BONE_SPEED_ATTACK_X);
				bone4->SetState(BONE_STATE_ATTACK);
				bone4->SetPosition(preDieX, preDieY);
				grid->AddObject(bone4);

				Bone *bone5 = new Bone();
				bone5->AddAnimation(700);
				bone5->SetSpeed(-BONE_SPEED_ATTACK_X, 0);
				bone5->SetState(BONE_STATE_ATTACK);
				bone5->SetPosition(preDieX, preDieY);
				grid->AddObject(bone5);

				Bone *bone6 = new Bone();
				bone6->AddAnimation(700);
				bone6->SetSpeed(-BONE_SPEED_ATTACK_X, BONE_SPEED_ATTACK_Y);
				bone6->SetState(BONE_STATE_ATTACK);
				bone6->SetPosition(preDieX, preDieY);
				grid->AddObject(bone6);

				isSecondCrash = true;
			}
			if (skeleton->GetHP() <= 0 && isCrash == false && lv2 == false) {
				Bone *bone = new Bone();
				bone->AddAnimation(700);
				bone->SetSpeed(BONE_SPEED_ATTACK_X, -BONE_SPEED_ATTACK_X);
				bone->SetState(BONE_STATE_ATTACK);
				bone->SetPosition(preDieX, preDieY);
				grid->AddObject(bone);

				Bone *bone1 = new Bone();
				bone1->AddAnimation(700);
				bone1->SetSpeed(0, -BONE_SPEED_ATTACK_X);
				bone1->SetState(BONE_STATE_ATTACK);
				bone1->SetPosition(preDieX, preDieY);
				grid->AddObject(bone1);

				Bone *bone2 = new Bone();
				bone2->AddAnimation(700);
				bone2->SetSpeed(-BONE_SPEED_ATTACK_X, -BONE_SPEED_ATTACK_Y);
				bone2->SetState(BONE_STATE_ATTACK);
				bone2->SetPosition(preDieX, preDieY);
				grid->AddObject(bone2);

				Bone *bone3 = new Bone();
				bone3->AddAnimation(700);
				bone3->SetSpeed(BONE_SPEED_ATTACK_X, 0);
				bone3->SetState(BONE_STATE_ATTACK);
				bone3->SetPosition(preDieX, preDieY);
				grid->AddObject(bone3);

				Bone *bone4 = new Bone();
				bone4->AddAnimation(700);
				bone4->SetSpeed(BONE_SPEED_ATTACK_X, BONE_SPEED_ATTACK_X);
				bone4->SetState(BONE_STATE_ATTACK);
				bone4->SetPosition(preDieX, preDieY);
				grid->AddObject(bone4);

				Bone *bone5 = new Bone();
				bone5->AddAnimation(700);
				bone5->SetSpeed(-BONE_SPEED_ATTACK_X, 0);
				bone5->SetState(BONE_STATE_ATTACK);
				bone5->SetPosition(preDieX, preDieY);
				grid->AddObject(bone5);

				Bone *bone6 = new Bone();
				bone6->AddAnimation(700);
				bone6->SetSpeed(-BONE_SPEED_ATTACK_X, BONE_SPEED_ATTACK_Y);
				bone6->SetState(BONE_STATE_ATTACK);
				bone6->SetPosition(preDieX, preDieY);
				grid->AddObject(bone6);

				isCrash = true;
			}
		}


		if (lv2 == true) {

			if (countLoadResourceLv2 == false)
			{
				game->mCamera->setX(0);
				grid->InitList(MAX_WIDTH_LV2, MAX_HEIGHT_LV2);
				aladdin->SetPosition(50, 50);
				grid->AddObject(aladdin);
				LoadResourceLv2();
				countLoadResourceLv2 = true;
				/*aladdin->SetPosition(100, 20);*/
				aladdin->SetState(SIMON_STATE_IDLE);
				aladdin->GetPosition(x, y);

				jafar->nx = -1;
				jafar->AddAnimation(609);
				jafar->AddAnimation(610);
				jafar->AddAnimation(611);
				jafar->AddAnimation(612);
				jafar->AddAnimation(613);
				jafar->AddAnimation(614);
				jafar->SetPosition(450, 300);
				jafar->SetState(JAFAR_STATE_IDLE);
				grid->AddObject(jafar);
				
			}
			if (GetTickCount() - jafar->time_start_shoot > 200 && isAlive == true)
			{
				jafar->time_start_shoot = GetTickCount();
				Stars *star = new Stars();
				star->AddAnimation(622);
				star->SetPosition(jafar->x+10, jafar->y+30);
				star->nx = jafar->nx;
				star->SetState(STAR_STATE_ATTACK);
				grid->AddObject(star);
			}

			if (aladdin->GetNumberapples() == 0 && isAppleAppear == false)
			{

				itemapple = new ItemApple();
				itemapple->AddAnimation(2000);
				itemapple->SetPosition(160, 290);
				grid->AddObject(itemapple);

				itemapple1 = new ItemApple();
				itemapple1->AddAnimation(2000);
				itemapple1->SetPosition(190, 250);
				grid->AddObject(itemapple1);

				itemapple2 = new ItemApple();
				itemapple2->AddAnimation(2000);
				itemapple2->SetPosition(220, 290);
				grid->AddObject(itemapple2);


				itemapple3 = new ItemApple();
				itemapple3->AddAnimation(2000);
				itemapple3->SetPosition(750, 290);
				grid->AddObject(itemapple3);

				itemapple4 = new ItemApple();
				itemapple4->AddAnimation(2000);
				itemapple4->SetPosition(780, 250);
				grid->AddObject(itemapple4);

				itemapple5 = new ItemApple();
				itemapple5->AddAnimation(2000);
				itemapple5->SetPosition(810, 290);
				grid->AddObject(itemapple5);

				isAppleAppear = true;

			}
			if (isAppleAppear == true && aladdin->GetNumberapples() == 0 
				&& itemapple->isDisappear 
				&& itemapple1->isDisappear
				&& itemapple2->isDisappear
				&& itemapple3->isDisappear
				&& itemapple4->isDisappear
				&& itemapple5->isDisappear) {
				isAppleAppear = false;
				is1FireAppear = false;
				is2FireAppear = false;
				is3FireAppear = false;
				is4FireAppear = false;
				
			}
			if (isAppleAppear) {
				if (itemapple->isDisappear && itemapple1->isDisappear && itemapple2->isDisappear) {
					is1FireAppear = true;
				}
				if (itemapple3->isDisappear && itemapple4->isDisappear && itemapple5->isDisappear) {
					is3FireAppear = true;
				}
			}
			//if (GetTickCount() - fire->time_start_shoot > 5000) {
			//	fire->SetState(FIRE_STATE_DIE);
			//	fire1->SetState(FIRE_STATE_DIE);
			//}
			if (is1FireAppear && is2FireAppear == false)
			{
				fire = new Fire();
				fire->AddAnimation(621);
				fire->SetPosition(140, 250);
				fire->SetState(FIRE_STATE_BURNING);
				grid->AddObject(fire);

				fire1 = new Fire();
				fire1->AddAnimation(621);
				fire1->SetPosition(200, 250);
				fire1->SetState(FIRE_STATE_BURNING);
				grid->AddObject(fire1);
				is2FireAppear = true;
				is1FireAppear = false;
			}

			if (is3FireAppear && is4FireAppear == false)
			{
				fire2 = new Fire();
				fire2->AddAnimation(621);
				fire2->SetPosition(740, 250);
				fire2->SetState(FIRE_STATE_BURNING);
				grid->AddObject(fire2);

				fire3 = new Fire();
				fire3->AddAnimation(621);
				fire3->SetPosition(800, 250);
				fire3->SetState(FIRE_STATE_BURNING);
				grid->AddObject(fire3);
				is4FireAppear = true;
				is3FireAppear = false;
			}
			
			if ( GetTickCount() - snake->time_start_shoot > 2000 && isJafarDead == true && isSnakeDead == false)
			{
				snake->time_start_shoot = GetTickCount();
				FireBullet *bullet = new FireBullet();
				bullet->nx = snake->nx;
				bullet->AddAnimation(619);
				bullet->AddAnimation(620);
				bullet->SetPosition(450, 320);
				bullet->SetState(FIRE_BULLET_STATE_ATTACK);
				grid->AddObject(bullet);
			}

			if (checkEndLv2) {
				aladdin->SetPosition(600, 300);
				delete mapg;
				if (GetTickCount() > 5000) {
					lv2 = false;
					lvEnd = true;
					aladdin->endGame = true;
				}
			}
		}

		if (lvEnd) {

			if (!aladdin->isSit && !aladdin->isAttack && !aladdin->isOnRope)
				aladdin->SetState(SIMON_STATE_WALK);
			if (!aladdin->isJump && !aladdin->isAttack)
				aladdin->nx = -1.0f;

			if (aladdin->GetX() < 10) {
				grid->RemoveObject(aladdin);
				/*delete aladdin;*/
				checkInfo = true;
			}
		}

	#pragma endregion

	#pragma region Collision
		vector<LPGAMEOBJECT> coObjects;
		if (aladdin->x < 0)
			currentCell = grid->GetCurrentCells(0, y);
		else
			currentCell = grid->GetCurrentCells(x, y);

		for (int i = 0; i < currentCell.size(); i++)
		{
			vector<LPGAMEOBJECT> listObjects = currentCell[i]->GetListObject();
			int listObjectSize = listObjects.size();
			for (int j = 0; j < listObjectSize; j++)
			{
				coObjects.push_back(listObjects[j]);
			}
		}

		aladdin->Update(dt, &coObjects);
		for (int i = 0; i < coObjects.size(); i++)
		{
			coObjects[i]->Update(dt + 200, &coObjects);
			grid->UpdateObjectInCell(coObjects[i]);
		}

	#pragma endregion	

	#pragma region Remove Object
		vector<LPGAMEOBJECT> listRemoveObjects;
		for (int i = 0; i < coObjects.size(); i++)
		{
			if (dynamic_cast<Zombie *>(coObjects.at(i)))
			{
				Zombie *zombie = dynamic_cast<Zombie *>(coObjects.at(i));

				if (zombie->GetHP() <= 0)
				{
					listRemoveObjects.push_back(zombie);
				}
			}
			else if (dynamic_cast<Skeleton *>(coObjects.at(i))) {
				Skeleton *skeletondie = dynamic_cast<Skeleton *>(coObjects.at(i));

				if (skeletondie->GetHP() == 0)
				{
					listRemoveObjects.push_back(skeletondie);
					preDieX = skeletondie->x;
					preDieY = skeletondie->y;
				}
			}
			else if (dynamic_cast<Soldier *>(coObjects.at(i)))
			{
				Soldier*soldier = dynamic_cast<Soldier *>(coObjects.at(i));

				if (soldier->GetHP() == 0)
				{
					listRemoveObjects.push_back(soldier);
				}
			}
			else if (dynamic_cast<Bat *>(coObjects.at(i)))
			{
				Bat*bat = dynamic_cast<Bat *>(coObjects.at(i));

				if (bat->GetState() == BAT_STATE_DIE)
				{
					listRemoveObjects.push_back(bat);
				}
			}
			else if (dynamic_cast<Bone *>(coObjects.at(i)))
			{
				Bone* bone = dynamic_cast<Bone *>(coObjects.at(i));

				if (bone->GetState() == BONE_STATE_DIE)
				{
					listRemoveObjects.push_back(bone);
				}
			}
			else if (dynamic_cast<Stars *>(coObjects.at(i)))
			{
				Stars* star = dynamic_cast<Stars *>(coObjects.at(i));

				if (star->GetState() == STAR_STATE_DIE)
				{
					listRemoveObjects.push_back(star);
				}
			}
			else if (dynamic_cast<FireBullet *>(coObjects.at(i)))
			{
				FireBullet*bullet = dynamic_cast<FireBullet *>(coObjects.at(i));

				if (bullet->GetState() == FIRE_BULLET_STATE_DIE)
				{
					listRemoveObjects.push_back(bullet);
				}
			}
			else if (dynamic_cast<BossJafar *>(coObjects.at(i)))
			{
				BossJafar *jafar = dynamic_cast<BossJafar *>(coObjects.at(i));

				if (jafar->GetHP() == 0)
				{
					listRemoveObjects.push_back(jafar);
					isAlive = false;
					snake->SetPosition(jafar->x, 280);
					snake->nx = -1;
					snake->AddAnimation(615);
					snake->AddAnimation(616);
					snake->AddAnimation(617);
					snake->AddAnimation(618);
					snake->SetState(SNAKE_STATE_ATTACK);
					isJafarDead = true;
					grid->AddObject(snake);
				}
			}
			else if (dynamic_cast<BossSnake *>(coObjects.at(i)))
			{
				BossSnake* snake = dynamic_cast<BossSnake *>(coObjects.at(i));
				snake->name = "Snake";
				if (snake->GetHP() == 0)
				{
					isSnakeDead = true;
					listRemoveObjects.push_back(snake);
				}
			}
			else if (dynamic_cast<Apple *>(coObjects.at(i)))
			{
				Apple* apple = dynamic_cast<Apple *>(coObjects.at(i));

				if (apple->isEaten)
				{
					listRemoveObjects.push_back(apple);
				}
			}
			else if (dynamic_cast<ItemApple *>(coObjects.at(i)))
			{
			ItemApple*itemapple = dynamic_cast<ItemApple *>(coObjects.at(i));

				if (itemapple->isEaten)
				{
					listRemoveObjects.push_back(itemapple);
				}
			}
			else if (dynamic_cast<GenieFace *>(coObjects.at(i)))
			{
			GenieFace *item = dynamic_cast<GenieFace *>(coObjects.at(i));

			if (item->isEaten)
			{
				listRemoveObjects.push_back(item);
			}
			}
			else if (dynamic_cast<GenieJar *>(coObjects.at(i)))
			{
			GenieJar*item = dynamic_cast<GenieJar *>(coObjects.at(i));

			if (item->isEaten)
			{
				listRemoveObjects.push_back(item);
			}
			}
			else if (dynamic_cast<ItemSpend *>(coObjects.at(i)))
			{
			ItemSpend* item = dynamic_cast<ItemSpend *>(coObjects.at(i));

			if (item->isEaten == true)
			{
				listRemoveObjects.push_back(item);
			}
			}
			else if (dynamic_cast<ItemHeart *>(coObjects.at(i)))
			{
			ItemHeart*item = dynamic_cast<ItemHeart *>(coObjects.at(i));

			if (item->isEaten)
			{
				listRemoveObjects.push_back(item);
			}
			}
			else if (dynamic_cast<Fire *>(coObjects.at(i)))
			{
			Fire*item = dynamic_cast<Fire *>(coObjects.at(i));

			if (item->GetState() == FIRE_STATE_DIE)
			{
				listRemoveObjects.push_back(item);
			}
			}
			
		}

		// Remove lần lượt từng object từ listRemoveObjects trong grid
		for (int i = 0; i < listRemoveObjects.size(); i++)
		{
			if (listRemoveObjects[i]->name == "Snake") {
				isDeadBossSnake = true;
			}
			grid->RemoveObject(listRemoveObjects[i]);
			delete listRemoveObjects[i];
		}

		if (isDeadBossSnake==true) {
			checkEndLv2 = true;
		}
	#pragma endregion	

	#pragma region Camera
		if (lv1 == true)
		{
			// Oy
			if (y > SCREEN_HEIGHT / 2 && y < MAX_HEIGHT_LV1 - SCREEN_HEIGHT / 2)
			{
				game->mCamera->setX(game->mCamera->getX());
				game->mCamera->setY(y - SCREEN_HEIGHT / 2);
			}
			else
			{
				game->mCamera->setX(game->mCamera->getX());
				game->mCamera->setY(game->mCamera->getY());
			}

			// Ox
			if (x > SCREEN_WIDTH / 2 && x < MAX_WIDTH_LV1 - SCREEN_WIDTH / 2)
			{

				game->mCamera->setX(x - SCREEN_WIDTH / 2);
				game->mCamera->setY(game->mCamera->getY());
			}
			else
			{
				game->mCamera->setX(game->mCamera->getX());
				game->mCamera->setY(game->mCamera->getY());
			}
		}

		if (lv2 == true)
		{
			// Oy
			if (y > SCREEN_HEIGHT / 2 && y < MAX_HEIGHT_LV2 - SCREEN_HEIGHT / 2)
			{
				game->mCamera->setX(game->mCamera->getX());
				game->mCamera->setY(y - SCREEN_HEIGHT / 2);
			}
			else
			{
				game->mCamera->setX(game->mCamera->getX());
				game->mCamera->setY(game->mCamera->getY());
			}

			// Ox
			if (x > SCREEN_WIDTH / 2 && x < MAX_WIDTH_LV2 - SCREEN_WIDTH / 2)
			{

				game->mCamera->setX(x - SCREEN_WIDTH / 2);
				game->mCamera->setY(game->mCamera->getY());
			}
			else
			{
				game->mCamera->setX(game->mCamera->getX());
				game->mCamera->setY(game->mCamera->getY());
			}
		}

		if (lvEnd == true)
		{
			// Oy
			if (y > SCREEN_HEIGHT / 2 && y < 370 - SCREEN_HEIGHT / 2)
			{
				game->mCamera->setX(game->mCamera->getX());
				game->mCamera->setY(y - SCREEN_HEIGHT / 2);
			}
			else
			{
				game->mCamera->setX(game->mCamera->getX());
				game->mCamera->setY(game->mCamera->getY());
			}

			// Ox
			if (x > SCREEN_WIDTH / 2 && x < 640 - SCREEN_WIDTH / 2)
			{

				game->mCamera->setX(x - SCREEN_WIDTH / 2);
				game->mCamera->setY(game->mCamera->getY());
			}
			else
			{
				game->mCamera->setX(game->mCamera->getX());
				game->mCamera->setY(game->mCamera->getY());
			}
		}
	#pragma endregion
		if (lv1 == true)
			bg->Update(gameTime / 1000, 1, aladdin);
		else
			bg->Update(gameTime / 1000, 2, aladdin);

		if (aladdin->preHP <= 0 && aladdin->life <= 0) {
			isDeadAladdin = true;
		}
}

void Render()
{
	LPDIRECT3DDEVICE9 d3ddv = game->GetDirect3DDevice();
	LPDIRECT3DSURFACE9 bb = game->GetBackBuffer();
	LPD3DXSPRITE spriteHandler = game->GetSpriteHandler();

	if (d3ddv->BeginScene())
	{
		// Clear back buffer with a color
		d3ddv->ColorFill(bb, NULL, BACKGROUND_COLOR);

		spriteHandler->Begin(D3DXSPRITE_ALPHABLEND);
		CTextures * textures = CTextures::GetInstance();
		LPDIRECT3DTEXTURE9 tex = textures->Get(ID_TEX_LV1);
		LPDIRECT3DTEXTURE9 tex2 = textures->Get(ID_TEX_LV1_2);
		float x, y;
		aladdin->GetPosition(x, y);

		LPDIRECT3DTEXTURE9 tileset = textures->Get(ID_TEX_TILESET);
		LPDIRECT3DTEXTURE9 tileset1 = textures->Get(ID_TEX_TILESET_2);
		if (lv1 == true)
		{

			mapg = new	Map(/*48, 10,*/ tileset, 32, 32);
			mapg->LoadMatrixMap("Resources\\Mapstate.txt");
			Sound::getInstance()->loadSound(GAME1_MUSIC, "man1");
			Sound::getInstance()->play("man1", true, 0);
			//map->Draw(game->x_cam, game->y_cam);
		}
		else
		{
			Sound::getInstance()->stop("man1");
		}
		if (lv2 == true)
		{
			
			mapg = new	Map(/*48, 10,*/ tileset1, 32, 32);
			mapg->LoadMatrixMap("Resources\\Mapstate1.txt");
			Sound::getInstance()->loadSound(GAME2_MUSIC, "man2");
			Sound::getInstance()->play("man2", true, 0);
			//map->Draw(game->x_cam, game->y_cam);
		}
		else
		{
			Sound::getInstance()->stop("man2");
		}
		mapg->Draw(game->mCamera->getX(), game->mCamera->getY());
		for (int i = 0; i < currentCell.size(); i++)
		{
			vector<LPGAMEOBJECT> listObject = currentCell[i]->GetListObject();
			int listObjectSize = listObject.size();

			for (int j = 0; j < listObjectSize; j++)
			{
				listObject[j]->Render();
			}
		}
		// chuyển qua màn kết thúc
		if (lvEnd && !checkInfo) {
			/*bg->RenderNextGame(game->mCamera->getX(), game->mCamera->getY(), aladdin);*/
			bg->RenderNextGame(game->mCamera->getX(), game->mCamera->getY(), aladdin);
			Sound::getInstance()->loadSound(BACKGROUND_MUSIC, "end");
			Sound::getInstance()->play("end", true, 0);
		}
		else
		{
			Sound::getInstance()->stop("end");
		}
		aladdin->Render();

		// giới thiệu thành viên
		if (lvEnd && checkInfo) {
			/*bg->RenderNextGame(game->mCamera->getX(), game->mCamera->getY(), aladdin);*/
			bg->RenderInfo(game->mCamera->getX(), game->mCamera->getY(), aladdin);
		}

		if (isDeadAladdin) {
			bg->RenderGameOver(game->mCamera->getX(), game->mCamera->getY(), aladdin);
		}

		if (lv1 == true && !isDeadAladdin)
		{
			bg->RenderPillar(game->mCamera->getX(), game->mCamera->getY(), aladdin);
		}

		// Game over
		if ((lv1 == true || lv2 == true) && !isDeadAladdin)
		{
			bg->Render(game->mCamera->getX(), game->mCamera->getY(), aladdin);
			Sound::getInstance()->loadSound(DIE_MUSIC, "die");
			Sound::getInstance()->play("die", true, 0);
		}
		else
		{
			Sound::getInstance()->stop("die");
		}
		spriteHandler->End();
		d3ddv->EndScene();
		if((lv1==true||lv2==true) && !isDeadAladdin)
			bg->RenderText(game->mCamera->getX(), game->mCamera->getY(), aladdin);
	}

	// Display back buffer content to the screen
	d3ddv->Present(NULL, NULL, NULL, NULL);
}

HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int ScreenWidth, int ScreenHeight)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;

	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	HWND hWnd =
		CreateWindow(
			WINDOW_CLASS_NAME,
			MAIN_WINDOW_TITLE,
			WS_OVERLAPPEDWINDOW, // WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			ScreenWidth,
			ScreenHeight,
			NULL,
			NULL,
			hInstance,
			NULL);

	if (!hWnd)
	{
		OutputDebugString(L"[ERROR] CreateWindow failed");
		DWORD ErrCode = GetLastError();
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

int Run()
{
	MSG msg;
	int done = 0;
	DWORD frameStart = GetTickCount();
	DWORD tickPerFrame = 1000 / MAX_FRAME_RATE;

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) done = 1;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		DWORD now = GetTickCount();

		// dt: the time between (beginning of last frame) and now
		// this frame: the frame we are about to render
		DWORD dt = now - frameStart;

		if (dt >= tickPerFrame)
		{
			frameStart = now;

			game->ProcessKeyboard();

			Update(dt);
			Render();
		}
		else
			Sleep(tickPerFrame - dt);
	}

	return 1;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd = CreateGameWindow(hInstance, nCmdShow, SCREEN_WIDTH, SCREEN_HEIGHT);

	game = CGame::GetInstance();
	game->Init(hWnd);
	
	keyHandler = new CSampleKeyHander();
	game->InitKeyboard(keyHandler);

	grid = Grid::GetInstance();

	LoadResources();

	SetWindowPos(hWnd, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	Run();

	return 0;
}