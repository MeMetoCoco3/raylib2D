/*
    camelCase = vars;
    CapitalCamelCase = structs && functions;
    snake_case = const;
    DEFINITIONS_LIKE = definitions;
    
    p->speed >= p->speed * deltaTime; 
*/

// LIBRARIES
# include <stdio.h>
# include <stdlib.h>
# include "C:\Users\torre\OneDrive\Desktop\raylib\raylib.h"
# include "C:\Users\torre\OneDrive\Desktop\raylib\raymath.h"
# include "constants.h"
# include "structs.h"


Player PlayerInstance(int kOrP);
Sound* SoundInstance(void);
Camera2D* CameraInstance(Player *player);
void PlayerUpdate (Player *p, float deltaTime, Sound *soundArr);
void CheckColision(Player *player, EnvItem *envItems, int envItemsLength, float deltaTime);
void ChangeSprite(Player *player, int *frameCounter, int frameSpeed, int *currentFrame);
void CameraUpdate (Player *player, Camera2D *camera, int *steps, int *direction, float deltaTime);

int main (void){
    
    // SET SCREEN AND AUDIO
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Retry");
    
    
    // PLAYER SOUNDS
    Sound *soundArr = SoundInstance(); 
    
    // PLAYER INSTANCE
    Player pig = PlayerInstance(PIG);
    Player player = PlayerInstance(KATUSHA);
    
    int characterChange = 0;
    
    // CAMERA INSTANCE
    Camera2D *camera = CameraInstance(&pig);
    /*
    Camera2D camera = { 0 };
    camera.target = (Vector2){ player.position.x, player.position.y};
    camera.offset = (Vector2){ SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 2;
    */
    // CAMERA PROPERTIES
    int steps = 0;
    int direction = 0;
    int frameCounter = 0;
    int currentFrame = 0;
    int frameSpeed = 8;
    
    // ENVIRONMENT 
    EnvItem envItems [] = {
        {{0, FLOOR+50, SCREEN_WIDTH*20, 50}, 1, GREEN},
        {{0, FLOOR+50, SCREEN_WIDTH*20, 50}, 1, GREEN},
        {{200, 400 , 32, 32}, 2, RED},
        {{150, 400 , 32, 32}, 2, RED},
        {{100, 350 , 32, 32}, 2, RED},
        {{50, 300 , 32, 32}, 2, RED},
        {{150, 350 , 32, 32}, 1, YELLOW}, 
        {{300, 350 , 20, 110}, 2, RED},
        {{0, 350 , 20, 110}, 2, RED}
    };
    int envItemsLength = sizeof(envItems)/sizeof(envItems[0]);
    
    // FPS
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        
        // CALL FUNCTIONS    
        if (IsKeyReleased(KEY_K)) characterChange++;
        if (characterChange%2 == 0){
            PlayerUpdate(&player, deltaTime, soundArr);   
            CameraUpdate (&player, camera, &steps, &direction, deltaTime);}       
        else{
            PlayerUpdate(&pig, deltaTime, soundArr);   
            CameraUpdate (&pig, camera, &steps, &direction, deltaTime);}   
        
        CheckColision(&player, envItems, envItemsLength, deltaTime);
        CheckColision(&pig, envItems, envItemsLength, deltaTime);
        
        ChangeSprite(&player, &frameCounter, frameSpeed, &currentFrame);
        ChangeSprite(&pig, &frameCounter, frameSpeed, &currentFrame);
        
        // DRAW
        BeginDrawing();  
            BeginMode2D(*camera);
                ClearBackground(RAYWHITE);
                for (int i = 1; i < envItemsLength; i++) DrawRectangleRec(envItems[i].rect, envItems[i].color);
                
                DrawTextureRec(player.sprite, player.sourceRec, (Vector2){player.position.x-player.size.x/2-4, player.position.y-player.size.y/2}, WHITE);
                DrawTextureRec(pig.sprite, pig.sourceRec, (Vector2){pig.position.x-pig.size.x/2-4, pig.position.y-pig.size.y/2}, WHITE);
                
                DrawRectangleLines(player.collisionBox.x, player.collisionBox.y,player.collisionBox.width,player.collisionBox.height, RED);
                DrawRectangleLines(pig.collisionBox.x, pig.collisionBox.y,pig.collisionBox.width,pig.collisionBox.height, RED);
            EndMode2D();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

Player PlayerInstance(int kOrP){  
    /*
        Katusha = 1;
        Pig = 0
    
    */  
    switch (kOrP){ 
        case KATUSHA:
            Texture2D* playerSpritesK = malloc(sizeof(Texture2D)*3);
            playerSpritesK[0] = LoadTexture("resources/kingpig/idleK.png");
            playerSpritesK[1] = LoadTexture("resources/kingpig/runK.png");
            playerSpritesK[2] = LoadTexture("resources/kingpig/jump.png");
            playerSpritesK[0].width = -abs(playerSpritesK[0].width);
            playerSpritesK[1].width = -abs(playerSpritesK[1].width);
            playerSpritesK[2].width = -abs(playerSpritesK[2].width);
            
            Rectangle* sourceRecsK = malloc(sizeof(Rectangle)*3);
            sourceRecsK[0] = (Rectangle) {0.0f, 0.0f, (float) playerSpritesK[0].width/NUM_FRAMES(playerSpritesK[0].width), playerSpritesK[0].height};
            sourceRecsK[1] = (Rectangle) {0.0f, 0.0f, (float) playerSpritesK[1].width/NUM_FRAMES(playerSpritesK[1].width), playerSpritesK[1].height};
            sourceRecsK[2] = (Rectangle) {0.0f, 0.0f, (float) playerSpritesK[2].width/NUM_FRAMES(playerSpritesK[2].width), playerSpritesK[2].height};

            Player player = { 0 };
            player.position = (Vector2) {SCREEN_WIDTH/4, SCREEN_HEIGHT-120};
            player.collisionBox = (Rectangle) {player.position.x-COLLISION_SIDE/2, player.position.y-COLLISION_SIDE/2, COLLISION_SIDE, COLLISION_SIDE};
            player.size = (Vector2) {32, 32}; 
            player.speed = 0;
            player.acceleration = P_ACCELERATION;
            player.maxSpeed = P_MAX_SPEED;
            player.jumpSpeed = 0;
            player.canJump = true;
            player.canDash = 0;
            player.dashing = 0;
            player.direction = RIGHT;
            player.sprite = playerSpritesK[0];
            player.baseSpr = playerSpritesK;
            player.sourceRec = sourceRecsK[0];
            player.baseSourceRec = sourceRecsK;
            player.currentState = IDLE;
            player.prevState = player.currentState;
            player.character = Katusha;
            player.prevDirection = player.direction;
            
            return player;
        
        case PIG:
            Texture2D* playerSpritesP = malloc(sizeof(Texture2D)*3);
            playerSpritesP[0] = LoadTexture("resources/kingpig/idleP.png");
            playerSpritesP[1] = LoadTexture("resources/kingpig/runP.png");
            playerSpritesP[2] = LoadTexture("resources/kingpig/jump.png");
            playerSpritesP[0].width = -abs(playerSpritesP[0].width);
            playerSpritesP[1].width = -abs(playerSpritesP[1].width);
            playerSpritesP[2].width = -abs(playerSpritesP[2].width);
            
            Rectangle* sourceRecsP = malloc(sizeof(Rectangle)*3);
            sourceRecsP[0] = (Rectangle) {0.0f, 0.0f, (float) playerSpritesP[0].width/NUM_FRAMES(playerSpritesP[0].width), playerSpritesP[0].height};
            sourceRecsP[1] = (Rectangle) {0.0f, 0.0f, (float) playerSpritesP[1].width/NUM_FRAMES(playerSpritesP[1].width), playerSpritesP[1].height};
            sourceRecsP[2] = (Rectangle) {0.0f, 0.0f, (float) playerSpritesP[2].width/NUM_FRAMES(playerSpritesP[2].width), playerSpritesP[2].height};

            Player pig = { 0 };
            pig.position = (Vector2) {SCREEN_WIDTH/4, SCREEN_HEIGHT-200};
            pig.collisionBox = (Rectangle) {pig.position.x-COLLISION_SIDE/2, pig.position.y-COLLISION_SIDE/2, COLLISION_SIDE, COLLISION_SIDE};
            pig.size = (Vector2) {32, 32}; 
            pig.speed = 0;
            pig.acceleration = P_ACCELERATION;
            pig.maxSpeed = P_MAX_SPEED;
            pig.jumpSpeed = 0;
            pig.canJump = true;
            pig.canDash = 0;
            pig.dashing = 0;
            pig.direction = RIGHT;
            pig.sprite = playerSpritesP[0];
            pig.baseSpr = playerSpritesP;
            pig.sourceRec = sourceRecsP[0];
            pig.baseSourceRec = sourceRecsP;
            pig.currentState = IDLE;
            pig.prevState = pig.currentState;
            pig.character = Pig;
            pig.prevDirection = pig.direction;
            
            return pig;
    }
}
Sound* SoundInstance(void){
    InitAudioDevice();
    
    Sound* soundArr = malloc(sizeof(Sound)*2);
    soundArr[0] = LoadSound("resources/audio/pig_grunt.mp3"); 
    soundArr[1] = LoadSound("resources/audio/pig_dash.mp3");
    return soundArr;
}
Camera2D* CameraInstance(Player *player){
    Camera2D* camarita = malloc(sizeof(Camera2D));
    camarita->target = (Vector2){ player->position.x, player->position.y};
    camarita->offset = (Vector2){ SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f};
    camarita->rotation = 0.0f;
    camarita->zoom = 2;
    
    return camarita;
}
void CameraUpdate(Player *player, Camera2D *camera, int *steps, int *direction, float deltaTime){
    Vector2* pCam = &(camera->target);
    
    if (player->speed>0) {
        (*direction) = 1;
        player->direction = RIGHT;
        (*steps)++;
    }else if (player->speed<0){
        (*direction) = -1;
        player->direction = LEFT;
        (*steps)++;
        }
        
    if (player->direction != player->prevDirection) {
        (*steps) = 0;
        player->prevDirection = player->direction;
        }
        
        Vector2 diff = Vector2Subtract((Vector2){player->position.x+CAM_DISTANCE*(*direction),player->position.y}, *pCam);
        float length = Vector2Length(diff);

    if (length >= MIN_EFFECT_LENGTH && (*steps) >= MAX_STEPS) {
        float speed = fmaxf(FRACTION_SPEED*length, MIN_SPEED);
        *pCam = Vector2Add(*pCam, Vector2Scale(diff, speed*deltaTime/length));
        }
        
        pCam->y = player->position.y;
    
    
}

void PlayerUpdate (Player *p, float deltaTime, Sound *soundArr){
    if (IsKeyDown(KEY_LEFT) && p->speed > -p->maxSpeed) 
    {   
        p->direction = LEFT;
        p->speed -= p->acceleration; 
        
    }
    else if (IsKeyDown(KEY_RIGHT) && p->speed < p->maxSpeed) 
    {   
        p->speed += p->acceleration; 
        p->direction = RIGHT;
    } 
    else if (p->speed > 0)
    {
        p->direction = RIGHT;
        p->speed -= p->acceleration;
        p->currentState = RUN;
    }
    else if (p->speed < 0)
    {
        p->direction = LEFT;
        p->speed += p->acceleration;
        p->currentState = RUN;
    }
    else if (p->speed == 0)
    {
        p->currentState = IDLE;
        
    }
    
    if (IsKeyDown(KEY_SPACE) && p->canJump)
    {   
        p->jumpSpeed = -P_JUMP_SPEED;
        p->canJump = false;
        PlaySound(soundArr[0]);
    }
    
    p->canDash--;
    if (IsKeyDown(KEY_LEFT_SHIFT) && p->canDash<=0){
        PlaySound(soundArr[1]);
        p->dashing = DASHING;
        p->canDash = DASH_RECOVERY;
    } 
    
    if (p->dashing>0 && p->direction==RIGHT){
        p->dashing--;
        p->position.x += DASH_SPEED;
    } else if (p->dashing>0 && p->direction==LEFT){
        p->dashing--;
        p->position.x -= DASH_SPEED;
    }
    p->position.x += p->speed*deltaTime;
    
}
void CheckColision(Player *player, EnvItem *envItems, int envItemsLength, float deltaTime){
    
    Vector2 *p = &(player->position);
    int grounded = 0;
    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem *ei = envItems + i;
        if (ei->blocking == 1 &&
            ei->rect.x <= p->x  &&
            ei->rect.x + ei->rect.width >= p->x &&
            ei->rect.y >= p->y &&
            ei->rect.y < p->y + player->jumpSpeed*deltaTime)
        {
            grounded = 1;
            player -> jumpSpeed = 0.0f;
            p->y = ei->rect.y;
        }
        
        Rectangle *collisionBox = &(player->collisionBox);
        //Checks DOWN
        if (ei->blocking == 2 && CheckCollisionPointRec((Vector2){collisionBox->x + collisionBox->width/2, collisionBox->y + collisionBox-> height + player->jumpSpeed*deltaTime}, ei->rect) &&
            (
            CheckCollisionPointRec((Vector2){collisionBox->x, collisionBox->y + collisionBox->height + player->jumpSpeed*deltaTime}, ei->rect) ||
            CheckCollisionPointRec((Vector2){collisionBox->x + collisionBox->width, collisionBox->y + collisionBox->height + player->jumpSpeed*deltaTime}, ei->rect))
            )
        {
            grounded = 1;
            player->jumpSpeed = 0.0f;

        }
        //Checks UP
        if (ei->blocking == 2 && CheckCollisionPointRec((Vector2){collisionBox->x + collisionBox->width/2, collisionBox->y + player->jumpSpeed*deltaTime}, ei->rect) &&
            (
            CheckCollisionPointRec((Vector2){collisionBox->x, collisionBox->y + player->jumpSpeed*deltaTime}, ei->rect) ||
            CheckCollisionPointRec((Vector2){collisionBox->x + collisionBox->width, collisionBox->y + player->jumpSpeed*deltaTime}, ei->rect))
            )
        {          
            player->jumpSpeed = 0.0f;   
            p->y = ei->rect.y + ei->rect.height+collisionBox->height/2;

        }
        
        //Checks RIGHT
        if (ei->blocking == 2 && CheckCollisionPointRec((Vector2){collisionBox->x + collisionBox->width + player->speed*deltaTime, collisionBox->y + collisionBox->height/2}, ei->rect) && 
            (
            CheckCollisionPointRec((Vector2){player->collisionBox.x + player->collisionBox.width + player->speed*deltaTime, player->collisionBox.y}, ei->rect) ||
            CheckCollisionPointRec((Vector2){player->collisionBox.x + player->collisionBox.width + player->speed*deltaTime, player->collisionBox.y + player->collisionBox.height}, ei->rect))
            )
        {          
            p->x = ei->rect.x - collisionBox->width/2;
            player->speed = 0;

        }
        //Checks LEFT
        if (ei->blocking == 2 && CheckCollisionPointRec((Vector2){collisionBox->x + player->speed*deltaTime, collisionBox->y + collisionBox->height/2}, ei->rect) && 
            (
            CheckCollisionPointRec((Vector2){player->collisionBox.x + player->speed*deltaTime, player->collisionBox.y}, ei->rect) ||
            CheckCollisionPointRec((Vector2){player->collisionBox.x + player->speed*deltaTime, player->collisionBox.y + player->collisionBox.height}, ei->rect))
            )
        {          
            p->x = ei->rect.x + ei->rect.width + collisionBox->width/2;
            player->speed = 0;

        }
     
    }
    if(!grounded)
    {   
        p->y += player->jumpSpeed*deltaTime; 
        player->jumpSpeed += G*deltaTime;
        
    } else player->canJump = true;
    
    if (grounded && player->speed != 0){
        player->currentState = RUN;
    } else if (grounded && player->speed == 0){
        player->currentState = IDLE;
    }
    
    player->collisionBox = (Rectangle) {player->position.x-COLLISION_SIDE/2, player->position.y-COLLISION_SIDE/2, COLLISION_SIDE, COLLISION_SIDE};
    
}
void ChangeSprite(Player *player, int *frameCounter, int frameSpeed, int *currentFrame){
    
    Rectangle *p = &(player->sourceRec); 
    switch (player->direction)
        {
            case LEFT:
                p->width = -abs(p->width);
                break;
                
            case RIGHT:
                p->width = abs(p->width);
                break;
        }
      
    if (player->currentState != player->prevState){   
        switch(player->currentState)
        {
            case IDLE:
                player->sprite = *(player->baseSpr);
                player->sourceRec = *(player->baseSourceRec);
                break;
            
            case RUN:
                player->sprite = *(player->baseSpr + 1);
                player->sourceRec = *(player->baseSourceRec + 1);
                break;
                
            case JUMP:
                player->sprite = *(player->baseSpr + 2);
                player->sourceRec = *(player->baseSourceRec + 2);
                break;
 
        }
        player->prevState = player->currentState;
    }
    
    (*frameCounter)++;
    if((*frameCounter) >= 60/frameSpeed)
    {
        (*frameCounter) = 0;
        (*currentFrame)++;
        if((*currentFrame)>=6) (*currentFrame) = 0;
        p->x = abs((*currentFrame)*p->width/NUM_FRAMES(p->width));     
    }
}



