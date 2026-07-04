

#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>


#define WRAP_AROUND_WALLS 0   
#define INITIAL_LENGTH    3
#define INITIAL_DELAY_MS  120
#define MIN_DELAY_MS      50
#define SPEEDUP_STEP_MS   4   
#define BOARD_MARGIN      1   


static const char *GLYPH_APPLE      = "\U0001F34E"; 
static const char *GLYPH_HEAD       = "\U0001F600"; 
static const char *GLYPH_HEAD_DEAD  = "\U0001F480"; 
static const char *GLYPH_BODY       = "\U0001F7E9"; 



typedef struct Point {
    int x;
    int y;
} Point;


typedef struct SnakeSegment {
    Point pos;
    struct SnakeSegment *next; 
    struct SnakeSegment *prev; 
} SnakeSegment;

typedef enum Direction { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT } Direction;

typedef struct Snake {
    SnakeSegment *head;
    SnakeSegment *tail;
    int length;
    Direction dir;
} Snake;

typedef struct Food {
    Point pos;
    bool active;
} Food;

typedef struct Game {
    int width, height;     
    Snake snake;
    Food food;
    int score;
    int delay_ms;
    bool paused;
    bool game_over;
} Game;



static SnakeSegment *make_segment(Point p) {
    SnakeSegment *s = malloc(sizeof(SnakeSegment));
    s->pos = p;
    s->next = s->prev = NULL;
    return s;
}

static void snake_push_head(Snake *sn, Point p) {
    SnakeSegment *s = make_segment(p);
    s->next = sn->head;
    if (sn->head) sn->head->prev = s;
    sn->head = s;
    if (!sn->tail) sn->tail = s;
    sn->length++;
}

static void snake_pop_tail(Snake *sn) {
    if (!sn->tail) return;
    SnakeSegment *old = sn->tail;
    sn->tail = old->prev;
    if (sn->tail) sn->tail->next = NULL;
    else sn->head = NULL;
    free(old);
    sn->length--;
}

static void snake_free(Snake *sn) {
    SnakeSegment *cur = sn->head;
    while (cur) {
        SnakeSegment *nxt = cur->next;
        free(cur);
        cur = nxt;
    }
    sn->head = sn->tail = NULL;
    sn->length = 0;
}

static bool snake_occupies(const Snake *sn, Point p) {
    for (SnakeSegment *cur = sn->head; cur; cur = cur->next) {
        if (cur->pos.x == p.x && cur->pos.y == p.y) return true;
    }
    return false;
}



static void spawn_food(Game *g) {
    Point p;
    do {
        p.x = 1 + rand() % g->width;
        p.y = 1 + rand() % g->height;
    } while (snake_occupies(&g->snake, p));
    g->food.pos = p;
    g->food.active = true;
}

static void game_init(Game *g, int width, int height) {
    g->width = width;
    g->height = height;
    g->snake.head = g->snake.tail = NULL;
    g->snake.length = 0;
    g->snake.dir = DIR_RIGHT;
    g->score = 0;
    g->delay_ms = INITIAL_DELAY_MS;
    g->paused = false;
    g->game_over = false;

    int startx = width / 2;
    int starty = height / 2;
    for (int i = INITIAL_LENGTH - 1; i >= 0; i--) {
        Point p = { startx - i, starty };
        snake_push_head(&g->snake, p);
    }
    spawn_food(g);
}

static void game_destroy(Game *g) {
    snake_free(&g->snake);
}

static Point next_head_pos(const Game *g) {
    Point p = g->snake.head->pos;
    switch (g->snake.dir) {
        case DIR_UP:    p.y--; break;
        case DIR_DOWN:  p.y++; break;
        case DIR_LEFT:  p.x--; break;
        case DIR_RIGHT: p.x++; break;
    }
#if WRAP_AROUND_WALLS
    if (p.x < 1) p.x = g->width;
    if (p.x > g->width) p.x = 1;
    if (p.y < 1) p.y = g->height;
    if (p.y > g->height) p.y = 1;
#endif
    return p;
}

static bool is_wall_collision(const Game *g, Point p) {
#if WRAP_AROUND_WALLS
    (void)g; (void)p;
    return false;
#else
    return p.x < 1 || p.x > g->width || p.y < 1 || p.y > g->height;
#endif
}


static bool game_step(Game *g) {
    if (g->paused || g->game_over) return true;

    Point newHead = next_head_pos(g);

    if (is_wall_collision(g, newHead)) {
        g->game_over = true;
        return false;
    }

    
    bool eating = (newHead.x == g->food.pos.x && newHead.y == g->food.pos.y);

    for (SnakeSegment *cur = g->snake.head; cur; cur = cur->next) {
        bool isTail = (cur == g->snake.tail);
        if (isTail && !eating) continue; 
        if (cur->pos.x == newHead.x && cur->pos.y == newHead.y) {
            g->game_over = true;
            return false;
        }
    }

    snake_push_head(&g->snake, newHead);

    if (eating) {
        g->score += 10;
        if (g->delay_ms - SPEEDUP_STEP_MS >= MIN_DELAY_MS)
            g->delay_ms -= SPEEDUP_STEP_MS;
        spawn_food(g);
    } else {
        snake_pop_tail(&g->snake);
    }

    return true;
}

static void set_direction(Game *g, Direction d) {
    Direction cur = g->snake.dir;
    
    if ((cur == DIR_UP && d == DIR_DOWN) ||
        (cur == DIR_DOWN && d == DIR_UP) ||
        (cur == DIR_LEFT && d == DIR_RIGHT) ||
        (cur == DIR_RIGHT && d == DIR_LEFT)) {
        return;
    }
    g->snake.dir = d;
}



#define SX(x) ((x) * 2)

static void draw_border(const Game *g) {
    int rightCol = SX(g->width + 1);
    for (int x = 0; x <= rightCol; x++) {
        mvaddch(0, x, ACS_HLINE);
        mvaddch(g->height + 1, x, ACS_HLINE);
    }
    for (int y = 0; y <= g->height + 1; y++) {
        mvaddch(y, 0, ACS_VLINE);
        mvaddch(y, rightCol, ACS_VLINE);
    }
    mvaddch(0, 0, ACS_ULCORNER);
    mvaddch(0, rightCol, ACS_URCORNER);
    mvaddch(g->height + 1, 0, ACS_LLCORNER);
    mvaddch(g->height + 1, rightCol, ACS_LRCORNER);
}

static void draw_game(const Game *g) {
    erase();
    draw_border(g);

    
    mvprintw(g->food.pos.y, SX(g->food.pos.x), "%s", GLYPH_APPLE);

    
    for (SnakeSegment *cur = g->snake.head->next; cur; cur = cur->next) {
        mvprintw(cur->pos.y, SX(cur->pos.x), "%s", GLYPH_BODY);
    }
    
    mvprintw(g->snake.head->pos.y, SX(g->snake.head->pos.x), "%s",
              g->game_over ? GLYPH_HEAD_DEAD : GLYPH_HEAD);

    
    mvprintw(g->height + 2, 0, "Score: %d   Length: %d   Speed: %dms",
             g->score, g->snake.length, g->delay_ms);
    if (g->paused && !g->game_over) {
        mvprintw(g->height + 3, 0, "-- PAUSED (press P to resume) --");
    }
    if (g->game_over) {
        mvprintw(g->height + 3, 0,
                 "GAME OVER! Final score: %d. Press R to restart or Q to quit.",
                 g->score);
    } else {
        mvprintw(g->height + 3, 0,
                 "Controls: WASD/Arrows move | P pause | Q quit           ");
    }
    refresh();
}



static void handle_input(Game *g, int ch) {
    switch (ch) {
        case 'w': case 'W': case KEY_UP:    set_direction(g, DIR_UP);    break;
        case 's': case 'S': case KEY_DOWN:  set_direction(g, DIR_DOWN);  break;
        case 'a': case 'A': case KEY_LEFT:  set_direction(g, DIR_LEFT);  break;
        case 'd': case 'D': case KEY_RIGHT: set_direction(g, DIR_RIGHT); break;
        case 'p': case 'P':
            if (!g->game_over) g->paused = !g->paused;
            break;
        default:
            break;
    }
}



int main(void) {
    setlocale(LC_ALL, "");         
    srand((unsigned)time(NULL));

    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);         

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    
    int width  = (cols - 2) / 2;
    int height = rows - 5;
    if (width < 10) width = 10;
    if (height < 10) height = 10;

    Game game;
    game_init(&game, width, height);

    bool running = true;
    while (running) {
        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            running = false;
            break;
        }
        if (game.game_over && (ch == 'r' || ch == 'R')) {
            game_destroy(&game);
            game_init(&game, width, height);
        } else {
            handle_input(&game, ch);
        }

        game_step(&game);
        draw_game(&game);

        napms(game.delay_ms);
    }

    game_destroy(&game);
    endwin();
    printf("Thanks for playing! Final score: %d\n", game.score);
    return 0;
}
