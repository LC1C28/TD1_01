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
	Vector2 position;
	Vector2 velocity;
	Vector2 acceleration;
	float radius;
	unsigned int color;
};

struct Poop
{
	Vector2 position;
	Vector2 velocity;
	Vector2 acceleration;
	float radius;
	int isShot;
	unsigned int color;
};

struct Human
{
	Vector2 position;
	Vector2 velocity;
	Vector2 acceleration;
	Vector2 size;
	unsigned int color;
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


int flameTimer = 0;
int currentFlame = 0;
int maxFlame = 3;

int poopFrame = 0;

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
			{bard.position.x, bard.position.y},
			{10.0f, 7.0f},
			{0.0f, 0.0f},
			10.0f,
			false,
			WHITE
		};
	};

	Human humans[kHumanMax];
	for (int j = 0; j< kHumanMax; j++)
	{
		humans[j] =
		{ 
			{100.0f + j * 100.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{64, 256},
			GREEN
		};
	}

	Line line
	{
		{0.0f,0.0f},
		{1280.0f,0.0f}
	};

	const int textureHandle = Novice::LoadTexture("./Resources/Sprite-hato.png");

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
					poops[i].position = bard.position;
					break;
				}
			}
		}

		if (isPressSpace)
		{
			bard.velocity.x += bard.acceleration.x;
			bard.velocity.y += bounce;
			bard.position.y += bard.velocity.y;
			canjump = false;
			for (size_t i = 0; i < kHumanMax; i++)
			{
				humans[i].velocity.x += humans[i].acceleration.x;
				humans[i].velocity.y += bounce;
				humans[i].position.y += humans[i].velocity.y;
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
				poops[i].position.x -= poops[i].velocity.x;
				poops[i].position.y -= poops[i].velocity.y;
			}
		}

		for (int i = 0; i < kPoopMax; i++)
		{
			if (poops[i].position.y <= poops[i].radius)
			{
				poops[i].isShot = false;
				poops[i].velocity.x = 10.0f;
			}
		}

		if (bard.position.y <= bard.radius)
		{
			bard.position.y = bard.radius;
			canjump = true;
		}

		for (int i = 0; i < kHumanMax; i++)
		{
			if (humans[i].position.y <= humans[i].size.y / 2)
			{
				humans[i].position.y = humans[i].size.y / 2;
			}
		}

		if (bard.position.y - bard.radius <= 0.0f)
		{
			bard.velocity.y = bard.velocity.y;
			poopFrame = 2;
		}

		flameTimer++;
		if (flameTimer >= 10)
		{
			flameTimer = 0;
			currentFlame = (currentFlame + 1) % maxFlame;
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

		for (int j = 0; j < kHumanMax; j++)
		{
			Novice::DrawBox
			(
				(int)humans[j].position.x - (int)humans[j].size.x / 2,
				(int)humans[j].position.y * -1 + worldpos - (int)humans[j].size.y / 2,
				(int)humans[j].size.x,
				(int)humans[j].size.y,
				0.0f,
				humans[j].color,
				kFillModeSolid
			);
		}

		for (int i = 0; i < kPoopMax; i++)
		{
			if (poops[i].isShot)
			{
				Novice::DrawEllipse
				(
					(int)poops[i].position.x,
					(int)poops[i].position.y * -1 + worldpos,
					(int)poops[i].radius,
					(int)poops[i].radius,
					0.0f,
					poops[i].color,
					kFillModeSolid
				);
			}
		}

		DrawSpriteSheet
		(
			textureHandle,
			bard.position.x - bard.radius,
			(bard.position.y + bard.radius) * -1 + worldpos,
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
