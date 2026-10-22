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
	int poopGauge;
	int flapping;
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

enum GameStatus {
	GAME_STATUS_START,
	GAME_STATUS_PLAYING,
	GAME_STATUS_GAME_OVER
};

GameStatus gameStatus = GAME_STATUS_START;

int isPressSpace = false;
int canjump = true;

const int worldpos = 700;

const int kPoopMax = 100;

const int kHumanMax = 8;

float maxHeight = 700;

int addScore = 0;

int maxFlame = 4;

int bardFrame = 0;

int score = 0;

int currentGraphHandleNum[5] = { 0 };
int currentNum = 0;

bool isGameOver = false;

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
		32.0f, WHITE,
		0,
		3
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
	const int damageBardTextureHandle = Novice::LoadTexture("./Resources/Sprite-damage-hato.png");
	const int humanTextureHandle = Novice::LoadTexture("./Resources/Sprite-warkHuman.png");
	const int backGrawndTextureHandle = Novice::LoadTexture("./Resources/Sprite-backgrowndTD1.png");
	const int poopTextureHandle = Novice::LoadTexture("./Resources/poop-export.png");
	const int warnTextureHandle = Novice::LoadTexture("./Resources/Sprite-warn.png");
	const int praneTextureHandle = Novice::LoadTexture("./Resources/Sprite-plane.png");
	const int scoreTextureHandle = Novice::LoadTexture("./Resources/Sprite-score.png");
	const int playingScoreTextureHandle = Novice::LoadTexture("./Resources/Sprite-score-playing.png");
	const int numberTextureHandle = Novice::LoadTexture("./Resources/number-sheet.png");
	//const int numberGameoverTextureHandle = Novice::LoadTexture("./Resources/numGameover-sheet.png");

	float bounce = -0.8f;

	const int kPoopGaugeMax = 300;
	//int poopShotCooltime = 0;

	int rapidFireCount = 0;
	int rapidFireTimer = 0;
	const int kRapidFireInterVal = 6;

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

		switch (gameStatus) {
		case GAME_STATUS_START:
			// STARTシーンの処理
			if (keys[DIK_SPACE] != 0 && preKeys[DIK_SPACE] == 0) {
				gameStatus = GAME_STATUS_PLAYING;
			}
			break;

		case GAME_STATUS_PLAYING:
			if (bard.poopGauge < kPoopGaugeMax)
			{
				bard.poopGauge++;
			}

			if (keys[DIK_SPACE] != 0 && preKeys[DIK_SPACE] == 0) {
				bard.velocity.y = 10.0f;
				isPressSpace = true;
				bard.flapping++;
				if (bardFrame == 1 || bardFrame == 2) {
					bardFrame = 0;
				}
				else if (bardFrame == 0) {
					bardFrame = 1;
				}
				if (bard.flapping >= 3)
				{
					for (int i = 0; i < kPoopMax; i++)
					{
						if (!poops[i].isShot)
						{
							poops[i].isShot = true;
							poops[i].leftTop = bard.leftTop;
							poops[i].velocity.x = 10.0f;
							break;
						}
					}
					bard.flapping = 0;
				}

				if (bard.poopGauge >= kPoopGaugeMax)
				{
					bard.poopGauge = 0;
					rapidFireCount = 5;
					rapidFireTimer = 0;
				}
			}
			if (rapidFireCount > 0)
			{
				rapidFireTimer--;
				if (rapidFireTimer <= 0)
				{
					for (int i = 0; i < kPoopMax; i++)
					{
						if (!poops[i].isShot)
						{
							poops[i].isShot = true;
							poops[i].leftTop = bard.leftTop;
							poops[i].velocity.x = 10.0f;
							break;
						}
					}
					rapidFireCount--;
					rapidFireTimer = kRapidFireInterVal;
				}
			}



			if (isPressSpace) {
				bard.velocity.x += bard.acceleration.x;
				bard.velocity.y += bounce;
				bard.leftTop.y += bard.velocity.y;
				canjump = false;
				for (size_t i = 0; i < kHumanMax; i++) {
					humans[i].velocity.x += humans[i].acceleration.x;
					humans[i].velocity.y += bounce;
					humans[i].leftTop.y += humans[i].velocity.y;
				}
			}

			for (int i = 0; i < kPoopMax; i++) {
				if (poops[i].isShot) {
					poops[i].shotpoopHeight = (int)bard.leftTop.y;
					if (poops[i].velocity.x > 0) {
						poops[i].velocity.x -= 0.3f;
					}
					poops[i].leftTop.x -= poops[i].velocity.x;
					poops[i].leftTop.y -= poops[i].velocity.y;
				}
			}

			for (int i = 0; i < kPoopMax; i++) {
				if (poops[i].leftTop.y <= poops[i].width) {
					poops[i].isShot = false;
					poops[i].velocity.x = 10.0f;
				}
			}

			if (bard.leftTop.y <= bard.radius + 64) {
				bard.leftTop.y = bard.radius + 64;
				canjump = true;
				bard.velocity.y = bard.velocity.y;
				bardFrame = 2;
			}

			for (int i = 0; i < kHumanMax; i++) {
				if (humans[i].leftTop.y <= humans[i].size.y / 2) {
					humans[i].leftTop.y = humans[i].size.y / 2;
				}
			}

			if ((bard.leftTop.y - bard.radius - 32) <= 0.0f) {
			}

			for (int i = 0; i < kHumanMax; i++) {
				humans[i].flameTimer++;
				if (humans[i].flameTimer >= 10) {
					humans[i].flameTimer = 0;

					if (humans[i].isAlive) {
						humans[i].currentFlame = (humans[i].currentFlame + 1) % maxFlame;
					}
					else {
						humans[i].hitCurrentFlame++;

						if (humans[i].hitCurrentFlame > 7) {
							humans[i].hitCurrentFlame = 4;
						}
					}
				}
			}

			radioControl.flameTimer++;
			if (radioControl.flameTimer >= 2) {
				radioControl.flameTimer = 0;
				if (radioControl.isAlive) {
					radioControl.currentFlame = (radioControl.currentFlame + 1) % maxFlame;
				}
			}

			for (int i = 0; i < kHumanMax; i++) {
				humans[i].leftTop.x -= humans[i].velocity.x;
			}

			if (!radioControl.isAlive) {
				radioControl.respawnTimer--;
			}
			else if (radioControl.isAlive) {
				radioControl.leftTop.x -= radioControl.velocity.x;
			}

			if (radioControl.respawnTimer <= 0) {
				radioControl.isAlive = true;
			}

			if (radioControl.leftTop.x <= -100) {
				radioControl.leftTop.x = 1300;
				radioControl.leftTop.y = (float)(rand() % (400 + 0 + 1) + 0);
				radioControl.respawnTimer = rand() % (120 + 240 + 1) + 120;
				radioControl.isAlive = false;
			}

			for (int i = 0; i < kHumanMax; i++) {
				if (humans[i].leftTop.x <= -100) {
					humans[i].leftTop.x = rand() % (1400 + 1340 + 1) + 1340.0f;
					humans[i].velocity.x = rand() % (4 + 2 + 1) + 3.0f;
					humans[i].currentFlame = rand() % (4 + 0 + 1) + 0;
					humans[i].hitCurrentFlame = 4;
					humans[i].flameTimer = 0;
					humans[i].isAlive = true;
				}
			}

			if (bard.leftTop.y >= 700) {
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

					if (hitX && hitY) {
						if (humans[j].isAlive) {
							addScore = (int)((maxHeight - poops[i].shotpoopHeight) / 5.0f);
							if (addScore < 1)
							{
								addScore = 1;
							}
							score += addScore;
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

			for (int j = 0; j < kHumanMax; j++) {

				// 人間の当たり判定
				float humanLeft = humans[j].leftTop.x + 16 - humans[j].size.x / 2;

				float humanTop = (humans[j].leftTop.y + humans[j].size.y / 2) * -1 + worldpos;

				float humanRight = humanLeft + humans[j].size.x;
				float humanBody = humanTop + humans[j].size.y;

				// 鳥の当たり判定
				float birdLeft = bard.leftTop.x - bard.radius;
				float birdTop = (bard.leftTop.y + bard.radius) * -1 + worldpos;

				float birdRight = birdLeft + bard.radius * 2;
				float birdBottom = birdTop + bard.radius * 2;

				// 横方向と縦方向の当たり判定
				bool hitX = birdRight > humanLeft && birdLeft < humanRight;

				bool hitY = birdBottom > humanTop && birdTop < humanBody;

				// 鳥が人間に当たったらゲームオーバー
				if (hitX && hitY) {
					isGameOver = true;
					break;
				}

				if (isGameOver) {
					gameStatus = GAME_STATUS_GAME_OVER;
				}
			}

			//==================================================
			// BirdとRadioControlの当たり判定
			//==================================================
			if (radioControl.isAlive) {

				// 鳥の当たり判定
				float birdLeft = bard.leftTop.x - bard.radius;
				float birdTop = (bard.leftTop.y + bard.radius) * -1 + worldpos;
				float birdRight = birdLeft + bard.radius * 2;
				float birdBottom = birdTop + bard.radius * 2;

				// ラジコン（飛行機）の当たり判定
				float planeLeft = radioControl.leftTop.x;
				float planeTop = radioControl.leftTop.y;
				float planeRight = planeLeft;
				float planeBottom = planeTop;

				// 横方向と縦方向の当たり判定
				bool hitX = birdRight > planeLeft && birdLeft < planeRight;

				bool hitY = birdBottom > planeTop && birdTop < planeBottom;

				// 鳥がラジコンに当たったらゲームオーバー
				if (hitX && hitY) {
					gameStatus = GAME_STATUS_GAME_OVER;
				}
			}

			break;

		case GAME_STATUS_GAME_OVER:
			// ゲームオーバーシーンの処理
			if (keys[DIK_SPACE] != 0 && preKeys[DIK_SPACE] == 0) {
				// シーンとスコアをリセット
				gameStatus = GAME_STATUS_START;
				score = 0;
				bardFrame = 0;
				isPressSpace = false;
				canjump = true;
				isGameOver = false;

				// 鳥を初期位置に戻す
				bard.leftTop = { 600.0f, 300.0f };
				bard.velocity = { 10.0f, 0.0f };
				bard.acceleration = { 0.0f, 0.0f };

				// フンをすべて初期化
				for (int i = 0; i < kPoopMax; i++) {
					poops[i] = {
						{bard.leftTop.x, bard.leftTop.y},
						{10.0f,          7.0f          },
						{0.0f,           0.0f          },
						16.0f, 16.0f, false, 0, WHITE
					};
				}

				// 人間を初期化
				for (int j = 0; j < kHumanMax; j++) {
					humans[j] = {
						{(float)(rand() % 1401 + 1340), 0.0f  },
						{(float)(rand() % 7 + 3),       0.0f  },
						{0.0f,                          0.0f  },
						{64.0f,                         256.0f},
						GREEN, true, 0, rand() % 5, 5
					};
				}

				// ラジコンを初期化
				radioControl.leftTop = { 1300.0f, (float)(rand() % 401) };
				radioControl.velocity = { 10.0f, 0.0f };
				radioControl.color = WHITE;
				radioControl.respawnTimer = rand() % 121 + 120;
				radioControl.isAlive = false;
				radioControl.flameTimer = 0;
				radioControl.currentFlame = 0;
			}

			if (score > 99999)
			{
				score = 99999;
			}

			currentNum = score;
			currentGraphHandleNum[0] = currentNum / 10000; currentNum %= 10000;
			currentGraphHandleNum[1] = currentNum / 1000; currentNum %= 1000;
			currentGraphHandleNum[2] = currentNum / 100; currentNum %= 100;
			currentGraphHandleNum[3] = currentNum / 10; currentNum %= 10;
			currentGraphHandleNum[4] = currentNum ;


			break;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		switch (gameStatus)
		{
		case GAME_STATUS_START:
			Novice::DrawBox(0, 0, 1280, 720, 0.0f, BLACK, kFillModeSolid);
			Novice::ScreenPrintf(560, 260, "MY GAME");
			Novice::ScreenPrintf(490, 320, "SPACE : Jump / Shoot");
			Novice::ScreenPrintf(520, 400, "Press SPACE to Start");
			break;

		case GAME_STATUS_PLAYING:
			Novice::DrawSprite(0, 0, backGrawndTextureHandle, 1, 1, 0.0f, WHITE);

			for (int i = 0; i < kHumanMax; i++) {

				if (humans[i].isAlive) {
					DrawSpriteSheet(
						humanTextureHandle, humans[i].leftTop.x - humans[i].size.x / 2, (humans[i].leftTop.y + humans[i].size.y / 2) * -1 + worldpos, humans[i].currentFlame, 0, 96, 256);
				}
				else {
					DrawSpriteSheet(
						humanTextureHandle, humans[i].leftTop.x - humans[i].size.x / 2, (humans[i].leftTop.y + humans[i].size.y / 2) * -1 + worldpos, humans[i].hitCurrentFlame, 0, 96, 256);
				}
			}

			for (int i = 0; i < kPoopMax; i++) {
				if (poops[i].isShot) {
					Novice::DrawSprite((int)poops[i].leftTop.x, (int)(poops[i].leftTop.y) * -1 + worldpos, poopTextureHandle, 1, 1, 0.0f, WHITE);
				}
			}

			DrawSpriteSheet(bardTextureHandle, bard.leftTop.x - bard.radius, (bard.leftTop.y + bard.radius) * -1 + worldpos, bardFrame, 0, 128, 128);

			if (radioControl.respawnTimer <= 60 && radioControl.respawnTimer >= 1) {

				DrawSpriteSheet(warnTextureHandle, 1206, (float)radioControl.leftTop.y, 1, 1, 64, 64);
			}

			if (radioControl.isAlive) {

				DrawSpriteSheet(praneTextureHandle, (float)radioControl.leftTop.x, (float)radioControl.leftTop.y, radioControl.currentFlame, 1, 128, 64);
			}

			Novice::ScreenPrintf(20, 0, "SCORE: %d", score);

			for (int i = 0; i < 5; i++)
			{
				DrawSpriteSheet(numberTextureHandle,40,0,currentGraphHandleNum[i],16,64,WHITE);
			}

			Novice::DrawSprite(20, 0, playingScoreTextureHandle, 1.0f, 1.0f, 0.0f, WHITE);
			break;

		case GAME_STATUS_GAME_OVER:
			// 画面全体を黒いボックスで覆う
			//	Novice::DrawBox(0, 0, 1280, 720, 0.0f, 0x000000FF, kFillModeSolid);

			// ゲームオーバー文字
			Novice::ScreenPrintf(570, 320, "GAME OVER");

			// スコア表示
			Novice::ScreenPrintf(570, 360, "SCORE: %d", score);

			Novice::DrawSprite(0, 0, backGrawndTextureHandle, 1, 1, 0.0f, WHITE);

			for (int i = 0; i < kHumanMax; i++) {

				if (humans[i].isAlive) {
					DrawSpriteSheet(
						humanTextureHandle, humans[i].leftTop.x - humans[i].size.x / 2, (humans[i].leftTop.y + humans[i].size.y / 2) * -1 + worldpos, humans[i].currentFlame, 0, 96, 256);
				}
				else {
					DrawSpriteSheet(
						humanTextureHandle, humans[i].leftTop.x - humans[i].size.x / 2, (humans[i].leftTop.y + humans[i].size.y / 2) * -1 + worldpos, humans[i].hitCurrentFlame, 0, 96, 256);
				}
			}

			for (int i = 0; i < kPoopMax; i++) {
				if (poops[i].isShot) {
					Novice::DrawSprite((int)poops[i].leftTop.x, (int)(poops[i].leftTop.y) * -1 + worldpos, poopTextureHandle, 1, 1, 0.0f, WHITE);
				}
			}

			DrawSpriteSheet(damageBardTextureHandle, bard.leftTop.x - bard.radius, (bard.leftTop.y + bard.radius) * -1 + worldpos, bardFrame, 0, 128, 128);

			if (radioControl.respawnTimer <= 60 && radioControl.respawnTimer >= 1) {

				DrawSpriteSheet(warnTextureHandle, 1206, (float)radioControl.leftTop.y, 1, 1, 64, 64);
			}

			if (radioControl.isAlive) {

				DrawSpriteSheet(praneTextureHandle, (float)radioControl.leftTop.x, (float)radioControl.leftTop.y, radioControl.currentFlame, 1, 128, 64);
			}

			Novice::DrawSprite(378, 310, scoreTextureHandle, 1.0f, 1.0f, 0.0f, WHITE);

			break;

		}

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