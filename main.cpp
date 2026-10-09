#include <Novice.h>
#include <time.h>

const char kWindowTitle[] = "LC1D_25_ホリウチ_ヨシキ_タイトル";

struct Vector2
{
	float x;
	float y;
};

struct Bird
{
	Vector2 leftTop;
	Vector2 velocity;
	Vector2 acceleration;
	float radius;
	unsigned int color;
};

struct Poop
{
	Vector2 leftTop;
	Vector2 velocity;
	Vector2 acceleration;
	float width;
	float height;
	int isShot;
	unsigned int color;
};

struct Human
{
	Vector2 leftTop;
	Vector2 velocity;
	Vector2 acceleration;
	Vector2 size;
	unsigned int color;
	int isAlive;
	int flameTimer;
	int currentFlame;
};

struct Line
{
	Vector2 start;
	Vector2 end;
};

int isPressSpace = false;
int canjump = true;

const int worldpos = 700;

const int kPoopMax = 100;

const int kHumanMax = 10;


int maxFlame = 4;

int poopFrame = 0;

int score = 0;

void DrawSpriteSheet
(const int textureHandle, float destX, float destY, int sheetX, int sheetY, int flameWidth, int flameHeight)
{
	int srcX = sheetX * flameWidth;
	int srcY = sheetY * flameHeight;

	Novice::DrawQuad(
		(int)destX, (int)destY,
		(int)destX + flameWidth, (int)destY,
		(int)destX, (int)destY + flameHeight,
		(int)destX + flameWidth, (int)destY + flameHeight,
		srcX, srcY,
		flameWidth, flameHeight,
		textureHandle,
		WHITE
	);

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

	Bird bard
	{
		{600.0f, 300.0f},
		{10.0f, 0.0f },
		{0.0f, 0.0f },
		64.0f,
		WHITE
	};

	Poop poops[kPoopMax];
	for (int i = 0; i < kPoopMax; i++)
	{
		poops[i] =
		{
			{bard.leftTop.x, bard.leftTop.y},
			{10.0f, 7.0f},
			{0.0f, 0.0f},
			20.0f,
			20.0f,
			false,
			WHITE
		};
	};

	Human humans[kHumanMax];
	for (int j = 0; j < kHumanMax; j++)
	{
		humans[j] =
		{
			{rand() % (1400 + 1340 + 1) + 1340.0f,0.0f},
			{rand() % (4 + 2 + 1) + 3.0f, 0.0f},
			{0.0f, 0.0f},
			{64, 256},
			GREEN,
			true,
			0,
			rand() % (4 + 0 + 1) + 0,
		};
	}

	Line line
	{
		{0.0f,0.0f},
		{1280.0f,0.0f}
	};

	const int bardTextureHandle = Novice::LoadTexture("./Resources/Sprite-hato.png");
	const int humanTextureHandle = Novice::LoadTexture("./Resources/Sprite-warkHuman.png");
	const int backGrawndTextureHandle = Novice::LoadTexture("./Resources/Sprite-backgrowndTD1.png");
	const int poopTextureHandle = Novice::LoadTexture("./Resources/poop.png");

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
				if (!poops[i].isShot)
				{
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
				humans[i].currentFlame = (humans[i].currentFlame + 1) % maxFlame;
			}
		}


		for (int i = 0; i < kHumanMax; i++)
		{
			if (humans[i].isAlive)
			{
				humans[i].leftTop.x -= humans[i].velocity.x;
			}
		}

		for (int i = 0; i < kHumanMax; i++)
		{
			if (humans[i].leftTop.x <= -100)
			{
				humans[i].leftTop.x = rand() % (1400 + 1340 + 1) + 1340.0f;
				humans[i].velocity.x = rand() % (4 + 2 + 1) + 3.0f;
				humans[i].currentFlame = rand() % (4 + 0 + 1) + 0;
			}
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		Novice::DrawLine
		(
			(int)line.start.x,
			(int)line.start.y * -1 + worldpos,
			(int)line.end.x,
			(int)line.end.y * -1 + worldpos,
			WHITE
		);



		Novice::DrawSprite
		(
			0,
			0,
			backGrawndTextureHandle,
			1,
			1,
			0.0f,
			WHITE
		);

		for (int i = 0; i < kHumanMax; i++)
		{
			Novice::DrawBox
			(
				(int)humans[i].leftTop.x,
				(int)humans[i].leftTop.y * -1 + worldpos,
				(int)humans[i].size.x,
				(int)humans[i].size.y,
				0.0f,
				WHITE,
				kFillModeSolid
				);
			DrawSpriteSheet
			(
				humanTextureHandle,
				humans[i].leftTop.x - humans[i].size.x / 2,
				(humans[i].leftTop.y + humans[i].size.y / 2) * -1 + worldpos,
				humans[i].currentFlame,
				0, 96, 256
			);

		}

		for (int i = 0; i < kPoopMax; i++)
		{
			if (poops[i].isShot)
			{
				Novice::DrawBox
				(
					(int)poops[i].leftTop.x,
					(int)poops[i].leftTop.y * -1 + worldpos,
					(int)poops[i].width,
					(int)poops[i].height,
					0.0f,
					WHITE,
					kFillModeSolid
				);
				Novice::DrawSprite
				(
					(int)poops[i].leftTop.x,
					(int)(poops[i].leftTop.y ) * -1 + worldpos,
					poopTextureHandle,
					1,
					1,
					0.0f,
					WHITE
				);

			}
		}

		DrawSpriteSheet
		(
			bardTextureHandle,
			bard.leftTop.x - bard.radius,
			(bard.leftTop.y + bard.radius) * -1 + worldpos,
			poopFrame,
			0, 128, 128
		);





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
