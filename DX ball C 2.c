#include "raylib.h" 
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 
#define SCREEN_WIDTH 800 
#define SCREEN_HEIGHT 600 
#define PADDLE_WIDTH 100 
#define PADDLE_HEIGHT 20 
#define BALL_SIZE 15 
#define BRICK_ROWS 5 
#define BRICK_COLUMNS 10 
#define BRICK_WIDTH 70 
#define BRICK_HEIGHT 20 
#define MAX_LEVELS 3 
typedef struct { 
Rectangle rect; 
bool active; 
} Brick; 
typedef struct { 
Rectangle rect; 
bool active; 
float speedY; 
} PowerUp; 
Brick bricks[BRICK_ROWS][BRICK_COLUMNS]; 
Rectangle ball, paddle; 
Vector2 ballSpeed = {5, -5}; 
PowerUp power; 
int score = 0; 
int currentLevel = 0; 
int paddleBoostTime = 0; 
Sound bounceSound, gameOverSound; 
int levelData[MAX_LEVELS][BRICK_ROWS][BRICK_COLUMNS] = { 
{{1,1,1,1,1,1,1,1,1,1},{1,0,1,0,1,0,1,0,1,0},{1,1,1,1,1,1,1,1,1,1},{0,1,0,1,0,1,0,1,0,1},{1,1,1
 ,1,1,1,1,1,1,1}}, 
{{1,1,1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1,1,1},{0,0,0,0,0,0,0,0,0,0},{1,1,1,1,1,1,1,1,1,1},{1,1,1
 ,1,1,1,1,1,1,1}}, 
{{1,0,1,0,1,0,1,0,1,0},{0,1,0,1,0,1,0,1,0,1},{1,0,1,0,1,0,1,0,1,0},{0,1,0,1,0,1,0,1,0,1},{1,1,1
 ,1,1,1,1,1,1,1}} 
}; 
void InitLevel(int level) { 
for (int i = 0; i < BRICK_ROWS; i++) { 
for (int j = 0; j < BRICK_COLUMNS; j++) { 
bricks[i][j].rect = (Rectangle){j * (BRICK_WIDTH + 5) + 35, i * (BRICK_HEIGHT 
+ 5) + 50, BRICK_WIDTH, BRICK_HEIGHT}; 
bricks[i][j].active = levelData[level][i][j]; 
} 
} 
// Reset ball and paddle 
paddle = (Rectangle){(SCREEN_WIDTH - PADDLE_WIDTH)/2, SCREEN_HEIGHT - 
40, PADDLE_WIDTH, PADDLE_HEIGHT}; 
ball = (Rectangle){paddle.x + PADDLE_WIDTH/2 - BALL_SIZE/2, paddle.y - 
BALL_SIZE, BALL_SIZE, BALL_SIZE}; 
ballSpeed = (Vector2){5, -5}; 
power.active = false; 
paddleBoostTime = 0; 
} 
bool AllBricksCleared() { 
for (int i = 0; i < BRICK_ROWS; i++) 
for (int j = 0; j < BRICK_COLUMNS; j++) 
if (bricks[i][j].active) return false; 
return true; 
} 
void ActivatePowerUp(Vector2 pos) { 
power.rect = (Rectangle){pos.x, pos.y, 20, 20}; 
power.active = true; 
power.speedY = 3; 
} 
int main() { 
InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "DX-Ball Raylib Full Version"); 
InitAudioDevice(); 
bounceSound = LoadSound("assets/bounce.wav"); 
gameOverSound = LoadSound("assets/gameover.wav"); 
SetTargetFPS(60); 
InitLevel(currentLevel); 
srand(time(NULL)); 
while (!WindowShouldClose()) { 
// Input 
if (IsKeyDown(KEY_LEFT)) paddle.x -= 8; 
if (IsKeyDown(KEY_RIGHT)) paddle.x += 8; 
if (paddle.x < 0) paddle.x = 0; 
if (paddle.x > SCREEN_WIDTH - paddle.width) paddle.x = SCREEN_WIDTH - 
paddle.width; 
// Ball movement 
ball.x += ballSpeed.x; 
ball.y += ballSpeed.y; 
// Wall collision 
if (ball.x <= 0 || ball.x + BALL_SIZE >= SCREEN_WIDTH) ballSpeed.x *= -1; 
if (ball.y <= 0) ballSpeed.y *= -1; 
// Paddle collision 
if (CheckCollisionRecs(ball, paddle)) { 
            ballSpeed.y *= -1; 
            ball.y = paddle.y - BALL_SIZE; 
            PlaySound(bounceSound); 
        } 
 
        // Brick collision 
        for (int i = 0; i < BRICK_ROWS; i++) { 
            for (int j = 0; j < BRICK_COLUMNS; j++) { 
                if (bricks[i][j].active && CheckCollisionRecs(ball, bricks[i][j].rect)) { 
                    bricks[i][j].active = false; 
                    ballSpeed.y *= -1; 
                    score += 10; 
                    PlaySound(bounceSound); 
                    if (rand() % 10 == 0) { 
                        ActivatePowerUp((Vector2){bricks[i][j].rect.x, bricks[i][j].rect.y}); 
                    } 
                } 
            } 
        } 
 
        // Power-up movement 
        if (power.active) { 
            power.rect.y += power.speedY; 
            if (CheckCollisionRecs(power.rect, paddle)) { 
                paddle.width += 40; 
                paddleBoostTime = 600; // 10 seconds at 60 FPS 
                power.active = false; 
} else if (power.rect.y > SCREEN_HEIGHT) { 
power.active = false; 
} 
} 
// Reset paddle width if power-up ends 
if (paddleBoostTime > 0) { 
paddleBoostTime--; 
if (paddleBoostTime == 0) { 
paddle.width = PADDLE_WIDTH; 
} 
} 
// Ball missed 
if (ball.y > SCREEN_HEIGHT) { 
PlaySound(gameOverSound); 
BeginDrawing(); 
ClearBackground(BLACK); 
DrawText("Game Over!", 300, 280, 40, RED); 
DrawText(TextFormat("Final Score: %d", score), 300, 330, 20, YELLOW); 
EndDrawing(); 
WaitTime(2.0); 
break; 
} 
// Level complete 
if (AllBricksCleared()) { 
currentLevel++; 
if (currentLevel >= MAX_LEVELS) { 
BeginDrawing(); 
ClearBackground(BLACK); 
DrawText("You Win!", 300, 280, 40, GREEN); 
DrawText(TextFormat("Final Score: %d", score), 300, 330, 20, YELLOW); 
EndDrawing(); 
WaitTime(2.0); 
break; 
} 
InitLevel(currentLevel); 
} 
// Draw everything 
BeginDrawing(); 
ClearBackground(BLACK); 
DrawRectangleRec(paddle, WHITE); 
DrawRectangleRec(ball, RED); 
for (int i = 0; i < BRICK_ROWS; i++) 
for (int j = 0; j < BRICK_COLUMNS; j++) 
if (bricks[i][j].active) 
DrawRectangleRec(bricks[i][j].rect, GREEN); 
if (power.active) 
DrawRectangleRec(power.rect, BLUE); 
DrawText(TextFormat("Score: %d", score), 10, 10, 20, YELLOW); 
DrawText(TextFormat("Level: %d", currentLevel + 1), 680, 10, 20, YELLOW); 
EndDrawing(); 
} 
UnloadSound(bounceSound); 
UnloadSound(gameOverSound); 
CloseAudioDevice(); 
CloseWindow(); 
return 0; 
}