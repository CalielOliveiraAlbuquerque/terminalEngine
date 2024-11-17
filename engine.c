//Main problems im facing: I can't render anything. The size of the screen is messed up
//


#include <locale.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <ncurses.h>
#include "memoryAllocationMacros.h"

#define SCREEN_LOWEST_VALUE 0
#define EMPTY_SPACE ' '

#ifdef unix
#define CLEAN_SCREEN() system("clear")
#else
#define CLEAN_SCREEN = system("clear")
#endif

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

struct camera{
    struct sprite** spriteList;
    vector2 position;
    int amountOfSprites, spriteListSize;
} camera = {0};

vector2 max;

vector2 addVector2(vector2 a, const vector2 b){
    a.x += b.x;
    a.y += b.y;
    return a;
}

vector2 subVector2(vector2 a, const vector2 b){
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

bool inScreen(vector2 point){
    if(point.x < SCREEN_LOWEST_VALUE)   return false;
    if(point.y < SCREEN_LOWEST_VALUE)   return false;
    if(point.x > max.x)                 return false;
    if(point.y > max.y)                 return false;
                                        return true;
}

struct pixel* addPixelSpace(struct sprite* sprite, const int size){
    if(size == 0) 
        return &(sprite->pixelList[sprite->amountOfPixels-1-size]);

    int returnPosition = sprite->amountOfPixels;
    sprite->amountOfPixels += size;
    if(sprite->amountOfPixels > sprite->pixelListSize){
        if(sprite->pixelListSize <= 0){
            sprite->pixelListSize = 1;
        }
        while(sprite->pixelListSize < sprite->amountOfPixels){
            sprite->pixelListSize *= 2;
        }
        REALLOC_OR_DIE(sprite->pixelList, sprite->pixelListSize * sizeof(struct pixel), __FILE__, __LINE__); 
    }

    return &(sprite->pixelList[returnPosition]);
}

void initSprite(struct sprite* sprite){
    sprite->amountOfPixels = 0;
    sprite->pixelListSize = 0;
    sprite->pixelList = NULL;
}

bool isNthBitSet (unsigned char c, int n) {
    static unsigned char mask[] = {128, 64, 32, 16, 8, 4, 2, 1};
    return ((c & mask[n]) != 0);
}
bool isComplementaryCharacter(unsigned char character){
    return isNthBitSet(character, 0) 
        && !isNthBitSet(character, 1);
}
bool isTwoBytesCharacter(unsigned char character){
    return isNthBitSet(character, 0) 
        && isNthBitSet(character, 1) 
        && !isNthBitSet(character, 2);
}
bool isThreeBytesCharacter(unsigned char character){
    return isNthBitSet(character, 0) 
        && isNthBitSet(character, 1) 
        && isNthBitSet(character, 2) 
        && !isNthBitSet(character, 3);
}
bool isFourBytesCharacter(unsigned char character){
    return isNthBitSet(character, 0) 
        && isNthBitSet(character, 1) 
        && isNthBitSet(character, 2) 
        && isNthBitSet(character, 3)
        && !isNthBitSet(character, 4);
}

void initSpriteWithFile(struct sprite* sprite, FILE* file){
    initSprite(sprite);

    assert(file);

    unsigned char model = ' ';
    vector2 pixelCoordinates = {0};
    const unsigned char   unsignedEOF = EOF;

    while((model = getc(file)) != unsignedEOF){
        if(model == '\n'){
            pixelCoordinates.x = 0;
            pixelCoordinates.y++;
            continue;
        }else if(model == '\t'){
            pixelCoordinates.x += 4;
            continue;
        }else if( model == ' '){
            pixelCoordinates.x++;
            continue;
        }

        int8_t size;
        if(isascii(model)){
            size = 1;
        } else if(isTwoBytesCharacter(model)){
            size = 2;
        } else if(isThreeBytesCharacter(model)){
            size = 3;
        } else if(isFourBytesCharacter(model)){
            size = 4;
        }

        struct pixel* pixel = addPixelSpace(sprite, 1);
        pixel->model[0] = model;
        
        int i=1;
        for(i=1;i<size;i++){
            model = getc(file);
            if(!isComplementaryCharacter(model)){
                puts("");
                break;
            }
            pixel->model[i] = model;
        }
        if(i != size)
            continue;

        pixel->model[size] = '\0';

        pixel->position = pixelCoordinates;
        pixelCoordinates.x++;
        sprite->amountOfPixels++;
    }

    if(sprite->amountOfPixels == 0){
        puts("no pixels");
        sprite->pixelListSize = 0;
        sprite->pixelList = NULL;
        return;
    } 

    REALLOC_OR_DIE(sprite->pixelList, sprite->pixelListSize * sizeof(struct pixel), __FILE__, __LINE__);
}

void displayMap(){
    clear();

    for(int i=0; i<camera.amountOfSprites; i++){
        struct sprite sprite = *camera.spriteList[i];
        for(int j=0; j<sprite.amountOfPixels; j++){
            struct pixel pixel = sprite.pixelList[j];
            pixel.position = addVector2(pixel.position, sprite.position);
            pixel.position = addVector2(pixel.position, sprite.displacement);
            pixel.position = subVector2(pixel.position, camera.position);
            mvprintw(pixel.position.y, pixel.position.x, "%s", pixel.model);
        }
    }

    refresh();
}

void initEngine() {
    setlocale(LC_ALL, "");
    initscr(); //Initialize the window
    noecho(); //Don't echo keypresses
    keypad(stdscr, TRUE);
    cbreak();
    timeout(10);
    curs_set(FALSE); //Don't display a cursor

    //Global var stdscr is created by the call to initscr()
    getmaxyx(stdscr, max.y, max.x);
}

void addToCamera(struct sprite* sprite){
    assert(sprite != NULL);
    camera.amountOfSprites++;
    if(camera.spriteListSize < camera.amountOfSprites){
        camera.spriteListSize = camera.spriteListSize == 0? 1 : camera.spriteListSize * 2;
        void* result = realloc(camera.spriteList, camera.spriteListSize * sizeof(struct pixel*));
        assert(result != NULL);
        camera.spriteList = result;
    }

    camera.spriteList[camera.amountOfSprites-1] = sprite;
}
