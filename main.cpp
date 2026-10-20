#include <Novice.h>
#include <time.h>

const char kWindowTitle[] = "LC1D_25_ホリウチ_ヨシキ_タイトル";

struct Vector2 {
	float x;
	float y;
};

struct Bird {
	Vector2 leftTop;
	Vector2 velocity;
	Vector2 acceleration;
	float radius;
	unsigned int color;
};

struct Poop {
	Vector2 leftTop;
	Vector2 velocity;
	Vector2 acceleration;
	float width;
	float height;
	int isShot;
	int shotpoopHeight;
	unsigned int color;
};

struct Human {
	Vector2 leftTop;
	Vector2 velocity;
	Vector2 acceleration;
	Vector2 size;
	unsigned int color;
	int isAlive;
	int flameTimer;
	int currentFlame;
	int hitCurrentFlame;
};

struct Line {
	Vector2 start;
	Vector2 end;
};

struct RadioControl
{
	Vector2 leftTop;
	Vector2 velocity;
	unsigned int color;
	int respawnTimer;
	int isAlive;
	int flameTimer;
	int currentFlame;
};

int isPressSpace = false;
int canjump = true;

const int worldpos = 700;

const int kPoopMax = 100;

const int kHumanMax = 8;

int maxFlame = 4;

int poopFrame = 0;

int score = 0;

void DrawSpriteSheet(const int textureHandle, float destX, float destY, int sheetX, int sheetY, int flameWidth, int flameHeight) {
	int srcX = sheetX * flameWidth;
	int srcY = sheetY * flameHeight;

	Novice::DrawQuad(
		(int)destX, (int)destY, (int)destX + flameWidth, (int)destY, (int)destX, (int)destY + flameHeight, (int)destX + flameWidth, (int)destY + flameHeight, srcX, srcY, flameWidth, flameHeight,
		textureHandle, WHITE);
}



// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	int currentTime = (int)time(nullptr);
	srand(currentTime);

	Bird bard{
		{600.0f, 300.0f},
		{10.0f,  0.0f  },
		{0.0f,   0.0f  },
		64.0f, WHITE
	};

	Poop poops[kPoopMax];
	for (int i = 0; i < kPoopMax; i++) {
		poops[i] = {
			{bard.leftTop.x, bard.leftTop.y},
			{10.0f,7.0f},
			{0.0f,0.0f},
			16.0f, 16.0f, false,0, WHITE
		};
	};

	Human humans[kHumanMax];
	for (int j = 0; j < kHumanMax; j++)
	{
		humans[j] = {
			{rand() % (1400 + 1340 + 1) + 1340.0f, 0.0f},
			{rand() % (4 + 2 + 1) + 3.0f,0.0f},
			{0.0f,0.0f},
			{64,256 },
			GREEN, true, 0,
			rand() % (4 + 0 + 1) + 0,
			5
		};
	}

	Line line
	{
		{0.0f,    0.0f},
		{1280.0f, 0.0f}
	};

	RadioControl radioControl
	{
		{1300,(float)(rand() % (400 + 0 + 1) + 0)},
		{10,0},
		WHITE,
		rand() % (120 + 240 + 1) + 120,
		false,
	};

	const int bardTextureHandle = Novice::LoadTexture("./Resources/Sprite-hato.png");
	const int humanTextureHandle = Novice::LoadTexture("./Resources/Sprite-warkHuman.png");
	const int backGrawndTextureHandle = Novice::LoadTexture("./Resources/Sprite-backgrowndTD1.png");
	const int poopTextureHandle = Novice::LoadTexture("./Resources/poop-export.png");
	const int warnTextureHandle = Novice::LoadTexture("./Resources/Sprite-warn.png");
	const int praneTextureHandle = Novice::LoadTexture("./Resources/Sprite-plane.png");

	float bounce = -0.8f;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		if (keys[DIK_SPACE] != 0 && preKeys[DIK_SPACE] == 0)
		{
			bard.velocity.y = 10.0f;
			isPressSpace = true;

			if (poopFrame == 1 || poopFrame == 2)
			{
				poopFrame = 0;
			}
			else if (poopFrame == 0)
			{
				poopFrame = 1;
			}

			for (int i = 0; i < kPoopMax; i++)
			{
				if (!poops[i].isShot) {
					poops[i].isShot = true;
					poops[i].leftTop = bard.leftTop;
					break;
				}
			}
		}

		if (isPressSpace)
		{
			bard.velocity.x += bard.acceleration.x;
			bard.velocity.y += bounce;
			bard.leftTop.y += bard.velocity.y;
			canjump = false;
			for (size_t i = 0; i < kHumanMax; i++)
			{
				humans[i].velocity.x += humans[i].acceleration.x;
				humans[i].velocity.y += bounce;
				humans[i].leftTop.y += humans[i].velocity.y;
			}
		}

		for (int i = 0; i < kPoopMax; i++)
		{
			if (poops[i].isShot)
			{
				poops[i].shotpoopHeight = (int)bard.leftTop.y;
				if (poops[i].velocity.x > 0)
				{
					poops[i].velocity.x -= 0.3f;
				}
				poops[i].leftTop.x -= poops[i].velocity.x;
				poops[i].leftTop.y -= poops[i].velocity.y;
			}
		}

		for (int i = 0; i < kPoopMax; i++)
		{
			if (poops[i].leftTop.y <= poops[i].width)
			{
				poops[i].isShot = false;
				poops[i].velocity.x = 10.0f;
			}
		}

		if (bard.leftTop.y <= bard.radius)
		{
			bard.leftTop.y = bard.radius;
			canjump = true;
		}

		for (int i = 0; i < kHumanMax; i++)
		{
			if (humans[i].leftTop.y <= humans[i].size.y / 2)
			{
				humans[i].leftTop.y = humans[i].size.y / 2;
			}
		}

		if (bard.leftTop.y - bard.radius <= 0.0f)
		{
			bard.velocity.y = bard.velocity.y;
			poopFrame = 2;
		}

		for (int i = 0; i < kHumanMax; i++)
		{
			humans[i].flameTimer++;
			if (humans[i].flameTimer >= 10)
			{
				humans[i].flameTimer = 0;

				if (humans[i].isAlive)
				{
					humans[i].currentFlame = (humans[i].currentFlame + 1) % maxFlame;
				}
				else
				{
					humans[i].hitCurrentFlame++;

					if (humans[i].hitCurrentFlame > 7)
					{
						humans[i].hitCurrentFlame = 4;
					}
				}
			}
		}

		radioControl.flameTimer++;
		if (radioControl.flameTimer >= 2)
		{
			radioControl.flameTimer = 0;
			if (radioControl.isAlive)
			{
				radioControl.currentFlame = (radioControl.currentFlame + 1) % maxFlame;
			}
		}

		for (int i = 0; i < kHumanMax; i++)
		{
			humans[i].leftTop.x -= humans[i].velocity.x;
		}

		if (!radioControl.isAlive)
		{
			radioControl.respawnTimer--;
		}
		else if (radioControl.isAlive)
		{
			radioControl.leftTop.x -= radioControl.velocity.x;
		}



		if (radioControl.respawnTimer <= 0)
		{
			radioControl.isAlive = true;
		}

		if (radioControl.leftTop.x <= -100)
		{
			radioControl.leftTop.x = 1300;
			radioControl.leftTop.y = (float)(rand() % (400 + 0 + 1) + 0);
			radioControl.respawnTimer = rand() % (120 + 240 + 1) + 120;
			radioControl.isAlive = false;
		}

		for (int i = 0; i < kHumanMax; i++)
		{
			if (humans[i].leftTop.x <= -100)
			{
				humans[i].leftTop.x = rand() % (1400 + 1340 + 1) + 1340.0f;
				humans[i].velocity.x = rand() % (4 + 2 + 1) + 3.0f;
				humans[i].currentFlame = rand() % (4 + 0 + 1) + 0;
				humans[i].hitCurrentFlame = 4;
				humans[i].flameTimer = 0;
				humans[i].isAlive = true;
			}
		}

		if (bard.leftTop.y >= 700)
		{
			bard.leftTop.y = 700;
		}

		//==================================================
		// PoopとHumanの当たり判定
		//==================================================
		for (int i = 0; i < kPoopMax; i++) {

			if (!poops[i].isShot) {
				continue;
			}

			for (int j = 0; j < kHumanMax; j++) {

				float humanLeft = (humans[j].leftTop.x + 16 - humans[j].size.x / 2);
				float humanTop = (humans[j].leftTop.y + humans[j].size.y / 2) * -1 + worldpos;
				float humanRight = humanLeft + humans[j].size.x;
				float humanBottom = humanTop + humans[j].size.y - 184;
				
				float poopLeft = poops[i].leftTop.x;
				float poopTop = poops[i].leftTop.y * -1 + worldpos;
				float poopRight = poopLeft + poops[i].width;
				float poopBottom = poopTop + poops[i].height;
				
				
				bool hitX = poopRight > humanLeft && poopLeft < humanRight;
				bool hitY = poopBottom > humanTop && poopTop < humanBottom;


				if (hitX && hitY)
				{
					if (humans[j].isAlive)
					{
						score += 10 * (poops[i].shotpoopHeight - 184) / 100;
						poops[i].isShot = false;
						poops[i].velocity.x = 10.0f;
						humans[j].isAlive = false;
						humans[j].velocity.x = 25.0f;
						humans[j].hitCurrentFlame = 4;
						humans[j].flameTimer = 0;
						break;
				
					}
				}
			}
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		Novice::DrawSprite(0, 0, backGrawndTextureHandle, 1, 1, 0.0f, WHITE);

		for (int i = 0; i < kHumanMax; i++)
		{

			if (humans[i].isAlive)
			{
				DrawSpriteSheet(humanTextureHandle, humans[i].leftTop.x - humans[i].size.x / 2, (humans[i].leftTop.y + humans[i].size.y / 2) * -1 + worldpos, humans[i].currentFlame, 0, 96, 256);
			}
			else
			{
				DrawSpriteSheet(humanTextureHandle, humans[i].leftTop.x - humans[i].size.x / 2, (humans[i].leftTop.y + humans[i].size.y / 2) * -1 + worldpos, humans[i].hitCurrentFlame, 0, 96, 256);
			}
		}

		for (int i = 0; i < kPoopMax; i++) {
			if (poops[i].isShot) {
				Novice::DrawSprite((int)poops[i].leftTop.x, (int)(poops[i].leftTop.y) * -1 + worldpos, poopTextureHandle, 1, 1, 0.0f, WHITE);
			}
		}

		DrawSpriteSheet(bardTextureHandle, bard.leftTop.x - bard.radius, (bard.leftTop.y + bard.radius) * -1 + worldpos, poopFrame, 0, 128, 128);

		if (radioControl.respawnTimer <= 60 && radioControl.respawnTimer >= 1)
		{


			DrawSpriteSheet(warnTextureHandle, 1206, (float)radioControl.leftTop.y, 1, 1, 64, 64);
		}

		if (radioControl.isAlive)
		{

			DrawSpriteSheet(praneTextureHandle, (float)radioControl.leftTop.x, (float)radioControl.leftTop.y, radioControl.currentFlame, 1, 128, 64);

		}

		Novice::ScreenPrintf(20, 0, "SCORE: %d", score);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}