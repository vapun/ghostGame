#include <raylib.h> // C++ and Raylib

int ghostAppearsFrame{};
int ghostIdleFrame{};
int ghostVanishFrame{};
const float updateTime{1.f / 12.f};
float runningTime{};
float scale{2.8}; // Scale background & ghost
float timetest;

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

    // Replace the existing initialization of the player rectangle
    Rectangle player = {(screenWidth - 60) / 2, (screenHeight - 120) / 2, 60, 120};

    // Ghost
    const int maxGhosts = 10;
    Rectangle ghosts[maxGhosts];
    int ghostSpeedsX[maxGhosts];
    int ghostSpeedsY[maxGhosts];
    float ghostSpawnTimers;
    float ghostChangeDirTimers;
    const float ghostSpawnTime = 2.0f; // Ghoat will spawn every X.XX sec
    float ghostChangeDirTime = 2.0f; // Ghost will change movement every X.XX sec
    int activeGhosts = 0;
    bool ghostActive[maxGhosts]{};

    float ghostAppearsWidth = ghostAppears.width / 6;
    float ghostAppearsHeight = ghostAppears.height;
    float ghostIdleWidth = ghostIdle.width / 7;
    float ghostIdleHeight = ghostIdle.height;
    float ghostVanishWidth = ghostVanish.width / 7;
    float ghostVanishHeight = ghostVanish.height;

    bool ghostAppeared[maxGhosts] = {false};
    bool ghostVanished[maxGhosts] = {false};

    // Bullet
    int maxbullet = 10;
    int bullet = maxbullet;
    float reloadtime = 1;
    float reloadtimer;

    for (int i = 0; i < maxGhosts; i++)
    {
        ghosts[i] = {GetRandomValue(0, screenWidth), GetRandomValue(0, screenHeight), 60, 120};
        ghostSpeedsX[i] = 0;
        ghostSpeedsY[i] = 0;
    } //for (int i = 0; i < maxGhosts; i++)

    Rectangle crosshair = {0, 0, 30, 30};
    Color crosshairColor = YELLOW;

    bool gameOver = false;

    int score = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (!gameOver)
        {
            runningTime += GetFrameTime();
            if (runningTime >= updateTime)
            {
                ghostAppearsFrame++;
                ghostIdleFrame++;
                ghostVanishFrame++;

                runningTime = 0;

                if (ghostAppearsFrame >= 6)
                {
                    ghostAppearsFrame = 0;
                }

                if (ghostIdleFrame >= 7)
                {
                    ghostIdleFrame = 0;
                }

                if (ghostVanishFrame >= 7)
                {
                    ghostVanishFrame = 0;
                }
            }

            // Control rosshair
            if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
                player.x += 10;
            if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
                player.x -= 10;
            if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
                player.y += 10;
            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
                player.y -= 10;

            // Ensure player stays within window boundaries
            if (player.x < 0)
                player.x = 0;
            if (player.x + player.width > screenWidth)
                player.x = screenWidth - player.width;
            if (player.y < 0)
                player.y = 0;
            if (player.y + player.height > screenHeight)
                player.y = screenHeight - player.height;

            // ghost spawning
            for (int i = 0; i <= activeGhosts; i++)
            {
                if (!ghostActive[i])
                {
                    ghostSpawnTimers += GetFrameTime();
                    timetest = ghostSpawnTime - ghostSpawnTimers;
                    if (ghostSpawnTimers >= ghostSpawnTime)
                    {
                        ghostSpawnTimers = 0.0f;
                        ghostActive[i] = true;
                        ghostAppeared[i] = false;
                        ghostVanished[i] = false;
                        activeGhosts++;
                        ghosts[i].x = GetRandomValue(0, screenWidth);
                        ghosts[i].y = GetRandomValue(0, screenHeight);
                    }
                }
            } //for (int i = 0; i <= activeGhosts; i++)

            // ghost movement
            for (int i = 0; i <= activeGhosts; i++)
            {
                if (ghostActive[i])
                {
                    ghostChangeDirTimers += GetFrameTime();
                    if (ghostChangeDirTimers >= ghostChangeDirTime && ghostAppeared[i])
                    {
                        ghostChangeDirTimers = 0.0f;
                        ghostSpeedsX[i] = GetRandomValue(-1, 1);
                        ghostSpeedsY[i] = GetRandomValue(-1, 1);
                    }

                    ghosts[i].x += ghostSpeedsX[i] * scale;
                    ghosts[i].y += ghostSpeedsY[i] * scale;

                    // Check boundaries and prevent ghosts from moving outside the screen
                    if (ghosts[i].x < 0)
                        ghosts[i].x = 0;
                    if (ghosts[i].x + ghostIdleWidth > screenWidth)
                        ghosts[i].x = screenWidth - ghostIdleWidth;
                    if (ghosts[i].y < 0)
                        ghosts[i].y = 0;
                    if (ghosts[i].y + ghostIdleHeight*2 > screenHeight)
                        ghosts[i].y = screenHeight - ghostIdleHeight*2;   
                } //if (ghostActive[i])                
            } //for (int i = 0; i <= activeGhosts; i++)

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
                            activeGhosts--;
                            score += 1; // Increase score

                            ghostVanished[i] = true;

                            ghostActive[i] = false; // Ghost disappears        
                            ghostSpeedsX[i] = 0;
                            ghostSpeedsY[i] = 0;   
                        }
                    }
                }
            } //if (IsKeyPressed(KEY_SPACE)) 

            // Handle input for spawning ghosts
            if (IsKeyPressed(KEY_H)) 
            {
                for (int i = 0; i <= activeGhosts; i++) 
                {
                    if (!ghostActive[i]) 
                    {
                        ghostActive[i] = true;
                        activeGhosts++;
                        ghosts[i].x = GetRandomValue(0, screenWidth);
                        ghosts[i].y = GetRandomValue(0, screenHeight);
                        break; // Spawned one ghost, exit the loop
                    }
                }
            }
        } //if (!gameOver)

        if (activeGhosts >= maxGhosts)
        {
            gameOver = true;
        }

        crosshair.x = player.x + player.width / 2 - crosshair.width / 2;
        crosshair.y = player.y + player.height / 2 - crosshair.height / 2;

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextureEx(background, {0, 0}, 0.f, scale, WHITE);
        DrawRectangleRec(player, BLANK);

        // Restart the game if Enter key is pressed
        if (IsKeyPressed(KEY_ENTER))
        {
            // Reset game variables here
            ghostAppearsFrame = 0;
            ghostIdleFrame = 0;
            ghostVanishFrame = 0;

            runningTime = 0;
            activeGhosts = 0;
            score = 0;
            timetest = 0;
            bullet = maxbullet;
            reloadtimer = 0;

            for (int i = 0; i < maxGhosts; i++)
            {
                ghostAppeared[i] = false;
                ghostVanished[i] = false;

                ghostActive[i] = false;
                ghostSpawnTimers = 0.0f;
                ghostSpeedsX[i] = 0;
                ghostSpeedsY[i] = 0;
            }

            player.x = (screenWidth - 60) / 2;
            player.y = (screenHeight - 120) / 2;
            gameOver = false;
        } //if (IsKeyPressed(KEY_ENTER))

        // Draw all active ghosts
        for (int i = 0; i < maxGhosts; i++)
        {
            if (ghostActive[i])
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

                if (!ghostAppeared[i])
                {
                    Rectangle source{ghostAppearsFrame * ghostAppearsWidth, 0.f, faceRight * ghostAppearsWidth, ghostAppearsHeight};
                    DrawTexturePro(ghostAppears, source, {ghosts[i].x - ghostAppearsWidth + 4, ghosts[i].y - ghostAppearsHeight + 20, ghostAppearsWidth * scale, ghostAppearsHeight * scale}, {0.f, 0.f}, 0.f, WHITE);

                    if (ghostAppearsFrame >= 5)
                    {
                        ghostAppeared[i] = true;
                        ghostAppearsFrame = 0;
                    }
                }
                else
                {
                    Rectangle source{ghostIdleFrame * ghostIdleWidth, 0.f, faceRight * ghostIdleWidth, ghostIdleHeight};
                    DrawTexturePro(ghostIdle, source, {ghosts[i].x - ghostIdleWidth + 4, ghosts[i].y - ghostIdleHeight + 20, ghostIdleWidth * scale, ghostIdleHeight * scale}, {0.f, 0.f}, 0.f, WHITE);
                }

                // สำหรับทดสอบพื้นที่ยิง DrawRectangleLines(ghosts[i].x, ghosts[i].y, ghosts[i].width, ghosts[i].height, GREEN);
            } //if (ghostActive[i])
            else
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

                if (ghostVanished[i])
                {
                    Rectangle source{ghostVanishFrame * ghostVanishWidth, 0.f, faceRight * ghostVanishWidth, ghostVanishHeight};
                    DrawTexturePro(ghostVanish, source, {ghosts[i].x - ghostVanishWidth + 4, ghosts[i].y - ghostVanishHeight + 20, ghostVanishWidth * scale, ghostVanishHeight * scale}, {0.f, 0.f}, 0.f, WHITE);

                    if (ghostVanishFrame >= 6)
                    {
                        ghostVanished[i] = false;
                        ghostVanishFrame = 0;
                    }
                }
            }
        } //for (int i = 0; i < maxGhosts; i++)
        
        DrawText(TextFormat("Score: %d", score), 10, 10, 20, GREEN);
        DrawText(TextFormat("Ghost: %d/%d", activeGhosts ,maxGhosts), 10, 40, 20, PURPLE);
        DrawText(TextFormat("Spawning: %.1f", timetest), 10, 70, 20, PURPLE);
        DrawText(TextFormat("Bullet: %d", bullet), 10, 100, 20, YELLOW);
        DrawText(TextFormat("Reloading: %.1f", reloadtime-reloadtimer), 10, 130, 20, YELLOW);

        DrawRectangle(crosshair.x - 10, crosshair.y + crosshair.height / 2 - 1, 10, 3, crosshairColor);
        DrawRectangle(crosshair.x + crosshair.width, crosshair.y + crosshair.height / 2 - 1, 10, 3, crosshairColor);
        DrawRectangle(crosshair.x + crosshair.width / 2 - 1, crosshair.y - 10, 3, 10, crosshairColor);
        DrawRectangle(crosshair.x + crosshair.width / 2 - 1, crosshair.y + crosshair.height, 3, 10, crosshairColor);

        if (gameOver)
        {
            DrawText("Game Over! Press Enter to Restart.", (screenWidth / 2) - (MeasureText("Game Over! Press Enter to Restart.", 20) / 2), screenHeight / 2, 20, YELLOW);
        }
        
        EndDrawing();
    } //while (!WindowShouldClose())

    CloseWindow();
    return 0;
} //int main()