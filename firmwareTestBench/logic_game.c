#include "./lcd.h"
#include <stdio.h>
#include <stdlib.h>
#include "globalvar.h"

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3


uint8_t snake[200][3];
uint8_t food [2][2];
uint16_t score;
uint8_t speed;
uint8_t count;
uint8_t c=0;


uint8_t dat[4];

static void idle(void){
    gameState=GAME;
    print_frame();
    
    snake[0][0]=8;
    snake[0][1]=15;
    snake[0][2]=HEAD;
    snake[1][0]=7;
    snake[1][1]=15;
    snake[1][2]=BODY;
    snake[2][0]=6;
    snake[2][1]=15;
    snake[2][2]=TAIL;
    snake[3][0]=5;
    snake[3][1]=15;
    snake[3][2]=GROUND;

    for(uint8_t i=4; i<200;i++){
        snake[i][0]=31;
        snake[i][1]=31;
        snake[i][2]=GROUND;
    }

    food[0][0]=(rand() % 28) + 2;
    food[0][1]=(rand() % 26) + 4;
    food[1][0]=(rand() % 28) + 2;
    food[1][1]=(rand() % 26) + 4;
    dir=1;
    score=0;
    speed=0x6;
    count=0;
}

static void UI(void){
    switch (gameState){
        case GAME:
            for(uint8_t i=0; i<200;i++){
                    if (snake[i][0]==31)break;
                    if (snake[i][1]==31)break;
                    print_tile(snake[i][2],snake[i][0],snake[i][1]);
                }
                print_tile(FOOD,food[0][0],food[0][1]);
                print_tile(GROUND,food[1][0],food[1][1]);

                dat[0]=score/1000;
                dat[1]=score/100-dat[0]*10;
                dat[2]=score/10-dat[0]*100-dat[1]*10;
                dat[3]=score/1-dat[0]*1000-dat[1]*100-dat[2]*10;

                print_tile(dat[0],2,2);
                print_tile(dat[1],3,2);
                print_tile(dat[2],4,2);
                print_tile(dat[3],5,2);
        break;

        case LOST:
            print_tile(LetterS,5,15);
            print_tile(LetterC,6,15);
            print_tile(LetterO,7,15);
            print_tile(LetterR,8,15);
            print_tile(LetterE,9,15);

            print_tile(dat[0],11,15);
            print_tile(dat[1],12,15);
            print_tile(dat[2],13,15);
            print_tile(dat[3],14,15);
        break;
    } 
}

static void refresh_food(void){
    food[1][0]=food[0][0];
    food[1][1]=food[0][1];
    food[0][0]=(rand() % 28) + 2;
    food[0][1]=(rand() % 26) + 4;
}

static void move(void){
        for(uint8_t i=0; i<200;i++){        
            if (snake[i][0]==31)break;
            if (snake[i][1]==31)break;
            c=i;
        }
        for(uint8_t i=c; i>0 ;i--){
            snake[i][0]=snake[i-1][0];
            snake[i][1]=snake[i-1][1];
        }   
            

        switch(dir){
            case UP:
                if((snake[0][0]==snake[1][0])&&(snake[0][1]>snake[1][1])){
                    snake[0][1]=snake[0][1]+1;
                }else{
                    snake[0][1]=snake[0][1]-1;        
                }
            break;

            case RIGHT:
                if(snake[0][0]<snake[1][0]){
                    snake[0][0]=snake[0][0]-1;
                }else{
                    snake[0][0]=snake[0][0]+1;        
                }
            break;

            case DOWN:
                if(snake[0][1]<snake[1][1]){
                    snake[0][1]=snake[0][1]-1;
                }else{
                    snake[0][1]=snake[0][1]+1;
                }
            break;

            case LEFT:
                if(snake[0][0]>snake[1][0]){
                    snake[0][0]=snake[0][0]+1;
                }else{
                    snake[0][0]=snake[0][0]-1;
                }
            break;
        }    
}

static void control(void){
    if(gameState==GAME){
            count++;
            if(count>=(0x10-speed)){
                move();
                count=0;
            }

            if(snake[0][0]==30)snake[0][0]=2;
            if(snake[0][0]==1)snake[0][0]=29;
            if(snake[0][1]==30)snake[0][1]=4;
            if(snake[0][1]==3)snake[0][1]=29;

            if((snake[0][0]==food[0][0])&&(snake[0][1]==food[0][1])){
                refresh_food();
                snake[c-1][2]=BODY;
                snake[c][2]=TAIL;
                snake[c+1][2]=GROUND;
                snake[c+1][0]=snake[c][0];
                snake[c+1][1]=snake[c][1];
                score++;
                speed++;
            }
            for (uint8_t i=1;i<c;i++){
                if((snake[0][0]==snake[i][0])&&(snake[0][1]==snake[i][1])){
                    idle();
                    gameState=LOST;
                    break;
                }
            }
    }

}
    
static void game(void){
    idle();
    for(uint8_t i=0;i<10000;i++){
    //printf("STATUS: %d\n",gameState);    
        UI();
        control();
    }
}
/*
for(uint8_t i=0; i<15;i++){
    printf("i: %d j: %d Tile: %d\n",snake[i][0],snake[i][1],snake[i][2]);    
}
for(uint8_t i=0; i<2;i++){
    printf("i: %d j: %d\n",food[i][0],food[i][1]);
    printf("--------- \n"); 
}*/
