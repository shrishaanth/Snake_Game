#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77
#define W 'w'
#define S 's'
#define A 'a'
#define D 'd'

#define RED "\033[31m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"
#define RESET "\033[0m"

typedef struct
{
    char username[50];
    char password[50];
    int bestscore, w, h;
    int foodchoice;
    int bodychoice1, bodychoice2;
    int headchoice1, headchoice2;
    int delaytime;
} User;

User current_user;
User a[50];
int index = 0, current_index = 0, mode = 0;
int heartexists = 0, hearttimer = 0, length1, length2, score1, score2, life1, life2;

struct coordinate
{
    int x, y, direction;
};
typedef struct coordinate c;
c head1, head2, food, heart, body1[100], body2[100];

void initialize(char name[], char pass[])
{
    strcpy(current_user.username, name);
    strcpy(current_user.password, pass);
    current_user.headchoice1 = 1;
    current_user.headchoice2 = 1;
    current_user.bodychoice1 = 1;
    current_user.bodychoice2 = 2;
    current_user.foodchoice = 1;
    current_user.bestscore = 0;
    current_user.w = 60;
    current_user.h = 20;
    current_user.delaytime = 98;
}

void store()
{
    FILE *f;
    f = fopen("data.txt", "a+");
    if (f == NULL)
    {
        printf("Failed to open file");
        return;
    }
    fprintf(f, "%s %s %d %d %d %d %d %d %d %d %d\n",
            current_user.username, current_user.password, current_user.bestscore,
            current_user.foodchoice, current_user.bodychoice1, current_user.bodychoice2,
            current_user.headchoice1, current_user.headchoice2, current_user.w, current_user.h, current_user.delaytime);
    fclose(f);
}

void read()
{
    FILE *f;
    f = fopen("data.txt", "r+");

    if (f == NULL)
    {
        f = fopen("users.txt", "w+");
        if (f == NULL)
        {
            perror("Failed to open or create file");
            exit(1);
        }
        printf("Created new file: user.txt\n");
    }

    while (!feof(f))
    {
        fscanf(f, "%s %s %d %d %d %d %d %d %d %d %d\n",
               a[index].username, a[index].password, &a[index].bestscore,
               &a[index].foodchoice, &a[index].bodychoice1, &a[index].bodychoice2,
               &a[index].headchoice1, &a[index].headchoice2, &a[index].w, &a[index].h, &a[index].delaytime);
        index++;
    }

    fclose(f);
}

int check(char name[], char pass[])
{
    for (int i = 0; i < index; i++)
    {
        if (strcmp(name, a[i].username) == 0)
        {
            if (strcmp(pass, a[i].password) == 0)
            {
                current_index = i;
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }
    return -1;
}

void load()
{
    current_user = a[current_index];
}

void write()
{
    FILE *f;
    f = fopen("data.txt", "w");
    if (f == NULL)
    {
        printf("Error");
        return;
    }
    a[current_index] = current_user;
    for (int i = 0; i < index; i++)
    {
        fprintf(f, "%s %s %d %d %d %d %d %d %d %d %d\n",
                a[i].username, a[i].password, a[i].bestscore,
                a[i].foodchoice, a[i].bodychoice1, a[i].bodychoice2,
                a[i].headchoice1, a[i].headchoice2, a[i].w, a[i].h, a[i].delaytime);
    }
    fclose(f);
}

void modify()
{
    a[current_index] = current_user;
}

void loading()
{
    int totalSteps = 20;
    printf("Loading: [");
    for (int i = 0; i < totalSteps; i++)
    {
        printf("🍎");
        delay(70);
    }
    printf("]\n");
}

void delay(int a)
{
    Sleep(a);
}

void gotoxy(int x, int y)
{
    COORD cc;
    cc.X = x;
    cc.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cc);
}

void print(int b)
{
    switch (b - 1)
    {
        case 0: printf("●"); break;
        case 1: printf("◯"); break;
        case 2: printf("■"); break;
        case 3: printf("□"); break;
    }
}

void drawBorders()
{
    for (int i = 0; i < current_user.w; i++)
    {
        gotoxy(i, 0);
        printf("─");
        gotoxy(i, current_user.h - 1);
        printf("─");
    }
    for (int i = 0; i < current_user.h; i++)
    {
        gotoxy(0, i);
        printf("│");
        gotoxy(current_user.w - 1, i);
        printf("│");
    }
    gotoxy(0, 0);
    printf("┌");
    gotoxy(current_user.w - 1, 0);
    printf("┐");
    gotoxy(0, current_user.h - 1);
    printf("└");
    gotoxy(current_user.w - 1, current_user.h - 1);
    printf("┘");
}

void gfood()
{
    srand(time(NULL));
    food.x = rand() % (current_user.w - 2) + 1;
    food.y = rand() % (current_user.h - 2) + 1;
    gotoxy(food.x, food.y);
    print(current_user.foodchoice);
}

void gheart()
{
    if (heartexists == 0)
    {
        srand(time(NULL));
        heart.x = rand() % (current_user.w - 2) + 1;
        heart.y = rand() % (current_user.h - 2) + 1;
        gotoxy(heart.x, heart.y);
        printf("♥");
        heartexists = 1;
    }
}
void reset()
{
    length1 = 5;
    life1 = 3;
    score1 = 0;
    head1.x = current_user.w / 4;
    head1.y = current_user.h / 2;
    head1.direction = RIGHT;
    for (int i = 0; i < length1; i++)
    {
        body1[i].x = head1.x - i;
        body1[i].y = head1.y;
    }

    length2 = 5;
    life2 = 3;
    score2 = 0;
    head2.x = 3 * current_user.w / 4;
    head2.y = current_user.h / 2;
    head2.direction = LEFT;
    for (int i = 0; i < length2; i++)
    {
        body2[i].x = head2.x + i;
        body2[i].y = head2.y;
    }

    heartexists = 0;
    hearttimer = 0;
}
void moveSnake(c *head, c body[], int length, int q)
{
    if(body[length-1].x!=0)
    {
        gotoxy(body[length - 1].x, body[length - 1].y);
        printf(" ");
    }
    for (int i = length - 1; i > 0; i--)
    {
        body[i] = body[i - 1];
    }

    body[0] = *head;
    gotoxy(head->x, head->y);

    if (current_user.headchoice1 == 1 && q == 1)
    {
        print(current_user.headchoice1);
    }
    else if (current_user.headchoice2 == 1 && q == 2)
    {
        print(current_user.headchoice2);
    }
    else
    {
        printf("O");
    }

    for (int i = 1; i < length; i++)
    {
        gotoxy(body[i].x, body[i].y);
        if (q == 1)
            print(current_user.bodychoice1);
        else
            print(current_user.bodychoice2);
    }
}

void drawRectangle(int startX, int startY, int width, int height)
{
    for (int i = 0; i < width; i++)
    {
        gotoxy(startX + i, startY);
        printf("═");
        gotoxy(startX + i, startY + height - 1);
        printf("═");
    }
    for (int i = 0; i < height; i++)
    {
        gotoxy(startX, startY + i);
        printf("║");
        gotoxy(startX + width - 1, startY + i);
        printf("║");
    }
}

int pause()
{
    int pauseX = (current_user.w - 30) / 2;
    int pauseY = (current_user.h - 6) / 2;
    system("cls");

    drawRectangle(pauseX, pauseY, 30, 6);
    gotoxy(pauseX + 1, pauseY + 1); printf("PAUSE MENU");
    gotoxy(pauseX + 1, pauseY + 2); printf("PRESS ANY KEY TO CONTINUE");
    gotoxy(pauseX + 1, pauseY + 3); printf("PRESS ESC TO EXIT");

    char ch = _getch();
    if (ch == 27)
        return 1;
    else
        return 0;
}

int input()
{
    if (_kbhit())
    {
        switch (_getch())
        {
            case UP:
                if (head1.direction != DOWN)
                    head1.direction = UP;
                break;
            case DOWN:
                if (head1.direction != UP)
                    head1.direction = DOWN;
                break;
            case LEFT:
                if (head1.direction != RIGHT)
                    head1.direction = LEFT;
                break;
            case RIGHT:
                if (head1.direction != LEFT)
                    head1.direction = RIGHT;
                break;
            case 27:
                return pause();
            if (mode)
            {
                case W:
                    if (head2.direction != DOWN)
                        head2.direction = UP;
                    break;
                case S:
                    if (head2.direction != UP)
                        head2.direction = DOWN;
                    break;
                case A:
                    if (head2.direction != RIGHT)
                        head2.direction = LEFT;
                    break;
                case D:
                    if (head2.direction != LEFT)
                        head2.direction = RIGHT;
                    break;
            }
        }
    }
    return -1;
}

void display2()
{
    gotoxy(current_user.w + 5, 2);
    printf("Player 1 Score : %d", score1);
    gotoxy(current_user.w + 5, 4);
    printf("Player 1 Lives : %d", life1);
    gotoxy(current_user.w + 5, 6);
    printf("Player 2 Score : %d", score2);
    gotoxy(current_user.w + 5, 8);
    printf("Player 2 Lives : %d", life2);
}

void logic()
{
    switch (head1.direction)
    {
        case UP:
            head1.y--;
            break;
        case DOWN:
            head1.y++;
            break;
        case LEFT:
            head1.x--;
            break;
        case RIGHT:
            head1.x++;
            break;
    }

    if (mode == 1)
    {
        switch (head2.direction)
        {
            case UP:
                head2.y--;
                break;
            case DOWN:
                head2.y++;
                break;
            case LEFT:
                head2.x--;
                break;
            case RIGHT:
                head2.x++;
                break;
        }
    }

    if (head1.x == food.x && head1.y == food.y)
    {
        score1 += 10;
        length1++;
        gfood();
    }

    if ((head2.x == food.x && head2.y == food.y) && mode)
    {
        score2 += 10;
        length2++;
        gfood();
    }

    if ((head1.x == heart.x && head1.y == heart.y) && heartexists)
    {
        life1++;
        gotoxy(heart.x, heart.y);
        printf(" ");
        heartexists = 0;
        hearttimer = 0;
    }

    if ((head2.x == heart.x && head2.y == heart.y) && heartexists && mode)
    {
        life2++;
        gotoxy(heart.x, heart.y);
        printf(" ");
        heartexists = 0;
        hearttimer = 0;
    }
}

void checkCollisions()
{
    if (head1.x >= current_user.w - 1 || head1.x <= 0 || head1.y <= 0 || head1.y >= current_user.h - 1)
    {
        life1--;
        if (life1 > 0)
        {
            head1.x = current_user.w / 4;
            head1.y = current_user.h / 2;
        }
    }

    if (head2.x >= current_user.w - 1 || head2.x <= 0 || head2.y <= 0 || head2.y >= current_user.h - 1)
    {
        life2--;
        if (life2 > 0)
        {
            head2.x = 3 * current_user.w / 4;
            head2.y = current_user.h / 2;
        }
    }

    for (int i = 0; i < length1; i++)
    {
        if (head1.x == body1[i].x && head1.y == body1[i].y)
        {
            life1--;
            if (life1 > 0)
            {
                head1.x = current_user.w / 4;
                head1.y = current_user.h / 2;
            }
        }
    }

    for (int i = 0; i < length2; i++)
    {
        if (head2.x == body2[i].x && head2.y == body2[i].y)
        {
            life2--;
            if (life2 > 0)
            {
                head2.x = 3 * current_user.w / 4;
                head2.y = current_user.h / 2;
            }
        }
    }

    if (head1.x == head2.x && head1.y == head2.y)
    {
        life1--;
        life2--;
        if (life1 > 0)
        {
            head1.x = current_user.w / 4;
            head1.y = current_user.h / 2;
        }
        if (life2 > 0)
        {
            head2.x = 3 * current_user.w / 4;
            head2.y = current_user.h / 2;
        }
    }

    for (int i = 0; i < length2; i++)
    {
        if (head1.x == body2[i].x && head1.y == body2[i].y)
        {
            life1--;
            life2--;
            if (life1 > 0)
            {
                head1.x = current_user.w / 4;
                head1.y = current_user.h / 2;
            }
            if (life2 > 0)
            {
                head2.x = 3 * current_user.w / 4;
                head2.y = current_user.h / 2;
            }
        }
    }

    for (int i = 0; i < length1; i++)
    {
        if (head2.x == body1[i].x && head2.y == body1[i].y)
        {
            life1--;
            life2--;
            if (life1 > 0)
            {
                head1.x = current_user.w / 4;
                head1.y = current_user.h / 2;
            }
            if (life2 > 0)
            {
                head2.x = 3 * current_user.w / 4;
                head2.y = current_user.h / 2;
            }
        }
    }
}

int gameOver()
{
    return life1 <= 0 || life2 <= 0;
}

void display1()
{
    gotoxy(current_user.w + 5, 2);
    printf("Score : %d", score1);
    gotoxy(current_user.w + 5, 4);
    printf("Lives : %d", life1);
}

int gameOver1()
{
    if (head1.x >= current_user.w - 1 || head1.x <= 0 || head1.y <= 0 || head1.y >= current_user.h - 1)
    {
        life1--;
        if (life1 > 0)
        {
            system("cls");
            drawBorders();
            gfood();
            display1();
            head1.x = current_user.w / 2;
            head1.y = current_user.h / 2;
            head1.direction = RIGHT;
            length1 = 5;
            heartexists = 0;
            return 0;
        }
        else
        {
            return 1;
        }
    }

    for (int i = 0; i < length1; i++)
    {
        if (head1.x == body1[i].x && head1.y == body1[i].y)
        {
            life1--;
            if (life1 > 0)
            {
                system("cls");
                drawBorders();
                gfood();
                display1();
                head1.x = current_user.w / 2;
                head1.y = current_user.h / 2;
                head1.direction = RIGHT;
                length1 = 5;
                heartexists = 0;
                return 0;
            }
            else
            {
                return 1;
            }
        }
    }
    return 0;
}

void option()
{
    int choice;
    system("cls");
    printf("PLAYER 1 ---------> PRESS 1\nPLAYER 2 ---------> PRESS 2");
    int n = _getch() - '0';
    system("cls");
    printf("1) MODIFY MY SNAKE\n");
    printf("2) CHANGE GRID SIZE\n");
    printf("3) CHANGE DIFFICULTY\n");
    printf("4) CHANGE MY PROFILE\n");
    printf("Enter your choice: ");
    choice = _getch() - '0';
    system("cls");
    switch (choice)
    {
        case 1:
            printf("1)HEAD\n2)BODY\n3)FOOD");
            int c = _getch() - '0';
            system("cls");
            switch (c)
            {
                case 1:
                    printf("1) ●\n2) O\n");
                    if (n == 1)
                        current_user.headchoice1 = _getch() - '0';
                    else
                        current_user.headchoice2 = _getch() - '0';
                    break;
                case 2:
                    printf("1) ●\n2) ◯\n3) ■\n4) □\n");
                    if (n == 1)
                        current_user.bodychoice1 = _getch() - '0';
                    else
                        current_user.bodychoice2 = _getch() - '0';
                    break;
                case 3:
                    printf("1) ●\n2) ◯\n3) ■\n4) □\n");
                    current_user.foodchoice = _getch() - '0';
                    break;
            }
            break;
        case 2:
            printf("1)SMALL SIZE GRID (BEST FOR ALL SCREENS)\n2)LARGE SIZE GRID(NEED TO BE IN FULLSCREEN FOR THIS)");
            switch (_getch() - '0')
            {
                case 1:
                    current_user.w = 60;
                    current_user.h = 20;
                    break;
                case 2:
                    current_user.w = 100;
                    current_user.h = 38;
                    break;
            }
            break;
        case 3:
            printf("1)EASY\n2)MEDIUM\n3)HARD\n");
            switch (_getch() - '0')
            {
                case 1:
                    current_user.delaytime = 98;
                    break;
                case 2:
                    current_user.delaytime = 78;
                    break;
                case 3:
                    current_user.delaytime = 50;
                    break;
            }
            break;
        case 4:
            printf("Enter New Username :");
            scanf("%s", current_user.username);
            printf("Enter New Password :");
            scanf("%s", current_user.password);
            loading();
            printf("Profile Updated Successfully!");
            _getch();
            break;
    }

    modify();
    write();
}

void main()
{
    char name[100], pass[100];
    read();
B1:
    system("cls");

    printf("Enter Username: ");
    scanf("%s", name);
    printf("Enter password: ");
    scanf("%s", pass);
    int login_status = check(name, pass);
    if (login_status == 0)
    {
        printf("Invalid Password. Please Reenter.\n");
        _getch();
        goto B1;
    }
    else if (login_status == -1)
    {
        printf("Creating New Profile .....\n");
        loading();
        initialize(name, pass);
        store();
        printf("\nNew profile created ...login successful.\n");
        _getch();
    }
    else
    {
        loading();
        printf("Welcome Back!\n");
        load();
        _getch();
    }
C1:
    system("cls");
    printf("\033[1;4;32mSNAKE GAME\033[0m\n");
    printf("1)PLAY\n2)OPTIONS\n3)EXIT\n");
    int choice = _getch() - '0';
    if (choice == 2)
    {
        option(), system("cls");
        goto C1;
    }
    else if (choice == 3)
    {
        exit(0);
    }
L1:
    system("cls");
    printf("\033[1;4;32mSNAKE GAME\033[0m\n");
    printf("1)SINGLE PLAYER\n2)DOUBLE PLAYER\nENTER THE MODE YOU WANT TO PLAY : \n");
    mode=_getch()-'0';
    int temp = mode;
    mode--;
    loading(), system("cls");
    switch (temp)
    {
        case 1:
            do
            {
                system("cls");
                printf("Welcome To The Snake Game!\n");
                printf("-> Use Arrow Keys To Move The Snake.\n");
                printf("-> You Will Be Provided Foods At Several Coordinates Of The Screen Which You Have To Eat. Every Time You Eat Food, The Length Of The Snake Will Increase By 1 Element And Thus The Score.\n");
                printf("-> Here You Are Provided With Three Lives. Your Life Will Decrease As You Hit The Wall Or Snake's Body.\n");
                printf("-> Hearts Will Be Generated At Some Intervals. If You Consume It, Your Health Will Increase.\n");
                printf("Enjoy your game!\n");
                printf("Press any key to continue...\n");
                _getch();
                system("cls");
                reset();
                drawBorders();
                gfood();
                display1();
                while (1)
                {
                    int p = input();
                    if (p == 1)
                    {
                        goto C1;
                    }
                    else if (p == 0)
                    {
                        system("cls");
                        drawBorders();
                        gfood();
                    }
                    logic();
                    hearttimer++;
                    if (hearttimer >= 400)
                    {
                        gheart();
                        hearttimer = 0;
                    }
                    if (gameOver1())
                    {
                        system("cls");
                        if (current_user.bestscore < score1)
                        {
                            current_user.bestscore = score1;
                            modify();
                            write();
                        }
                        printf(RED "GAME OVER!\n" RESET);
                        printf(YELLOW "FINAL SCORE: %d\n" RESET, score1);
                        printf(CYAN "BEST SCORE: %d\n" RESET, current_user.bestscore);
                        break;
                    }
                    moveSnake(&head1, body1, length1, 1);
                    display1();
                    delay(current_user.delaytime);
                }
                char choice;
                printf("DO YOU WANT TO RETRY ? :");
                choice = _getch();
                if (choice == 'n' || choice == 'N')
                    goto C1;
            } while (1);
            break;
        case 2:
            do
            {
                system("cls");
                printf("Welcome To The Snake Game - 2 Player Mode!\n");
                printf("-> Player 1: Use Arrow Keys To Move.\n");
                printf("-> Player 2: Use WASD To Move.\n");
                printf("-> You Will Be Provided Foods At Several Coordinates Of The Screen Which You Have To Eat. The Length Of The Snake Will Increase By 1 Element, And So Will The Score.\n");
                printf("-> Hearts Will Be Generated At Some Intervals. If You Consume It, Your Life Will Increase.\n");
                printf("-> If Either Player Hits The Wall Or Their Own Body, They Will Lose A Life. The Game Will End When A Player Has No Lives Left.\n");
                printf("Press any key to continue...\n");
                _getch();
                system("cls");
                reset();
                drawBorders();
                gfood();
                display2();
                while (1)
                {
                    int p = input();
                    if (p == 1)
                    {
                        goto C1;
                    }
                    else if (p == 0)
                    {
                        system("cls");
                        drawBorders();
                        gfood();
                    }
                    logic();
                    hearttimer++;
                    if (hearttimer >= 400)
                    {
                        gheart();
                        hearttimer = 0;
                    }
                    checkCollisions();
                    if (gameOver())
                    {
                        system("cls");
                        printf("GAME OVER!\n");

                        if (score1 > score2)
                        {
                            printf("Player 1 Wins! FINAL SCORE: %d\n", score1);
                        }
                        else if (score2 > score1)
                        {
                            printf("Player 2 Wins! FINAL SCORE: %d\n", score2);
                        }
                        else
                        {
                            printf("It's a Draw! FINAL SCORE: %d - %d\n", score1, score2);
                        }
                        break;
                    }
                    moveSnake(&head1, body1, length1, 1);
                    moveSnake(&head2, body2, length2, 2);
                    display2();
                    delay(current_user.delaytime);
                }
                char choice;
                printf("DO YOU WANT TO RETRY? (y/n): ");
                choice = _getch();
                if (choice == 'n' || choice == 'N')
                {
                    goto C1;
                }
            } while (1);
            break;
        default:
            goto L1;
    }
}
