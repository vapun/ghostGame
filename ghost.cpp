// studentID 66010483 create game by C++ and Raylib
// Save
#include "raylib.h" // 66010483 C++ and Raylib
#include <stdio.h>
#include <string.h>
#include <string>
#include <math.h>
#include <algorithm>
#include <vector>

float ghostAppearsRunningTime{};
float ghostIdleRunningTime{};
float ghostVanishRunningTime{};

int ghostIdleFrame{};
const float updateTime{1.f / 12.f};
float scale{2.8}; // Scale background & ghost

// Scoreboardfuntion
void EndgameUI();

// Window
const int screenWidth = 661 * scale;
const int screenHeight = 300 * scale;

int main()
{   
    InitWindow(screenWidth, screenHeight, "Ghost Hunt"); // Game name

    // Graphics
    Texture2D background = LoadTexture("graphic and animation/old-dark-castle-interior-background.png");
    Texture2D ghostIdle = LoadTexture("graphic and animation/ghost-idle.png");
    Texture2D ghostAppears = LoadTexture("graphic and animation/ghost-appears.png"); // Ghost spawning spike
    Texture2D ghostShriek = LoadTexture("graphic and animation/ghost-shriek.png");
    Texture2D ghostVanish = LoadTexture("graphic and animation/ghost-vanish.png");

    // Sound
    InitAudioDevice();
    Sound backGroundSound{LoadSound("sfx/spiderDanceOrigin.mp3")};
    SetSoundVolume(backGroundSound, 0.5);
    Sound shootingSound{LoadSound("sfx/shoot02wav-14562.mp3")};
    Sound reloadSound{LoadSound("sfx/load-gun-sound-effect-5-11003.mp3")};
    Sound hitSound{LoadSound("sfx/steam_burst-6822.mp3")};
    SetSoundVolume(hitSound, 2.0);
    Sound startGameSound{LoadSound("sfx/game-start-6104.mp3")};
    Sound spawningSound{LoadSound("sfx/mixkit-aggressive-beast-roar-13.wav")};
    Sound gameOverSound{LoadSound("sfx/kl-peach-game-over-iii-142453.mp3")};
    Sound menuSound{LoadSound("sfx/many-ringtones-com-3893792.mp3")};
    Sound noBulletSound{LoadSound("sfx/empty-gun-shot-6209.mp3")};
    Sound nextLevelSound{LoadSound("sfx/80s-achievement-unlocked-94452.mp3")};
    Sound ramboSound{LoadSound("sfx/auto-machine-gun-84533.mp3")};
    Sound fastAimSound{LoadSound("sfx/audi-v8-acceleration-sound-6067.mp3")};
    SetSoundVolume(fastAimSound, 0.35);
    
    // Replace the existing initialization of the player rectangle
    Rectangle player = {(screenWidth - 60) / 2, (screenHeight - 120) / 2, 60, 120};

    // Ghost
    const int reallyMaxGhosts = 200;
    const int maxGhostsSave = 10;
    int maxGhosts = maxGhostsSave;
    Rectangle ghosts[reallyMaxGhosts];
    int ghostSpeedsX[reallyMaxGhosts];
    int ghostSpeedsY[reallyMaxGhosts];
    float ghostSpeedBuff = 1.f;
    float ghostSpawnTimers = 0.f;
    const float ghostSpawnTimeSave = 2.f; // Ghoat will spawn every X.XX sec
    float ghostSpawnTime = ghostSpawnTimeSave;
    int activeGhosts = 0;
    bool ghostActive[reallyMaxGhosts] = {false};

    int ghostVanishFrame[reallyMaxGhosts] = {0};
    int ghostAppearsFrame[reallyMaxGhosts] = {0};

    float ghostAppearsWidth = ghostAppears.width / 6;
    float ghostAppearsHeight = ghostAppears.height;
    float ghostIdleWidth = ghostIdle.width / 7;
    float ghostIdleHeight = ghostIdle.height;
    float ghostVanishWidth = ghostVanish.width / 7;
    float ghostVanishHeight = ghostVanish.height;

    bool ghostAppeared[reallyMaxGhosts] = {false};
    bool ghostVanished[reallyMaxGhosts] = {false};

    // Bullet
    const int maxBulletSave = 10;
    int maxBullet = maxBulletSave;
    int bullet = maxBullet;
    const float reloadTimeSave = 2.f;
    float reloadTime = reloadTimeSave;
    float reloadTimer;
    float reloadTimeSkillSave;

    // Player
    int cons;
    char playerName[256] = "";
    float playerSpeedBuff = 1.f;

    int playerSpeedSkill = 0;
    int playerSpeedActiveSkill = 0;
    float playerSpeedTimerSkill = 0.f;

    int playerBulletSkill = 0;
    int playerBulletActiveSkill = 0;
    float playerBulletTimerSkill = 0.f;

    // Game
    Rectangle crosshair = {0, 0, 30, 30};
    Color crosshairColor = YELLOW;
    bool gameOver = true;
    bool menu = true;
    bool enterName = true;
    bool saveScore = false;
    bool pauseGame = false;
    bool restartGame = false;
    float time = 0.f;
    float level = 0.f;
    const float nextStageSave = 1.f;
    float nextStage = nextStageSave;
    int score = 0;
    int scoreLevel = 0;
    int getSkill = 0;
    int playerBuffType;
    int ghostsBuffType;
    float nameTime = 0.f;

    SetTargetFPS(60);

    // Game activating
    while (!WindowShouldClose())
    {
        // Game graphic
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextureEx(background, {0, 0}, 0.f, scale, WHITE);
        DrawRectangleRec(player, BLANK);

        if (!pauseGame)
        {
            if (!gameOver && !menu && !enterName)
            {
                // BG music
                if(!IsSoundPlaying(backGroundSound) && !IsSoundPlaying(startGameSound))
                    PlaySound(backGroundSound);

                // Time
                time += GetFrameTime();
                
                // ghost spawning
                ghostSpawnTimers += GetFrameTime();
                for (int i = 0; i <= activeGhosts; i++)
                {   
                    if (!ghostActive[i])
                    {
                        if (ghostSpawnTimers >= ghostSpawnTime)
                        {
                            ghosts[i] = {GetRandomValue(0, screenWidth), GetRandomValue(0, screenHeight), 60, 120};
                            ghostSpeedsX[i] = (GetRandomValue(-1, 1));
                            ghostSpeedsY[i] = (ghostSpeedsX[i] == 0) ? GetRandomValue(0, 1) == 0 ? 1 : -1 : GetRandomValue(-1, 1);

                            ghostSpawnTimers = 0.f;
                            ghostActive[i] = true;
                            ghostAppeared[i] = true;
                            PlaySound(spawningSound);
                            activeGhosts++;
                            ghosts[i].x = GetRandomValue(0, screenWidth);
                            ghosts[i].y = GetRandomValue(0, screenHeight);
                        }
                    }
                } //for (int i = 0; i <= activeGhosts; i++)

                // Reloading
                if (bullet < maxBullet)
                {
                    reloadTimer += GetFrameTime();
                    if (reloadTimer >= reloadTime)
                    {
                        bullet++;
                        PlaySound(reloadSound);
                        reloadTimer = 0;
                    }
                }

                // Shooting ghosts when spacebar is pressed
                if (IsKeyPressed(KEY_SPACE) && !(playerBulletActiveSkill > 0)) 
                {
                    if (bullet > 0)
                    {
                        bullet--;
                        PlaySound(shootingSound);
                        for (int i = 0; i < maxGhosts; i++) 
                        {
                            // Check collision between crosshair and ghost
                            if (ghostActive[i] && CheckCollisionRecs(crosshair, ghosts[i]) && ghostVanished[i] == false) 
                            {
                                ghostVanished[i] = true;
                                PlaySound(hitSound);
                            }
                        }
                    }
                    else
                        PlaySound(noBulletSound);
                } //if (IsKeyPressed(KEY_SPACE)) 

                // Control rosshair
                if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
                    player.x += scale * 2.5 * playerSpeedBuff;
                if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
                    player.x -= scale * 2.5 * playerSpeedBuff;
                if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
                    player.y += scale * 2.5 * playerSpeedBuff;
                if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
                    player.y -= scale * 2.5 * playerSpeedBuff;

                // Handle input for spawning ghosts
                if (IsKeyPressed(KEY_H)) 
                    ghostSpawnTimers = ghostSpawnTime;
            } //if (!gameOver && !menu && !enterName)

            // Level++
            if(scoreLevel >= nextStage)
            {
                scoreLevel = 0;
                level++;
                PlaySound(nextLevelSound);
                getSkill++;    
                nextStage ++;
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

            // Ghost movement
            for (int i = 0; i < maxGhosts; i++)
            {
                if (!ghostAppeared[i] && ghostActive[i] && !ghostVanished[i])
                { 
                    ghosts[i].x += ghostSpeedsX[i] * scale * ghostSpeedBuff;
                    ghosts[i].y += ghostSpeedsY[i] * scale * ghostSpeedBuff;
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
            } //for (int i = 0; i < maxGhosts; i++)

            // Stop animation
            ghostIdleRunningTime += GetFrameTime();
            if (ghostIdleRunningTime >= updateTime)
            {
                ghostIdleFrame++;
                ghostIdleRunningTime = 0;

                if (ghostIdleFrame >= 7)
                    ghostIdleFrame = 0;
            } //if (ghostIdleRunningTime >= updateTime)
        } //if (!pauseGame)

        // Crosshair important
        crosshair.x = player.x + player.width / 2 - crosshair.width / 2;
        crosshair.y = player.y + player.height / 2 - crosshair.height / 2;

        if(IsKeyPressed(KEY_E) && !menu && !gameOver)
        {
            // Change pause and continue
            pauseGame = !pauseGame;

            // Pause sound
            if(IsSoundPlaying(startGameSound))
                PauseSound(startGameSound);
            else
                ResumeSound(startGameSound);

            if(IsSoundPlaying(backGroundSound))
                PauseSound(backGroundSound);
            else
                ResumeSound(backGroundSound);

            if(IsSoundPlaying(nextLevelSound))
                PauseSound(nextLevelSound);
            else
                ResumeSound(nextLevelSound);

            if(IsSoundPlaying(reloadSound))
                PauseSound(reloadSound);
            else
                ResumeSound(reloadSound);

            if(IsSoundPlaying(spawningSound))
                PauseSound(spawningSound);
            else
                ResumeSound(spawningSound);

            if(IsSoundPlaying(shootingSound))
                PauseSound(shootingSound);
            else
                ResumeSound(shootingSound);

            if(IsSoundPlaying(hitSound))
                PauseSound(hitSound);
            else
                ResumeSound(hitSound);

            if(IsSoundPlaying(noBulletSound))
                PauseSound(noBulletSound);
            else
                ResumeSound(noBulletSound);
        }

        if (!gameOver && !menu)
        {
            // Draw text
            cons =  100 - (activeGhosts * 100 / maxGhosts);
            Color consColor;
            consColor = (cons >= 75) ? GREEN : (cons >= 50) ? YELLOW : (cons >= 25) ? ORANGE : RED;
            DrawText(TextFormat("CONSCIOUSNESS: %d%%", cons), 10, 10, 30, consColor);

            DrawText(TextFormat("Player Name: [%s]", playerName), 10, 50, 20, WHITE);
            DrawText(TextFormat("Score: %d", score), 10, 80, 20, GREEN);
            DrawText(TextFormat("Level: %.0f", level), 10, 110, 20, GREEN);
            DrawText(TextFormat("Ghost Limit: %d/%d", activeGhosts, maxGhosts), 10, 140, 20, PURPLE);
            DrawText(TextFormat("Spawning: %.1f/%.1f", ghostSpawnTimers, ghostSpawnTime), 10, 170, 20, PURPLE);
            DrawText(TextFormat("Bullet: %d/%d", bullet, maxBullet), 10, 200, 20, YELLOW);
            DrawText(TextFormat("Reloading: %.1f/%.1f", reloadTimer, reloadTime), 10, 230, 20, YELLOW);
            DrawText(TextFormat("Time: %.1f", time), 10, 260, 20, WHITE);
            DrawText(TextFormat("Next Level: %d/%.0f", scoreLevel, nextStage), 10, 290, 20, WHITE);
        }

        // Draw all active ghosts
        for (int i = 0; i < maxGhosts; i++)
        {
            float faceRight = -1.f;
            faceRight = (ghostSpeedsX[i] > 0) ? -1.f : 1.f;


            // Ghosts appear
            if(ghostAppeared[i] && ghostActive[i] && !ghostVanished[i])
            {
                Rectangle sourceAppears{ghostAppearsFrame[i] * ghostAppearsWidth, 0.f, faceRight * ghostAppearsWidth, ghostAppearsHeight};
                DrawTexturePro(ghostAppears, sourceAppears, {ghosts[i].x - ghostAppearsWidth + 4, ghosts[i].y - ghostAppearsHeight + 40, ghostAppearsWidth * scale, ghostAppearsHeight * scale}, {0.f, 0.f}, 0.f, WHITE);

                if(!pauseGame)
                    ghostAppearsRunningTime += GetFrameTime();

                if(ghostAppearsRunningTime >= updateTime)
                {
                    ghostAppearsFrame[i]++;
                    ghostAppearsRunningTime = 0;
                } 

                if(ghostAppearsFrame[i] > 5)
                {
                    ghostAppeared[i] = false;
                    ghostAppearsFrame[i] = 0;
                }
            }

            // Ghosts active
            if(ghostActive[i] && !ghostVanished[i] && !ghostAppeared[i])
            {
                Rectangle sourceIdle{ghostIdleFrame * ghostIdleWidth, 0.f, faceRight * ghostIdleWidth, ghostIdleHeight};
                DrawTexturePro(ghostIdle, sourceIdle, {ghosts[i].x - ghostIdleWidth + 4, ghosts[i].y - ghostIdleHeight + 20, ghostIdleWidth * scale, ghostIdleHeight * scale}, {0.f, 0.f}, 0.f, WHITE);
            }

            // Ghosts vanish
            if(ghostVanished[i] && ghostActive[i]) // && !ghostAppeared[i])
            {
                Rectangle sourceVanish{ghostVanishFrame[i] * ghostVanishWidth, 0.f, faceRight * ghostVanishWidth, ghostVanishHeight};
                DrawTexturePro(ghostVanish, sourceVanish, {ghosts[i].x - ghostVanishWidth + 4, ghosts[i].y - ghostVanishHeight + 20, ghostVanishWidth * scale, ghostVanishHeight * scale}, {0.f, 0.f}, 0.f, WHITE);

                if (!pauseGame)
                    ghostVanishRunningTime += GetFrameTime();

                if (ghostVanishRunningTime >= updateTime)
                {
                    ghostVanishFrame[i]++;
                    ghostVanishRunningTime = 0;
                } 

                if (ghostVanishFrame[i] > 6)
                {
                    activeGhosts--;
                    score += level+1; // Increase score
                    scoreLevel ++;
                    ghostActive[i] = false; // Ghost disappears        
                    ghostSpeedsX[i] = (GetRandomValue(-1, 1));
                    ghostSpeedsY[i] = (ghostSpeedsX[i] == 0) ? GetRandomValue(0, 1) == 0 ? 1 : -1 : GetRandomValue(-1, 1);

                    ghostVanished[i] = false;
                    ghostVanishFrame[i] = 0;
                }
            } //if (ghostVanished[i])
        } //for (int i = 0; i < maxGhosts; i++)

        // Draw crosshair
        crosshairColor = YELLOW;
        for(int i = 0; i < maxGhosts; i++) 
        {
            if(ghostActive[i] && CheckCollisionRecs(crosshair, ghosts[i])) 
                crosshairColor = RED;
        } //for (int i = 0; i < maxGhosts; i++) 

        if(!gameOver)
        {
            DrawRectangle(crosshair.x - 10, crosshair.y + crosshair.height / 2 - 1, 10, 3, crosshairColor);
            DrawRectangle(crosshair.x + crosshair.width, crosshair.y + crosshair.height / 2 - 1, 10, 3, crosshairColor);
            DrawRectangle(crosshair.x + crosshair.width / 2 - 1, crosshair.y - 10, 3, 10, crosshairColor);
            DrawRectangle(crosshair.x + crosshair.width / 2 - 1, crosshair.y + crosshair.height, 3, 10, crosshairColor);
        }

        if(activeGhosts >= maxGhosts)
            gameOver = true;

        if(pauseGame)
        {
            DrawText("PAUSE", (screenWidth / 2) - (MeasureText("PAUSE", 75) / 2), 50, 75, YELLOW);
            DrawText("Press E to Continue", (screenWidth / 2) - (MeasureText("Press E to Continue", 40) / 2), 150, 40, YELLOW);
            DrawText("Press Enter to Endgame", (screenWidth / 2) - (MeasureText("Press Enter to Endgame", 40) / 2), 700, 40, YELLOW);
        }

        // Give a buff to player
        if(getSkill > 0)
        {
            // Player buff
            playerBuffType = (reloadTime > 0.8) ? GetRandomValue(0, 3) : GetRandomValue(1, 3);

            if(playerBuffType == 0)
            {
                reloadTime -= 0.2; 
                maxBullet++;
            }
            else if(playerBuffType == 1)
            {
                playerSpeedSkill++;
            }
            else if(playerBuffType == 2)
            {
                maxGhosts += 10;
            }
            else if(playerBuffType == 3)
            {
                playerBulletSkill++;
            }

            // Ghost buff
            ghostsBuffType = (ghostSpawnTime > 0.8) ? GetRandomValue(0, 1) : 0;

            if(ghostsBuffType == 0)
                ghostSpeedBuff += 0.2;
            else if(ghostsBuffType == 1)
                ghostSpawnTime -= 0.2;

            getSkill--;
        }

        if(IsSoundPlaying(nextLevelSound) && !gameOver && !menu)
        {
            DrawText(TextFormat("- LEVEL %.0f -", level), (screenWidth / 2) - (MeasureText(TextFormat("- LEVEL %.0f -", level), 75) / 2), 50, 75, YELLOW);

            const char* buffTexts[] = { "Ammo Buff", "Get Fast Aim Skill", "Consciousness Buff", "Get Rambo Skill" };
            int buffIndex = playerBuffType;

            if (buffIndex >= 0 && buffIndex < 4) 
            {
                DrawText(buffTexts[buffIndex], (screenWidth / 2) - (MeasureText(buffTexts[buffIndex], 40) / 2), 700, 40, GREEN);
            }


            if(ghostsBuffType == 0)
            {
                DrawText("Ghost Crazy", (screenWidth / 2) - (MeasureText("Ghost Crazy", 40) / 2), 150, 40, RED);
            }
            else if(ghostsBuffType == 1)
            {
                DrawText("Ghost Spawn Faster", (screenWidth / 2) - (MeasureText("Ghost Spawn Faster", 40) / 2), 150, 40, RED);
            }
        }

        if((playerSpeedSkill > 0) && IsKeyPressed(KEY_F) && !pauseGame)
        {
            playerSpeedBuff += 0.8;
            playerSpeedActiveSkill++;
            playerSpeedSkill --;
        }

        if(playerSpeedActiveSkill > 0)
        {
            if(pauseGame && IsSoundPlaying(fastAimSound))
                PauseSound(fastAimSound);
            else if(!pauseGame && IsSoundPlaying(fastAimSound))
                ResumeSound(fastAimSound);
            else if(!pauseGame && !IsSoundPlaying(fastAimSound))
                PlaySound(fastAimSound);

            if (!gameOver && !menu)
                DrawText(TextFormat("Fast Aim Activating: %.1f", 12-playerSpeedTimerSkill), 10, 380, 20, WHITE);
                
            if(!pauseGame)
                playerSpeedTimerSkill += GetFrameTime();

            if(playerSpeedTimerSkill >= 12)
            {
                playerSpeedBuff -= 0.8;
                playerSpeedTimerSkill = 0;
                playerSpeedActiveSkill --;
                if(IsSoundPlaying(fastAimSound))
                    StopSound(fastAimSound);
            }
        }

        if((playerBulletSkill > 0) && IsKeyPressed(KEY_R) && !pauseGame)
        {
            playerBulletActiveSkill ++;
            playerBulletSkill --;
        }

        if(playerBulletActiveSkill > 0)
        {
            if(pauseGame && IsSoundPlaying(ramboSound))
                PauseSound(ramboSound);
            else if(!pauseGame && IsSoundPlaying(ramboSound))
                ResumeSound(ramboSound);
            else if(!pauseGame && !IsSoundPlaying(ramboSound))
                PlaySound(ramboSound);

            if (!gameOver && !menu)
                DrawText(TextFormat("Rambo Activating: %.1f", 12-playerBulletTimerSkill), 10, 410, 20, WHITE);

            for (int i = 0; i < maxGhosts; i++) 
            {
                // Check collision between crosshair and ghost
                if (ghostActive[i] && CheckCollisionRecs(crosshair, ghosts[i]) && ghostVanished[i] == false) 
                {
                    ghostVanished[i] = true;
                    PlaySound(hitSound);
                }
            }
            if(!pauseGame)
                playerBulletTimerSkill += GetFrameTime();

            if(playerBulletTimerSkill >= 12)
            {
                playerBulletTimerSkill = 0;
                playerBulletActiveSkill --;
                if(IsSoundPlaying(ramboSound))
                    StopSound(ramboSound);
            }
        }

        if (!gameOver && !menu)
        {
            DrawText(TextFormat("[F] Fast Aim x %d", playerSpeedSkill), 10, 320, 20, WHITE);
            DrawText(TextFormat("[R] Rambo x %d", playerBulletSkill), 10, 350, 20, WHITE);
        }

        if(!menu)
        {
            // Restart the game if Enter key is pressed
            if(gameOver)
            {
                DrawText("GAME OVER!", (screenWidth / 2) - (MeasureText("GAME OVER!", 75) / 2), 50, 75, RED);
                DrawText(TextFormat("66010483 PUNNAWIT SUKHUMVADA KMITL"), 10, 10, 20, YELLOW);
                DrawText("Press Enter to Restart", (screenWidth / 2) - (MeasureText("Press Enter to Restart", 40) / 2), 150, 40, RED);
                DrawText("Press Right Shift to Menu", (screenWidth / 2) - (MeasureText("Press Right Shift to Menu", 40) / 2), 700, 40, RED);

                // Save score
                FILE *scoreHistory = fopen("scores.txt", "a");
                if(saveScore)
                {
                    fprintf(scoreHistory, "[%s]\t%d\n", playerName, score);
                    saveScore = false;
                    PlaySound(gameOverSound);
                }
                fclose(scoreHistory);

                if(!IsSoundPlaying(menuSound) && !IsSoundPlaying(gameOverSound))
                    PlaySound(menuSound);

                if(IsKeyPressed(KEY_ENTER))
                    restartGame = true;
            } //if (gameOver && !menu)
            else 
            {
                if (IsKeyPressed(KEY_ENTER))
                {
                    if (pauseGame)
                        pauseGame = false;

                    gameOver = true;
                }
            } //else      

            if (IsKeyPressed(KEY_RIGHT_SHIFT))
            {
                if (pauseGame)
                    pauseGame = !pauseGame;

                gameOver = true;
                enterName = true;
                menu = true;
            }
        } //if(!menu)

        if (menu || gameOver)
        {
            // Show scoreboard(COPY)
            EndgameUI();

            if(enterName)
            {
                if ((IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) && strlen(playerName) != 0)
                    enterName = false;

                if ((IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) && strlen(playerName) == 0)
                    PlaySound(shootingSound);

                int key = GetKeyPressed();

                if (key >= 32 && key <= 125 && key != KEY_SPACE && strlen(playerName) <= 25)
                {
                    PlaySound(noBulletSound);
                    playerName[strlen(playerName)] = (char)key;
                }
                else if (key == KEY_BACKSPACE && strlen(playerName) > 0)
                {
                    PlaySound(shootingSound);
                    playerName[strlen(playerName) - 1] = '\0';
                }
            }

            // Show player name
            nameTime += GetFrameTime();
            if (nameTime >= 0.25)
            {
                float nameWidth = MeasureText(playerName, 30)+10;
                DrawRectangle((screenWidth / 2) - (nameWidth / 2), 200, nameWidth, 30, BLACK);
                DrawText(playerName, (screenWidth / 2) - (MeasureText(playerName, 30) / 2), 200, 30, WHITE);

                if (nameTime >= 1.25)
                    nameTime = 0;
            }

            // Stop soiderdance
            StopSound(fastAimSound);
            StopSound(ramboSound);
            StopSound(backGroundSound);
        }

        if (menu)
        {
            if(!IsSoundPlaying(menuSound) && !IsSoundPlaying(gameOverSound))
                PlaySound(menuSound);

            DrawText("GHOST HUNT", (screenWidth / 2) - (MeasureText("GHOST HUNT", 75) / 2), 50, 75, YELLOW);
            DrawText(TextFormat("66010483 PUNNAWIT SUKHUMVADA KMITL"), 10, 10, 20, YELLOW);
            DrawText("ENTER YOUR NAME", (screenWidth / 2) - (MeasureText("ENTER YOUR NAME", 40) / 2), 150, 40, YELLOW);

            if (strlen(playerName) > 0)
                DrawText("Press Enter/Space to Start", (screenWidth / 2) - (MeasureText("Press Enter/Space to Start", 40) / 2), 700, 40, YELLOW);

            // Srat game
            if((IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) && strlen(playerName) > 0)
                restartGame = true;
        } //if (menu)

        // Reset game variables here
        if(restartGame)
        {
            // Clear sound
            StopSound(menuSound);
            StopSound(gameOverSound);
            StopSound(nextLevelSound);
            PlaySound(startGameSound);

            // Reset game
            saveScore = true;
            score = 0;
            scoreLevel = 0;
            nextStage = nextStageSave;
            level = 0;
            time = 0; 
            menu = false;

            // Reset player
            playerSpeedActiveSkill = 0;
            playerBulletActiveSkill = 0;
            playerSpeedTimerSkill = 0;
            playerBulletTimerSkill = 0;
            playerSpeedBuff = 1;
            playerSpeedSkill = 0;
            playerBulletSkill = 0;

            // Reset ghosts
            maxGhosts = maxGhostsSave;
            ghostIdleFrame = 0;
            ghostAppearsRunningTime = 0;
            ghostIdleRunningTime = 0;
            ghostVanishRunningTime = 0;
            ghostSpawnTime = ghostSpawnTimeSave;
            activeGhosts = 0;
            ghostSpeedBuff = 1;

            // Reset crosshair
            player.x = (screenWidth - 60) / 2;
            player.y = (screenHeight - 120) / 2;   

            // Reset bullet
            maxBullet = maxBulletSave;
            bullet = maxBullet;
            reloadTimer = 0;
            reloadTime = reloadTimeSave;

            // Close pause game
            if(pauseGame)
                pauseGame = false;

            // Reset ghost array
            for (int i = 0; i < reallyMaxGhosts; i++)
            {
                ghostVanishFrame[i] = 0;
                ghostAppearsFrame[i] = 0;

                ghostAppeared[i] = false;
                ghostVanished[i] = false;

                ghostActive[i] = false;
                ghostSpawnTimers = 0.f;
                ghostSpeedsX[i] = (GetRandomValue(-1, 1));
                ghostSpeedsY[i] = (ghostSpeedsX[i] == 0) ? (GetRandomValue(0, 1) == 0 ? 1 : -1) : GetRandomValue(-1, 1);
            } //for (int i = 0; i < maxGhosts; i++)   

            // Start game
            gameOver = false;     

            // Reset game variables success
            restartGame = false;
        } //if(restartGame)

        EndDrawing();
    } //while (!WindowShouldClose())

    CloseWindow();
    return 0;
} //int main()

// ScoreBoard function
void EndgameUI()
{
    int yOffset = 240;
    // int yOffset = screenHeight / 4 + 100;

    struct ScoreData
    {
        char *name;
        int score;
    };

    std::vector<ScoreData> fileScores;

    FILE *scoreFile = fopen("scores.txt", "rb");
    if (scoreFile != NULL)
    {
        char line[256];
        while (fgets(line, sizeof(line), scoreFile))
        {
            char name[100];
            int score;

            if (sscanf(line, "%s %d", name, &score) == 2)
                fileScores.push_back({strdup(name), score});
        }
        fclose(scoreFile);

        // Sort the scores in descending order
        std::sort(fileScores.begin(), fileScores.end(), [](const ScoreData &a, const ScoreData &b)
                  { return a.score > b.score; });

        // Display the top 8 scores in the scoreboard
        int scoresToDisplay = std::min(static_cast<int>(fileScores.size()), 10); // Display up to 10 scores
        DrawText("Name", screenWidth / 5 + 175, yOffset, 30, WHITE);
        DrawText("Score", screenWidth * 3 / 5 + 100, yOffset, 30, WHITE);
        yOffset += 40;
        for (int i = 0; i < scoresToDisplay; ++i)
        {
            DrawText(fileScores[i].name, screenWidth / 5 + 175, yOffset , 30, PURPLE);
            DrawText(std::to_string(fileScores[i].score).c_str(), screenWidth * 3 / 5 + 100, yOffset, 30, GREEN);
            yOffset += 40;
        }

        // Free the memory allocated for name
        for (auto &scoreData : fileScores)
            free(scoreData.name);
    }
}
