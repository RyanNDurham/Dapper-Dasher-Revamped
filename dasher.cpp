#include "raylib.h" 
#include <string>
#include <iostream>
#include <fstream>

//*******************************************************************************
// Structures 
//*******************************************************************************

// AnimData is for Scarfy 
struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    int maxFrame;
    float updateTime;
    float runningTime;
    bool backwards;
};

// NebAnimData is for the randomly generated Nebulaes
struct NebAnimData
{
    Rectangle rec;
    Vector2 pos;
    int xFrame;
    int yFrame;
    int maxXFrame;
    int maxYFrame;
    float updateTime;
    float runningTime;
    bool scored;
    float vel;
};

// BgData
struct BgData
{
    Vector2 pos;
    float rotation;
    float bgX;
    float vel;
    float size;

};

//*******************************************************************************
// Global Varaiables
//*******************************************************************************

// Deafult high score 
// Need to look into how to store this for an html5 game
int currentHighScore{2100};

// Window dimensions
const int windowWidth{800};
const int windowHeight{450};

// Jump velocity
const int jumpVel{-600};

// Initial velocity
int velocity{0};

// Gravity vaiue
const int gravity{1'000};

// Bool on if Scarfy is in the air
bool isInAir{false};

// Universal update time for sprite animation
const float standardUpdateTime{1.0f/8.0f};

// Amount of nebulaes created
// After testing various amounts out 4 seemed to be a happy medium 
const int sizeOfNebulae = 4;

// Amount of backgrounds generated
// 3 are needed to keep the scrolling backgrounds from having visible seams
const int sizeOfBackgrounds = 3;

// Starting Volume
float volume{0.5f};

// Starting Score
int score{0};

// Score counter to determine bonus
int scoreCount{0};

// Frame count to display bonus
int bonusFrameCounter{0};

// Bonus storage to display bonus
int bonus{0};

// Bonus moving up velocity
int moveUp{-5};

// Bool to make sure new high score is not displayed ever time after the first time
bool newHighscore{};

// Frame count to display new high score
int highscoreCounter{0};

// Frame count to change color every 10 frames
int colorCount{0};

// bool check for color count
bool changeColor{};

// bool for if the music is muted or not
bool mute{false};

// Frame count to display volume control changes
int muteFrameCounter{0};
int lowerVolFrameCounter{0};
int raiseVolFrameCounter{0};

// Current info page;
int pageNum{1};

// bool to check if volume has been changed
bool volChange{};

// bool to check if gameplay is paused
bool pause{false};

// Default text color
Color color{WHITE};

// Frame count to change paused color
int pauseFrameCounter{0};

// bool check for pause frame count
bool pauseColorSwitch{true};

// Jump buffer so the Scarfy does not have to be on the ground before jumping
bool jumpBuffer{false};


// Enum for screen types
typedef enum GameScreen {TITLE = 0, INFO, GAMEPLAY, ENDING } GameScreen;

// Starting game screen
GameScreen currentScreen = TITLE;

// Nebulae array decleration
NebAnimData nebulae[sizeOfNebulae]{}; 

std::string endingString{"Press ENTER to go back to the Title"};

std::string highscoreString = "Highscore: " + std::to_string(currentHighScore);

bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
};

NebAnimData UpdateNebAnimData(NebAnimData data, float dt)
{        
    data.runningTime += dt;

    if(data.runningTime >+ data.updateTime)
    {
        data.runningTime = 0.0;

        if(data.yFrame == data.maxYFrame)
        {
            data.rec.x = data.xFrame * data.rec.width;
            data.rec.y = data.yFrame * data.rec.height;
            data.xFrame++;
            if(data.xFrame > 4)
            {
                data.xFrame = 0;
                data.yFrame = 0;
            }
        }
        else
        {
            data.rec.x = data.xFrame * data.rec.width;
            data.rec.y = data.yFrame * data.rec.height;
            data.xFrame++;
            if(data.xFrame > data.maxXFrame)
            {
                data.xFrame = 0;
                data.yFrame++;
            }
        }

    }

    return data;
};


AnimData UpdateAnimData(AnimData data, float dt, bool backwards)
{        
    data.runningTime += dt;

    if(data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;

        if(!backwards)
        {
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if(data.frame > data.maxFrame)
        {
            data.frame = 0;
        }
        }
        else
        {
            if(data.frame == 0)
            {
                data.frame = data.maxFrame;
            }
            data.rec.x = data.frame * data.rec.width;
            data.frame--;
        }
    }

    return data;
};

 void DrawBackground(BgData data[], Texture2D texture, float dt, const int size, bool pause, Color color)
{

        for (int i = 0; i < size; i++)
        {

            
            if(!pause)
            {
                data[i].bgX += data[i].vel * dt;

                if(data[i].bgX <= -texture.width * 2.5f)
                {
                    if (i == 0)
                    {
                        data[i].bgX = data[size - 1].bgX + texture.width * 2.5f - 5.0f;
                    }
                    else
                    {
                        data[i].bgX = data[i - 1].bgX + texture.width * 2.5f - 5.0f;
                    }
                    
                }

                data[i].pos = {data[i].bgX, 0.0f};

                DrawTextureEx(texture, data[i].pos, data[i].rotation, data[i].size, color);

            }
            else
            {
            DrawTextureEx(texture, data[i].pos, data[i].rotation, data[i].size, color);
            }

        }
};

void DrawBackground(BgData data[], Texture2D texture, float dt, const int size, Color color)
{

        for (int i = 0; i < size; i++)
        {
                data[i].bgX += data[i].vel * dt;

                if(data[i].bgX <= -texture.width * 2.5f)
                {
                    if (i == 0)
                    {
                        data[i].bgX = data[size - 1].bgX + texture.width * 2.5f - 5.0f;
                    }
                    else
                    {
                        data[i].bgX = data[i - 1].bgX + texture.width * 2.5f - 5.0f;
                    }
                    
                }

                data[i].pos = {data[i].bgX, 0.0f};

                DrawTextureEx(texture, data[i].pos, data[i].rotation, data[i].size, color);
        }
};

int UpdateScore(int count)
{
    if (count % 25 == 0)
    {
        return 1000;
    }
    else if (count % 10 == 0)
    {
        return 500;
    }
    else if (count % 5 == 0)
    {
        return 200;
    }
    else
    {
        return 0;
    }
};



using namespace std;

int main()
{        
    

    InitWindow(windowWidth, windowHeight, "Dapper Dasher");

    // Audio intialization
    InitAudioDevice();
    
    // Load Music
    Music music = LoadMusicStream("music/return-of-club.ogg");

    // Default Music Volume to 50%
    SetMusicVolume(music, volume);

    //Start Music on Start
    PlayMusicStream(music);
    
    // Scarfy Texture
    Texture2D scarfy = LoadTexture("textures/scarfy.png");

    // Gameplay Scarfy Data
    AnimData scarfyData{
        {0, 0, (float)scarfy.width / 6.0f, (float)scarfy.height},
        {(float)windowWidth / 2.0f - (float)scarfy.width / 6.0f/ 2.0f, (float)windowHeight - (float)scarfy.height}, 
        0, 
        5, 
        standardUpdateTime, 
        0,
        false,
        };    

    // Info Screen Scarfy Data  
    AnimData scarfyInfoData{
        {0, 0, (float)scarfy.width / 6.0f, (float)scarfy.height},
        {(float)windowWidth / 2.0f - (float)scarfy.width / 6.0f/ 2.0f, (float)windowHeight / 2 - (float)scarfy.height / 2}, 
        0, 
        5, 
        standardUpdateTime, 
        0,
        false,
        };

    // Nebula setup
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    // Nebulae Data
    for(int i = 0; i < sizeOfNebulae; i++)
    {
        nebulae[i].rec.x = 0;
        nebulae[i].rec.y = 0;
        nebulae[i].rec.width = nebula.width / 8;
        nebulae[i].rec.height = nebula.height / 8;
        nebulae[i].pos.x = windowWidth + i * GetRandomValue(150, 800);
        nebulae[i].pos.y = windowHeight - nebula.height / 8;
        nebulae[i].vel = GetRandomValue(150, 250);
        nebulae[i].xFrame = 0; 
        nebulae[i].yFrame = 0; 
        nebulae[i].maxXFrame = 7;
        nebulae[i].maxYFrame = 7;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = standardUpdateTime;
        nebulae[i].scored = false;
    }

    // Background Texture
    Texture2D background = LoadTexture("textures/far-buildings.png");

    // Background Data
    BgData backgrounds[sizeOfBackgrounds];
    for(int i = 0; i < sizeOfBackgrounds; i++)
    {
        backgrounds[i].pos = {0.0f,0.0f};
        backgrounds[i].bgX = 0.0f + background.width * i * 2.5f;
        backgrounds[i].rotation = 0.0f;
        backgrounds[i].size = 2.5f;
        backgrounds[i].vel = -20.0f;

    }

    // Midground Texture
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    
    // Midground Data
    BgData midgrounds[sizeOfBackgrounds];
    for(int i = 0; i < sizeOfBackgrounds; i++)
    {
        midgrounds[i].pos = {0.0f,0.0f};
        midgrounds[i].bgX = 0.0f + background.width * i * 2.5f;
        midgrounds[i].rotation = 0.0f;
        midgrounds[i].size = 2.5f;
        midgrounds[i].vel = -40.0f;
    }

    // Foreground Texture
    Texture2D foreground = LoadTexture("textures/foreground.png");

    // Foreground Data
    BgData foregrounds[sizeOfBackgrounds];
    for(int i = 0; i < sizeOfBackgrounds; i++)
    {
        foregrounds[i].pos = {0.0f,0.0f};
        foregrounds[i].bgX = 0.0f + background.width * i * 2.5f;
        foregrounds[i].rotation = 0.0f;
        foregrounds[i].size = 2.5f;
        foregrounds[i].vel = -80.0f;
    }

    //Info Border
    Texture2D infoBorder = LoadTexture("textures/border.png");

    SetTargetFPS(60);
    while(!WindowShouldClose())
    {        

        UpdateMusicStream(music);

        const float dt {GetFrameTime()};

    BeginDrawing();
    ClearBackground(WHITE);

    string bonusString = "+ " + to_string(bonus);
    string scoreString = "Score: " + to_string(score);
    //Updates based on screen type
    switch(currentScreen)
    {
        case TITLE:
        {

            if(IsKeyPressed(KEY_ENTER))
            {
                currentScreen = GAMEPLAY;
            }

            if(IsKeyPressed(KEY_R))
            {
                currentHighScore = 0;
                highscoreString = "Highscore: " + to_string(currentHighScore);
            }

            if(IsKeyPressed(KEY_I))
            {
                currentScreen = INFO;
            }

            if(IsKeyPressed(KEY_M) || volChange)
            {
                if(!volChange)
                {
                    mute = !mute;
                    muteFrameCounter = 30;
                }

                if(mute)
                {
                    SetMusicVolume(music, 0.0);
                }
                else
                {
                    SetMusicVolume(music, volume);
                }

                if(volChange && mute)
                {
                    SetMusicVolume(music, volume);
                    mute = false;
                }
                
                volChange = false;
            }

            if(IsKeyPressed(KEY_EQUAL))
            {
                raiseVolFrameCounter = 30;
                if(volume < 1.0)
                {
                    volume += 0.1;
                    SetMusicVolume(music, volume);
                    volChange = true;
                }
            }                
            
            if(IsKeyPressed(KEY_MINUS))
            {
                lowerVolFrameCounter = 30;
                if(!(volume - 0.1 < 0.0))
                {
                    volume -= 0.1;
                    SetMusicVolume(music, volume);
                    volChange = true;
                }
            }

        } break;
        case INFO:
        {
            //Music Controls
            if(IsKeyPressed(KEY_M) || volChange)
            {
                if(!volChange)
                {
                    mute = !mute;
                    muteFrameCounter = 30;
                }

                if(mute)
                {
                    SetMusicVolume(music, 0.0);
                }
                else
                {
                    SetMusicVolume(music, volume);
                }

                if(volChange && mute)
                {
                    SetMusicVolume(music, volume);
                    mute = false;
                }
                
                volChange = false;
            }

            if(IsKeyPressed(KEY_EQUAL))
            {
                raiseVolFrameCounter = 30;
                if(volume < 1.0)
                {
                    volume += 0.1;
                    SetMusicVolume(music, volume);
                    volChange = true;
                }
            }                
            
            if(IsKeyPressed(KEY_MINUS))
            {
                lowerVolFrameCounter = 30;
                if(!(volume - 0.1 < 0.0))
                {
                    volume -= 0.1;
                    SetMusicVolume(music, volume);
                    volChange = true;
                }
            }


            if(IsKeyPressed(KEY_ENTER))
            {
                currentScreen = TITLE;
                pageNum = 1;
                scarfyInfoData.pos = {(float)windowWidth / 2.0f - (float)scarfy.width / 6.0f/ 2.0f, (float)windowHeight / 2 - (float)scarfy.height / 2};

            }

            if(IsKeyPressed(KEY_LEFT))
            {
                if(pageNum > 1)
                {
                    pageNum--;
                }
            }

            if(IsKeyPressed(KEY_RIGHT))
            {
                if(pageNum < 4)
                {
                    pageNum++;
                }
            } 

            if(pageNum != 1)
            {
                scarfyInfoData.pos = {(float)windowWidth / 2.0f - (float)scarfy.width / 6.0f/ 2.0f, (float)windowHeight / 2 - (float)scarfy.height / 2};
            }

            if(pageNum == 1)
            {
                if(isOnGround(scarfyInfoData, windowHeight / 2 + scarfy.height / 2) )
                {           
                    velocity = 0;
                    isInAir = false;  
                }
                else
                {
                    velocity += gravity * dt;
                    isInAir = true;  
                }   

                if(IsKeyPressed(KEY_SPACE) && !isInAir)  
                {
                    velocity += jumpVel;
                    isInAir = true;
                }


                if(IsKeyDown(KEY_D))
                {

                    if(!isInAir)
                    {
                        if(scarfyInfoData.pos.x <= windowWidth - 200)
                        {
                            scarfyInfoData.pos.x += 200 * dt;
                        }
                    }
                    else
                    {   
                        if(scarfyInfoData.pos.x <= windowWidth - 200)
                        {
                            scarfyInfoData.pos.x += 125 * dt;
                        }
                    } 
                }   
                
                if(IsKeyUp(KEY_D))
                {
                    scarfyInfoData.updateTime = standardUpdateTime;
                }
                
                if(IsKeyDown(KEY_A))
                {
                    if(!isInAir)
                    {
                        if(scarfyInfoData.pos.x >= 100)
                        {
                            scarfyInfoData.pos.x -= 200 * dt;
                        }
                    }
                    else
                    { 
                        if(scarfyInfoData.pos.x >= 100)
                        {
                            scarfyInfoData.pos.x -= 125 * dt;
                        }
                    }
                    scarfyInfoData.backwards = true;      
                }
                
                if(IsKeyUp(KEY_A))
                {
                    scarfyInfoData.backwards = false;      
                }
                
                if( scarfyInfoData.pos.y + velocity * dt > windowHeight - scarfyInfoData.rec.height)
                {
                    scarfyInfoData.pos.y = windowHeight - scarfyInfoData.rec.height;
                }
                else
                {
                    scarfyInfoData.pos.y += velocity * dt; 
                }
            
            }

            scarfyInfoData = UpdateAnimData(scarfyInfoData, dt, false);

            nebulae[0] = UpdateNebAnimData(nebulae[0], dt);

        } break;
        case GAMEPLAY:
        {                
            //Music Controls
            if(IsKeyPressed(KEY_M) || volChange)
            {
                if(!volChange)
                {
                    mute = !mute;
                    muteFrameCounter = 30;
                }

                if(mute)
                {
                    SetMusicVolume(music, 0.0);
                }
                else
                {
                    SetMusicVolume(music, volume);
                }

                if(volChange && mute)
                {
                    SetMusicVolume(music, volume);
                    mute = false;
                }
                
                volChange = false;
            }

            if(IsKeyPressed(KEY_EQUAL))
            {
                raiseVolFrameCounter = 30;
                if(volume < 1.0)
                {
                    volume += 0.1;
                    SetMusicVolume(music, volume);
                    volChange = true;
                }
            }                
            
            if(IsKeyPressed(KEY_MINUS))
            {
                lowerVolFrameCounter = 30;
                if(!(volume - 0.1 < 0.0))
                {
                    volume -= 0.1;
                    SetMusicVolume(music, volume);
                    volChange = true;
                }
            }

            if(IsKeyPressed(KEY_P))
            {
                pause = !pause;

                if(pause)
                {
                    color = DARKGRAY;
                }
                else
                {
                    color = WHITE;
                }
            }

            if(!pause)
            {

                //Game Updates
                //Check if scarfy is on the ground
                if(isOnGround(scarfyData, windowHeight))
                {           
                    velocity = 0;
                    isInAir = false;  
                }
                else
                {
                    velocity += gravity * dt;
                    isInAir = true;  
                }   

                if(!isInAir && jumpBuffer)
                {
                    velocity += jumpVel;
                    isInAir = true;
                    jumpBuffer = false;
                }

                if(IsKeyPressed(KEY_SPACE) && !isInAir)  
                {
                    velocity += jumpVel;
                    isInAir = true;
                }                    
                else if(IsKeyPressed(KEY_SPACE) && isInAir && scarfyData.pos.y >= windowHeight - 200)
                {
                        jumpBuffer = true;
                }

                if(IsKeyDown(KEY_D))
                {

                    if(!isInAir)
                    {
                        if(scarfyData.pos.x <= windowWidth - scarfyData.rec.width)
                        {
                            scarfyData.pos.x += 200 * dt;
                        }
                    }
                    else
                    {   
                        if(scarfyData.pos.x <= windowWidth - scarfyData.rec.width)
                        {
                            scarfyData.pos.x += 125 * dt;
                        }
                    } 
                }   
                
                if(IsKeyUp(KEY_D))
                {
                    scarfyData.updateTime = standardUpdateTime;
                }
                
                if(IsKeyDown(KEY_A))
                {
                    if(!isInAir)
                    {
                        if(scarfyData.pos.x >= 0)
                        {
                            scarfyData.pos.x -= 200 * dt;
                        }
                    }
                    else
                    { 
                        if(scarfyData.pos.x >= 0)
                        {
                            scarfyData.pos.x -= 125 * dt;
                        }
                    }
                    scarfyData.backwards = true;      
                }
                
                if(IsKeyUp(KEY_A))
                {
                    scarfyData.backwards = false;      
                }
                

                // update scarfy y position based on velocity and ground position
                if( scarfyData.pos.y + velocity * dt > windowHeight - scarfyData.rec.height)
                {
                    scarfyData.pos.y = windowHeight - scarfyData.rec.height;
                }
                else
                {
                    scarfyData.pos.y += velocity * dt; 
                }

                //update nebulae x poisition based on nebula's velocity
                for(int i = 0; i < sizeOfNebulae; i++)
                {
                    if (nebulae[i].pos.x <= -nebulae[i].rec.width)
                    {
                        nebulae[i].pos.x = GetRandomValue(windowWidth, windowWidth + 500);
                        nebulae[i].vel = GetRandomValue(200,300);
                        nebulae[i].scored = false;
                    }
                    else
                    {
                        nebulae[i].pos.x -= nebulae[i].vel * dt;
                    }
                } 

                //update nebulae animation
                for(int i = 0; i < sizeOfNebulae; i++)
                {
                    nebulae[i] = UpdateNebAnimData(nebulae[i], dt);
                }

                //update scarfy animation and freeze animatino if jumping
                if(!isInAir)
                {
                    scarfyData = UpdateAnimData(scarfyData, dt, scarfyData.backwards);
                }

                // Check for Collision  between each nebula and Scarfy
                for (int i = 0; i < sizeOfNebulae; i++)
                {
                    float pad{50};

                    Rectangle nebRec
                    {
                        nebulae[i].pos.x + pad,
                        nebulae[i].pos.y + pad,
                        nebulae[i].rec.width - 2 * pad,
                        nebulae[i].rec.height - 2 * pad
                    };

                    Rectangle scarfyRec
                    {
                        scarfyData.pos.x,
                        scarfyData.pos.y,
                        scarfyData.rec.width,
                        scarfyData.rec.height
                        
                    };

                    
                    if(CheckCollisionRecs(nebRec, scarfyRec))
                    {
                        for(int i = 0; i < sizeOfNebulae; i++)
                        {
                            nebulae[i].scored = true;
                        }
                        currentScreen = ENDING;
                        break;
                    }
                                        
                }
                // Check if Scarfy has passed a nebula and landing safely on the ground
                // Award points based on how many nebulae he has passed
                for(int i = 0; i < sizeOfNebulae; i++)
                {
                    if(nebulae[i].scored == false && scarfyData.pos.x > nebulae[i].pos.x && isOnGround(scarfyData, windowHeight))
                    {
                        scoreCount++;
                        if (UpdateScore(scoreCount) > 0)
                        {
                            bonus = UpdateScore(scoreCount);
                            score += bonus;
                            bonusFrameCounter = 30; 
                        }
                        else
                        {
                            score += 100;
                        }

                        nebulae[i].scored = true;
                    }
                }

                // Check if there is a new high score
                if(score > currentHighScore && !newHighscore)
                {
                    currentHighScore = score;
                    highscoreString = "Highscore: " + to_string(currentHighScore);
                    newHighscore = true;
                    highscoreCounter = 180;
                }
                else if(newHighscore)
                {
                    currentHighScore = score;
                    highscoreString = "Highscore: " + to_string(currentHighScore);

                }

                bonusString = "+ " + to_string(bonus);

                scoreString = "Score: " + to_string(score);
            }

        } break;
        case ENDING:
        {
            if(IsKeyPressed(KEY_M) || volChange)
            {
                if(!volChange)
                {
                    mute = !mute;
                    muteFrameCounter = 30;
                }

                if(mute)
                {
                    SetMusicVolume(music, 0.0);
                }
                else
                {
                    SetMusicVolume(music, volume);
                }

                if(volChange && mute)
                {
                    SetMusicVolume(music, volume);
                    mute = false;
                }

                volChange = false;
            }

            if(IsKeyPressed(KEY_EQUAL))
            {
                raiseVolFrameCounter = 30;
                if(volume < 1.0)
                {
                    volume += 0.1;
                    SetMusicVolume(music, volume);
                    volChange = true;
                }
            }                
            
            if(IsKeyPressed(KEY_MINUS))
            {
                lowerVolFrameCounter = 30;
                if(!(volume - 0.1 < 0.0))
                {
                    volume -= 0.1;
                    SetMusicVolume(music, volume);
                    volChange = true;
                }
            }

            // Go to TITLE Screen and reset all game items that need to be reset
            if(IsKeyPressed(KEY_ENTER))
            {
                currentScreen = TITLE;

                scarfyData.pos = {(float)windowWidth / 2.0f - (float)scarfy.width / 6.0f/ 2.0f, (float)windowHeight - (float)scarfy.height}; 
        

                for(int i = 0; i < sizeOfNebulae; i++)
                {
                    nebulae[i].rec.x = 0;
                    nebulae[i].rec.y = 0;
                    nebulae[i].pos.x = windowWidth + i * GetRandomValue(150, 800);
                    nebulae[i].scored = false;
                    nebulae[i].vel = GetRandomValue(200, 300);
                    
                }

                
                velocity = 0;
                score = 0;
                
                scoreCount = 0;

                bonusFrameCounter = 0;

                bonus = 0;

                moveUp = -5;

                highscoreCounter = 0;

                newHighscore = false;
                jumpBuffer = false;
            }

        } break;
        default:  break;         
    }
    
    //Drawing everything to screen based on screen type
    switch(currentScreen)
    {
        case TITLE:
        {
            //Draw Background
            DrawBackground(backgrounds, background, dt, sizeOfBackgrounds, color);

            //Draw Midground
            DrawBackground(midgrounds, midground, dt, sizeOfBackgrounds, color);

            //Draw foreground
            DrawBackground(foregrounds, foreground, dt, sizeOfBackgrounds, color);

            // Title Text
            DrawText("Dapper Dasher", 20, 20, 60, LIGHTGRAY);
            DrawText("Press ENTER to play Dapper Dasher", windowWidth / 2 - MeasureText("Press ENTER to play Dapper Dasher", 36) / 2, windowHeight / 2, 36, WHITE);
            DrawText("(Press R to reset highscore or Press I to see Info)", windowWidth / 2 - MeasureText("(Press R to reset highscore or Press I to see Info)", 24) / 2, windowHeight / 2 + 60, 24, WHITE);

            // Footer control display
            DrawText("M = Mute Music \t - = Lower Volume \t + = Raise Volume", windowWidth / 2 - MeasureText("M = Mute Music \t - = Lower Volume \t + = Raise Volume", 12) / 2, windowHeight - 12, 12, WHITE);
            DrawText(highscoreString.c_str(), windowWidth - MeasureText(highscoreString.c_str(), 24) - 24, 24, 24, WHITE);

            
            // Music control updates
            if(muteFrameCounter > 0)
            {
                muteFrameCounter--;

                if(mute)
                {
                    DrawText("Muted Music", windowWidth - MeasureText("Muted Music", 12) - 10, windowHeight - 24, 12, WHITE);
                }
                else
                {
                    DrawText("Unmuted Music", windowWidth - MeasureText("Unmuted Music", 12) - 10, windowHeight - 24, 12, WHITE);
                }
            }   
                        
            if(lowerVolFrameCounter > 0 &&  lowerVolFrameCounter > raiseVolFrameCounter)
            {
                lowerVolFrameCounter--;
                if(raiseVolFrameCounter > 0)
                {
                    raiseVolFrameCounter = 0;
                }
                string loweredString = "Volume Lowered: Volume = " + to_string((int)(volume * 100));
                DrawText(loweredString.c_str(), windowWidth - MeasureText(loweredString.c_str(), 12) - 10, windowHeight - 12, 12, WHITE);
            }     
            else if(raiseVolFrameCounter > 0)
            {
                raiseVolFrameCounter--;
                if(lowerVolFrameCounter > 0)
                {
                    lowerVolFrameCounter = 0;
                }    
                string raisedString = "Volume Raised: Volume = " + to_string((int)(volume * 100));
                DrawText(raisedString.c_str(), windowWidth - MeasureText(raisedString.c_str(), 12) - 10, windowHeight - 12, 12, WHITE);            
            }


        } break;
        case INFO:
        {   
            string pageText = "Page " + to_string(pageNum);

            //Draw Background
            DrawBackground(backgrounds, background, dt, sizeOfBackgrounds, color);

            //Draw Midground
            DrawBackground(midgrounds, midground, dt, sizeOfBackgrounds, color);

            //Draw foreground
            DrawBackground(foregrounds, foreground, dt, sizeOfBackgrounds, color);

            // Draw Info Border
            DrawTextureEx(infoBorder, {10.0f, -160.0f }, 0.0f, 0.39f, WHITE);

            // Check which info page to draw
            switch(pageNum)
            {
                // Scarfy info page
                case 1:
                {             
                    DrawTextureRec(scarfy, scarfyInfoData.rec, scarfyInfoData.pos, WHITE);

                    DrawText("Player", windowWidth / 2 - MeasureText("Player", 48) / 2, windowHeight / 2 - 145, 48, WHITE);

                    DrawText("This is Scarfy. Help him avoid Nebulae!", 
                    windowWidth / 2 - MeasureText("This is Scarfy! Help him avoid Nebulae", 24) / 2, windowHeight / 2 + scarfyData.rec.height / 2 + 12, 24, WHITE);

                    DrawText("A = Left \t D = Right \t SPACE = Jump",
                    windowWidth / 2 - MeasureText("A = Left \t D = Right \t SPACE = Jump", 24) / 2, windowHeight / 2 + scarfyData.rec.height / 2 + 48, 24, WHITE);

                    DrawText("ENTER = Title \t -> = page up",
                    windowWidth / 2 - MeasureText("ENTER = Title \t -> = page up", 24) / 2, windowHeight / 2 + scarfyData.rec.height / 2 + 84, 24, WHITE);

                    DrawText(pageText.c_str(), windowWidth / 2 - MeasureText(pageText.c_str(), 12) / 2, windowHeight / 2 + scarfyData.rec.height / 2 + 120, 12, WHITE);

                }break;
                
                // Neula info page
                case 2:
                {                        
                    DrawTextureRec(nebula, nebulae[0].rec, {windowWidth / 2 - nebulae[0].rec.width / 2, windowHeight / 2 - nebulae[0].rec.height / 2}, WHITE);

                    DrawText("Enemies", windowWidth / 2 - MeasureText("Enemies", 48) / 2, windowHeight / 2 - 145, 48, WHITE);

                    DrawText("This is a Nebula. Avoid these at all costs!", 
                    windowWidth / 2 - MeasureText("This is a Nebula. Avoid these at all costs!", 24) / 2, windowHeight / 2 + nebulae[0].rec.height / 2 + 12, 24, WHITE);
                    
                    DrawText("Hint: You can move in the air.", 
                    windowWidth / 2 - MeasureText("Hint: You can move in the air.", 24) / 2, windowHeight / 2 + nebulae[0].rec.height / 2 + 48, 24, WHITE);

                    DrawText("ENTER = Title \t <- = page down \t -> = page up",
                    windowWidth / 2 - MeasureText("ENTER = Title \t <- = page down \t -> = page up", 24) / 2, windowHeight / 2 + nebulae[0].rec.height / 2 + 84, 24, WHITE);


                    DrawText(pageText.c_str(), windowWidth / 2 - MeasureText(pageText.c_str(), 12) / 2, windowHeight / 2 + scarfyData.rec.height / 2 + 120, 12, WHITE);
                }break;

                // Bonus info page
                case 3:
                {
                    DrawText("Bonuses", windowWidth / 2 - MeasureText("Bonuses", 48) / 2, windowHeight / 2 - 145, 48, WHITE);

                    DrawText("Passing a Nebula = +100 points", windowWidth / 2 - MeasureText("Passing a Nebula = +100 points", 24) / 2, windowHeight / 2 - 48, 24, WHITE);
                    DrawText("Passing 5 Nebulae = +200 points", windowWidth / 2 - MeasureText("Passing 5 Nebulae = +200 points", 24) / 2, windowHeight / 2, 24, WHITE);
                    DrawText("Passing 10 Nebula = +500 points", windowWidth / 2 - MeasureText("Passing 10 Nebula = +500 points", 24) / 2, windowHeight / 2 + 48, 24, WHITE);
                    DrawText("Passing 25 Nebula = +1000 points", windowWidth / 2 - MeasureText("Passing 25 Nebula = +1000 points", 24) / 2, windowHeight / 2 + 96, 24, WHITE);

                    DrawText("ENTER = Title \t <- = page down \t -> = page up",
                    windowWidth / 2 - MeasureText("ENTER = Title \t <- = page down \t -> = page up", 24) / 2, windowHeight / 2 + 144, 24, WHITE);

                    DrawText(pageText.c_str(), windowWidth / 2 - MeasureText(pageText.c_str(), 12) / 2, windowHeight / 2 + scarfyData.rec.height / 2 + 120, 12, WHITE);

                }break;

                // Credits page
                case 4:
                {
                    DrawText("Credits", windowWidth / 2 - MeasureText("Credits", 48) / 2, windowHeight / 2 - 145, 48, WHITE);

                    DrawText("Music", windowWidth / 2 - MeasureText("Music", 24) / 2, windowHeight / 2 - 72, 24, WHITE);
                    DrawText("_____", windowWidth / 2 - MeasureText("_____", 24) / 2, windowHeight / 2 - 68, 24, WHITE);
                    DrawText("Joshua McLean: mrjoshuamclean.com", windowWidth / 2 - MeasureText("Joshua McLean: mrjoshuamclean.com", 24) / 2, windowHeight / 2 - 48, 24, WHITE);

                    DrawText("Info Background", windowWidth / 2 - MeasureText("nfo Background", 24) / 2, windowHeight / 2, 24, WHITE);
                    DrawText("_______________", windowWidth / 2 - MeasureText("_______________", 24) / 2, windowHeight / 2 + 4, 24, WHITE);
                    DrawText("PingTree: pngtree.com/588ku_12402278", windowWidth / 2 - MeasureText("PingTree: pngtree.com/588ku_12402278", 24) / 2, windowHeight / 2 + 24, 24, WHITE); 

                    DrawText("Textures and Engine", windowWidth / 2 - MeasureText("Textures and Engine", 24) / 2, windowHeight / 2 + 72, 24, WHITE);
                    DrawText("___________________", windowWidth / 2 - MeasureText("___________________", 24) / 2, windowHeight / 2 + 76, 24, WHITE);
                    DrawText("Raylib: raylib.com", windowWidth / 2 - MeasureText("Raylib: raylib.com", 24) / 2, windowHeight / 2 + 96, 24, WHITE);

                    DrawText("ENTER = Title \t <- = page down",
                    windowWidth / 2 - MeasureText("ENTER = Title \t <- = page down", 24) / 2, windowHeight / 2 + 144, 24, WHITE);

                    DrawText(pageText.c_str(), windowWidth / 2 - MeasureText(pageText.c_str(), 12) / 2, windowHeight / 2 + scarfyData.rec.height / 2 + 120, 12, WHITE);
                }break;
                default: break;

            }

            // Footer control display
            DrawText("M = Mute Music \t - = Lower Volume \t + = Raise Volume", windowWidth / 2 - MeasureText("M = Mute Music \t - = Lower Volume \t + = Raise Volume", 12) / 2, windowHeight - 12, 12, WHITE);

            // Music control updates
            if(muteFrameCounter > 0)
            {
                muteFrameCounter--;

                if(mute)
                {
                    DrawText("Muted Music", windowWidth - MeasureText("Muted Music", 12) - 10, windowHeight - 24, 12, WHITE);
                }
                else
                {
                    DrawText("Unmuted Music", windowWidth - MeasureText("Unmuted Music", 12) - 10, windowHeight - 24, 12, WHITE);
                }
            }   
                        
            if(lowerVolFrameCounter > 0 &&  lowerVolFrameCounter > raiseVolFrameCounter)
            {
                lowerVolFrameCounter--;
                if(raiseVolFrameCounter > 0)
                {
                    raiseVolFrameCounter = 0;
                }
                string loweredString = "Volume Lowered: Volume = " + to_string((int)(volume * 100));
                DrawText(loweredString.c_str(), windowWidth - MeasureText(loweredString.c_str(), 12) - 10, windowHeight - 12, 12, WHITE);
            }     
            else if(raiseVolFrameCounter > 0)
            {
                raiseVolFrameCounter--;
                if(lowerVolFrameCounter > 0)
                {
                    lowerVolFrameCounter = 0;
                }    
                string raisedString = "Volume Raised: Volume = " + to_string((int)(volume * 100));
                DrawText(raisedString.c_str(), windowWidth - MeasureText(raisedString.c_str(), 12) - 10, windowHeight - 12, 12, WHITE);            
            }

        } break;
        case GAMEPLAY:
        {
            
                //Draw Background
                DrawBackground(backgrounds, background, dt, sizeOfBackgrounds, pause, color);

                //Draw Midground
                DrawBackground(midgrounds, midground, dt, sizeOfBackgrounds, pause, color);

                //Draw foreground
                DrawBackground(foregrounds, foreground, dt, sizeOfBackgrounds, pause, color);

                for(int i = 0; i < sizeOfNebulae; i++)
                {
                    //draw nebula
                    DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, color);
                }

                //draw scarfy
                DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, color);

                // Draw current score
                DrawText(scoreString.c_str(), 24, 24, 24, color);

                // Draw high score
                DrawText(highscoreString.c_str(), windowWidth - MeasureText(highscoreString.c_str(), 24) - 24, 24, 24, color);
                
                // Header control display
                DrawText("P = Pause \t M = Mute Music \t - = Lower Volume \t + = Raise Volume", windowWidth / 2 - MeasureText("P = Pause \t M = Mute Music \t - = Lower Volume \t + = Raise Volume", 12) / 2, 12, 12, color);

                // Draw bonus aboe Scarfy 
                if(bonusFrameCounter > 0)
                {
                    bonusFrameCounter--;
                    moveUp--;
                    DrawText(bonusString.c_str(), scarfyData.pos.x, scarfyData.pos.y + moveUp, 24, color);
                }

                if(bonusFrameCounter <= 0)
                {
                    moveUp = -5;
                }

                // Display Pause over grayed out game
                if(pause)
                {
                    pauseFrameCounter++;

                    if (pauseFrameCounter == 15)
                    {
                        pauseFrameCounter = 0;
                        pauseColorSwitch = !pauseColorSwitch;

                    }

                        if(pauseColorSwitch)
                        {
                            DrawText("Game Paused", windowWidth / 2 - MeasureText("Game Paused", 48) / 2, windowHeight / 2 - 48, 48, WHITE);
                        }
                        else
                        {
                            DrawText("Game Paused", windowWidth / 2 - MeasureText("Game Paused", 48) / 2, windowHeight / 2 - 48, 48, LIGHTGRAY);
                        }
                }
                    

                
                // Check for new high score and draw if there is a new one
                if(highscoreCounter > 0)
                {
                    if(colorCount % 10 == 0)
                    {
                        changeColor = !changeColor;
                    }
                    
                    if(!changeColor)
                    {
                        DrawText("NEW HIGHSCORE!!!", windowWidth / 2 - MeasureText("NEW HIGHSCORE!!!", 50) / 2, windowHeight / 2, 50, PURPLE);
                    }
                    else
                    {
                        DrawText("NEW HIGHSCORE!!!", windowWidth / 2 - MeasureText("NEW HIGHSCORE!!!", 50) / 2, windowHeight / 2, 50, SKYBLUE);
                    }

                    colorCount++;
                    highscoreCounter--; 
                }

            // Music control updates
            if(muteFrameCounter > 0)
            {
                muteFrameCounter--;

                if(mute)
                {
                    DrawText("Muted Music", windowWidth - MeasureText("Muted Music", 12) - 10, windowHeight - 24, 12, WHITE);
                }
                else
                {
                    DrawText("Unmuted Music", windowWidth - MeasureText("Unmuted Music", 12) - 10, windowHeight - 24, 12, WHITE);
                }
            }   
                        
            if(lowerVolFrameCounter > 0 &&  lowerVolFrameCounter > raiseVolFrameCounter)
            {
                lowerVolFrameCounter--;
                if(raiseVolFrameCounter > 0)
                {
                    raiseVolFrameCounter = 0;
                }
                string loweredString = "Volume Lowered: Volume = " + to_string((int)(volume * 100));
                DrawText(loweredString.c_str(), windowWidth - MeasureText(loweredString.c_str(), 12) - 10, windowHeight - 12, 12, WHITE);
            }     
            else if(raiseVolFrameCounter > 0)
            {
                raiseVolFrameCounter--;
                if(lowerVolFrameCounter > 0)
                {
                    lowerVolFrameCounter = 0;
                }    
                string raisedString = "Volume Raised: Volume = " + to_string((int)(volume * 100));
                DrawText(raisedString.c_str(), windowWidth - MeasureText(raisedString.c_str(), 12) - 10, windowHeight - 12, 12, WHITE);            
            }

        } break;
        case ENDING:
        {   
            //Draw Background
            DrawBackground(backgrounds, background, dt, sizeOfBackgrounds, color);

            //Draw Midground
            DrawBackground(midgrounds, midground, dt, sizeOfBackgrounds, color);

            
            //Draw foreground
            DrawBackground(foregrounds, foreground, dt, sizeOfBackgrounds, color);

            // Draw ENDING Text
            DrawText("Game Over!", windowWidth / 2 - MeasureText("Game Over!", 24) / 2, windowHeight / 2, 24, WHITE);
            DrawText(scoreString.c_str(), windowWidth / 2 - MeasureText(scoreString.c_str(), 24) / 2, windowHeight /2 + 24, 24, WHITE);
            DrawText(endingString.c_str(), windowWidth / 2 - MeasureText(endingString.c_str(), 24) / 2, windowHeight /2 + 48, 24, WHITE);
            
            // Header control display
            DrawText("M = Mute Music \t - = Lower Volume \t + = Raise Volume", windowWidth / 2 - MeasureText("M = Mute Music \t - = Lower Volume \t + = Raise Volume", 12) / 2, 12, 12, WHITE);

            //music control updates 
            if(muteFrameCounter > 0)
            {
                muteFrameCounter--;

                if(mute)
                {
                    DrawText("Muted Music", windowWidth - MeasureText("Muted Music", 12) - 10, windowHeight - 24, 12, WHITE);
                }
                else
                {
                    DrawText("Unmuted Music", windowWidth - MeasureText("Unmuted Music", 12) - 10, windowHeight - 24, 12, WHITE);
                }
            }   
                        
            if(lowerVolFrameCounter > 0 &&  lowerVolFrameCounter > raiseVolFrameCounter)
            {
                lowerVolFrameCounter--;
                if(raiseVolFrameCounter > 0)
                {
                    raiseVolFrameCounter = 0;
                }
                string loweredString = "Volume Lowered: Volume = " + to_string((int)(volume * 100));
                DrawText(loweredString.c_str(), windowWidth - MeasureText(loweredString.c_str(), 12) - 10, windowHeight - 12, 12, WHITE);
            }     
            else if(raiseVolFrameCounter > 0)
            {
                raiseVolFrameCounter--;
                if(lowerVolFrameCounter > 0)
                {
                    lowerVolFrameCounter = 0;
                }    
                string raisedString = "Volume Raised: Volume = " + to_string((int)(volume * 100));
                DrawText(raisedString.c_str(), windowWidth - MeasureText(raisedString.c_str(), 12) - 10, windowHeight - 12, 12, WHITE);            
            }

        } break;
        default:  break;

    }

    EndDrawing();        
    }
    
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();
};

