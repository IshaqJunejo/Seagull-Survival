#include "raylib.h"
#include <math.h>

int Height = 900;
int Width = Height * 0.5625;

float speed = 2.0f;
int score = 0;

bool died = false;
bool menu = true;

class Player
{
    public:
        float PosX;
        float PosY;
        float Width;
        float Height;
        float frameWidth;

    private:
        Texture2D Bird;
        int TiltPos;
        float MousePosX;
        float MousePosY;
        float BirdPosX;
        bool died;
    
    public:
        Player(float x, float y)
        {
            Bird = LoadTexture("Assets/spritesheet.png");

            Width = Bird.width;
            Height = Bird.height;
            frameWidth = Width / 3;

            PosX = x;
            PosY = y;

            TiltPos = 0;
            MousePosX = 0;
            MousePosY = 0;
            BirdPosX = 0;

            died = false;
        };

        void Update()
        {
            if (IsMouseButtonPressed(0))
            {
                MousePosX = GetMouseX();
                MousePosY = GetMouseY();
                BirdPosX = PosX;
            };
            if (IsMouseButtonDown(0))
            {
                float MouseDX = MousePosX - GetMouseX();
                if (MouseDX > 0)
                {
                    TiltPos = 0;
                    PosX -= 0.75 * GetFrameTime() * 60;
                }
                else if (MouseDX < 0)
                {
                    TiltPos = 2;
                    PosX += 0.75 * GetFrameTime() * 60;
                }
                else
                {
                    TiltPos = 1;
                };
                
                
            }else
            {
                TiltPos = 1;
            };            
            
        }

        void Draw()
        {
            if (IsMouseButtonDown(0))
            {
                DrawCircle(MousePosX, MousePosY, 2.0f, (Color){255, 255, 255, 100});
            }
            
            DrawTextureRec(Bird, Rectangle{TiltPos*(Width/3), 0, Width/3, Height}, Vector2{PosX - Width/6, PosY - Height/2}, (Color){255, 255, 255, 255});
        };

        void DrawCirlce()
        {
            if (IsMouseButtonDown(0))
            {
                DrawCircle(MousePosX, MousePosY, 12.0f, (Color){255, 255, 255, 125});
                DrawCircle(MousePosX, MousePosY, 4.0f, (Color){255, 255, 255, 100});

                DrawCircle(GetMouseX(), GetMouseY(), 12.0f, (Color){255, 255, 255, 125});
                DrawCircle(GetMouseX(), GetMouseY(), 4.0f, (Color){255, 255, 255, 100});

                DrawLine(MousePosX, MousePosY, GetMouseX(), GetMouseY(), (Color){255, 255, 255, 100});
            };
        };
};

class obstacle
{
    public:
        float PoleX;
        float PoleY;
        float PoleWidth;
        float PoleHeight;

    private:
        Texture2D Pole;

    public:
        obstacle()
        {
            Pole = LoadTexture("Assets/Pole.png");

            PoleX = GetRandomValue(Width/2 - 500, Width/2);
            PoleY = 0;

            PoleWidth = Pole.width;
            PoleHeight = Pole.height;
        }

        void update(Player obj, Sound fx)
        {
            PoleY += 0.5 * GetFrameTime() * 60 * speed;

            if (PoleY >= Height/3)
            {
                PoleY = -300;
                PoleX = GetRandomValue(Width/2 - 500, Width/2);
            };
            
            if (CheckCollisionRecs(Rectangle{obj.PosX - obj.Width/2 + obj.frameWidth, obj.PosY - obj.Height/2, obj.Width/3, obj.Height}, Rectangle{PoleX, PoleY, PoleWidth, PoleHeight}))
            {
                if (!died)
                {
                    PlaySound(fx);
                }

                died = true;
            }
        };

        void Draw()
        {
            DrawTexture(Pole, (int)PoleX, (int)PoleY, (Color){255, 255, 255, 255});
        };
};

int main()
{
    Image icon = LoadImage("Assets/Pole.png");
    InitWindow(Width, Height, "Seagull Survival");
    SetWindowIcon(icon);
    InitAudioDevice();

    SetMasterVolume(0.1f);

    Texture2D TEXTURE = LoadTexture("Assets/sea.png");
    float SeaOffset = 0.0f;
    Texture2D LOGO = LoadTexture("Assets/logo.png");

    Player seagull(0, 120);

    obstacle poles[5];
    for (int i = 0; i < 5; i++)
    {
        poles[i].PoleY =( i * -150 )- 250;
    };

    Camera2D cam = { 0 };
    cam.offset = Vector2{(float)Width/2, (float)Height/2};
    cam.zoom = 2.0f;

    Sound fxLaunch = LoadSound("Assets/launch.wav");
    Sound fxSplash = LoadSound("Assets/splash.wav");

    float WaveFuncOffset = 0;
    float ScoreOffsetY = 30;
    int frames = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (frames >= 120)
        {
            if (!died and menu)
            {
                if (IsMouseButtonPressed(0))
                {
                    menu = false;
                    PlaySound(fxLaunch);
                };
                
            }else if (!died and !menu)
            {
                SeaOffset += 0.5 * GetFrameTime() * 60 * speed;
                if (SeaOffset >= TEXTURE.width)
                {
                    SeaOffset = 0.0f;
                    speed += 0.03;
                    score++;
                    if (speed >= 10.0f)
                    {
                        speed = 2.0f;
                    };
                };

                seagull.Update();

                for (int i = 0; i < 5; i++)
                {
                    poles[i].update(seagull, fxSplash);
                };
            }else if (died and !menu)
            {
                if (seagull.PosY < 900)
                {
                    seagull.PosY += 0.5 * GetFrameTime() * 60 * speed;
                };

                for (int i = 0; i < 5; i++)
                {
                    poles[i].update(seagull, fxSplash);
                };

                SeaOffset += 0.5 * GetFrameTime() * 60 * speed;
                if (SeaOffset >= TEXTURE.width)
                {
                    SeaOffset = 0.0f;
                };
                
                if (IsMouseButtonPressed(0) and ScoreOffsetY > 400)
                {
                    died = false;
                    for (int i = 0; i < 5; i++)
                    {
                        poles[i].PoleY = i * -60 - 250;
                        poles[i].PoleX = GetRandomValue(Width/2 - 500, Width/2);
                    };
                    seagull.PosX = 0;
                    seagull.PosY = 120;

                    speed = 2.0f;
                    score = 0;
                    ScoreOffsetY = 30;
                    PlaySound(fxLaunch);
                }
                
            };
        }else
        {
            frames++;
        };

        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode2D(cam);
                if (frames >= 120)
                {
                    if (!menu)
                    {
                        for (int i = -4; i < 4; i++)
                        {
                            for (int j = -8; j < 7; j++)
                            {
                                DrawTexture(TEXTURE, i * TEXTURE.width, (j * TEXTURE.height) + SeaOffset, (Color){255, 255, 255, 255});
                            }
                            
                        }
                        seagull.Draw();

                        for (int i = 4; i > -1; i--)
                        {
                            poles[i].Draw();
                        };
                    }else
                    {
                        for (int i = -4; i < 4; i++)
                        {
                            for (int j = -8; j < 7; j++)
                            {
                                DrawTexture(TEXTURE, i * TEXTURE.width, (j * TEXTURE.height) + SeaOffset, (Color){255, 255, 255, 255});
                            }
                            
                        }
                    }
                }else
                {
                    // code
                };
            EndMode2D();
            if (frames >= 120)
            {
                if (!died and !menu)
                {
                    seagull.DrawCirlce();
                    DrawText(TextFormat("%i", score), 240, ScoreOffsetY, 50, (Color){255, 255, 255, 125});
                }else if (died and !menu)
                {
                    DrawText(TextFormat("%i", score), 240, ScoreOffsetY, 50, (Color){255, 255, 255, 125});
                    if (ScoreOffsetY < 440)
                    {
                        ScoreOffsetY += 3.5;
                    }else
                    {
                        DrawText("Click To Restart", 40, 500, 50, (Color){255, 255, 255, 125});
                    };
                    
                }else if (!died and menu)
                {
                    WaveFuncOffset += 0.05;
                    float WaveFunc = sin(WaveFuncOffset) * 25;
                    DrawText("Seagull Survival", 50, 120 + WaveFunc, 50, (Color){255, 255, 255, 200});

                    DrawText("Click Anywhere to Start", 40, 500, 35, (Color){255, 255, 255, 200});
                }
            }else
            {
                if (frames < 60)
                {
                    DrawTexture(LOGO, Width/2 - 250, Height/2 - 250, (Color){255, 255, 255, 255});
                }else
                {
                    unsigned char alpha = (120.0 / (float)frames) * 255;
                    DrawTexture(LOGO, Width/2 - 250, Height/2 - 250, (Color){255, 255, 255, alpha});
                }
                DrawCircleLines(Width/2, Height/2, 250, (Color){0, 0, 0, 255});
            }
            
        EndDrawing();
    }
    CloseWindow();
    return 0;
}