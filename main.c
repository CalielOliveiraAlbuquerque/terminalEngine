#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h> 
#include "engine.h"

struct sprite player;
extern struct camera camera;

void keypress(){
    char move;
    read(STDIN_FILENO, &move, 1);
    switch(move){
        case 'a':
            player.position.x -= 2;
            break;
        case 'd':
            player.position.x += 2;
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
    initEngine();
    player.position.x = camera.position.x + 2;
    player.position.y = camera.position.y + 2;
    {
        FILE* file = fopen("sprites/player", "r");
        initSpriteWithFile(&player, file);
        fclose(file);
    }

    struct sprite konata = {0};
    konata.position = (vector2){80, 10};
    {
        FILE* file = fopen("sprites/konata", "r");
        initSpriteWithFile(&konata, file);
        fclose(file);
    }

    struct sprite garden = {0};
    garden.position = (vector2){60, 10};
    {
        FILE* file = fopen("sprites/garden", "r");
        initSpriteWithFile(&garden, file);
        fclose(file);
    }

    struct sprite text = {0};
    {
        FILE* file = fopen("sprites/text", "r");
        initSpriteWithFile(&text, file);
        fclose(file);
    }

    addToCamera(&player);
    addToCamera(&konata);
    addToCamera(&garden);
    addToCamera(&text);

    int i=0;
    while(1){
        i++;
        keypress();
        displayMap();
    }
}

