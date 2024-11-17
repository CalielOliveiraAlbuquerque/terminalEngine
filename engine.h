#include <stdio.h>
#include <ncurses.h>

typedef struct{
    int x;
    int y;
}vector2;
struct square{
    vector2 upLeft;
    vector2 upRight;
    vector2 bottomLeft;
    vector2 bottomRight;
};
struct bounds{
    int up;
    int down;
    int left;
    int right;
};
struct pixel{
    vector2 position;
    char model[5];
};
struct sprite{
    vector2 position;
    vector2 displacement;
    struct pixel* pixelList; 
    int amountOfPixels;
    int pixelListSize;
};
struct collider{
    vector2 mesh[4];
    struct collider* collisions;
    int amountOfCollisions;
};
extern struct camera{
    struct sprite** spriteList;
    vector2 position;
    int amountOfSprites, spriteListSize;
} camera;

void addToCamera(struct sprite* sprite);
void initEngine() ;
void displayMap();
void initSpriteWithFile(struct sprite* sprite, FILE* file);
void initSprite(struct sprite* sprite);
struct pixel* addPixelSpace(struct sprite* sprite, const int size);
bool inScreen(vector2 point);
vector2 addVector2(vector2 a, const vector2 b);
vector2 subVector2(vector2 a, const vector2 b);
