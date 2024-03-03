// Enums
enum Character{
    Katusha,
    Pig
};

enum Direction{
    LEFT,
    RIGHT  
};

enum States{
    IDLE,
    RUN,
    JUMP
};

// Structs
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
    enum Direction prevDirection;
    int canDash;
    int dashing;
    Texture2D sprite;
    Texture2D* baseSpr;
    Rectangle sourceRec;
    Rectangle* baseSourceRec;
    enum States currentState;
    enum States prevState;
    enum Character character;
} Player;

typedef struct {
    Rectangle rect;
    int blocking;
    Color color; 
    Texture2D texture;
} EnvItem ;