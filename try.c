/*
    camelCase = vars;
    CapitalCamelCase = structs && functions;
    snake_case = const;
    DEFINITIONS_LIKE = definitions;
    
    p->speed >= p->speed * deltaTime; 
*/

# include "stdio.h"
# include "raylib.h"
# include "raymath.h"

# define P_MAX_SPEED  225.0f
# define P_ACCELERATION 20.0f
# define P_DEACELERATION 50.0f
# define P_JUMP_SPEED 500.0f  
# define G 2000
# define CAM_DISTANCE 40.0f
# define MAX_STEPS 15
# define DASH_SPEED 2

# define DASH_RECOVERY 60 
# define DASHING 15
# define COLLISION_SIDE 18
# define FLOOR 408

# define NUM_FRAMES(image_width) (image_width/38)



enum Direction{
    LEFT,
    RIGHT  
};

enum States{
    IDLE,
    RUN,
    JUMP
};

typedef struct {
    Vector2 position;
    Vector2 size;
    Rectangle collisionBox;
    float speed;
    float acceleration;
    float maxSpeed;
    float jumpSpeed;
    float fallSpeed;
    bool canJump;
    enum Direction direction;
    int canDash;
    int dashing;
    Texture2D sprite;
    Rectangle sourceRec;
    enum States currentState;
    enum States prevState;
} Player;

typedef struct {
    Rectangle rect;
    int blocking;
    Color color; 
    Texture2D texture;
} EnvItem ;

void PlayerUpdate (Player *p, EnvItem *envItems, int envItemsLength, float deltaTime, Sound *soundArr);
void SwapPlayerSprite (Player *player);
void ChangeSprite(Player *player, Texture2D *sprites, Rectangle *sourceRecs);

int main (void){
    
    const int screen_width = 512;
    const int screen_height = 512;

    Texture2D playerSprites[3];
    playerSprites[0] = LoadTexture("resources/kingpig/idle.png");
    playerSprites[1] = LoadTexture("resources/kingpig/run.png");
    playerSprites[2] = LoadTexture("resources/kingpig/jump.png");
    
    Rectangle sourceRecs[3];
    sourceRecs[0] = (Rectangle) {0.0f, 0.0f, (float) playerSprites[0].width/NUM_FRAMES(playerSprites[0].width), playerSprites[0].height};
    sourceRecs[1] = (Rectangle) {0.0f, 0.0f, (float) playerSprites[1].width/NUM_FRAMES(playerSprites[1].width), playerSprites[1].height};
    sourceRecs[2] = (Rectangle) {0.0f, 0.0f, (float) playerSprites[2].width/NUM_FRAMES(playerSprites[2].width), playerSprites[2].height};
    
    Sound soundArr[2];
    soundArr[0] = LoadSound("resources/audio/pig_grunt.mp3"); 
    soundArr[1] = LoadSound("resources/audio/pig_dash.mp3");
    
    //playerIdleAnim.width = -abs(playerIdleAnim.width);
           
    
    Player player = { 0 };
    player.position = (Vector2) {screen_width/4, screen_height-120};
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
    player.sprite = playerSprites[0];
    player.sourceRec = sourceRecs[0];
    player.currentState = IDLE;
    player.prevState = player.currentState;
    enum Direction prevDirection = player.direction;
    
    
    Camera2D camera = { 0 };
    camera.target = (Vector2){ player.position.x, player.position.y};
    camera.offset = (Vector2){ screen_width/2.0f, screen_height/2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 2;
    int steps = 0;
    int direction = 0;
    int frameCounter = 0;
    int currentFrame = 0;
    int frameSpeed = 8;
    
    EnvItem envItems [] = {
        {{0, FLOOR+50, screen_width*20, 50}, 1, GREEN},
        {{0, FLOOR+50, screen_width*20, 50}, 1, GREEN},
        {{200, 400 , 32, 32}, 2, RED},
        {{150, 400 , 32, 32}, 2, RED},
        {{100, 350 , 32, 32}, 2, RED},
        {{50, 300 , 32, 32}, 2, RED},
        {{150, 350 , 32, 32}, 1, YELLOW}, 
        {{300, 350 , 20, 110}, 2, RED},
        {{0, 350 , 20, 110}, 2, RED}
    };
    int envItemsLength = sizeof(envItems)/sizeof(envItems[0]);
    
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        
        float deltaTime = GetFrameTime();
        
        Vector2* p = &(player.position);
        Vector2* pCam = &(camera.target);
        player.collisionBox = (Rectangle) {p->x-COLLISION_SIDE/2, p->y-COLLISION_SIDE/2, COLLISION_SIDE, COLLISION_SIDE};
        
        static float minSpeed = 30;
        static float minEffectLength = 2;
        static float fractionSpeed = 4.0f;

        if (player.speed>0) {
            direction = 1;
            player.direction = RIGHT;
            steps++;
        }else if (player.speed<0){
            direction = -1;
            player.direction = LEFT;
            steps++;}
        
        if (player.direction != prevDirection)
        {
            steps = 0;
            prevDirection = player.direction;
        }
        
        Vector2 diff = Vector2Subtract((Vector2){p->x+CAM_DISTANCE*direction,p->y}, *pCam);
        float length = Vector2Length(diff);

        if (length >= minEffectLength && steps >= MAX_STEPS)
        {
            float speed = fmaxf(fractionSpeed*length, minSpeed);
            *pCam = Vector2Add(*pCam, Vector2Scale(diff, speed*deltaTime/length));
        }
        
        pCam->y = p->y;
        
        frameCounter++;
        if(frameCounter >= (60/frameSpeed))
        {
            frameCounter = 0;
            currentFrame++;
            
            if(currentFrame>3) currentFrame = 0;
            player.sourceRec.x = abs(currentFrame*player.sprite.width/NUM_FRAMES(player.sprite.width));
            
            if (player.speed < 0) {
                player.sourceRec.width = -abs(player.sourceRec.width);}
            else if (player.speed > 0) {
                player.sourceRec.width = abs(player.sourceRec.width);}
            
        }
        
        
        PlayerUpdate(&player, envItems, envItemsLength, deltaTime, soundArr);   
        ChangeSprite(&player, playerSprites, sourceRecs);
        SwapPlayerSprite(&player);
        
        BeginDrawing();
            
            BeginMode2D(camera);
                ClearBackground(RAYWHITE);
                
                for (int i = 1; i < envItemsLength; i++) DrawRectangleRec(envItems[i].rect, envItems[i].color); 

                DrawTextureRec(player.sprite, player.sourceRec, (Vector2){player.position.x-player.size.x/2-3, player.position.y-player.size.y/2}, WHITE);
                DrawRectangleLines(player.collisionBox.x, player.collisionBox.y,player.collisionBox.width,player.collisionBox.height, RED);
   

            EndMode2D();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}


void PlayerUpdate (Player *p, EnvItem *envItems, int envItemsLength, float deltaTime, Sound *soundArr){
    
    if (IsKeyDown(KEY_LEFT) && p->speed > -p->maxSpeed) 
    {   
        p->speed -= p->acceleration; 
    }
    else if (IsKeyDown(KEY_RIGHT) && p->speed < p->maxSpeed) 
    {   
        p->speed += p->acceleration; 
    } 
    else if (p->speed > 0)
    {
        p->speed -= p->acceleration;
    }
    else if (p->speed < 0)
    {
        p->speed += p->acceleration;
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
    
    Vector2 *player = &(p->position);
    
    int grounded = 0;
    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem *ei = envItems + i;
        
        if (ei->blocking == 1 &&
            ei->rect.x <= player->x  &&
            ei->rect.x + ei->rect.width >= player->x &&
            ei->rect.y >= player->y &&
            ei->rect.y < player->y + p->jumpSpeed*deltaTime)
        {
            grounded = 1;
            p -> jumpSpeed = 0.0f;
            player->y = ei->rect.y;
        }
        
        
        
        Rectangle *collisionBox = &(p->collisionBox);
        
        //Checks DOWN
        if (ei->blocking == 2 && CheckCollisionPointRec((Vector2){collisionBox->x + collisionBox->width/2, collisionBox->y + collisionBox-> height + p->jumpSpeed*deltaTime}, ei->rect) &&
            (
            CheckCollisionPointRec((Vector2){collisionBox->x, collisionBox->y + collisionBox->height + p->jumpSpeed*deltaTime}, ei->rect) ||
            CheckCollisionPointRec((Vector2){collisionBox->x + collisionBox->width, collisionBox->y + collisionBox->height + p->jumpSpeed*deltaTime}, ei->rect))
            )
        {
            grounded = 1;
            p -> jumpSpeed = 0.0f;
            //printf("%s", "CHECKS DOWN");
        }
        //Checks UP
        if (ei->blocking == 2 && CheckCollisionPointRec((Vector2){collisionBox->x + collisionBox->width/2, collisionBox->y + p->jumpSpeed*deltaTime}, ei->rect) &&
            (
            CheckCollisionPointRec((Vector2){collisionBox->x, collisionBox->y + p->jumpSpeed*deltaTime}, ei->rect) ||
            CheckCollisionPointRec((Vector2){collisionBox->x + collisionBox->width, collisionBox->y + p->jumpSpeed*deltaTime}, ei->rect))
            )
        {          
            p -> jumpSpeed = 0.0f;   
            p -> position.y = ei->rect.y + ei->rect.height+collisionBox->height/2;
            printf("%s", "CHECKS UP");
        }
        
        //Checks RIGHT
        if (ei->blocking == 2 && CheckCollisionPointRec((Vector2){collisionBox->x + collisionBox->width + p->speed*deltaTime, collisionBox->y + collisionBox->height/2}, ei->rect) && 
            (
            CheckCollisionPointRec((Vector2){p->collisionBox.x + p->collisionBox.width + p->speed*deltaTime, p->collisionBox.y}, ei->rect) ||
            CheckCollisionPointRec((Vector2){p->collisionBox.x + p->collisionBox.width + p->speed*deltaTime, p->collisionBox.y + p->collisionBox.height}, ei->rect))
            )
        {          
            p->position.x = ei->rect.x - collisionBox->width/2;
            p->speed = 0;
            printf("%s", "CHECKS RIGHT");
        }
        //Checks LEFT
        if (ei->blocking == 2 && CheckCollisionPointRec((Vector2){collisionBox->x + p->speed*deltaTime, collisionBox->y + collisionBox->height/2}, ei->rect) && 
            (
            CheckCollisionPointRec((Vector2){p->collisionBox.x + p->speed*deltaTime, p->collisionBox.y}, ei->rect) ||
            CheckCollisionPointRec((Vector2){p->collisionBox.x + p->speed*deltaTime, p->collisionBox.y + p->collisionBox.height}, ei->rect))
            )
        {          
            p->position.x = ei->rect.x + ei->rect.width + collisionBox->width/2;
            p->speed = 0;
            printf("%s", "CHECKS LEFT");
        }
        

        
     
    }
    if(!grounded)
    {   
        p->position.y += p->jumpSpeed*deltaTime; 
        p->jumpSpeed += G*deltaTime;
        
    } else 
    {   
        p->currentState = JUMP;
        p->canJump = true;
    }
    if (grounded && p->speed != 0){
        p->currentState = RUN;
    } else if (grounded && p->speed == 0){
        p->currentState = IDLE;
    }
    
}

void SwapPlayerSprite (Player *player){
    if (player->prevState != player->currentState){
        player->prevState = player->currentState;
    }  
}

void ChangeSprite(Player *player, Texture2D* sprites, Rectangle* sourceRecs){
    if (player->prevState != player->currentState){
        switch(player->currentState)
        {
            case IDLE:
                player->sprite = *sprites;
                player->sourceRec = *sourceRecs;
                break;
            
            case RUN:
                player->sprite = *(sprites + 1);
                player->sourceRec = *(sourceRecs + 1);
                break;
                
            case JUMP:
                player->sprite = *(sprites + 2);
                player->sourceRec = *(sourceRecs + 2);
                break;
 
        }
    }
    
}