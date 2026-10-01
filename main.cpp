#include <Novice.h>

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
	Vector2 positon;
	Vector2 velocity;
	Vector2 acceleration;
	float radius;
	unsigned int color;
};

struct Line
{
	Vector2 start;
	Vector2 end;
};

int isPressSpace = false;
int isShotPoop = false;
int canjump = true;

const int worldpos = 500;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Bird bard
	{
		{600.0f, 300.0f},
		{10.0f, 0.0f },
		{0.0f, 0.0f },
		50.0f,
		WHITE
	};

	for (int i = 0; i < 3; i++)
	{
		Bird poop
		{
			{600.0f, 300.0f},
			{10.0f, 0.0f },
			{0.0f, 0.0f },
			50.0f,
			WHITE
		};
	}

	Line line
	{
		{0.0f,0.0f},
		{1280.0f,0.0f}
	};

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


		if (keys[DIK_SPACE] != 0)
		{

			bard.velocity.y = 10.0f;
			isPressSpace = true;
			isShotPoop = true;
		}

		if (isPressSpace)
		{
			bard.velocity.x += bard.acceleration.x;
			bard.velocity.y += bounce;


			bard.position.y += bard.velocity.y;
			canjump = false;
		}

		if (bard.position.y <= bard.radius)
		{
			bard.position.y = bard.radius;
			canjump = true;
		}

		if (bard.position.y - bard.radius <= 0.0f)
		{
			bard.velocity.y = bard.velocity.y;
		}

		if (bard.position.y + bard.radius <= -150.0f)
		{
			bard.position.y = -150.0f;
		}

		if (bard.position.x - bard.radius >= 1280.0f)
		{
			bard.position.x = 1.0f + bard.radius;
		}

		if (bard.position.x + bard.radius <= 0.0f)
		{
			bard.position.x = 1280.0f + bard.radius;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		Novice::DrawEllipse
		(
			(int)bard.position.x,
			(int)bard.position.y * -1 + worldpos,
			(int)bard.radius,
			(int)bard.radius,
			0.0f,
			bard.color,
			kFillModeSolid
		);

		Novice::DrawLine
		(
			(int)line.start.x,
			(int)line.start.y * -1 + worldpos,
			(int)line.end.x,
			(int)line.end.y * -1 + worldpos,
			WHITE
		);

		Novice::ScreenPrintf
		(
			10, 10,
			"bardPosX:%f   bardPosY:%f"
			, bard.position.x, bard.position.y
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
