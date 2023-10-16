#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

// Размер карты
#define WIDTH 120
#define HEIGHT 30

// Элеметы карты
#define WALL '#'
#define SNAKE 'o'
#define EMPYTI ' '
#define EAT '*'

#define GETLENARR(n) sizeof(n) / sizeof(n[0])

// Клавишы перемещения 
#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'

// Переменые змейки
int xy[2][WIDTH * HEIGHT / 2];
int lenSnake;
char direction;
int eatXY[2];
int speed;
int dificulty;
int score;

int quit = 0;


// Доп функции консоли
void gotoxy(int x,int y) {
	printf("%c[%d;%df",0x1B,y,x);
}

int kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF)
    {
    ungetc(ch, stdin);
    return 1;
    }

    return 0;
}

char getch() {
    char c;
    system("stty raw");
    c= getchar();
    system("stty sane");
    return(c);
}

// Основной код

// void renderMenu(char menuTitle[], int lenArr1, char menuElems[][2], int lenArr2) {
//     int i;
//     gotoxy((WIDTH / 2) - lenArr1 / 2, HEIGHT / 2);
//     printf("%i", lenArr2);
//     printf("%s", menuTitle);

//     printf("%s", menuElems[0]);

//     // for (i = 0; i < lenArr2; i++) {
//     //     printf("%s", menuElems[i]);
//     // }
// }

void gameOver() {
    char pressed;
    gotoxy((WIDTH / 2) - 5, (HEIGHT / 2) - 2);
    printf("GAME OVER!");
    gotoxy((WIDTH / 2) - 5, (HEIGHT / 2) - 1);
    printf("q - Выход");
    gotoxy((WIDTH / 2) - 5, HEIGHT / 2);
    printf("r - Заново");

    pressed = getch();

    if (pressed == 'q') {
        quit = 1;
    }
    else if (pressed == 'r') {
        startGame();
    }
    
    
}

// Колизия змейки с едой и стенами

int snakeColision() {
    for (int i = 1; i < lenSnake; i++) {
        if (eatXY[0] == xy[0][i - 1] && eatXY[1] == xy[1][i - 1]) {
            lenSnake += 1;
            score = lenSnake - 4;
            renderScore();
            generateEat();
            return 0;
        }
        // if (xy[0][0] == xy[0][i] && xy[0][1] == xy[1][i]) {
        //     gameOver();
        // }
    }
    if (xy[0][0] <= 1 || xy[0][0] >= WIDTH || xy[1][0] <= 1 || xy[1][0] >= HEIGHT) {
        gameOver();
    }

    gotoxy(0, HEIGHT - 2);
    printf("                     ");
    gotoxy(0, HEIGHT + 2);
    printf("%i - %i", eatXY[0], eatXY[1]);
}

// генерация еды на карте

void generateEat() {
    gen1:
        srand ( time(NULL) );
        eatXY[0] = rand() % WIDTH - 1;
        if (eatXY[0] < 1){
            goto gen1;
        }
    
    gen2:
        srand ( time(NULL) );
        eatXY[1] = rand() % HEIGHT - 1;
        if (eatXY[1] < 1){
            goto gen2;
        }

    gotoxy(eatXY[0], eatXY[1]);
    printf("%c", EAT);    
}

// Перемщение змейки

void moveSnake() {
    int x;
	int y;

	x = xy[0][lenSnake-1];
	y = xy[1][lenSnake-1];
	
	gotoxy(x,y);
	printf("%c",EMPYTI);	
    for (int i = lenSnake; i > 0; i--) {
        xy[0][i] = xy[0][i - 1];
        xy[1][i] = xy[1][i - 1];
    }
    switch(direction) {
        case UP:
            xy[1][0] -= 1;
            break;
        case DOWN:
            xy[1][0] += 1;
            break;
        case LEFT:
            xy[0][0] -= 1;
            break;
        case RIGHT:
            xy[0][0] += 1;
            break;
    }
}

// Получение направления змеи

void getDirection() {
    if (kbhit()) {
        char pressed = getch();
        if (pressed == UP && direction != DOWN) {
            direction = pressed;
            return;
        }
        if (pressed == DOWN && direction != UP) {
            direction = pressed;
            return;
        }
        if (pressed == LEFT && direction != RIGHT) {
            direction = pressed;
            return;
        }
        if (pressed == RIGHT && direction != LEFT) {
            direction = pressed;
            return;
        }
    }
}

// Рендер счета

void renderScore() {
    gotoxy(0, HEIGHT + 4);
    printf("           ");
    gotoxy(0, HEIGHT + 4);
    printf("SCORE: %i", score);
}

// Рендер карты

void renderMap() {
    int x, y = 1;

    for (; x < WIDTH + 1; x++) {
        gotoxy(x, y);
        printf("%c", WALL);

        gotoxy(x, HEIGHT);
        printf("%c", WALL);
    }

    x = 0;

    for (; y < HEIGHT + 1; y++) {
        gotoxy(x, y);
        printf("%c", WALL);

        gotoxy(WIDTH, y);
        printf("%c", WALL);
    }
}

// рендер змеи

void renderSnake() {
    for (int i = 0; i < lenSnake; i++) {
        gotoxy(xy[0][i], xy[1][i]);
        printf("%c", SNAKE);

        
    }
}

int startGame() {
    // Получение сложности
    int pressed;
    system("clear");
    gotoxy((WIDTH / 2) - 2, (HEIGHT / 2) - 2);
    printf("SNAKE GAME");
    inputDif:
        gotoxy((WIDTH / 2) - 14, (HEIGHT / 2) - 1);
        printf("Выберите сложность от 1 до 4: ");
        scanf("%d", &pressed);
        if (!(pressed >= 1 && pressed <= 4)) {
            goto inputDif;
        }
    speed = 1;
    lenSnake = 4;
    direction = RIGHT;
    dificulty = pressed;
    score = 0;
    xy[0][0] = 30;
    xy[1][0] = 15;
    system("clear");
 
    generateEat();
    moveSnake();
    renderScore();
    renderMap();
}


int main() {
    startGame();
    while (quit == 0) {
        renderSnake();
        usleep(100000 / speed);
        getDirection();
        moveSnake();
        snakeColision();
        if (!(lenSnake % (int)(30 / dificulty))) {
            speed += 1;
            lenSnake += 1;
            
        }
    }
    
    return 0;
}
