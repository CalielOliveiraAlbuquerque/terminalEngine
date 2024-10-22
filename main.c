#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>

#define MAP_WIDTH 50 
#define MAP_HEIGHT 10
#define EMPTY_SPACE ' '

#ifdef unix
#define CLEAN_SCREEN() system("clear")
#else
#define CLEAN_SCREEN = system("clear")
#endif


struct vector2{
    int x,y;
};
struct object{
    struct vector2 position;
    char model;
} player = {0};
struct camera{
    //null terminated
    struct object** objectList;
    struct vector2 position;
    int amountOfEntities, objectListSize;
} camera = {0};

void addVector2(struct vector2* a, const struct vector2* b){
    a->x += b->x;
    a->y += b->y;
}

void subVector2(struct vector2* a, const struct vector2* b){
    a->x -= b->x;
    a->y -= b->y;
}

int cmpVector2(const struct vector2* a, const struct vector2* b){
    if(a->x < b->x || a->y < b->y) return -1;
    else if(a->x > b->x || a->y > b->y) return 1;
    return 0;
}

void displayMap(){
    CLEAN_SCREEN();
    char canvas[MAP_HEIGHT][MAP_WIDTH];
    memset(canvas, EMPTY_SPACE, MAP_WIDTH * MAP_HEIGHT);

    for(int i=0; i<camera.amountOfEntities; i++){
        struct vector2 screenPosition = camera.objectList[i]->position;
        subVector2(&screenPosition, &camera.position);

        if(screenPosition.x < 0 || screenPosition.y < 0 || screenPosition.x >= MAP_WIDTH || screenPosition.y >= MAP_HEIGHT){
            continue;
        }

        canvas[screenPosition.y][screenPosition.x] = camera.objectList[i]->model;
    }

    for(int i=0; i<MAP_HEIGHT; i++){
        for(int j=0; j<MAP_WIDTH; j++){
            printf("%c", canvas[i][j]);
        }
        puts("|");
    }
}

void addToCamera(struct object* objectPointer){
    assert(objectPointer != NULL);
    camera.amountOfEntities++;
    if(camera.objectListSize < camera.amountOfEntities){
        camera.objectListSize = camera.objectListSize == 0? 1 : camera.objectListSize * 2;
        void* result = realloc(camera.objectList, camera.objectListSize * sizeof(struct object*));
        assert(result != NULL);
        camera.objectList = result;
    }

    camera.objectList[camera.amountOfEntities-1] = objectPointer;
}

void movement(){
    char move = getchar();
    switch(move){
        case 'a':
            player.position.x--;
            break;
        case 'd':
            player.position.x++;
            break;
        case 's':
            player.position.y++;
            break;
        case 'w':
            player.position.y--;
            break;
    }
}

int main(){
    struct termios terminalSettings;
    tcgetattr(STDIN_FILENO, &terminalSettings);
    const struct termios originalTerminalSettings = terminalSettings;

    terminalSettings.c_lflag &= ~(ICANON | ECHO); 
    tcsetattr(STDIN_FILENO, TCSANOW, &terminalSettings);
    
    player.model = 'O';
    addToCamera(&player);

    struct object potion;
    potion.position.x = 10;
    potion.position.y = 10;
    potion.model = 'P';
    addToCamera(&potion);

    while(1){
        displayMap();
        movement();
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &originalTerminalSettings);
}
