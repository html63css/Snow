#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>
#include <Windows.h>

#define FLOOR 800
#define WIDTH 1024
#define HEIGHT (FLOOR+32)
#define LIFE 50
#define SPEED_PLAYER 8
#define SPEED_SNOW 1
#define TRIANGLE 80
using namespace sf;

int map[HEIGHT - 32 + 1][WIDTH + 1] = { 0 };
class PLAYER 
{
public:
    float x;
    Sprite sprite;

    PLAYER(Texture& image)
    {
        x = (WIDTH - 250) / 2;
        sprite.setTexture(image);
        sprite.scale(.5f,.5f);
    }

    void update()
    {
        sprite.setPosition(x,0);
    }
};
class Snowflake
{
public:
    int x, y;
    int timeLife;
    bool isEnd;
    CircleShape cirkl;
        int inert;

    Snowflake(Vector2f pos)
    {
        inert = 0;
        int x1 = pos.x;
        int x2 = x1 + 249;
        x = rand() % int(x2 - x1 + 1) + x1;
        y = 107;
        timeLife = LIFE;
        isEnd = 0;
        cirkl = CircleShape(1);
        cirkl.setPosition(x, y);
    }

    bool update()
    {
        if (y == FLOOR)
            isEnd = 1;

        if (!isEnd)
        {
            if (x > 0)
            {
                if (map[y + SPEED_SNOW][x] == 0)
                {
                    if (map[y][x] != 2) map[y][x] = 0;
                    y += SPEED_SNOW;
                    if (x > 0) map[y][x] = 1;
                }
                else if (map[y + SPEED_SNOW][x] == 2)
                {
                    if (map[y][x] != 2) map[y][x] = 0;
                    if (map[y + 2][x + 1] == 2)
                    {
                        x += 1;
                    }
                    else
                    {
                         x -= 1;
                    }
                    y += SPEED_SNOW;
                    if (x > 0) map[y][x] = 1;
                    --timeLife;
                }
                else
                {
                    if (map[y][x] != 2) map[y][x] = 0;
                    y += SPEED_SNOW;
                    for (; map[y][x] != 0; --y);
                    map[y][x] = 1;
                }
            }
            else
            {
                return 0;
            }
        }
        else
        {
            --timeLife;
        }

        if (!timeLife)
        {
            map[y][x] = 0;
            return 0;
        }
        if (map[y][x] == 2) printf("A");
        cirkl.setPosition(x, y);
        return 1;
    }
};
class Block
{
public:
    float x, y;
    ConvexShape convex;
    Block(int x, int y, Texture& image)
    {
        convex.setTexture(&image);
        convex.setFillColor(Color::Red);
        convex.setPointCount(3);
        convex.setPoint(0, Vector2f(x + TRIANGLE / 2, y));
        convex.setPoint(1, Vector2f(x + 2, y + (TRIANGLE / 2)));
        convex.setPoint(2, Vector2f(x + TRIANGLE, y + (TRIANGLE / 2)));
        map[y][x + TRIANGLE / 2] = 2;
        for (int i = 1 ; i <= TRIANGLE /2; ++i)
        {
            map[y + i][x + TRIANGLE/2 + i] = 2;
            map[y + i][x + TRIANGLE/2 - i] = 2;
        }
    }
};
int main()
{
    //Убираем консольную строку при запуске
    HWND Hide;
    AllocConsole();
    Hide = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(Hide, 0);

    //Открываем окно
	RenderWindow window(VideoMode(WIDTH, HEIGHT), L"Снежок", Style::Close);
    window.setPosition(Vector2i(0,0));
    window.setFramerateLimit(40);
    
    //Загружаем иконку
    Image icon;
    if (!icon.loadFromFile("image/icon.png"))
        icon.loadFromFile("../image/icon.png");
    window.setIcon(32, 32, icon.getPixelsPtr());

    //Загружаем картинку облака
    Texture texture_cloud;
    if (!texture_cloud.loadFromFile("image/cloud.png"))
        texture_cloud.loadFromFile("../image/cloud.png");
    PLAYER player(texture_cloud);

    //Загружаем задний фон
    Texture texture_background;
    Sprite sprite_background;
    if (!texture_background.loadFromFile("image/background.png"))
        texture_background.loadFromFile("../image/background.png");
    sprite_background.setTexture(texture_background);
    sprite_background.setPosition(0, 0);

    //Загружаем картинку для паузы
    /*Texture texture_pause;
    Sprite sprite_pause;
    if (!texture_pause.loadFromFile("image/pause.png"))
        texture_pause.loadFromFile("../image/pause.png");
    sprite_pause.setTexture(texture_pause);
    texture_pause.setSmooth(true);
    sprite_pause.scale(0.5f, 0.5f);
    sprite_pause.setPosition((WIDTH - 600) / 2, (HEIGHT - 315)/ 2 );*/

    //Загружаем картинку для пола
    Texture texture_floor;
    Sprite sprite_floor;
    if (!texture_floor.loadFromFile("image/floor.png"))
        texture_floor.loadFromFile("../image/floor.png");
    sprite_floor.setTexture(texture_floor);
    sprite_floor.setScale(.3f, .3f);

    //Переменные для работы
    Event event;
    std::list <Snowflake*> snow;
    bool tab = 0;
    bool STOP = 0;

    //Создаём блоки
    Texture texture_block;
    if (!texture_block.loadFromFile("image/floor.png"))
        texture_block.loadFromFile("../image/floor.png");
    std::list <Block*> blocks;
    for (int i = 0; i < 2; ++i)
    {
        int rand_x = rand() % (WIDTH - 50 + 1);
        int rand_y = rand() % (HEIGHT - 196 - 32 + 1) + 32;
        blocks.push_back(new Block(rand_x,rand_y,texture_block));
    }

    //Основной цикл
    while (window.isOpen())
    {

        //Обработка событий
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case Event::Closed:
                window.close();
                break;
            case Event::LostFocus:
                STOP = 1;
                break;
            case Event::GainedFocus:
                STOP = 0;
                break;
            }
            //Закрыть на Escape
            if (Keyboard::isKeyPressed(Keyboard::Escape))
            {
                window.close();
            }
            //Движение влево
            if (Keyboard::isKeyPressed(Keyboard::Left))
            {
                player.x += player.x <= 0 ? 0 : -SPEED_PLAYER;
            }
            //Движение вправо
            if (Keyboard::isKeyPressed(Keyboard::Right))
            {
                player.x += player.x >= (WIDTH - 250) ? 0 : SPEED_PLAYER;
            }
            //Пустить снежинку
            if (Keyboard::isKeyPressed(Keyboard::Space))
            {
                snow.push_back(new Snowflake(player.sprite.getPosition()));
            }
            //Режим снегопада (вкл)
            if (Keyboard::isKeyPressed(Keyboard::Tab))
            {
                tab = 1;
            }
            //Режим снегопада (выкл)
            if (Keyboard::isKeyPressed(Keyboard::Tab) && Keyboard::isKeyPressed(Keyboard::LShift))
            {
                tab = 0;
            }
        }

        player.update();

        //Режим снегопада (логика)
        if (tab == 1)
        {
            snow.push_back(new Snowflake(player.sprite.getPosition()));
        }

        //Падение снега и проверка на таяние
        for (auto i = snow.begin(); i != snow.end();)
        {
            if (!((*i)->update()))
            {
                i = snow.erase(i);
                if (snow.size() == 0) break;
            }
            else
                ++i;
        }

        //Цвет заднего фона
        window.clear();
        sprite_background.setColor(Color(158, 158, 158));
        if (STOP) 
            sprite_background.setColor(Color(100, 100, 100)); 
        window.draw(sprite_background);
        
        //Прорисовка пола
        for (float i = 0; i < 1024; i += 32)
        {
            sprite_floor.setPosition(i, FLOOR);
            window.draw(sprite_floor);
        }

        //Прорисовка снега, облака, паузы
        for (auto element : snow)
            window.draw(element->cirkl);
        window.draw(player.sprite);
        //if (STOP)
          //  window.draw(sprite_pause);

        //Прорисовка блоков
        for (auto block : blocks)
            window.draw(block->convex);

        window.display();
    }
    return 0;
}