#include "raylib.h" // C++ and Raylib
#include <stdio.h>
#include <string.h>

float ghostAppearsRunningTime{};
float ghostIdleRunningTime{};
float ghostVanishRunningTime{};

int ghostIdleFrame{};
const float updateTime{1.f / 12.f};
float scale{2.8}; // Scale background & ghost

int main()
{   
    // Window
    const int screenWidth = 661 * scale;
    const int screenHeight = 300 * scale;
    InitWindow(screenWidth, screenHeight, "Ghost Hunt"); // Game name

    // Graphics
    Texture2D background = LoadTexture("old-dark-castle-interior-background.png");
    Texture2D ghostIdle = LoadTexture("ghost-idle.png");
    Texture2D ghostAppears = LoadTexture("ghost-appears.png"); // Ghost spawning spike
    Texture2D ghostShriek = LoadTexture("ghost-shriek.png");
    Texture2D ghostVanish = LoadTexture("ghost-vanish.png");

    // Sound
    InitAudioDevice();
    Sound backGround{LoadSound("spiderDanceOrigin.mp3")};
    
    // Replace the existing initialization of the player rectangle
    Rectangle player = {(screenWidth - 60) / 2, (screenHeight - 120) / 2, 60, 120};

    // Ghost
    const int maxGhosts = 30;
    Rectangle ghosts[maxGhosts];
    int ghostSpeedsX[maxGhosts];
    int ghostSpeedsY[maxGhosts];
    float ghostSpawnTimers;
    const float ghostSpawnTime = 2.0f; // Ghoat will spawn every X.XX sec
    int activeGhosts = 0;
    bool ghostActive[maxGhosts]{};

    int ghostVanishFrame[maxGhosts];
    int ghostAppearsFrame[maxGhosts];

    float ghostAppearsWidth = ghostAppears.width / 6;
    float ghostAppearsHeight = ghostAppears.height;
    float ghostIdleWidth = ghostIdle.width / 7;
    float ghostIdleHeight = ghostIdle.height;
    float ghostVanishWidth = ghostVanish.width / 7;
    float ghostVanishHeight = ghostVanish.height;

    bool ghostAppeared[maxGhosts] = {false};
    bool ghostVanished[maxGhosts] = {false};

    // Bullet
    int maxbullet = 20;
    int bullet = maxbullet;
    float reloadtime = 3;
    float reloadtimer;

    // Game
    Rectangle crosshair = {0, 0, 30, 30};
    Color crosshairColor = YELLOW;
    bool gameOver = true;
    bool menu = true;
    bool enterName = true;
    bool saveScore = false;
    float time = 0;
    float level = 0;
    float timeLevel = 0;
    float nextStage = 5;
    int score = 0;
    int scoreLevel = 0;
    char playerName[256] = "";

    SetTargetFPS(60);

    // Set ghost default
    for (int i = 0; i < maxGhosts; i++)
    {
        ghosts[i] = {GetRandomValue(0, screenWidth), GetRandomValue(0, screenHeight), 60, 120};

        ghostVanishFrame[i] = 0;
        ghostAppearsFrame[i] = 0;

        ghostAppeared[i] = false;
        ghostVanished[i] = false;

        ghostActive[i] = false;
        ghostSpawnTimers = 0.0f;
        ghostSpeedsX[i] = (GetRandomValue(-1, 1));
        if (ghostSpeedsX[i] == 0)
        {
            ghostSpeedsY[i] = (GetRandomValue(0, 1) == 0 ? 1 : -1);
        }
        else
        {
            ghostSpeedsY[i] = (GetRandomValue(-1, 1));
                        }
    } //for (int i = 0; i < maxGhosts; i++)

    while (!WindowShouldClose())
    {
        if (!gameOver && !menu &&!enterName)
        {
            // BG music
            if(!IsSoundPlaying(backGround))
                PlaySound(backGround);

            // Time
            time += GetFrameTime();

            // Handle input for spawning ghosts
            if (IsKeyPressed(KEY_H)) 
            {
                ghostSpawnTimers = ghostSpawnTime;

            }
            
            // ghost spawning
            ghostSpawnTimers += GetFrameTime();
            for (int i = 0; i <= activeGhosts; i++)
            {
                if (!ghostActive[i])
                {
                    if (ghostSpawnTimers >= ghostSpawnTime - timeLevel)
                    {
                        ghostSpawnTimers = 0.0f;
                        ghostActive[i] = true;
                        ghostAppeared[i] = true;
                        activeGhosts++;
                        ghosts[i].x = GetRandomValue(0, screenWidth);
                        ghosts[i].y = GetRandomValue(0, screenHeight);
                    }
                }
            } //for (int i = 0; i <= activeGhosts; i++)

            // Reloading
            if (bullet < maxbullet)
            {
                reloadtimer += GetFrameTime();
                if (reloadtimer >= reloadtime)
                {
                    bullet++;
                    reloadtimer = 0;
                }
            }

            // Shooting ghosts when spacebar is pressed
            if (IsKeyPressed(KEY_SPACE)) 
            {
                if (bullet > 0)
                {
                    bullet--;
                    for (int i = 0; i < maxGhosts; i++) 
                    {
                        // Check collision between crosshair and ghost
                        if (ghostActive[i] && CheckCollisionRecs(crosshair, ghosts[i])) 
                        {
                            ghostVanished[i] = true;
                        }
                    }
                }
            } //if (IsKeyPressed(KEY_SPACE)) 

            // Control rosshair
            if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
                player.x += 7;
            if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
                player.x -= 7;
            if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
                player.y += 7;
            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
                player.y -= 7;
        } //if (!gameOver && !menu &&!enterName)

        // Level++
        timeLevel = level*0.1;
        if(scoreLevel >= nextStage)
        {
            scoreLevel = 0;
            level++;
        }

        // Ensure player stays within window boundaries
        if (player.x < 0)
            player.x = 0;
        if (player.x + player.width > screenWidth)
            player.x = screenWidth - player.width;
        if (player.y < 0)
            player.y = 0;
        if (player.y + player.height > screenHeight)
            player.y = screenHeight - player.height;

        for (int i = 0; i <= maxGhosts; i++)
        {
            if (ghostActive[i] && !ghostVanished[i])
            {
                if (!ghostAppeared[i])
                {
                    ghosts[i].x += ghostSpeedsX[i] * scale;
                    ghosts[i].y += ghostSpeedsY[i] * scale;
                }
            } //if (ghostActive[i])

            // Check boundaries and prevent ghosts from moving outside the screen
            if (ghosts[i].x < 0)
            {
                ghosts[i].x = 0;
                ghostSpeedsX[i] = 1;
            }
            if (ghosts[i].x + ghostIdleWidth > screenWidth)
            {
                ghosts[i].x = screenWidth - ghostIdleWidth;
                ghostSpeedsX[i] = -1;
            }
            if (ghosts[i].y < 0)
            {
                ghosts[i].y = 0;
                ghostSpeedsY[i] = 1;
            }
            if (ghosts[i].y + ghostIdleHeight*2 > screenHeight)
            {
                ghosts[i].y = screenHeight - ghostIdleHeight*2;   
                ghostSpeedsY[i] = -1;
            }
        } //for (int i = 0; i <= activeGhosts; i++)

        crosshair.x = player.x + player.width / 2 - crosshair.width / 2;
        crosshair.y = player.y + player.height / 2 - crosshair.height / 2;

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextureEx(background, {0, 0}, 0.f, scale, WHITE);
        DrawRectangleRec(player, BLANK);

        // Draw text
        DrawText(TextFormat("Player Name: %s", playerName), 10, 10, 20, WHITE);
        DrawText(TextFormat("Score: %d", score), 10, 40, 20, GREEN);
        DrawText(TextFormat("Level: %.0f", level), 10, 70, 20, GREEN);
        DrawText(TextFormat("Ghost: %d/%d", activeGhosts ,maxGhosts), 10, 100, 20, PURPLE);
        DrawText(TextFormat("Spawning: %.1f/%.1f", ghostSpawnTimers, ghostSpawnTime - timeLevel), 10, 130, 20, PURPLE);
        DrawText(TextFormat("Bullet: %d/%d", bullet, maxbullet), 10, 160, 20, YELLOW);
        DrawText(TextFormat("Reloading: %.1f/%.1f", reloadtimer, reloadtime), 10, 190, 20, YELLOW);
        DrawText(TextFormat("Time: %.1f", time), 10, 220, 20, WHITE);
        DrawText(TextFormat("Crosshair: %.0f,%.0f", crosshair.x,crosshair.y), 10, 250, 20, WHITE);

        ghostIdleRunningTime += GetFrameTime();
        if (ghostIdleRunningTime >= updateTime)
        {
            ghostIdleFrame++;
            ghostIdleRunningTime = 0;

            if (ghostIdleFrame >= 7)
            {
                ghostIdleFrame = 0;
            }
        } //if (ghostIdleRunningTime >= updateTime)

        // Draw all active ghosts
        for (int i = 0; i < maxGhosts; i++)
        {
            float faceRight = -1.f;
            if (ghostSpeedsX[i] > 0)
            {   
                faceRight = -1.f;
            }
            else
            {
                faceRight = 1.f;
            } 

            if (ghostActive[i])
            {
                if (ghostAppeared[i] && !ghostVanished[i])
                {
                    Rectangle sourceAppears{ghostAppearsFrame[i] * ghostAppearsWidth, 0.f, faceRight * ghostAppearsWidth, ghostAppearsHeight};
                    DrawTexturePro(ghostAppears, sourceAppears, {ghosts[i].x - ghostAppearsWidth + 4, ghosts[i].y - ghostAppearsHeight + 40, ghostAppearsWidth * scale, ghostAppearsHeight * scale}, {0.f, 0.f}, 0.f, WHITE);

                    ghostAppearsRunningTime += GetFrameTime();
                    if (ghostAppearsRunningTime >= updateTime)
                    {
                        ghostAppearsFrame[i]++;
                        ghostAppearsRunningTime = 0;
                    } //if (ghostAppearsRunningTime >= updateTime)

                    if (ghostAppearsFrame[i] > 5)
                    {
                        ghostAppeared[i] = false;
                        ghostAppearsFrame[i] = 0;
                    }
                } //if (!ghostAppeared[i])
                else if(!ghostVanished[i])
                {
                    Rectangle sourceIdle{ghostIdleFrame * ghostIdleWidth, 0.f, faceRight * ghostIdleWidth, ghostIdleHeight};
                    DrawTexturePro(ghostIdle, sourceIdle, {ghosts[i].x - ghostIdleWidth + 4, ghosts[i].y - ghostIdleHeight + 20, ghostIdleWidth * scale, ghostIdleHeight * scale}, {0.f, 0.f}, 0.f, WHITE);
                }

                //hit blocks
                DrawRectangleLines(ghosts[i].x, ghosts[i].y, ghosts[i].width, ghosts[i].height, GREEN);
            } //if (ghostActive[i])        

            if (ghostVanished[i])
            {
                Rectangle sourceVanish{ghostVanishFrame[i] * ghostVanishWidth, 0.f, faceRight * ghostVanishWidth, ghostVanishHeight};
                DrawTexturePro(ghostVanish, sourceVanish, {ghosts[i].x - ghostVanishWidth + 4, ghosts[i].y - ghostVanishHeight + 20, ghostVanishWidth * scale, ghostVanishHeight * scale}, {0.f, 0.f}, 0.f, WHITE);

                ghostVanishRunningTime += GetFrameTime();
                if (ghostVanishRunningTime >= updateTime)
                {
                    ghostVanishFrame[i]++;
                    ghostVanishRunningTime = 0;
                } //if (ghostVanishRunningTime >= updateTime)

                if (ghostVanishFrame[i] > 6)
                {
                    activeGhosts--;
                    score += 1; // Increase score
                    scoreLevel += 1;
                    ghostActive[i] = false; // Ghost disappears        
                    ghostSpeedsX[i] = (GetRandomValue(-1, 1));
                    if (ghostSpeedsX[i] == 0)
                    {
                        ghostSpeedsY[i] = (GetRandomValue(0, 1) == 0 ? 1 : -1);
                    }
                    else
                    {
                        ghostSpeedsY[i] = (GetRandomValue(-1, 1));
                    }
                    ghostVanished[i] = false;
                    ghostVanishFrame[i] = 0;
                }
            } //if (ghostVanished[i])
        } //for (int i = 0; i < maxGhosts; i++)

        // Draw crosshair
        crosshairColor = YELLOW;
        for (int i = 0; i < maxGhosts; i++) 
        {
            if (ghostActive[i] && CheckCollisionRecs(crosshair, ghosts[i])) 
            {
                crosshairColor = RED;
            }
        } //for (int i = 0; i < maxGhosts; i++) 

        if (!gameOver)
        {
            DrawRectangle(crosshair.x - 10, crosshair.y + crosshair.height / 2 - 1, 10, 3, crosshairColor);
            DrawRectangle(crosshair.x + crosshair.width, crosshair.y + crosshair.height / 2 - 1, 10, 3, crosshairColor);
            DrawRectangle(crosshair.x + crosshair.width / 2 - 1, crosshair.y - 10, 3, 10, crosshairColor);
            DrawRectangle(crosshair.x + crosshair.width / 2 - 1, crosshair.y + crosshair.height, 3, 10, crosshairColor);
        }

        if (activeGhosts >= maxGhosts)
        {
            gameOver = true;
        }

        if(gameOver || menu)
        {
            // Stop soiderdance
            StopSound(backGround);
        }

        if(!menu)
        {
            if (IsKeyPressed(KEY_RIGHT_SHIFT))
            {
                enterName = true;
                gameOver = true;
                menu = true;
            }

            // Restart the game if Enter key is pressed
            if (gameOver && !menu)
            {
                // Save scoreboard
                FILE *scoreBoard = fopen("ScoreBoard.txt", "a");
                if (saveScore)
                {
                    fprintf(scoreBoard, "Name: %s\tScore: %d\n", playerName, score);
                    saveScore = false;
                }
                fclose(scoreBoard);

                DrawText("Game Over!", (screenWidth / 2) - (MeasureText("Game Over!", 75) / 2), 50, 75, RED);
                DrawText("Press Enter to Restart", (screenWidth / 2) - (MeasureText("Press Enter to Restart", 40) / 2), 150, 40, RED);
                DrawText("Press Right Shift to Menu", (screenWidth / 2) - (MeasureText("Press Right Shift to Menu", 40) / 2), 650, 40, RED);

                if (IsKeyPressed(KEY_ENTER))
                {
                    saveScore = true;

                    // Reset game variables here
                    ghostIdleFrame = 0;
                    ghostAppearsRunningTime = 0;
                    ghostIdleRunningTime = 0;
                    ghostVanishRunningTime = 0;

                    activeGhosts = 0;
                    score = 0;
                    scoreLevel = 0;
                    level = 0;
                    time = 0;
                    bullet = maxbullet;
                    reloadtimer = 0;

                    for (int i = 0; i < maxGhosts; i++)
                    {
                        ghostVanishFrame[i] = 0;
                        ghostAppearsFrame[i] = 0;

                        ghostAppeared[i] = false;
                        ghostVanished[i] = false;

                        ghostActive[i] = false;
                        ghostSpawnTimers = 0.0f;
                        ghostSpeedsX[i] = (GetRandomValue(-1, 1));
                        if (ghostSpeedsX[i] == 0)
                        {
                            ghostSpeedsY[i] = (GetRandomValue(0, 1) == 0 ? 1 : -1);
                        }
                        else
                        {
                            ghostSpeedsY[i] = (GetRandomValue(-1, 1));
                        }
                    } //for (int i = 0; i < maxGhosts; i++)

                    player.x = (screenWidth - 60) / 2;
                    player.y = (screenHeight - 120) / 2;
                    gameOver = false;
                } //if (IsKeyPressed(KEY_ENTER))
            } //if (gameOver && !menu)
            else 
            {
                if(IsKeyPressed(KEY_ENTER))
                {
                    gameOver = true;
                }
            } //else         
        } //if(!menu)

        if (menu)
        {
            if(enterName)
            {
                float nameWidth = MeasureText(playerName, 30)+10;
                DrawText("MENU", (screenWidth / 2) - (MeasureText("MENU", 75) / 2), 50, 75, YELLOW);
                DrawText("ENTER YOUR NAME", (screenWidth / 2) - (MeasureText("ENTER YOUR NAME", 40) / 2), 150, 40, YELLOW);
                DrawRectangle((screenWidth / 2) - (nameWidth / 2), 200, nameWidth, 30, BLACK);
                DrawText(playerName, (screenWidth / 2) - (MeasureText(playerName, 30) / 2), 200, 30, WHITE);
                DrawText("Press Enter to Start", (screenWidth / 2) - (MeasureText("Press Enter to Start", 40) / 2), 650, 40, YELLOW);
                if (IsKeyPressed(KEY_ENTER))
                {
                    enterName = false;
                }
                int key = GetKeyPressed();

                if (key >= 32 && key <= 125)
                {
                    playerName[strlen(playerName)] = (char)key;
                }
                else if (key == KEY_BACKSPACE && strlen(playerName) > 0)
                {
                    playerName[strlen(playerName) - 1] = '\0';
                }
            }

            if(IsKeyPressed(KEY_ENTER))
            {
                menu = false;
                saveScore = true;

                // Reset game variables here
                ghostIdleFrame = 0;
                ghostAppearsRunningTime = 0;
                ghostIdleRunningTime = 0;
                ghostVanishRunningTime = 0;

                activeGhosts = 0;
                score = 0;
                scoreLevel = 0;
                level = 0;
                time = 0;
                bullet = maxbullet;
                reloadtimer = 0;

                for (int i = 0; i < maxGhosts; i++)
                {
                    ghostVanishFrame[i] = 0;
                    ghostAppearsFrame[i] = 0;

                    ghostAppeared[i] = false;
                    ghostVanished[i] = false;

                    ghostActive[i] = false;
                    ghostSpawnTimers = 0.0f;
                    ghostSpeedsX[i] = (GetRandomValue(-1, 1));
                    if (ghostSpeedsX[i] == 0)
                    {
                        ghostSpeedsY[i] = (GetRandomValue(0, 1) == 0 ? 1 : -1);
                    }
                    else
                    {
                        ghostSpeedsY[i] = (GetRandomValue(-1, 1));
                    }
                } //for (int i = 0; i < maxGhosts; i++)

                player.x = (screenWidth - 60) / 2;
                player.y = (screenHeight - 120) / 2;
                gameOver = false;
            } //if(IsKeyPressed(KEY_ENTER))
        } //if (menu)

        EndDrawing();
    } //while (!WindowShouldClose())

    CloseWindow();
    return 0;
} //int main()
