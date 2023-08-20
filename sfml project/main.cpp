#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <SFML/Audio.hpp>
#include <sstream>

using namespace sf;
using namespace std;

//decleration of global variables
RenderWindow win(VideoMode(1920, 1080), "Dungeon Escape", Style::Default);
Sprite background;
Texture background_tex;
bool grounded = false;
Event ev;
Font font;
Text menu[3];
SoundBuffer background_buffer, menu_buffer[2];
Sound menu_sounds[2], background_music;
bool keys[5], gate_pass[5];
int health1 = 3, health2 = 3, indicator = 1, coins_cntr = 0;
float music_vol = 0, sfx_vol = 0;



struct FockingBoss {
private:
    Texture boss_tex, laser_tex, health_bar_tex[15];
    Sprite laser[2], boss, health_bar;

    int image_count = 10, ctrX = 0, ctrY = 0, tex_indicator = -1, health = 14; //0->idle , 1->charging , 2->dead , 3->dead2

    RectangleShape laser_col[2];

    IntRect rect, rect_laser;

    Sound laser_s, boss_dam, boss_death, lava;
    SoundBuffer l_s, b_d, b_dth, lava_buf;

    //boss delays
    long long frame_time = 3, time_elapsed = 0, cool_down = 120, Elapsed_cool = 0;


    //laser delays
    int laser_change = 1, laser_switch = 3, laser_cnt = 0, laser_frame = 1, end_frames = 0, dam_delay = 30, damage_ctr_P1 = 30;
    int damage_ctr_P2 = 30, death_delay = 0;
    bool got_p1, got_p2;


    bool moved = 0, death_S = 0;

    void idle() {
        image_count = 10, tex_indicator = 0;
        ctrY = 6, ctrX = 0, frame_time = 3, time_elapsed = 0, cool_down = 180, Elapsed_cool = 0;
        laser_frame = 1;
        laser[0].setPosition(2000, 2000);
        laser[1].setPosition(2000, 2000);
        laser_col[0].setPosition(laser[0].getPosition());
        laser_col[1].setPosition(laser[1].getPosition());
        if (moved) boss.move(0, 40);
        moved = 0;
    }

    void charging() {
        image_count = 6, tex_indicator = 1;
        ctrY = 1, ctrX = 1, frame_time = 10;
        laser_change = 1;
        got_p1 = 0, got_p2 = 0;
        end_frames = 0;
        damage_ctr_P1 = dam_delay;
        damage_ctr_P2 = dam_delay;
        boss.move(0, -40);
        moved = 1;
    }

    void dead() {
        time_elapsed = 0;
        image_count = 10, tex_indicator = 2;
        ctrY = 7, ctrX = 0, frame_time = 2;
        laser[0].setPosition(2000, 2000);
        laser[1].setPosition(2000, 2000);
        laser_col[0].setPosition(laser[0].getPosition());
        laser_col[1].setPosition(laser[1].getPosition());
        if (moved) boss.move(0, 40);
        moved = 0;
        lava.play();
    }
    void dead2() {

        image_count = 4, tex_indicator = 3;
        ctrX = 0, ctrY = 8;
    }

public:
    bool dead1 = 0, dead3 = 0;

    FockingBoss(Vector2f boss_scale, Vector2f boss_pos) {
        boss_tex.loadFromFile("Character_sheet.png");
        laser_tex.loadFromFile("laser_sheet.png");
        boss.setTexture(boss_tex);
        laser[0].setTexture(laser_tex);
        laser[1].setTexture(laser_tex);
        boss.setScale(boss_scale);
        boss.setPosition(boss_pos);
        laser[0].setScale(10, 4);
        laser[1].setScale(10, 4);


        rect.width = boss_tex.getSize().x / 10;
        rect.height = (boss_tex.getSize().y / 9) - 10;


        health_bar_tex[14].loadFromFile("GHealth1.png");
        health_bar_tex[13].loadFromFile("GHealth2.png");
        health_bar_tex[12].loadFromFile("GHealth3.png");
        health_bar_tex[11].loadFromFile("GHealth4.png");
        health_bar_tex[10].loadFromFile("GHealth5.png");
        health_bar_tex[9].loadFromFile("GHealth6.png");
        health_bar_tex[8].loadFromFile("GHealth7.png");
        health_bar_tex[7].loadFromFile("GHealth8.png");
        health_bar_tex[6].loadFromFile("GHealth9.png");
        health_bar_tex[5].loadFromFile("GHealth10.png");
        health_bar_tex[4].loadFromFile("GHealth11.png");
        health_bar_tex[3].loadFromFile("GHealth12.png");
        health_bar_tex[2].loadFromFile("GHealth13.png");
        health_bar_tex[1].loadFromFile("GHealth14.png");
        health_bar_tex[0].loadFromFile("GHealth15.png");

        health_bar.setTexture(health_bar_tex[health]);
        health_bar.setPosition(boss_pos.x + 200, boss_pos.y);
        rect_laser.width = (laser_tex.getSize().x / 14);
        rect_laser.height = laser_tex.getSize().y;
        laser[0].setTextureRect(rect_laser);
        laser[1].setTextureRect(rect_laser);

        laser_col[0].setSize(Vector2f(150, 1080));
        laser_col[1].setSize(Vector2f(150, 1080));
        laser_col[0].setFillColor(Color::Red);
        laser_col[1].setFillColor(Color::Red);


        l_s.loadFromFile("Laser.wav");
        b_d.loadFromFile("BossDamage.wav");
        b_dth.loadFromFile("BossDeath.wav");
        laser_s.setBuffer(l_s);
        boss_dam.setBuffer(b_d);
        boss_death.setBuffer(b_dth);
        lava_buf.loadFromFile("Lava sound.wav");
        lava.setBuffer(lava_buf);
    }

    void update(Sprite& bullet, Sprite& P1, Sprite& P2) {
        if (bullet.getPosition().x > 770 && bullet.getPosition().x < 800 && !dead1) { health--, bullet.setPosition(2000, 2000), boss_dam.play(), health_bar.setTexture(health_bar_tex[health]); }

        if (!health) dead1 = 1;

        health_bar.setTexture(health_bar_tex[health]);

        if (!dead1) {
            //idle
            if (cool_down != Elapsed_cool || (tex_indicator == 1 && laser_frame == 0 && ctrX != 7)) {
                Elapsed_cool++;
                if (tex_indicator != 0) idle();
                else {
                    if (time_elapsed == frame_time) ctrX++, time_elapsed = 0;
                    else time_elapsed++;
                    ctrX %= image_count;
                }
            }

            //charging
            else {
                if (tex_indicator != 1) charging();
                else {
                    if (time_elapsed == frame_time) {
                        time_elapsed = 0;
                        if (ctrX != image_count - 1) ctrX++;
                    }
                    else time_elapsed++;
                }
            }

            //laser
            if (tex_indicator == 1 && ctrX == image_count - 1) {
                if (!got_p1) laser[0].setPosition(P1.getPosition().x - 350, 50), got_p1 = 1;
                if (!got_p2) laser[1].setPosition(P2.getPosition().x - 350, 50), got_p2 = 1;
                if (laser_switch == laser_cnt) {
                    laser_cnt = 0;
                    laser_frame += laser_change;
                    if (laser_frame == 13 || (end_frames != 3 && laser_frame == 10 && end_frames != 0)) laser_change *= -1;
                    if (laser_frame == 10) end_frames++;
                }
                else laser_cnt++;

                rect_laser.top = 0;
                rect_laser.left = (laser_frame * rect_laser.width);
                laser[0].setTextureRect(rect_laser);
                laser[1].setTextureRect(rect_laser);

                if (laser_frame == 8) {
                    laser_col[0].setPosition(laser[0].getPosition().x + 340, 50);
                    laser_col[1].setPosition(laser[1].getPosition().x + 340, 50);
                }
                else if (laser_change < 0 && laser_frame == 7) {
                    laser_col[0].setPosition(2000, 2000);
                    laser_col[1].setPosition(2000, 2000);
                }

                if (laser_frame == 8 && laser_change == 1) laser_s.play();

                //collision with players
                if (laser_col[0].getGlobalBounds().intersects(P1.getGlobalBounds()) || laser_col[1].getGlobalBounds().intersects(P1.getGlobalBounds())) {
                    if (damage_ctr_P1 == dam_delay)health1 -= 2, health1 = max(0, health1), damage_ctr_P1 = 0;
                    else damage_ctr_P1++;
                }

                else damage_ctr_P1 = dam_delay;


                if ((laser_col[0].getGlobalBounds().intersects(P2.getGlobalBounds()) || laser_col[1].getGlobalBounds().intersects(P2.getGlobalBounds()))) {
                    if (damage_ctr_P2 == dam_delay) health2 -= 2, health2 = max(0, health2), damage_ctr_P2 = 0;
                    else damage_ctr_P2++;
                }
                else damage_ctr_P2 = dam_delay;
            }

            rect.left = rect.width * ctrX;
            rect.top = rect.height * ctrY;
            boss.setTextureRect(rect);

        }
        //dead
        else {
            health_bar.setPosition(2000, 2000);
            if (tex_indicator == 0 || tex_indicator == 1) dead();

            else if (tex_indicator == 2 && ctrX != image_count - 1) {

                if (time_elapsed == frame_time) ctrX++, time_elapsed = 0;
                else time_elapsed++;

            }

            else {
                if (boss.getPosition().y < 420) boss.move(0, 8);
                else if (!death_S) boss_death.play(), death_S++;
                if (tex_indicator != 3 || boss.getPosition().y < 420)
                    dead2();
                else {
                    if (ctrX != image_count - 1) {
                        if (time_elapsed == frame_time) ctrX++, time_elapsed = 0;
                        else time_elapsed++;
                    }
                    else dead3 = 1;
                }
            }



            rect.left = rect.width * ctrX;
            rect.top = rect.height * ctrY;
            boss.setTextureRect(rect);
        }
    }

    void draw() {
        win.draw(boss);
        win.draw(laser[1]);
        win.draw(laser[0]);
        win.draw(health_bar);
    }
};

struct colright
{
private:
    RectangleShape Rect;

public:

    colright(Vector2f position, Vector2f size)
    {
        Rect.setFillColor(Color::Red);
        Rect.setSize(size);
        Rect.setPosition(position);

    }

    void rotation(float n) { Rect.rotate(n); }

    void update(Sprite& sPlayer1, Sprite& sPlayer2, float& velocityY1, float& velocityY2, bool grounded1, bool grounded2, bool mov = 0, Vector2f pos = { 0 , 0 })
    {
        if ((Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::A)) || (Keyboard::isKeyPressed(Keyboard::Key::A) && (!grounded1 || mov)))
        {
            if (sPlayer1.getGlobalBounds().intersects(Rect.getGlobalBounds()))
            {
                sPlayer1.move(10, 0);
                velocityY1 = -10;

            }
        }
        if ((Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Left)) || (Keyboard::isKeyPressed(Keyboard::Key::Left) && (!grounded2 || mov))) {

            if (sPlayer2.getGlobalBounds().intersects(Rect.getGlobalBounds()))
            {
                sPlayer2.move(10, 0);
                velocityY2 = -10;

            }
        }
        if (mov) {
            Rect.setPosition(pos);
        }
    }
    void draw()
    {
        win.draw(Rect);
    }
};


struct colleft
{
private:
    RectangleShape Rect;

public:
    colleft(Vector2f position, Vector2f size)
    {
        Rect.setFillColor(Color::Red);
        Rect.setSize(size);
        Rect.setPosition(position);


    }

    void rotation(float n) { Rect.rotate(n); }

    void update(Sprite& sPlayer1, Sprite& sPlayer2, float& velocityY1, float& velocityY2, bool grounded1, bool grounded2, bool mov = 0, Vector2f pos = { 0 , 0 })
    {
        if ((Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::D)) || ((Keyboard::isKeyPressed(Keyboard::Key::D) && (!grounded1 || mov))))
        {


            if (sPlayer1.getGlobalBounds().intersects(Rect.getGlobalBounds()))
            {
                sPlayer1.move(-10, 0);
                velocityY1 = -10;

            }


        }
        if ((Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Right)) || ((Keyboard::isKeyPressed(Keyboard::Key::Right) && (!grounded2 || mov)))) {

            if (sPlayer2.getGlobalBounds().intersects(Rect.getGlobalBounds()))
            {
                sPlayer2.move(-10, 0);
                velocityY2 = -10;

            }
        }
        if (mov) {
            Rect.setPosition(pos);
        }
    }

    void draw()
    {
        win.draw(Rect);
    }

};


struct collup {

private:
    RectangleShape rect;
    float velocity;

public:
    collup(Vector2f pos, Vector2f size, float velocity = 0) {
        this->velocity = velocity;
        rect.setPosition(pos);
        rect.setSize(size);
        rect.setFillColor(sf::Color::Blue);
    }

    void update(bool& grounded, bool& grounded2, Sprite& p1, Sprite& p2, bool mov = 0, pair<float, float> range = { 0 , 0 }) {
        if (rect.getGlobalBounds().intersects(p1.getGlobalBounds())) {
            grounded = true;
            if (mov && !Keyboard::isKeyPressed(Keyboard::W)) p1.setPosition(p1.getPosition().x, rect.getPosition().y - 75);
        }

        if (rect.getGlobalBounds().intersects(p2.getGlobalBounds())) {
            grounded2 = true;
            if (mov && !Keyboard::isKeyPressed(Keyboard::Up)) p2.setPosition(p2.getPosition().x, rect.getPosition().y - 75);
        }
        if (mov) {
            if (rect.getPosition().y < range.first || rect.getPosition().y > range.second) velocity *= -1;
            rect.move(0, velocity);
        }

    }
    Vector2f getpos() {
        return rect.getPosition();
    }

    void setpos(Vector2f pos) {
        rect.setPosition(pos);
    }

    bool collision(Sprite& p) {
        return rect.getGlobalBounds().intersects(p.getGlobalBounds());
    }


    void draw() {
        win.draw(rect);
    }
};


struct collup2 {

private:
    RectangleShape rect;
    float velocityY, velocityX;

public:
    collup2(Vector2f pos, Vector2f size, float velocityX, float velocityY = 0) {
        this->velocityY = velocityY, this->velocityX = velocityX;
        rect.setPosition(pos);
        rect.setSize(size);
        rect.setFillColor(sf::Color::Blue);
    }

    void update(bool& grounded, bool& grounded2, Sprite& p1, Sprite& p2, pair<pair<float, float>, pair<float, float>> range) {
        if (rect.getGlobalBounds().intersects(p1.getGlobalBounds())) {
            grounded = true;
            if (!Keyboard::isKeyPressed(Keyboard::W)) p1.setPosition(p1.getPosition().x, rect.getPosition().y - 75);
            if (Keyboard::isKeyPressed(Keyboard::A)) p1.setPosition(p1.getPosition().x - 5 + velocityX, rect.getPosition().y - 75);
            if (Keyboard::isKeyPressed(Keyboard::D)) p1.setPosition(p1.getPosition().x + 5 + velocityX, rect.getPosition().y - 75);
            if (!Keyboard::isKeyPressed(Keyboard::A) && !Keyboard::isKeyPressed(Keyboard::D)) p1.setPosition(p1.getPosition().x + velocityX, rect.getPosition().y - 75);
        }

        if (rect.getGlobalBounds().intersects(p2.getGlobalBounds())) {
            grounded2 = true;
            if (!Keyboard::isKeyPressed(Keyboard::Up)) p2.setPosition(p2.getPosition().x, rect.getPosition().y - 75);
            if (Keyboard::isKeyPressed(Keyboard::Left)) p2.setPosition(p2.getPosition().x - 5 + velocityX, rect.getPosition().y - 75);
            if (Keyboard::isKeyPressed(Keyboard::Right)) p2.setPosition(p2.getPosition().x + 5 + velocityX, rect.getPosition().y - 75);
            if (!Keyboard::isKeyPressed(Keyboard::Left) && !Keyboard::isKeyPressed(Keyboard::Right)) p2.setPosition(p2.getPosition().x + velocityX, rect.getPosition().y - 75);
        }
        if (rect.getPosition().y < range.second.first || rect.getPosition().y > range.second.second) velocityY *= -1;
        if (rect.getPosition().x < range.first.first || rect.getPosition().x > range.first.second) velocityX *= -1;
        rect.move(velocityX, velocityY);
    }
    Vector2f getpos() {
        return rect.getPosition();
    }

    bool collision(Sprite& p) {
        return rect.getGlobalBounds().intersects(p.getGlobalBounds());
    }


    void draw() {
        win.draw(rect);
    }
};


struct colldown {

private:
    RectangleShape rect;
public:
    colldown(Vector2f pos, Vector2f size) {
        rect.setPosition(pos);
        rect.setSize(size);
        rect.setFillColor(sf::Color::Blue);
    }
    //movement indicator // position of the lower rect relative to upper rect
    void update(float& v1, float& v2, Sprite& p1, Sprite& p2, bool mov = 0, Vector2f pos = { 0 , 0 }) {

        if (rect.getGlobalBounds().intersects(p1.getGlobalBounds())) {
            v1 -= 5;
        }

        if (rect.getGlobalBounds().intersects(p2.getGlobalBounds())) {
            v2 -= 5;
        }
        if (mov) {
            rect.setPosition(pos);
        }
    }

    void draw() {
        win.draw(rect);
    }

};


struct kaboom
{
private:
    int direction = 0;
    int cntr = 0;
    Texture cannon, bullet;
    Sprite canon;
    RectangleShape rectt;
    Sound Canongo, damage;
    SoundBuffer canonshot, damageplayer;

    int cntr1 = 0, cntr2 = 0;


public:
    Sprite bulet;
    kaboom(Vector2f position, int direction)
    {
        this->direction = direction;


        cannon.loadFromFile("canon.png");
        canon.setTexture(cannon);
        canon.setPosition(position);
        canon.setScale(1.5, 1.5);

        bullet.loadFromFile("bullet.png");
        bulet.setTexture(bullet);
        bulet.setPosition(position.x + (29 * direction), position.y + 11);
        bulet.setScale(3, 3);


        rectt.setPosition(bulet.getPosition());
        rectt.setSize(Vector2f(20, 20));
        rectt.setFillColor(Color::Blue);
        canonshot.loadFromFile("Canon Sound.wav");
        Canongo.setBuffer(canonshot);

        damageplayer.loadFromFile("Damage Taken.wav");
        damage.setBuffer(damageplayer);

        if (direction == -1) {
            canon.setTexture(cannon);
            bullet.loadFromFile("bulletleft.png");
            bulet.setTexture(bullet);
        }

        else if (direction == 1)
        {

            cannon.loadFromFile("canonL.png");
            canon.setTexture(cannon);
        }

    }
    void update(RectangleShape b3, RectangleShape b2, Sprite& sPlayer1, Sprite& sPlayer2)
    {
        Canongo.setVolume((abs(sfx_vol - 970) / 1260) * 50);
        damage.setVolume((abs(sfx_vol - 970) / 1260) * 50);
        if ((bulet.getPosition().x > 1920 || bulet.getPosition().x < 0) && direction == 1)
        {
            bulet.setPosition(canon.getPosition().x + 150, canon.getPosition().y);
            Canongo.play();
            cntr1 = 0; cntr2 = 0;
        }

        if ((bulet.getPosition().x > 1920 || bulet.getPosition().x < 0) && direction == -1)
        {
            bulet.setPosition(canon.getPosition().x - 150, canon.getPosition().y);
            Canongo.play();
            cntr1 = 0; cntr2 = 0;
        }

        if (!bulet.getGlobalBounds().intersects(b3.getGlobalBounds()))
        {

            bulet.move(10 * direction, 0 * direction);
        }
        else if (bulet.getGlobalBounds().intersects(b3.getGlobalBounds()))

        {


            if (direction = -1)
            {
                if (bulet.getGlobalBounds().intersects(b3.getGlobalBounds()))
                {

                    bulet.setPosition(3000, 3000);
                    cntr++;

                    if (cntr % 30 == 0)
                    {
                        cntr = 0;
                        bulet.setPosition(canon.getPosition().x + 150, canon.getPosition().y);
                    }


                }
                rectt.setPosition(bulet.getPosition().x - 20, bulet.getPosition().y);

            }

            if (direction = 1)
            {
                if (bulet.getGlobalBounds().intersects(b2.getGlobalBounds()))
                {
                    cntr++;
                    cntr2 = 0;
                    cntr1 = 0;
                    if (cntr % 30 == 0)
                    {
                        bulet.setPosition(canon.getPosition());
                    }


                }


            }



        }
        if (direction == -1)  rectt.setPosition(bulet.getPosition().x + 20, bulet.getPosition().y);
        else if (direction == 1)     rectt.setPosition(bulet.getPosition().x - 20, bulet.getPosition().y);

        if (rectt.getGlobalBounds().intersects(sPlayer1.getGlobalBounds())) {

            if (cntr1 == 0) {
                damage.play();
                health1--;
                cntr1 = 1;;
            }

        }
        if (rectt.getGlobalBounds().intersects(sPlayer2.getGlobalBounds())) {

            if (cntr2 == 0) {
                damage.play();
                health2--;
                cntr2 = 1;
            }

        }

    }

    void draw()
    {
        win.draw(bulet);
        win.draw(canon);

    }
};


struct transition {
private:
    Texture Level[5];
    Sprite level;
    bool fade = 0; // 0 ----> lighten the color 1 -----> fade to black
    RectangleShape screen;
    int cntr = 0;
public:
    transition(bool fade) {
        this->fade = fade;
        Level[0].loadFromFile("Level1.png");
        Level[1].loadFromFile("Level2.png");
        Level[2].loadFromFile("Level3.png");
        Level[3].loadFromFile("Level4.png");
        Level[4].loadFromFile("Final_Boss.png");
        if (!fade) {
            cntr = 254;
        }
        screen.setFillColor(Color(0, 0, 0, cntr));
        screen.setSize(Vector2f(1920, 1080));
    }

    bool retrn() {
        return cntr == 254;
    }

    void update(int Level) {
        level.setTexture(this->Level[Level - 1]);
        if (fade) {
            if (cntr <= 254) {
                cntr += 2;
                screen.setFillColor(Color(0, 0, 0, cntr));
            }
        }
        if (!fade) {
            if (cntr > 0) {
                cntr -= 2;
                screen.setFillColor(Color(0, 0, 0, cntr));
            }

        }

    }


    void draw() {
        if (!fade && cntr > 50) {
            win.draw(level);

        }
        win.draw(screen);
    }
};


struct enemy {
private:
    Texture tex_walk, tex_attack, tex_dead, tex_attak_left;
    Sprite body;
    RectangleShape range;
    IntRect rect;
    Sound s[3]; SoundBuffer buffer[3]; // 1- attack 2- death
    int switch_time1 = 2, total_time1 = 1, dir_switch = 50, cur_time = 1;
    int ctrX = 0, speed = 2;
    int ctrY = 0;
    int moved = 0, movedX = 0;
    bool killed = 0;
    int imageCount;
    int tex_indicator; // walk = 1 , attack = 2(player1) , attack = 3(player2) , dead = 4
    int direction = 1; // right = 1 , left = -1
    //attack texture
    void attack() {
        body.setTexture(tex_attack);
        if (direction == 1)
            body.move(10, -20.f);
        else { body.move(-90.f, -20.f); movedX = 1; }
        switch_time1 = 2;
        ctrX = 0;
        moved = 1;
        rect.height = tex_attack.getSize().y, rect.left = 5, rect.top = 0, rect.width = tex_attack.getSize().x / 18;
        body.setTextureRect(rect), imageCount = 18;
    }
    //walk texture
    void walk() {
        body.setTexture(tex_walk);
        if (moved) body.move(0, 20.f);
        if (movedX) body.move(90.f, 0);
        moved = 0, movedX = 0;
        ctrX = 0;
        tex_indicator = 1;
        rect.height = 33, rect.left = 0, rect.top = 0, rect.width = tex_walk.getSize().x / 13;
        body.setTextureRect(rect), imageCount = 13;
    }
    //dead texture
    void dead() {
        body.setTexture(tex_dead);
        if (moved) body.move(0, 20.f);
        if (movedX) body.move(90.f, 0);
        tex_indicator = 4;
        killed = 1;
        ctrX = 0, total_time1 = 1, cur_time = 1;
        rect.height = tex_dead.getSize().y, rect.left = 0, rect.top = 0, rect.width = tex_dead.getSize().x / 15;
        body.setTextureRect(rect), imageCount = 15;
    }
public:
    //constructor
    enemy(Vector2f pos, Vector2f size, Vector2f scale = { 3 , 4 }) {
        tex_walk.loadFromFile("Skeleton Walk.png");
        tex_attack.loadFromFile("Skeleton Attack.png");
        tex_dead.loadFromFile("Skeleton dead.png");

        body.setPosition(pos);
        body.setScale(scale);

        //setting the range of sight
        range.setSize(size);
        tex_indicator = 1;
        range.setPosition(pos);
        buffer[0].loadFromFile("Skeleton Attack.wav"), buffer[1].loadFromFile("Skeleton Death.wav"), buffer[2].loadFromFile("Damage Taken.wav");
        s[0].setBuffer(buffer[0]), s[1].setBuffer(buffer[1]), s[2].setBuffer(buffer[2]);
        walk();
    }

    void update(Vector2f player1Pos, Vector2f player2Pos) {

        s[0].setVolume((abs(sfx_vol - 970) / 1260) * 100);
        s[1].setVolume((abs(sfx_vol - 970) / 1260) * 100);
        s[2].setVolume((abs(sfx_vol - 970) / 1260) * 50);

        //movement and animation update
        //attacking player
        if (killed) {
            if (total_time1 % switch_time1 > 0) total_time1++;
            else if (ctrX == imageCount - 1);
            else ctrX++, total_time1 = 1;
            if (ctrX == 3) s[1].play();
        }
        else if ((tex_indicator == 2 || tex_indicator == 3) && ctrX)
        {
            if (total_time1 % switch_time1 > 0) total_time1++;
            else ctrX++, total_time1 = 1;
            if (ctrX == imageCount && tex_indicator == 3 && abs(player2Pos.x - body.getPosition().x) < ((direction == 1) ? 80 : 120)) health2--, s[2].play();
            if (ctrX == imageCount && tex_indicator == 2 && abs(player1Pos.x - body.getPosition().x) < ((direction == 1) ? 80 : 120)) health1--, s[2].play();
            if (ctrX == 4) s[0].play();
            goto animation;
        }
        else if (abs(player1Pos.x - body.getPosition().x) < ((direction == 1) ? 80 : 120) &&
            (range.getGlobalBounds().contains(player1Pos.x, player1Pos.y))) {
            if (tex_indicator == 2) {
                if (total_time1 % switch_time1 > 0) total_time1++;
                else ctrX++, total_time1 = 1;
            }
            else {
                tex_indicator = 2;
                attack();
            }
            if (player1Pos.x < body.getPosition().x) direction = -1;
            else direction = 1;
        }
        else if (abs(player2Pos.x - body.getPosition().x) < ((direction == 1) ? 80 : 120) &&
            range.getGlobalBounds().contains(player2Pos.x, player2Pos.y)) {
            if (tex_indicator == 3) {
                if (total_time1 % switch_time1 > 0) total_time1++;
                else ctrX++, total_time1 = 1;
            }
            else {
                tex_indicator = 3;
                attack();
            }
            if (player2Pos.x < body.getPosition().x) direction = -1;
            else direction = 1;
        }
        // death of enemy
        //movement
        else
        {
            if (tex_indicator == 1 && cur_time == 1) {
                body.move(speed * direction, 0);
                if (total_time1 % switch_time1 > 0) total_time1++;
                else ctrX++, total_time1 = 1;
            }
            else if (tex_indicator != 1) {
                cur_time = 1;
                walk();
                ctrX = 0, ctrY = 0;
            }
            //chasing player1
            if (range.getGlobalBounds().contains(player1Pos.x, player1Pos.y)) {
                cur_time = 1;
                speed = 3;
                switch_time1 = 1;
                if (player1Pos.x < body.getPosition().x) direction = -1;
                else direction = 1;
            }
            //chasing player2
            else if (range.getGlobalBounds().contains(player2Pos.x, player2Pos.y)) {
                cur_time = 1;
                speed = 3;
                switch_time1 = 1;
                if (player2Pos.x < body.getPosition().x) direction = -1;
                else direction = 1;
            }
            else if (!range.getGlobalBounds().contains(body.getPosition().x, body.getPosition().y)) {
                if (dir_switch == cur_time)
                    direction *= -1, cur_time = 1;
                else if (cur_time % dir_switch > 0)
                    cur_time++, ctrX = 13;
            }
            if (!(range.getGlobalBounds().contains(player1Pos.x, player1Pos.y)) &&
                !(range.getGlobalBounds().contains(player2Pos.x, player2Pos.y))) speed = 2, switch_time1 = 2;

            //movement



        }
    animation:;
        if (direction == 1) {
            if (tex_indicator != 4)
                ctrX %= imageCount;
            rect.left = ctrX * rect.width;
            rect.width = abs(rect.width);
            body.setTextureRect(rect);
        }
        else {
            if (tex_indicator != 4)
                ctrX %= imageCount;
            rect.left = (ctrX + 1) * abs(rect.width);
            rect.width = -abs(rect.width);
            body.setTextureRect(rect);
        }
    }

    //return 1 if intersect player1 , return 2 for player 2 , 0 if there is no collision
    int collisionCheck(Sprite& player1, Sprite& player2) {
        if (body.getGlobalBounds().intersects(player1.getGlobalBounds()) && player1.getPosition().y < body.getPosition().y)
        {
            kill();
            return 1;
        }
        else if (body.getGlobalBounds().intersects(player2.getGlobalBounds()) && player2.getPosition().y < body.getPosition().y)
        {
            kill();
            return 2;
        }
        return 0;
    }

    void kill() { if (!killed) dead(); }


    Sprite getCopyOfSprite() { return body; }

    void draw() {
        win.draw(body);
    }
};


void soundControl()
{
    Text text[3];
    text[0].setString("MUSIC");
    text[0].setFont(font);
    text[0].setFillColor(Color::White);
    text[0].setPosition(Vector2f(200 + 450, 365));
    text[0].setCharacterSize(70);

    text[1].setString("SFX");
    text[1].setFont(font);
    text[1].setFillColor(Color::White);
    text[1].setPosition(Vector2f(200 + 450, 585));
    text[1].setCharacterSize(70);

    text[2].setString("BACK");
    text[2].setFont(font);
    text[2].setFillColor(Color::White);
    text[2].setPosition(Vector2f(960, 870));
    text[2].setCharacterSize(70);

    RectangleShape music_volume[10], sfx_volume[10], controller[2], col[2], collisionBox;
    int counter = 0;
    for (auto& i : music_volume) {
        i.setSize(Vector2f(20.f, 35.f));
        i.setPosition(500 + 40 * counter + 450, 385);
        i.setFillColor(Color::White);
        counter++;
    }

    counter = 0;
    controller[0].setSize(Vector2f(20.f, 45.f));
    controller[0].setFillColor(Color::Yellow);
    controller[0].setPosition((music_vol) ? music_vol : 520 + 40 * 9.f + 450, 380.f);

    controller[1].setSize(Vector2f(20.f, 45.f));
    controller[1].setFillColor(Color::Yellow);
    controller[1].setPosition((sfx_vol) ? sfx_vol : 520 + 40 * 9.f + 450, 595.f);

    col[0].setPosition(500 + 450, 385), col[1].setPosition(500 + 450, 600);
    col[0].setSize(Vector2f(500 + 40 * counter, 45.f)), col[1].setSize(Vector2f(500 + 40 * counter, 45.f));

    View v(Vector2f(960, 540), Vector2f(1920, 1080));

    for (auto& i : sfx_volume)
    {
        i.setSize(Vector2f(20.f, 35.f));
        i.setPosition(500 + 40 * counter + 450, 600);
        i.setFillColor(Color::White);
        counter++;
    }
    collisionBox.setPosition(text[2].getPosition().x, text[2].getPosition().y + 15);
    collisionBox.setSize(Vector2f(90.f, 60.f));

    while (win.isOpen())
    {

        while (win.pollEvent(ev))
        {
            switch (ev.type)
            {
            case Event::Closed:
                background_music.stop();
                win.close();
            case Event::KeyReleased:
                if (ev.key.code == Keyboard::Escape)
                    return;

            default:
                break;
            }
        }


        Vector2i mousePos = Mouse::getPosition(win);
        Vector2f mousePosView = win.mapPixelToCoords(mousePos);
        if (Mouse::isButtonPressed(Mouse::Left))
        {
            if (mousePos.x < music_volume[9].getPosition().x + music_volume[9].getSize().x && mousePos.x > music_volume[0].getPosition().x &&
                col[0].getGlobalBounds().contains(mousePosView))
            {
                controller[0].setPosition(mousePos.x, 380);
                background_music.setVolume((mousePos.x - music_volume[0].getPosition().x) / 5);
                music_vol = controller[0].getPosition().x;
            }
            if (mousePos.x < music_volume[9].getPosition().x + sfx_volume[9].getSize().x && mousePos.x > sfx_volume[0].getPosition().x &&
                col[1].getGlobalBounds().contains(mousePosView))
            {
                controller[1].setPosition(mousePos.x, 595);
                menu_sounds[0].setVolume((mousePos.x - music_volume[0].getPosition().x) / 5);
                sfx_vol = controller[1].getPosition().x;
            }
        }
        if (collisionBox.getGlobalBounds().contains(mousePosView))
        {
            text[2].setFillColor(Color::Yellow);

            if (Mouse::isButtonPressed(Mouse::Left))
            {
                menu_sounds[0].play();
                return;
            }
        }
        else text[2].setFillColor(Color::White);
        win.setView(v);
        win.clear();
        //drawing
        win.draw(background);
        for (auto& i : music_volume)
            win.draw(i);
        for (auto& i : sfx_volume)
            win.draw(i);
        win.draw(controller[0]);
        win.draw(controller[1]);
        win.draw(text[0]);
        win.draw(text[1]);
        win.draw(text[2]);

        win.display();
    }
}


void pauseMenu(int& event_indicator, int lvl, pair<float, float> pos = { 960,540 }) {
    //1 - resume / 2 - retry / 3 - volume /4 - main
    indicator = 1;

    RectangleShape back, front;
    Text text[4];
    back.setFillColor(Color(0, 0, 0, 80)), front.setFillColor(Color(0, 0, 0, 150));
    back.setSize(Vector2f(1920, 1080)), front.setSize(Vector2f(500, 1080)), front.setPosition(200 + pos.first - 960, 0 + pos.second - 540);
    back.setPosition(pos.first - 960, pos.second - 540);

    {
        text[0].setString("RESUME");
        text[0].setFont(font);
        text[0].setFillColor(Color(180, 0, 0));
        text[0].setPosition(Vector2f(300 + pos.first - 960, 350 + pos.second - 540));
        text[0].setCharacterSize(80);

        text[1].setString("RETRY");
        text[1].setFont(font);
        text[1].setFillColor(Color(180, 0, 0));
        text[1].setPosition(Vector2f(300 + pos.first - 960, 450 + pos.second - 540));
        text[1].setCharacterSize(80);

        text[2].setString("VOLUME");
        text[2].setFont(font);
        text[2].setFillColor(Color(180, 0, 0));
        text[2].setPosition(Vector2f(300 + pos.first - 960, 550 + pos.second - 540));
        text[2].setCharacterSize(80);

        text[3].setString("MAIN MENU");
        text[3].setFont(font);
        text[3].setFillColor(Color(180, 0, 0));
        text[3].setPosition(Vector2f(300 + pos.first - 960, 650 + pos.second - 540));
        text[3].setCharacterSize(80);
    }

    RectangleShape menuCollision[4];
    {
        int i = 0;
        for (auto& e : menuCollision)
        {
            e.setPosition(text[i].getPosition().x, text[i].getPosition().y + 25);
            i++;
        }
        menuCollision[1].setPosition(text[1].getPosition().x + 15, text[1].getPosition().y + 25);
        menuCollision[0].setSize(Vector2f(100.f, 40.f));
        menuCollision[1].setSize(Vector2f(70.f, 40.f));
        menuCollision[2].setSize(Vector2f(130.f, 40.f));
        menuCollision[3].setSize(Vector2f(190.f, 40.f));
    }
    win.draw(back), win.draw(front);
    bool dis = 0;
    while (win.isOpen()) {

        while (win.pollEvent(ev))
        {
            switch (ev.type)
            {
            case Event::Closed:
                background_music.stop();
                win.close();

            default:
                break;
            }
        }


        Vector2i mousePos = Mouse::getPosition(win);
        Vector2f mousePosVeiw = win.mapPixelToCoords(mousePos);
        {
            if (menuCollision[0].getGlobalBounds().contains(mousePosVeiw))
            {
                if (Mouse::isButtonPressed(Mouse::Left))
                {
                    menu_sounds[0].play();
                    event_indicator = 1;
                    return;
                }
            }

            if (menuCollision[1].getGlobalBounds().contains(mousePosVeiw))
            {
                if (Mouse::isButtonPressed(Mouse::Left)) {
                    menu_sounds[0].play();
                    event_indicator = 2;
                    health1 = health2 = 3;
                    return;
                }
            }

            if (menuCollision[2].getGlobalBounds().contains(mousePosVeiw))
            {
                if (Mouse::isButtonPressed(Mouse::Left))
                {
                    menu_sounds[0].play();
                    event_indicator = 3;
                    return;
                }
            }

            if (menuCollision[3].getGlobalBounds().contains(mousePosVeiw))
            {
                if (Mouse::isButtonPressed(Mouse::Left))
                {
                    menu_sounds[0].play();
                    event_indicator = 4;
                    return;
                }
            }
        }

        for (auto& i : text)
            win.draw(i);

        if (!dis) win.display(), dis = 1;
    }
}


void lvl6()
{

    View camera;
    long switch_time1 = 4, total_time1 = 1, switch_time2 = 4, total_time2 = 1;
    int ctrX = 0, ctrX2 = 0, ctrY = 0, ctrY2 = 0, color_cnt = 0, text_cnt = 0, text_cnt2 = 0;
    bool arrived = false, end = false, move = false;

    //sprites and there textures
    Texture Player1, Player2, mapp;
    Sprite sPlayer1, sPlayer2, map;


    Player1.loadFromFile("Player1.png");
    Player2.loadFromFile("Player2.png");
    mapp.loadFromFile("Design2.jpg");


    sPlayer1.setTexture(Player1);
    sPlayer1.setTextureRect(IntRect(0, 0 * 35, 32, 40));
    sPlayer1.setScale(5, 5);
    sPlayer1.setPosition(350, 800);

    map.setTexture(mapp);

    sPlayer2.setTexture(Player2);
    sPlayer2.setTextureRect(IntRect(0, 0 * 36, 32, 40));
    sPlayer2.setScale(5, 5);
    sPlayer2.setPosition(250, 800);

    camera.setSize(1920, 1080);
    camera.setCenter(1920 / 2, 540.f);

    //background music
    Music music;
    music.openFromFile("Level 4 SoundTrack_2.wav");
    music.play();
    music.setVolume(15.0f);



    //credits
    RectangleShape screen_rect(Vector2f(4000, 1080));
    screen_rect.setPosition(1920, 0);
    screen_rect.setFillColor(sf::Color(0, 0, 0, color_cnt));

    Text text1, text2, text3;
    Font font1, font2;
    font1.loadFromFile("MonoscreenDemoRegular.ttf");
    text1.setFont(font1);
    text1.setCharacterSize(80);
    text1.setPosition(2650, 520);
    text1.setString("TEAM FCIS FTW");
    text1.setFillColor(sf::Color(255, 255, 255, text_cnt));

    font2.loadFromFile("SpeedDemon.ttf");
    text2.setFont(font2);
    text2.setCharacterSize(60);
    text2.setFillColor(sf::Color(255, 255, 255, 255));
    text2.setPosition(2650, 1100);
    text2.setString
    ("          ALI TAREK \n\n  ABDELRAHMAN SALAH\n\n        OMAR FAROUK\n\n        MARWA EHAB\n\nABDELRAHMAN RADWAN\n\n         AMR KHALED\n\n       OMAR RADWAN");

    text3.setFont(font1);
    text3.setCharacterSize(80);
    text3.setPosition(2850, 520);
    text3.setString("THE END");
    text3.setFillColor(sf::Color(255, 255, 255, text_cnt2));


    sf::Clock movement;

    while (win.isOpen())
    {
        Event event;
        while (win.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                win.close();
        }

        if (arrived) {
            if (color_cnt < 180)
                color_cnt += 2;
            screen_rect.setFillColor(sf::Color(0, 0, 0, color_cnt));

            if (color_cnt > 100) {
                if (text_cnt < 255)
                    text_cnt += 3;
            }
            text1.setFillColor(sf::Color(255, 255, 255, text_cnt));

            if (text_cnt == 255) {
                text1.move(0, -2);
                text2.move(0, -2);
            }

            if (text2.getPosition().y < -800) {
                if (text_cnt2 < 255)
                    text_cnt2 += 3;
            }
            text3.setFillColor(sf::Color(255, 255, 255, text_cnt2));

            if (text2.getPosition().y < -1400) {
                if (color_cnt < 253)
                    color_cnt += 1.5;
            }

            if (color_cnt > 250) {
                camera.setCenter(960, 540);
                win.setView(camera);
                return;
            }
        }

        if (!arrived) {
            if ((sPlayer1.getPosition().x + sPlayer2.getPosition().x) / 2 - (1920 / 2) > 0 &&
                (sPlayer1.getPosition().x + sPlayer2.getPosition().x) / 2 + (1920 / 2) < 4000)
                camera.setCenter((sPlayer1.getPosition().x + sPlayer2.getPosition().x) / 2, 540.f);

            //the events the characters will do in the game
            if (movement.getElapsedTime().asSeconds() > 1)
                move = true;

            if (sPlayer1.getPosition().x <= 3300) {
                if (move) {
                    sPlayer1.move(4, 0);
                    ctrY = 2;
                    if (total_time1 % switch_time1 > 0) total_time1++;
                    else ctrX++, total_time1 = 1;
                }
            }


            if (sPlayer2.getPosition().x <= 3300) {
                if (move) {
                    sPlayer2.move(4, 0);
                    ctrY2 = 2;
                    if (total_time2 % switch_time2 > 0) total_time2++;
                    else ctrX2++, total_time2 = 1;
                }
            }


            ctrX %= 2;
            sPlayer1.setTextureRect(IntRect(ctrX * 32, ctrY * 34, 32, 28));

            ctrX2 %= 2;
            sPlayer2.setTextureRect(IntRect(ctrX2 * 32, ctrY2 * 33, 32, 28));
        }

        if (sPlayer1.getPosition().x > 3290 && sPlayer2.getPosition().x > 3290)
            arrived = true;


        win.clear();
        win.setView(camera);
        win.draw(map);
        win.draw(sPlayer1);
        win.draw(sPlayer2);
        win.draw(screen_rect);
        win.draw(text1);
        win.draw(text2);
        win.draw(text3);

        win.display();
    }

}


void LvlBoss() {
    int ctrX = 0, ctrX2 = 0, ctrY = 0, ctrY2 = 0, velocityY = 0, sound_cnt = 0,
        hb_delay1 = 60, hb_delay2 = 60, gameOver_delay = 0, color_cnt = 0, cntrex = 0;

    float velocityY1 = 0, velocityY2 = 0;
    bool dead = false, buttonPressed_ = false, grounded = 0, grounded1 = false, grounded2 = false, sound_played = false, complete = 0, lavacheck = false, keycheck = false;
    bool can = 1;

    // this is the sprites and there textures we are using in the game


    Texture Player1, Player2, mapp, key_texture, button_texture, buttonpressed_texture, door_texture, opened_gate_texture, score_texture, health_bar1_texture[4], lava_tex,
        health_bar2_texture[4], hearts_texture, moving_tile_tex, game_over_texture, ground, tiles;

    Sprite sPlayer1, sPlayer2, map, button[2], door, health_bar1, health_bar2, score, game_over, floor1, tile[2], moving_tile, lava, key;

    Clock c;


    tiles.loadFromFile("floor6.png");
    ground.loadFromFile("ground.png");
    buttonpressed_texture.loadFromFile("Button_Pressed.png");
    door_texture.loadFromFile("gate.png");
    opened_gate_texture.loadFromFile("open_door.png");
    button_texture.loadFromFile("button.png");
    mapp.loadFromFile("map_vignette.png");
    map.setTexture(mapp);
    health_bar1_texture[3].loadFromFile("P1_health_bar_full.png");
    health_bar1_texture[2].loadFromFile("P1_health_bar_1.png");
    health_bar1_texture[1].loadFromFile("P1_health_bar_2.png");
    health_bar1_texture[0].loadFromFile("P1_health_bar_empty.png");
    health_bar2_texture[3].loadFromFile("P2_health_bar_full.png");
    health_bar2_texture[2].loadFromFile("P2_health_bar_1.png");
    health_bar2_texture[1].loadFromFile("P2_health_bar_2.png");
    health_bar2_texture[0].loadFromFile("P2_health_bar_empty.png");
    score_texture.loadFromFile("Score_Coin.png");
    game_over_texture.loadFromFile("Game_Over.png");
    moving_tile_tex.loadFromFile("floor3.png");
    lava_tex.loadFromFile("Lava1.png");
    key_texture.loadFromFile("key.png");



    key.setTexture(key_texture);
    lava.setTexture(lava_tex);
    moving_tile.setTexture(moving_tile_tex);



    RectangleShape b3(Vector2f(5, 1900));

    b3.setPosition(1915, 5); // right border

    Player1.loadFromFile("Player1.png");
    Player2.loadFromFile("Player2.png");

    //health bar 2
    health_bar2.setPosition(1630.0f, -35.0f);
    health_bar2.setScale(1.4, 1.4);


    //health bar 1
    health_bar1.setPosition(15.0f, -35.0f);
    health_bar1.setScale(1.4, 1.4);

    //game over
    game_over.setTexture(game_over_texture);


    //buttons
    for (int i = 0; i < 2; i++)
    {
        button[i].setTexture(button_texture);
        button[i].setScale(0.6f, 0.6f);

    }
    button[1].setPosition(100, 940);
    button[0].setPosition(1400, 940);

    //the portal (wooden gate)
    door.setTexture(door_texture);
    door.setPosition(1750, 85);
    door.setScale(1.2f, 1.2f);


    //Background music
    Music music;
    music.openFromFile("Boss_Music.OGG");
    music.play();
    music.setVolume(50.0f);


    Sound wooden_gate_sound;
    SoundBuffer wooden_gate_sound_buffer;
    wooden_gate_sound_buffer.loadFromFile("Wooden Door Opening.wav");
    wooden_gate_sound.setBuffer(wooden_gate_sound_buffer);

    Sound button_sound;
    SoundBuffer button_sound_buffer;
    button_sound_buffer.loadFromFile("Button Pushed.wav");
    button_sound.setBuffer(button_sound_buffer);


    Sound key_sound;
    SoundBuffer key_sound_buffer;
    key_sound_buffer.loadFromFile("Key is taken.wav");
    key_sound.setBuffer(key_sound_buffer);
    key_sound.setVolume(200.0f);

    Sound jump_sound;
    SoundBuffer jump_sound_buffer;
    jump_sound_buffer.loadFromFile("Jump_Sound.wav");
    jump_sound.setBuffer(jump_sound_buffer);
    jump_sound.setVolume(30.0f);

    Sound movement_sound;
    SoundBuffer movement_sound_buffer;
    movement_sound_buffer.loadFromFile("Movement.wav");
    movement_sound.setBuffer(movement_sound_buffer);
    movement_sound.setVolume(1.0f);

    Sound damage_sound;
    SoundBuffer damage_sound_buffer;
    damage_sound_buffer.loadFromFile("Damage Taken.wav");
    damage_sound.setBuffer(damage_sound_buffer);

    Sound dead_sound;
    SoundBuffer dead_sound_buffer;
    dead_sound_buffer.loadFromFile("Game Over.wav");
    dead_sound.setBuffer(dead_sound_buffer);

    //score
    score.setTexture(score_texture);
    score.scale(0.3f, 0.3f);
    score.setPosition(900, 20);
    Font font;
    font.loadFromFile("Amatic-Bold.ttf");
    Text score_num;
    score_num.setFont(font);
    score_num.setFillColor(sf::Color::White);
    score_num.setPosition(960, 15);
    score_num.setCharacterSize(50);



    sPlayer1.setTexture(Player1);
    sPlayer1.setTextureRect(IntRect(0, 0 * 35, 32, 40));
    sPlayer1.setScale(3, 3);

    sPlayer2.setTexture(Player2);
    sPlayer2.setTextureRect(IntRect(0, 0 * 36, 32, 40));
    sPlayer2.setScale(3, 3);
    sPlayer2.setPosition(100, 0);

    //transition
    transition start(0), end(1);

    FockingBoss boss(Vector2f(7, 7), Vector2f(540, 240));


    RectangleShape b1(Vector2f(5, 1080)), b2(Vector2f(5, 1900)), gate_rect(Vector2f(5, 130)), square_rect(Vector2f(7, 2)),
        square3_rect(Vector2f(5, 40)), square4_rect(Vector2f(5, 40)), screen_rect(Vector2f(1920, 1080));

    gate_rect.setPosition(1750, 85);

    lava.setScale(4, 1);
    b1.setPosition(-20, 1); //left border
    b2.setPosition(1930, 1); // right border
    square_rect.setPosition(415, 865);
    square3_rect.setPosition(415, 890);
    square4_rect.setPosition(415, 890);
    screen_rect.setFillColor(sf::Color(0, 0, 0, color_cnt));
    kaboom canonn1(Vector2f(40, 430), 1);


    for (int i = 0; i < 2; i++)
    {
        tile[i].setTexture(tiles);
    }
    tile[0].setPosition(1570, 200); //Tile under the door
    tile[1].setPosition(-100, 500); //Tile under the canon
    floor1.setTexture(ground);
    floor1.setPosition(0, 1000);

    collup upper_left_tile_rect__(Vector2f(floor1.getPosition()), Vector2f(1920, 10));
    collup upper_right_tile_rect(Vector2f(tile[0].getPosition().x + 0, tile[0].getPosition().y), Vector2f(435, 10));
    collup moving_t(Vector2f(1700, 1000), Vector2f(350, 10), -10);


    collup button_rect(Vector2f(button[1].getPosition().x + 30, button[1].getPosition().y + 40), Vector2f(1, 5));
    collup button_rect2(Vector2f(button[0].getPosition().x + 30, button[0].getPosition().y + 40), Vector2f(1, 5));


    RectangleShape lavab1, lavab2;
    lavab1.setPosition(-650, 1000);
    lavab1.setSize(Vector2f(530, 30));
    lavab1.setFillColor(Color::Red);
    lavab2.setPosition(-1630, 930);
    lavab2.setSize(Vector2f(1000, 100));




    lava.setPosition(-7700, 555);
    key.setPosition(2000, 800);
    moving_tile.setPosition(1400, 950);

    sf::Clock player_switch1, player_switch2, movement_sound_switch, switch_time;


    while (win.isOpen())
    {
        //coins & hearts animation 

        Event event;
        while (win.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                win.close();

        }


        if ((button_rect2.collision(sPlayer1) && (button_rect.collision(sPlayer2)) && can) || (canonn1.bulet.getPosition().x < 775
            && canonn1.bulet.getPosition().x > 80))
        {
            canonn1.update(b1, b2, sPlayer1, sPlayer2);
            c.restart();
        }

        if ((button_rect2.collision(sPlayer2) && (button_rect.collision(sPlayer1)) && can) || (canonn1.bulet.getPosition().x < 775
            && canonn1.bulet.getPosition().x > 80))
        {
            canonn1.update(b1, b2, sPlayer1, sPlayer2);
            c.restart();
        }
        if (c.getElapsedTime().asSeconds() < 3) can = 0, button[0].setTexture(buttonpressed_texture), button[1].setTexture(buttonpressed_texture);
        else can = 1, button[0].setTexture(button_texture), button[1].setTexture(button_texture);

        if (sPlayer1.getGlobalBounds().intersects(key.getGlobalBounds()) || sPlayer2.getGlobalBounds().intersects(key.getGlobalBounds()))
        {
            key.setScale(0, 0);
            keys[4] = true;
            door.setTexture(opened_gate_texture);
            key_sound.play();
            wooden_gate_sound.play();

        }


        if (boss.dead3 && lava.getPosition().x != -100)
            lava.move(2, 0), lavab1.move(2, 0), lavab2.move(2, 0);


        moving_tile.setPosition(moving_t.getpos().x, moving_t.getpos().y);


        music.setVolume((abs(music_vol - 970) / 1260) * 100);
        jump_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        movement_sound.setVolume((abs(sfx_vol - 970) / 1260) * 5);
        damage_sound.setVolume((abs(sfx_vol - 970) / 1260) * 50);
        dead_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);

        if (dead)
        {
            health_bar1.setTexture(health_bar1_texture[health1]);
            health_bar2.setTexture(health_bar2_texture[health2]);
            if (color_cnt < 200)
                color_cnt += 2;
            screen_rect.setFillColor(sf::Color(0, 0, 0, color_cnt));
            if (color_cnt == 200) {
                if (gameOver_delay != 180) gameOver_delay++;
                else { indicator = 2; return; }
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Escape) || indicator != 1) { if (!indicator) { indicator = -1; goto render; } pauseMenu(indicator, 5); }

        if (indicator == 2 || indicator == 4) return;

        if (indicator == 3) soundControl(), indicator = 0;
        //fade to black at the end of the level
        if (complete) {
            end.update(5);
            if (end.retrn()) {
                return;
            }
        }
        if (indicator == 1) {
            if (!dead && !complete) {

                if (Keyboard::isKeyPressed(Keyboard::Key::W)) {
                    sPlayer1.move(0, -5);
                    movement_sound.pause();
                    if (grounded1)
                        jump_sound.play();
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
                    sPlayer1.move(-5, 0);
                    ctrY = 3;
                    if (grounded1) {
                        if (movement_sound_switch.getElapsedTime().asSeconds() > 0.50) {
                            movement_sound.play();
                            movement_sound_switch.restart();
                        }
                    }

                    if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                        ctrX++;
                        player_switch1.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
                    sPlayer1.move(5, 0);
                    ctrY = 2;

                    if (grounded1) {
                        if (movement_sound_switch.getElapsedTime().asSeconds() > 0.50) {
                            movement_sound.play();
                            movement_sound_switch.restart();
                        }
                    }

                    if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                        ctrX++;
                        player_switch1.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::D)) {
                    sPlayer1.move(3, 3);
                    ctrY = 2;

                    if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                        ctrX++;
                        player_switch1.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::A)) {
                    sPlayer1.move(-3, 3);
                    ctrY = 3;

                    if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                        ctrX++;
                        player_switch1.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
                    sPlayer2.move(0, -5);
                    movement_sound.pause();
                    if (grounded2)
                        jump_sound.play();
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                    sPlayer2.move(-5, 0);
                    ctrY2 = 3;
                    if (grounded2) {
                        if (movement_sound_switch.getElapsedTime().asSeconds() > 0.50) {
                            movement_sound.play();
                            movement_sound_switch.restart();
                        }
                    }

                    if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                        ctrX2++;
                        player_switch2.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                    sPlayer2.move(5, 0);
                    ctrY2 = 2;
                    if (grounded2) {
                        if (movement_sound_switch.getElapsedTime().asSeconds() > 0.50) {
                            movement_sound.play();
                            movement_sound_switch.restart();

                        }
                    }

                    if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                        ctrX2++;
                        player_switch2.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                    sPlayer2.move(3, 3);
                    ctrY2 = 2;

                    if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                        ctrX2++;
                        player_switch2.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                    sPlayer2.move(-3, 3);
                    ctrY2 = 3;

                    if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                        ctrX2++;
                        player_switch2.restart();
                    }
                }

                ctrX %= 2;
                sPlayer1.setTextureRect(IntRect(ctrX * 32, ctrY * 34, 32, 28));

                ctrX2 %= 2;
                sPlayer2.setTextureRect(IntRect(ctrX2 * 32, ctrY2 * 33, 32, 28));


                //collision
                if (sPlayer1.getGlobalBounds().intersects(b1.getGlobalBounds())) {
                    sPlayer1.move(5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::A)) {
                        sPlayer1.move(3, 3);
                    }
                }

                if (sPlayer2.getGlobalBounds().intersects(b1.getGlobalBounds())) {
                    sPlayer2.move(5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                        sPlayer2.move(3, 3);
                    }
                }

                if (sPlayer1.getGlobalBounds().intersects(b2.getGlobalBounds())) {
                    sPlayer1.move(-5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::D)) {
                        sPlayer1.move(-3, 3);
                    }
                }

                if (sPlayer2.getGlobalBounds().intersects(b2.getGlobalBounds())) {
                    sPlayer2.move(-5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                        sPlayer2.move(-3, 3);
                    }
                }
                if (sPlayer1.getGlobalBounds().intersects(lavab1.getGlobalBounds()) || sPlayer1.getGlobalBounds().intersects(lavab2.getGlobalBounds()))
                {
                    health1 = 0;

                }
                if (sPlayer2.getGlobalBounds().intersects(lavab1.getGlobalBounds()) || sPlayer2.getGlobalBounds().intersects(lavab2.getGlobalBounds()))
                {
                    health2 = 0;

                }


                health_bar1.setTexture(health_bar1_texture[health1]);
                health_bar2.setTexture(health_bar2_texture[health2]);

                //the events the characters will do in the game
                score_num.setString("x" + to_string(coins_cntr));




                //jumping & Collision with rectangles
                grounded1 = 0;
                grounded2 = 0;
                upper_left_tile_rect__.update(grounded1, grounded2, sPlayer1, sPlayer2);
                upper_right_tile_rect.update(grounded1, grounded2, sPlayer1, sPlayer2);
                boss.update(canonn1.bulet, sPlayer1, sPlayer2);
                if (moving_t.getpos().y > 200 && keys[4] && moving_t.collision(sPlayer1) && moving_t.collision(sPlayer2))
                    moving_t.update(grounded1, grounded2, sPlayer1, sPlayer2, 1, { 200 , 1010 });


                //transition
                start.update(5);

                if (!grounded1)
                {
                    velocityY1 -= 1;
                }

                if (grounded1)
                {
                    velocityY1 = 0;
                    if (Keyboard::isKeyPressed(Keyboard::Key::W)) {
                        velocityY1 = 15;
                    }
                }

                sPlayer1.move(0, -velocityY1);

                if (!grounded2) {
                    velocityY2 -= 1;
                }

                if (grounded2) {
                    velocityY2 = 0;
                    if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
                        velocityY2 = 15;
                    }
                }


                if (sPlayer1.getGlobalBounds().intersects(gate_rect.getGlobalBounds())
                    && sPlayer2.getGlobalBounds().intersects(gate_rect.getGlobalBounds())
                    && keys[4] == true && coins_cntr == 41) {

                    gate_pass[4] = true;
                    complete = 1;
                }


                sPlayer2.move(0, -velocityY2);
                ctrX %= 2;
                sPlayer1.setTextureRect(IntRect(ctrX * 32, ctrY * 34, 32, 28));

                ctrX2 %= 2;
                sPlayer2.setTextureRect(IntRect(ctrX2 * 32, ctrY2 * 33, 32, 28));


                if (health1 == 0) {
                    sPlayer1.setTextureRect(IntRect(0, 11 * 32.3, 32, 28));
                    dead = true;
                }

                if (health2 == 0) {
                    sPlayer2.setTextureRect(IntRect(0, 4 * 32.4, 32, 28));
                    dead = true;
                }
            }
        }



    render:;
        win.clear();
        win.draw(map);


        //draw
        for (int i = 0; i < 2; i++)
        {
            win.draw(tile[i]);
        }

        canonn1.draw();
        win.draw(floor1);
        for (int i = 0; i < 2; i++)
        {
            win.draw(button[i]);

        }
        if (boss.dead3)
        {
            key.setPosition(1800, 900);
            win.draw(moving_tile);


            key.setScale(0.2, 0.2);
            if (keys[4]) key.setPosition(2000, 2000);
            win.draw(key);

        }

        win.draw(door);
        boss.draw();
        win.draw(sPlayer1);
        win.draw(sPlayer2);
        win.draw(health_bar1);
        win.draw(health_bar2);
        win.draw(score);
        win.draw(score_num);
        win.draw(lava);
        win.draw(screen_rect);

        if (color_cnt > 110) {
            win.draw(game_over);
            if (!sound_played) {
                music.pause();
                dead_sound.play();
                sound_played = true;
            }
        }
        end.draw();
        start.draw();
        win.display();
    }
}


void lvl4()
{
    //counters and booleans used throughout the level.for coins,hearts,player animations,speed and to check whether a player is on ground.
    int coins_cnt = 0, hearts_cnt = 0, ctrX = 0, ctrX2 = 0, ctrY = 0, ctrY2 = 0, coins_score = 0, hb_delay1 = 60, hb_delay2 = 60,
        gameOver_delay = 0, color_cnt = 0, jumper_cnt1 = 0, jumper_cnt2 = 0, num1 = 0, num2 = 0;
    float velocityY1 = 0, velocityY2 = 0;
    bool grounded1 = false, grounded2 = 0, dead = 0, sound_played = 0, complete = 0, istaken = 0, on1 = 0, on2 = 0, damage1 = 0,
        damage2 = 0;

    //Declaring sprites and textures
    Sprite background[3], tile1, tile2, tile3, movingTile, tile4, tile5, tile6, tile7, tile8, tile9, tile10, squared_tile, key, coins[3],
        coins1[3], coins2[3], coins3[3], hearts[2], ladder[2], gate, sPlayer1, sPlayer2, cannon1, cannon2, HealthBarP1,
        HealthBarP2, score, game_over, lava, jumperSprite[4];


    Texture background_texture, player1_texture, player2_texture, tile1_texture, tile2_texture, movingTile_texture, tile5_texture, tile6_texture,
        tile8_texture, key_texture, coins_texture, hearts_texture, ladder_texture, gate_texture, opened_gate_texture, Player1, Player2, cannonT,
        Health_Bar1[4], Health_Bar2[4], Score, game_over_texture, lava_texture, squared_tile_texture, jumper_texture;


    //Loading textures from files
    background_texture.loadFromFile("map_vignette.png");
    Player1.loadFromFile("Player1.png");
    Player2.loadFromFile("Player2.png");
    tile1_texture.loadFromFile("LavaGround.png");
    tile2_texture.loadFromFile("AcidTile.png");
    movingTile_texture.loadFromFile("floor3.png");
    tile5_texture.loadFromFile("LongGround.png");
    tile6_texture.loadFromFile("floor1.png");
    tile8_texture.loadFromFile("floor2.png");
    squared_tile_texture.loadFromFile("tile3.png");
    jumper_texture.loadFromFile("Jumper.png");
    key_texture.loadFromFile("key.png");
    coins_texture.loadFromFile("coins1.png");
    hearts_texture.loadFromFile("hearts.png");
    ladder_texture.loadFromFile("ladder.png");
    gate_texture.loadFromFile("gate.png");
    opened_gate_texture.loadFromFile("open_door.png");
    cannonT.loadFromFile("Canon.png");
    Health_Bar1[3].loadFromFile("P1_health_bar_full.png");
    Health_Bar1[2].loadFromFile("P1_health_bar_1.png");
    Health_Bar1[1].loadFromFile("P1_health_bar_2.png");
    Health_Bar1[0].loadFromFile("P1_health_bar_empty.png");
    Health_Bar2[3].loadFromFile("P2_health_bar_full.png");
    Health_Bar2[2].loadFromFile("P2_health_bar_1.png");
    Health_Bar2[1].loadFromFile("P2_health_bar_2.png");
    Health_Bar2[0].loadFromFile("P2_health_bar_empty.png");
    Score.loadFromFile("Score_Coin.png");
    game_over_texture.loadFromFile("Game_Over.png");
    lava_texture.loadFromFile("Lava.png");


    //View
    View camera;
    camera.setSize(1920, 1080);
    camera.setCenter(1920 / 2, 1080 / 2);

    //Border rectangles
    RectangleShape b1(Vector2f(5, 7160)), b2(Vector2f(5, 7160)), screen_rect(Vector2f(1920, 2160)), gate_rect(Vector2f(5, 130)),
        jumpers[4], ladder_rect[2], acid_rect[15], lava_rect;

    for (int i = 0; i < 2; i++) {
        ladder_rect[i].setSize(Vector2f(1, 250));
        ladder_rect[i].setPosition(90 + (i * 190), -400 - (i * 278));
    }

    for (int i = 0; i < 4; i++) {
        jumpers[i].setSize(Vector2f(10, 10));
    }

    for (int i = 0; i < 15; i++) {
        acid_rect[i].setSize(Vector2f(1, 5));
    }

    for (int i = 0; i < 5; i++)
        acid_rect[i].setPosition(367 + (i * 345), 610);

    for (int i = 5; i < 10; i++)
        acid_rect[i].setPosition(182 + ((i - 5) * 345), 360);

    for (int i = 10; i < 15; i++)
        acid_rect[i].setPosition(367 + ((i - 10) * 345), 110);

    screen_rect.setFillColor(sf::Color(0, 0, 0, color_cnt));

    lava_rect.setSize(Vector2f(1920, 5));
    lava_rect.setPosition(0, 1050);

    //Setting texture, position....etc
    for (int i = 0; i < 3; i++)
        background[i].setTexture(background_texture);

    background[1].setPosition(0, -1050);
    background[2].setPosition(0, -2130);

    b1.setPosition(-20, -2000); //left border
    b2.setPosition(1930, -2000); // right border
    gate_rect.setPosition(1850, -1065);

    //ground
    tile1.setTexture(tile1_texture);
    tile1.setPosition(0, 980);

    //first tile
    tile2.setTexture(tile2_texture);
    tile2.setPosition(60, 540);

    //second tile
    tile3.setTexture(tile2_texture);
    tile3.setPosition(-125, 290.0f);

    //third tile
    tile4.setTexture(tile2_texture);
    tile4.setPosition(60, 40);

    //fourth tile
    tile5.setTexture(tile5_texture);
    tile5.setPosition(-15, -150);

    //lower left tile
    tile6.setTexture(tile6_texture);
    tile6.setPosition(-125, -410);
    tile6.setScale(0.9, 0.9);

    //upper left tile
    tile7.setTexture(tile6_texture);
    tile7.setPosition(-125, -690);
    tile7.setScale(0.9, 0.9);

    //lower right tile
    tile8.setTexture(tile8_texture);
    tile8.setPosition(1400, -410);
    tile8.setScale(0.9, 0.9);

    //upper right tile
    tile9.setTexture(tile8_texture);
    tile9.setPosition(1400, -690);
    tile9.setScale(0.9, 0.9);

    //gate tile
    tile10.setTexture(tile8_texture);
    tile10.setPosition(1400, -940);
    tile10.setScale(0.9, 0.9);

    //squared tile in the ground
    squared_tile.setTexture(squared_tile_texture);
    squared_tile.setPosition(855, 980);

    //moving tile
    movingTile.setTexture(movingTile_texture);
    movingTile.setPosition(400, -690);

    //jumpers
    Vector2u jumper_texture_size = jumper_texture.getSize();
    jumper_texture_size.x /= 6;
    for (int i = 0; i < 4; i++) {
        jumperSprite[i].setTexture(jumper_texture);
        jumperSprite[i].setScale(0.5f, 0.5f);
        jumperSprite[i].setTextureRect(IntRect(0, 0, jumper_texture_size.x, jumper_texture_size.y));
    }

    //jumper above the ground
    jumperSprite[0].setPosition(30, 897);

    //jumper above first tile
    jumperSprite[1].setPosition(1785, 530);

    //jumper above second tile
    jumperSprite[2].setPosition(30, 280);

    //jumper tile above third tile
    jumperSprite[3].setPosition(1785, 30);

    //lava
    lava.setTexture(lava_texture);
    lava.setPosition(0, 1030);

    //ladder
    for (int i = 0; i < 2; i++) {
        ladder[i].setTexture(ladder_texture);
        ladder[i].setPosition(-60 + (i * 190), -490 - (i * 278));
        ladder[i].setScale(0.8, 1);
    }

    //key texture and position
    key.setTexture(key_texture);
    key.setPosition(900.0f, 950.0f);
    key.setScale(0.12f, 0.12f);

    //gate texture and position
    gate.setTexture(gate_texture);
    gate.setPosition(1750.0f, -1075.0f);
    gate.setScale(1.4f, 1.4f);

    //cannons texture and position
    cannon1.setTexture(cannonT);
    cannon1.setScale(1.8, 1.8);
    cannon1.setPosition(1735, -500);
    cannon2.setTexture(cannonT);
    cannon2.setScale(1.8, 1.8);
    cannon2.setPosition(1735, -780);

    //Health Bar P1 & P2
    HealthBarP1.setTexture(Health_Bar1[0]);
    HealthBarP1.setScale(1.4, 1.4);
    HealthBarP2.setTexture(Health_Bar2[0]);
    HealthBarP2.setScale(1.4, 1.4);

    //Score
    score.setTexture(Score);
    score.scale(0.3f, 0.3f);
    Font font;
    font.loadFromFile("Amatic-Bold.ttf");
    Text score_num;
    score_num.setFont(font);
    score_num.setFillColor(sf::Color::White);
    score_num.setCharacterSize(50);

    //game over
    game_over.setTexture(game_over_texture);

    //Player1 texture,animations...etc
    sPlayer1.setTexture(Player1);
    sPlayer1.setTextureRect(IntRect(0, 0 * 35, 32, 40));
    sPlayer1.setScale(3, 3);
    sPlayer1.setPosition(1750.0f, 830.0f);

    //Player2 texture,animations...etc
    sPlayer2.setTexture(Player2);
    sPlayer2.setTextureRect(IntRect(0, 0 * 36, 32, 40));
    sPlayer2.setScale(3, 3);
    sPlayer2.setPosition(1850.0f, 830.0f);

    //Loop to set textures,positions,animations,scale to coins
    Vector2u coins_texture_size = coins_texture.getSize();
    coins_texture_size.x /= 6;
    for (int i = 0; i < 3; i++) {
        //first tile coins
        coins[i].setTexture(coins_texture);
        coins[i].setPosition(770.0f + (i * 90), 570.0f);
        coins[i].setTextureRect(IntRect(0, 0, coins_texture_size.x, coins_texture_size.y));
        coins[i].setScale(0.2f, 0.2f);

        //second tile coins
        coins1[i].setTexture(coins_texture);
        coins1[i].setPosition(600.0f + (i * 70), 320.0f);
        coins1[i].setTextureRect(IntRect(0, 0, coins_texture_size.x, coins_texture_size.y));
        coins1[i].setScale(0.2f, 0.2f);

        //second tile coins
        coins2[i].setTexture(coins_texture);
        coins2[i].setPosition(1300.0f + (i * 70), 320.0f);
        coins2[i].setTextureRect(IntRect(0, 0, coins_texture_size.x, coins_texture_size.y));
        coins2[i].setScale(0.2f, 0.2f);

        //third Right tile coins
        coins3[i].setTexture(coins_texture);
        coins3[i].setPosition(1150.0f + (i * 70), 70.0f);
        coins3[i].setTextureRect(IntRect(0, 0, coins_texture_size.x, coins_texture_size.y));
        coins3[i].setScale(0.2f, 0.2f);
    }

    //Loop to set textures,positions,animations,scale to hearts
    Vector2u hearts_texture_size = hearts_texture.getSize();
    hearts_texture_size.x /= 6;
    for (size_t i = 0; i < 2; i++) {
        hearts[i].setTexture(hearts_texture);
        hearts[i].setPosition(1550.0f + (i * 90), -990.0f);
        hearts[i].setTextureRect(IntRect(0, 0, hearts_texture_size.x, hearts_texture_size.y));
        hearts[i].setScale(0.5f, 0.5f);
    }


    //rectangles above the tiles (used in collision)
    //                             position                                                        size
    collup ground_rect(Vector2f(tile1.getPosition().x + 1250, tile1.getPosition().y), Vector2f(700, 10));
    collup ground_Left_rect(Vector2f(tile1.getPosition().x, tile1.getPosition().y), Vector2f(630, 10));
    collup middle_ground_rect(Vector2f(squared_tile.getPosition().x + 30, squared_tile.getPosition().y), Vector2f(125, 10), 2);
    collup first_Tile_rect1(Vector2f(tile2.getPosition().x + 150, tile2.getPosition().y + 70), Vector2f(3000, 10));
    collup second_Tile_rect1(Vector2f(tile3.getPosition().x, tile3.getPosition().y + 70), Vector2f(1840, 10));
    collup third_Tile_rect1(Vector2f(tile4.getPosition().x + 150, tile4.getPosition().y + 70), Vector2f(3000, 10));
    collup fourth_Tile_rect1(Vector2f(tile5.getPosition().x - 25, tile5.getPosition().y + 15), Vector2f(1750, 10));
    collup lower_right_Tile_rect1(Vector2f(tile8.getPosition().x + 150, tile8.getPosition().y), Vector2f(500, 10));
    collup upper_right_Tile_rect1(Vector2f(tile9.getPosition().x + 150, tile9.getPosition().y), Vector2f(1000, 10));
    collup gate_Tile_rect1(Vector2f(tile10.getPosition().x + 150, tile10.getPosition().y), Vector2f(500, 10));
    collup lower_left_Tile_rect1(Vector2f(tile6.getPosition().x, tile6.getPosition().y), Vector2f(500, 10));
    collup upper_left_Tile_rect1(Vector2f(tile7.getPosition().x, tile7.getPosition().y), Vector2f(500, 10));
    for (int i = 0; i < 4; i++) {
        jumpers[i].setPosition(jumperSprite[i].getPosition().x + 48, jumperSprite[i].getPosition().y + 60);
    }



    //rectangles under the tiles (used in collision)
    //                                     position                                                size
    colldown first_Tile_rect2(Vector2f(tile2.getPosition().x + 150, tile2.getPosition().y + 130), Vector2f(3000, 10));
    colldown second_Tile_rect2(Vector2f(tile3.getPosition().x, tile3.getPosition().y + 130), Vector2f(1840, 10));
    colldown third_Tile_rect2(Vector2f(tile4.getPosition().x + 150, tile4.getPosition().y + 130), Vector2f(3000, 10));
    colldown fourth_Tile_rect2(Vector2f(tile5.getPosition().x - 25, tile5.getPosition().y + 70), Vector2f(1750, 10));
    colldown lower_right_Tile_rect2(Vector2f(tile8.getPosition().x + 150, tile8.getPosition().y + 70), Vector2f(1750, 10));
    colldown upper_right_Tile_rect2(Vector2f(tile9.getPosition().x + 150, tile9.getPosition().y + 70), Vector2f(1000, 10));
    colldown gate_Tile_rect2(Vector2f(tile10.getPosition().x + 150, tile10.getPosition().y + 70), Vector2f(1000, 10));
    colldown lower_left_Tile_rect2(Vector2f(tile6.getPosition().x, tile6.getPosition().y + 70), Vector2f(470, 10));
    colldown upper_left_Tile_rect2(Vector2f(tile7.getPosition().x, tile7.getPosition().y + 70), Vector2f(470, 10));

    //rectangles collision to the Right of the tiles
    //                                           position                                                     size
    colright second_Tile_rect3(Vector2f(tile3.getPosition().x + 1840, tile3.getPosition().y + 80), Vector2f(10, 50));
    colright fourth_Tile_rect3(Vector2f(tile5.getPosition().x + 1730, tile5.getPosition().y + 20), Vector2f(10, 50));

    //rectangles collision to the left of the tiles
    //                                          position                                                      size
    colleft first_Tile_rect3(Vector2f(tile2.getPosition().x + 140, tile2.getPosition().y + 80), Vector2f(10, 50));
    colleft third_Tile_rect3(Vector2f(tile4.getPosition().x + 140, tile4.getPosition().y + 80), Vector2f(10, 50));
    colleft lower_right_Tile_rect3(Vector2f(tile8.getPosition().x + 150, tile8.getPosition().y + 20), Vector2f(10, 50));
    lower_right_Tile_rect3.rotation(-30);
    colleft upper_right_Tile_rect3(Vector2f(tile9.getPosition().x + 150, tile9.getPosition().y + 20), Vector2f(10, 50));
    upper_right_Tile_rect3.rotation(-30);
    colleft gate_Tile_rect3(Vector2f(tile10.getPosition().x + 150, tile10.getPosition().y + 20), Vector2f(10, 50));
    gate_Tile_rect3.rotation(-30);

    //Moving Tile.
    collup2 moving_tile(Vector2f(550, -700), Vector2f(280, 10), 6.5, 2);


    //Canon
    kaboom lower_right_canon(Vector2f(cannon1.getPosition().x + 100, cannon1.getPosition().y + 10), -1);
    kaboom upper_right_canon(Vector2f(cannon2.getPosition().x + 100, cannon2.getPosition().y + 10), -1);

    //transition
    transition start(0), end(1);

    //Sounds
    Music music;
    music.openFromFile("SoundTrack.wav");
    music.play();
    music.setVolume(15.0f);


    Sound coins_sound;
    SoundBuffer coins_sound_buffer;
    coins_sound_buffer.loadFromFile("coin sound effect.wav");
    coins_sound.setBuffer(coins_sound_buffer);


    Sound hearts_sound;
    SoundBuffer hearts_sound_buffer;
    hearts_sound_buffer.loadFromFile("Heart Taken.wav");
    hearts_sound.setBuffer(hearts_sound_buffer);


    Sound key_sound;
    SoundBuffer key_sound_buffer;
    key_sound_buffer.loadFromFile("Key is taken.wav");
    key_sound.setBuffer(key_sound_buffer);
    key_sound.setVolume(200.0f);


    Sound wooden_gate_sound;
    SoundBuffer wooden_gate_sound_buffer;
    wooden_gate_sound_buffer.loadFromFile("Wooden Door Opening.wav");
    wooden_gate_sound.setBuffer(wooden_gate_sound_buffer);


    Sound jump_sound;
    SoundBuffer jump_sound_buffer;
    jump_sound_buffer.loadFromFile("Jump_Sound.wav");
    jump_sound.setBuffer(jump_sound_buffer);


    Sound movement_sound;
    SoundBuffer movement_sound_buffer;
    movement_sound_buffer.loadFromFile("Movement.wav");
    movement_sound.setBuffer(movement_sound_buffer);
    movement_sound.setVolume(1.0f);


    Sound damage_sound;
    SoundBuffer damage_sound_buffer;
    damage_sound_buffer.loadFromFile("Damage Taken.wav");
    damage_sound.setBuffer(damage_sound_buffer);


    Sound dead_sound;
    SoundBuffer dead_sound_buffer;
    dead_sound_buffer.loadFromFile("Game Over.wav");
    dead_sound.setBuffer(dead_sound_buffer);


    Music lava_music;
    lava_music.openFromFile("Lava sound.wav");
    lava_music.play();
    lava_music.setVolume(40.0f);


    Sound jumper_sound;
    SoundBuffer jumper_sound_buffer;
    jumper_sound_buffer.loadFromFile("Boing.wav");
    jumper_sound.setBuffer(jumper_sound_buffer);


    Clock switch_time, player_switch1, player_switch2, movement, jumper_switch1, jumper_switch2;

    //Game loop
    while (win.isOpen()) {
        //Event to close the window
        Event evnt;
        while (win.pollEvent(evnt)) {
            switch (evnt.type) {
            case Event::Closed:
                win.close();
                break;
            }
        }

        music.setVolume((abs(music_vol - 970) / 1260) * 100);
        coins_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        hearts_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        key_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        wooden_gate_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        jump_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        movement_sound.setVolume((abs(sfx_vol - 970) / 1260) * 5);
        damage_sound.setVolume((abs(sfx_vol - 970) / 1260) * 50);
        dead_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        lava_music.setVolume((abs(sfx_vol - 970) / 1260) * 150);
        jumper_sound.setVolume((abs(sfx_vol - 970) / 1260) * 10);



        HealthBarP1.setPosition(15.0f, camera.getCenter().y - 575);
        HealthBarP2.setPosition(1630.0f, camera.getCenter().y - 575);
        score.setPosition(900, camera.getCenter().y - 520);
        score_num.setPosition(960, camera.getCenter().y - 525);
        screen_rect.setPosition(camera.getCenter().x - 960, camera.getCenter().y - 540);
        game_over.setPosition(camera.getCenter().x - 960, camera.getCenter().y - 540);


        if (dead)
        {
            HealthBarP1.setTexture(Health_Bar1[health1]);
            HealthBarP2.setTexture(Health_Bar2[health2]);
            if (color_cnt < 200)
                color_cnt += 2;
            screen_rect.setFillColor(sf::Color(0, 0, 0, color_cnt));
            if (color_cnt == 200) {
                if (gameOver_delay != 180) gameOver_delay++;
                else { indicator = 2; return; }
            }
        }


        if (Keyboard::isKeyPressed(Keyboard::Escape) || indicator != 1) {
            if (!indicator) {
                indicator = -1; goto render;
            }
            pauseMenu(indicator, 4, { camera.getCenter().x, camera.getCenter().y });
        }

        if (indicator == 2 || indicator == 4) return;

        if (indicator == 3) soundControl(), indicator = 0;

        //fade to black at the end of the level
        if (complete) {
            end.update(4);
            if (end.retrn()) {
                return;
            }
        }

        if (indicator == 1) {
            if (!dead && !complete) {

                //View
                if ((sPlayer1.getPosition().y + sPlayer2.getPosition().y) / 2 < 540 &&
                    (sPlayer1.getPosition().y + sPlayer2.getPosition().y) / 2 > -660 &&
                    sPlayer1.getPosition().y + sPlayer1.getGlobalBounds().height <= camera.getCenter().y + (1080 / 2) &&
                    sPlayer1.getPosition().y + sPlayer1.getGlobalBounds().height >= camera.getCenter().y - (1080 / 2) &&
                    sPlayer2.getPosition().y + sPlayer2.getGlobalBounds().height <= camera.getCenter().y + (1080 / 2) &&
                    sPlayer2.getPosition().y + sPlayer2.getGlobalBounds().height >= camera.getCenter().y - (1080 / 2))
                    camera.setCenter(960, (sPlayer1.getPosition().y + sPlayer2.getPosition().y) / 2);

                grounded1 = 0, grounded2 = 0;
                //collision with upper rects
                ground_rect.update(grounded1, grounded2, sPlayer1, sPlayer2);
                ground_Left_rect.update(grounded1, grounded2, sPlayer1, sPlayer2);
                middle_ground_rect.update(grounded1, grounded2, sPlayer1, sPlayer2, 1, { 970,1090 });
                squared_tile.setPosition(middle_ground_rect.getpos().x - 30, middle_ground_rect.getpos().y);
                key.setPosition(middle_ground_rect.getpos().x + 30, middle_ground_rect.getpos().y - 50);
                first_Tile_rect1.update(grounded1, grounded2, sPlayer1, sPlayer2);
                second_Tile_rect1.update(grounded1, grounded2, sPlayer1, sPlayer2);
                third_Tile_rect1.update(grounded1, grounded2, sPlayer1, sPlayer2);
                fourth_Tile_rect1.update(grounded1, grounded2, sPlayer1, sPlayer2);
                lower_right_Tile_rect1.update(grounded1, grounded2, sPlayer1, sPlayer2);
                upper_right_Tile_rect1.update(grounded1, grounded2, sPlayer1, sPlayer2);
                gate_Tile_rect1.update(grounded1, grounded2, sPlayer1, sPlayer2);
                lower_left_Tile_rect1.update(grounded1, grounded2, sPlayer1, sPlayer2);
                upper_left_Tile_rect1.update(grounded1, grounded2, sPlayer1, sPlayer2);

                //collision with lower rects
                first_Tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                second_Tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                third_Tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                fourth_Tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                lower_right_Tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                upper_right_Tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                lower_left_Tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                upper_left_Tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);

                //collision to the right of the tiles
                second_Tile_rect3.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                fourth_Tile_rect3.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);

                //collision to the left of the tiles        
                first_Tile_rect3.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                third_Tile_rect3.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);

                //Moving tile
                moving_tile.update(grounded1, grounded2, sPlayer1, sPlayer2, { {550,1200},{-900,-700} });
                movingTile.setPosition(moving_tile.getpos().x - 50, moving_tile.getpos().y);

                //transition
                start.update(4);

                //Acid
                HealthBarP1.setTexture(Health_Bar1[health1]);
                HealthBarP2.setTexture(Health_Bar2[health2]);

                //canons
                lower_right_canon.update(b1, b2, sPlayer1, sPlayer2);
                upper_right_canon.update(b1, b2, sPlayer1, sPlayer2);

                //coins & hearts animations counter. clock starts from 0 and starts counting,once it reaches > 0.08 seconds the counters increase by 1 up until it reaches 6 (the number of photos in the sprite sheet).
                // then we make each counter modulus 6 as to reset it to 0 and start over. if you increase 0.08,the animation becomes slower.
                if (switch_time.getElapsedTime().asSeconds() > 0.08) {
                    coins_cnt++;
                    hearts_cnt++;
                    switch_time.restart();//resets the clock back to 0.
                }
                coins_cnt %= 6;
                hearts_cnt %= 6;

                //the events the characters will do in the game(player movements..etc).also it increases the animation counter for both players on x and y axis.
                //for example, if the player clicks A player 1 will move to the left so ctrY goes to row 3 (remember it's 0 based!) where the walking to the left
                //animations happens and starts increasing the counter on x once it reaches 2 we modulus it by 2 and restart over.
                if (Keyboard::isKeyPressed(Keyboard::Key::W)) {
                    sPlayer1.move(0, -5);
                    if (grounded1) {
                        jump_sound.play();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
                    sPlayer1.move(-5, 0);
                    ctrY = 3;

                    if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                        ctrX++;
                        player_switch1.restart();
                    }

                    if (movement.getElapsedTime().asSeconds() > 0.5) {
                        movement_sound.play();
                        movement.restart();
                    }
                }


                if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
                    sPlayer1.move(5, 0);
                    ctrY = 2;

                    if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                        ctrX++;
                        player_switch1.restart();
                    }
                    if (movement.getElapsedTime().asSeconds() > 0.5) {
                        movement_sound.play();
                        movement.restart();
                    }
                }


                if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::D)) {
                    sPlayer1.move(3, 3);
                    ctrY = 2;
                    movement_sound.pause();
                    if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                        ctrX++;
                        player_switch1.restart();
                    }
                }


                if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::A)) {
                    sPlayer1.move(-3, 3);
                    ctrY = 3;
                    movement_sound.pause();
                    if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                        ctrX++;
                        player_switch1.restart();
                    }
                }


                if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
                    sPlayer2.move(0, -5);
                    if (grounded2) {
                        jump_sound.play();
                    }
                }


                if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                    sPlayer2.move(-5, 0);
                    ctrY2 = 3;

                    if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                        ctrX2++;
                        player_switch2.restart();
                    }
                    if (movement.getElapsedTime().asSeconds() > 0.5) {
                        movement_sound.play();
                        movement.restart();
                    }
                }


                if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                    sPlayer2.move(5, 0);
                    ctrY2 = 2;

                    if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                        ctrX2++;
                        player_switch2.restart();
                    }
                    if (movement.getElapsedTime().asSeconds() > 0.5) {
                        movement_sound.play();
                        movement.restart();
                    }
                }


                if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                    sPlayer2.move(3, 3);
                    ctrY2 = 2;
                    movement_sound.pause();
                    if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                        ctrX2++;
                        player_switch2.restart();

                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                    sPlayer2.move(-3, 3);
                    ctrY2 = 3;
                    movement_sound.pause();
                    if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                        ctrX2++;
                        player_switch2.restart();
                    }
                }

                ctrX %= 2;
                sPlayer1.setTextureRect(IntRect(ctrX * 32, ctrY * 34, 32, 28));

                ctrX2 %= 2;
                sPlayer2.setTextureRect(IntRect(ctrX2 * 32, ctrY2 * 33, 32, 28));


                //Collisions with window border
                if (sPlayer1.getGlobalBounds().intersects(b1.getGlobalBounds())) {
                    sPlayer1.move(5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::A)) {
                        sPlayer1.move(3, 3);
                    }
                }

                if (sPlayer1.getGlobalBounds().intersects(b2.getGlobalBounds())) {
                    sPlayer1.move(-5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::D)) {
                        sPlayer1.move(-3, 3);
                    }
                }

                if (sPlayer2.getGlobalBounds().intersects(b1.getGlobalBounds())) {
                    sPlayer2.move(5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                        sPlayer2.move(3, 3);
                    }
                }

                if (sPlayer2.getGlobalBounds().intersects(b2.getGlobalBounds())) {
                    sPlayer2.move(-5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                        sPlayer2.move(-3, 3);
                    }
                }

                //Collision with key
                if (sPlayer1.getGlobalBounds().intersects(key.getGlobalBounds())) {
                    gate.setTexture(opened_gate_texture);
                    gate.setScale(1.43f, 1.43f);
                    key.setScale(0, 0);
                    key_sound.play();
                    wooden_gate_sound.play();
                    keys[3] = true;
                }

                if (sPlayer2.getGlobalBounds().intersects(key.getGlobalBounds())) {
                    gate.setTexture(opened_gate_texture);
                    gate.setScale(1.43f, 1.43f);
                    key.setScale(0, 0);
                    key_sound.play();
                    wooden_gate_sound.play();
                    keys[3] = true;
                }

                //collision with jumpers
                for (int i = 0; i < 4; i++) {
                    if (sPlayer1.getGlobalBounds().intersects(jumpers[i].getGlobalBounds())) {
                        istaken = true;
                        on1 = true;
                        grounded1 = 0;
                        velocityY1 = 28;
                        num1 = i;
                        jumper_sound.play();
                    }
                }

                for (int i = 0; i < 4; i++) {
                    if (sPlayer2.getGlobalBounds().intersects(jumpers[i].getGlobalBounds())) {
                        istaken = true;
                        on2 = true;
                        grounded2 = 0;
                        velocityY2 = 28;
                        num2 = i;
                        jumper_sound.play();
                    }
                }

                if (on1) {
                    if (jumper_switch1.getElapsedTime().asSeconds() > 0.08) {
                        jumper_cnt1++;
                        jumper_switch1.restart();
                    }
                }

                if (on2) {
                    if (jumper_switch2.getElapsedTime().asSeconds() > 0.08) {
                        jumper_cnt2++;
                        jumper_switch2.restart();
                    }
                }

                if (jumper_cnt1 == 6)
                    on1 = false;

                if (jumper_cnt2 == 6)
                    on2 = false;

                jumper_cnt1 %= 6;
                jumper_cnt2 %= 6;

                if (num1 == num2 && on1)
                    jumperSprite[num1].setTextureRect(IntRect(jumper_cnt1 * jumper_texture_size.x, 0, jumper_texture_size.x, jumper_texture_size.y));

                else if (num1 == num2 && on2)
                    jumperSprite[num2].setTextureRect(IntRect(jumper_cnt2 * jumper_texture_size.x, 0, jumper_texture_size.x, jumper_texture_size.y));

                else if (num1 != num2) {
                    jumperSprite[num1].setTextureRect(IntRect(jumper_cnt1 * jumper_texture_size.x, 0, jumper_texture_size.x, jumper_texture_size.y));
                    jumperSprite[num2].setTextureRect(IntRect(jumper_cnt2 * jumper_texture_size.x, 0, jumper_texture_size.x, jumper_texture_size.y));
                }

                //Collision with acid
                HealthBarP1.setTexture(Health_Bar1[health1]);
                HealthBarP2.setTexture(Health_Bar2[health2]);

                for (int i = 0; i < 15; i++) {
                    if (sPlayer1.getGlobalBounds().intersects(acid_rect[i].getGlobalBounds())) {
                        damage1 = 1;
                        break;
                    }
                }

                if (damage1) {
                    damage1 = 0;
                    if (hb_delay1 == 60) {

                        if (health1 > 0) {
                            health1--;
                            damage_sound.play();
                        }
                        hb_delay1 = 0;
                    }
                    else
                        hb_delay1++;
                }
                else {
                    hb_delay1 = 60;
                }

                for (int i = 0; i < 15; i++) {
                    if (sPlayer2.getGlobalBounds().intersects(acid_rect[i].getGlobalBounds())) {
                        damage2 = 1;
                        break;
                    }
                }

                if (damage2) {
                    damage2 = 0;
                    if (hb_delay2 == 60) {

                        if (health2 > 0) {
                            health2--;
                            damage_sound.play();
                        }
                        hb_delay2 = 0;
                    }
                    else
                        hb_delay2++;
                }
                else {
                    hb_delay2 = 60;
                }

                //lava
                if (istaken)
                    lava.move(0, -0.8);

                lava_rect.setPosition(lava.getPosition().x, lava.getPosition().y + 30);

                if (sPlayer1.getGlobalBounds().intersects(lava_rect.getGlobalBounds())) {
                    health1 = 0;
                    damage_sound.play();
                }

                if (sPlayer2.getGlobalBounds().intersects(lava_rect.getGlobalBounds())) {
                    health2 = 0;
                    damage_sound.play();
                }

                //collision with ladders
                for (int i = 0; i < 2; i++) {
                    //Player1
                    if (sPlayer1.getGlobalBounds().intersects(ladder_rect[i].getGlobalBounds())) {
                        ctrY = 1;
                        velocityY1 = 1;
                    }

                    if (sPlayer1.getGlobalBounds().intersects(ladder_rect[i].getGlobalBounds()) && Keyboard::isKeyPressed(Keyboard::Key::D)) {
                        ctrY = 1;
                        velocityY1 = -8;
                    }

                    if (sPlayer1.getGlobalBounds().intersects(ladder_rect[i].getGlobalBounds()) && Keyboard::isKeyPressed(Keyboard::Key::A)) {
                        ctrY = 1;
                        velocityY1 = -8;
                    }

                    if (sPlayer1.getGlobalBounds().intersects(ladder_rect[i].getGlobalBounds()) && Keyboard::isKeyPressed(Keyboard::Key::W)) {
                        ctrY = 1;
                        velocityY1 = 3;

                        if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                            ctrX++;
                            player_switch1.restart();
                        }

                        if (movement.getElapsedTime().asSeconds() > 0.5) {
                            movement_sound.play();
                            movement.restart();
                        }
                    }

                    //Player2
                    if (sPlayer2.getGlobalBounds().intersects(ladder_rect[i].getGlobalBounds())) {
                        ctrY2 = 1;
                        velocityY2 = 1;
                    }

                    if (sPlayer1.getGlobalBounds().intersects(ladder_rect[i].getGlobalBounds()) && Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                        ctrY = 1;
                        velocityY1 = -6;
                    }

                    if (sPlayer1.getGlobalBounds().intersects(ladder_rect[i].getGlobalBounds()) && Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                        ctrY = 1;
                        velocityY1 = -6;
                    }

                    if (sPlayer2.getGlobalBounds().intersects(ladder_rect[i].getGlobalBounds()) && Keyboard::isKeyPressed(Keyboard::Key::Up)) {
                        ctrY2 = 1;
                        velocityY2 = 3;

                        if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                            ctrX2++;
                            player_switch2.restart();
                        }

                        if (movement.getElapsedTime().asSeconds() > 0.5) {
                            movement_sound.play();
                            movement.restart();
                        }
                    }
                }

                //Collision with hearts
                if (sPlayer1.getGlobalBounds().intersects(hearts[0].getGlobalBounds()) && health1 < 3) {
                    hearts[0].setScale(0, 0);
                    hearts_sound.play();
                    health1++;
                }

                if (sPlayer2.getGlobalBounds().intersects(hearts[1].getGlobalBounds()) && health2 < 3) {
                    hearts[1].setScale(0, 0);
                    hearts_sound.play();
                    health2++;
                }

                //Collision with coins
                score_num.setString("x" + to_string(coins_cntr));
                for (int i = 0; i < 3; i++) {
                    if (sPlayer1.getGlobalBounds().intersects(coins[i].getGlobalBounds())) {
                        coins[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }

                    if (sPlayer1.getGlobalBounds().intersects(coins1[i].getGlobalBounds())) {
                        coins1[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }

                    if (sPlayer1.getGlobalBounds().intersects(coins2[i].getGlobalBounds())) {
                        coins2[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }

                    if (sPlayer1.getGlobalBounds().intersects(coins3[i].getGlobalBounds())) {
                        coins3[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }

                    if (sPlayer2.getGlobalBounds().intersects(coins[i].getGlobalBounds())) {
                        coins[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }

                    if (sPlayer2.getGlobalBounds().intersects(coins1[i].getGlobalBounds())) {
                        coins1[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }

                    if (sPlayer2.getGlobalBounds().intersects(coins2[i].getGlobalBounds())) {
                        coins2[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }

                    if (sPlayer2.getGlobalBounds().intersects(coins3[i].getGlobalBounds())) {
                        coins3[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }
                }

                //Collision with gate
                if (sPlayer1.getGlobalBounds().intersects(gate_rect.getGlobalBounds())
                    && sPlayer2.getGlobalBounds().intersects(gate_rect.getGlobalBounds())
                    && keys[3] == true && coins_cntr == 41) {

                    gate_pass[3] = true;
                    complete = 1;
                }


                //gravity of player 1
                if (!grounded1)
                {
                    velocityY1 -= 1;
                }

                if (grounded1)
                {
                    velocityY1 = 0;
                    if (Keyboard::isKeyPressed(Keyboard::Key::W)) {
                        velocityY1 = 15;
                    }
                }

                sPlayer1.move(0, -velocityY1);

                //gravity of player 2
                if (!grounded2) {
                    velocityY2 -= 1;
                }

                if (grounded2) {
                    velocityY2 = 0;
                    if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
                        velocityY2 = 15;
                    }
                }

                sPlayer2.move(0, -velocityY2);

                if (health1 == 0) {
                    sPlayer1.setTextureRect(IntRect(0, 11 * 32.3, 32, 28));
                    dead = true;
                }

                if (health2 == 0) {
                    sPlayer2.setTextureRect(IntRect(0, 4 * 32.4, 32, 28));
                    dead = true;
                }
            }
        }
    render:;

        //drawing and displaying objects
        win.clear();

        win.setView(camera);
        for (int i = 0; i < 3; i++)
            win.draw(background[i]);

        win.draw(key);
        win.draw(tile1);
        win.draw(tile2);
        win.draw(tile3);
        win.draw(tile4);
        win.draw(tile5);
        win.draw(tile6);
        win.draw(tile7);
        win.draw(tile8);
        win.draw(tile9);
        win.draw(tile10);
        win.draw(squared_tile);
        win.draw(movingTile);



        for (int i = 0; i < 4; i++)
            win.draw(jumperSprite[i]);

        win.draw(cannon1);
        win.draw(cannon2);
        win.draw(gate);
        win.draw(score);
        win.draw(score_num);

        for (int i = 0; i < 2; i++) {
            win.draw(ladder[i]);
        }
        lower_right_canon.draw();
        upper_right_canon.draw();
        //Loop to make coin animations
        for (int i = 0; i < 3; i++) {
            if (!dead)
            {
                coins[i].setTextureRect(IntRect(coins_cnt * coins_texture_size.x, 0, coins_texture_size.x, coins_texture_size.y));
                coins1[i].setTextureRect(IntRect(coins_cnt * coins_texture_size.x, 0, coins_texture_size.x, coins_texture_size.y));
                coins2[i].setTextureRect(IntRect(coins_cnt * coins_texture_size.x, 0, coins_texture_size.x, coins_texture_size.y));
                coins3[i].setTextureRect(IntRect(coins_cnt * coins_texture_size.x, 0, coins_texture_size.x, coins_texture_size.y));
            }
            win.draw(coins[i]);
            win.draw(coins1[i]);
            win.draw(coins2[i]);
            win.draw(coins3[i]);
        }

        for (int i = 0; i < 2; i++) {
            if (!dead)
                hearts[i].setTextureRect(IntRect(hearts_cnt * hearts_texture_size.x, 0, hearts_texture_size.x, hearts_texture_size.y));
            win.draw(hearts[i]);
        }

        win.draw(HealthBarP1);
        win.draw(HealthBarP2);
        win.draw(sPlayer1);
        win.draw(sPlayer2);

        win.draw(lava);

        win.draw(screen_rect);

        if (color_cnt > 110) {
            win.draw(game_over);
            if (!sound_played) {
                music.pause();
                dead_sound.play();
                sound_played = true;
            }
        }
        start.draw();
        end.draw();
        win.display();
    }

    camera.setCenter(1920 / 2, 1080 / 2);
}


void lvl3()
{
    int ctrX = 0, ctrX2 = 0, ctrY = 0, ctrY2 = 0, velocityY = 0, coins_cnt = 0, sound_cnt = 0, coins_score = 0, hearts_cnt = 0,
        hb_delay1 = 60, hb_delay2 = 60, gameOver_delay = 0, color_cnt = 0;

    float velocityY1 = 0, velocityY2 = 0;
    bool dead = false, buttonPressed_ = false, grounded = 0, grounded1 = false, grounded2 = false, sound_played = false, complete = 0;


    // this is the sprites and there textures we are using in the game


    Texture Player1, Player2, mapp, flooor, tiles, gatefloor, block, spikes, coins_texture, key_texture, button_texture, buttonpressed_texture, door_texture, gate_texture, opened_gate_texture, score_texture, health_bar1_texture[4],
        health_bar2_texture[4], hearts_texture, moving_tile_tex, game_over_texture;

    Sprite sPlayer1, sPlayer2, map, floor, tile[8], floorgate[2], blockk[2], spike[4], coins[8], button, door, gate, key, health_bar1, health_bar2, score, hearts[2], moving_tile, game_over;

    coins_texture.loadFromFile("coins1.png");
    key_texture.loadFromFile("key.png");
    buttonpressed_texture.loadFromFile("Button_Pressed.png");
    door_texture.loadFromFile("gate.png");
    opened_gate_texture.loadFromFile("open_door.png");
    gate_texture.loadFromFile("Gate1.png");
    button_texture.loadFromFile("button.png");
    mapp.loadFromFile("map_vignette.png");
    map.setTexture(mapp);
    hearts_texture.loadFromFile("hearts.png");
    health_bar1_texture[3].loadFromFile("P1_health_bar_full.png");
    health_bar1_texture[2].loadFromFile("P1_health_bar_1.png");
    health_bar1_texture[1].loadFromFile("P1_health_bar_2.png");
    health_bar1_texture[0].loadFromFile("P1_health_bar_empty.png");
    health_bar2_texture[3].loadFromFile("P2_health_bar_full.png");
    health_bar2_texture[2].loadFromFile("P2_health_bar_1.png");
    health_bar2_texture[1].loadFromFile("P2_health_bar_2.png");
    health_bar2_texture[0].loadFromFile("P2_health_bar_empty.png");
    score_texture.loadFromFile("Score_Coin.png");
    moving_tile_tex.loadFromFile("floor3.png");
    game_over_texture.loadFromFile("Game_Over.png");

    RectangleShape b3(Vector2f(5, 1900));

    b3.setPosition(1915, 5); // right border

    kaboom canonn1(Vector2f(1750, 430), -1);
    kaboom canonn2(Vector2f(1750, 630), -1);
    kaboom canonn3(Vector2f(-5, 430), 1);
    kaboom canonn4(Vector2f(-5, 630), 1);

    spikes.loadFromFile("Peaks.png");
    block.loadFromFile("small.png");

    for (int i = 0; i < 2; i++)
    {
        blockk[i].setTexture(block);

    }

    blockk[0].setPosition(470, 250);  //between spawn tile and gate close tile
    blockk[1].setPosition(800, 600);  //beween middle tiles of top and bottom tiles between cannons


    tiles.loadFromFile("floor4.png");

    Player1.loadFromFile("Player1.png");
    Player2.loadFromFile("Player2.png");


    gatefloor.loadFromFile("floor2.png");
    for (int i = 0; i < 2; i++)
    {
        floorgate[i].setTexture(gatefloor);

    }
    floorgate[0].setPosition(1100, 200);
    floorgate[1].setPosition(1500, 200);


    for (int i = 0; i < 4; i++)
    {
        spike[i].setTexture(spikes);
        spike[i].setScale(2.5, 2.5);

    }

    //health bar 2
    health_bar2.setPosition(1630.0f, -35.0f);
    health_bar2.setScale(1.4, 1.4);


    //health bar 1
    health_bar1.setPosition(15.0f, -35.0f);
    health_bar1.setScale(1.4, 1.4);

    //game over
    game_over.setTexture(game_over_texture);

    //key
    key.setTexture(key_texture);
    key.setPosition(1580, 960);
    key.setScale(0.14, 0.15);


    //buttons
    button.setTexture(button_texture);
    button.setPosition(1600, 145);
    button.setScale(0.6f, 0.6f);


    //the portal (wooden gate)
    door.setTexture(door_texture);
    door.setPosition(1750, 85);
    door.setScale(1.2f, 1.2f);

    //the door in front of the key (metal gate)
    gate.setTexture(gate_texture);
    gate.setPosition(1350, -10);
    gate.setScale(0.87, 0.87);


    spike[0].setPosition(880, 240); // spikes on the tile beside the gate
    spike[1].setPosition(1170, 690); // spikes on the tile between the bottom cannons
    spike[2].setPosition(850, 990); //floor spikes on the left
    spike[3].setPosition(1400, 990); //floor spikes on the right


    flooor.loadFromFile("ground.png");
    floor.setTexture(flooor);
    floor.setPosition(0, 1000);

    for (int i = 0; i < 8; i++)
    {
        tile[i].setTexture(tiles);

    }

    tile[0].setPosition(-350, 500);  // left bottom cannon tile 
    tile[1].setPosition(-350, 700);  //top left cannon tile 
    tile[2].setPosition(1650, 700);  //right bottom cannon tile  
    tile[3].setPosition(1650, 500);  //top right cannon tile
    tile[4].setPosition(600, 250);  //top gate beside tile 
    tile[5].setPosition(-200, 200); //top tile to spawn on 
    tile[6].setPosition(200, 500); //tile between the top cannons
    tile[7].setPosition(900, 700); //tile between the bottom cannons
    moving_tile.setPosition(1450, 1000);
    moving_tile.setTexture(moving_tile_tex);


    //Background music
    Music music;
    music.openFromFile("SoundTrack.wav");
    music.play();
    music.setVolume(15.0f);

    //Sounds
    Sound coins_sound;
    SoundBuffer coins_sound_buffer;
    coins_sound_buffer.loadFromFile("coin sound effect.wav");
    coins_sound.setBuffer(coins_sound_buffer);
    coins_sound.setVolume(10.0f);

    Sound hearts_sound;
    SoundBuffer hearts_sound_buffer;
    hearts_sound_buffer.loadFromFile("Heart Taken.wav");
    hearts_sound.setBuffer(hearts_sound_buffer);

    Sound key_sound;
    SoundBuffer key_sound_buffer;
    key_sound_buffer.loadFromFile("Key is taken.wav");
    key_sound.setBuffer(key_sound_buffer);
    key_sound.setVolume(200.0f);

    Sound metal_gate_sound;
    SoundBuffer metal_gate_sound_buffer;
    metal_gate_sound_buffer.loadFromFile("Metal_Door_Opening.wav");
    metal_gate_sound.setBuffer(metal_gate_sound_buffer);


    Sound wooden_gate_sound;
    SoundBuffer wooden_gate_sound_buffer;
    wooden_gate_sound_buffer.loadFromFile("Wooden Door Opening.wav");
    wooden_gate_sound.setBuffer(wooden_gate_sound_buffer);

    Sound button_sound;
    SoundBuffer button_sound_buffer;
    button_sound_buffer.loadFromFile("Button Pushed.wav");
    button_sound.setBuffer(button_sound_buffer);

    Sound jump_sound;
    SoundBuffer jump_sound_buffer;
    jump_sound_buffer.loadFromFile("Jump_Sound.wav");
    jump_sound.setBuffer(jump_sound_buffer);
    jump_sound.setVolume(30.0f);

    Sound movement_sound;
    SoundBuffer movement_sound_buffer;
    movement_sound_buffer.loadFromFile("Movement.wav");
    movement_sound.setBuffer(movement_sound_buffer);
    movement_sound.setVolume(1.0f);

    Sound damage_sound;
    SoundBuffer damage_sound_buffer;
    damage_sound_buffer.loadFromFile("Damage Taken.wav");
    damage_sound.setBuffer(damage_sound_buffer);

    Sound dead_sound;
    SoundBuffer dead_sound_buffer;
    dead_sound_buffer.loadFromFile("Game Over.wav");
    dead_sound.setBuffer(dead_sound_buffer);

    //score
    score.setTexture(score_texture);
    score.scale(0.3f, 0.3f);
    score.setPosition(900, 20);
    Font font;
    font.loadFromFile("Amatic-Bold.ttf");
    Text score_num;
    score_num.setFont(font);
    score_num.setFillColor(sf::Color::White);
    score_num.setPosition(960, 15);
    score_num.setCharacterSize(50);

    //hearts
    Vector2u hearts_texture_size = hearts_texture.getSize();
    hearts_texture_size.x /= 6;
    for (size_t i = 0; i < 2; i++) {
        hearts[i].setTexture(hearts_texture);
        hearts[i].setPosition(1300 + (i * 210), 140.0f);
        hearts[i].setTextureRect(IntRect(0, 0, hearts_texture_size.x, hearts_texture_size.y));
        hearts[i].setScale(0.5f, 0.5f);
    }

    //coins
    Vector2u coins_texture_size = coins_texture.getSize();
    coins_texture_size.x /= 6;
    for (int i = 0; i < 8; i++) {
        //lower left tile coins
        coins[i].setTexture(coins_texture);
        coins[i].setTextureRect(IntRect(0, 0, coins_texture_size.x, coins_texture_size.y));
        coins[i].setScale(0.2f, 0.2f);
    }
    coins[0].setPosition(500, 210);
    coins[1].setPosition(800, 210);
    coins[2].setPosition(1000, 210);
    coins[3].setPosition(830, 560);
    coins[4].setPosition(865, 945);
    coins[5].setPosition(865, 885);
    coins[6].setPosition(600, 955);
    coins[7].setPosition(1200, 955);


    sPlayer1.setTexture(Player1);
    sPlayer1.setTextureRect(IntRect(0, 0 * 35, 32, 40));
    sPlayer1.setScale(3, 3);

    sPlayer2.setTexture(Player2);
    sPlayer2.setTextureRect(IntRect(0, 0 * 36, 32, 40));
    sPlayer2.setScale(3, 3);
    sPlayer2.setPosition(100, 0);


    //Spikes coll
    collup upperspike0(Vector2f(spike[0].getPosition().x + 30, spike[0].getPosition().y + 10), Vector2f(3, 5));
    collup upperspike1(Vector2f(spike[1].getPosition().x + 30, spike[1].getPosition().y + 10), Vector2f(3, 5));
    collup upperspike2(Vector2f(spike[2].getPosition().x + 30, spike[2].getPosition().y + 10), Vector2f(3, 5));
    collup upperspike3(Vector2f(spike[3].getPosition().x + 30, spike[3].getPosition().y + 10), Vector2f(3, 5));


    //rectangles above the tiles (used in collision)
    collup upperblock2(Vector2f(blockk[1].getPosition().x + 30, blockk[1].getPosition().y), Vector2f(40, 10));
    collup upperblock(Vector2f(blockk[0].getPosition().x + 30, blockk[0].getPosition().y), Vector2f(40, 10));
    collup upper_left_tile_rect___(Vector2f(floorgate[0].getPosition().x + 180, floorgate[0].getPosition().y), Vector2f(900, 10));
    collup upper_left_tile_rect__(Vector2f(floor.getPosition()), Vector2f(2000, 10));
    collup upper_left_tile_rect(Vector2f(tile[0].getPosition()), Vector2f(550, 10));
    collup lower_left_tile_rect(Vector2f(tile[1].getPosition()), Vector2f(550, 10));
    collup upper_right_tile_rect(Vector2f(tile[2].getPosition().x + 200, tile[2].getPosition().y), Vector2f(435, 10));
    collup lower_right_tile_rect(Vector2f(tile[3].getPosition().x + 210, tile[3].getPosition().y), Vector2f(425, 10));
    collup upper_middle_tile_rect(Vector2f(tile[4].getPosition().x + 180, tile[4].getPosition().y), Vector2f(280, 10));
    collup upper_middle_tile_rect_(Vector2f(tile[5].getPosition().x + 180, tile[5].getPosition().y), Vector2f(280, 10));
    collup lower_middle_tile_rect(Vector2f(tile[6].getPosition().x + 180, tile[6].getPosition().y), Vector2f(280, 10));
    collup lower_middle_tile_rect_(Vector2f(tile[7].getPosition().x + 180, tile[7].getPosition().y), Vector2f(280, 10));
    collup moving_t(Vector2f(1450, 1000), Vector2f(280, 10), -10);
    collup button_rect(Vector2f(button.getPosition().x + 30, button.getPosition().y + 40), Vector2f(1, 5));
    //rectangles under the tiles (used in collision)
    colldown underblock2(Vector2f(blockk[1].getPosition().x + 30, blockk[1].getPosition().y + 60), Vector2f(40, 10));
    colldown underblock(Vector2f(blockk[0].getPosition().x + 30, blockk[0].getPosition().y + 60), Vector2f(40, 10));
    colldown upper_left_tile_rect2(Vector2f(tile[0].getPosition().x - 50, tile[0].getPosition().y + 80), Vector2f(535, 10));
    colldown lower_left_tile_rect2(Vector2f(tile[1].getPosition().x - 50, tile[1].getPosition().y + 80), Vector2f(545, 10));
    colldown upper_right_tile_rect2(Vector2f(tile[2].getPosition().x + 140, tile[2].getPosition().y + 90), Vector2f(425, 10));
    colldown lower_right_tile_rect2(Vector2f(tile[3].getPosition().x + 140, tile[3].getPosition().y + 90), Vector2f(425, 10));
    colldown upper_middle_tile_rect2(Vector2f(tile[4].getPosition().x + 180, tile[4].getPosition().y + 80), Vector2f(280, 10));
    colldown upper_middle_tile_rect2_(Vector2f(tile[5].getPosition().x + 180, tile[5].getPosition().y + 80), Vector2f(280, 10));
    colldown lower_middle_tile_rect2(Vector2f(tile[6].getPosition().x + 180, tile[6].getPosition().y + 80), Vector2f(280, 10));
    colldown lower_middle_tile_rect2_(Vector2f(tile[7].getPosition().x + 180, tile[7].getPosition().y + 80), Vector2f(280, 10));

    //right rectangles (used in collision)
    colright topright(Vector2f(tile[5].getPosition().x + 475, tile[5].getPosition().y), Vector2f(5, 80));
    colright leftmid2(Vector2f(tile[7].getPosition().x + 480, tile[7].getPosition().y), Vector2f(5, 80));
    colright leftmid(Vector2f(tile[6].getPosition().x + 480, tile[6].getPosition().y + 0), Vector2f(5, 80));
    colright rightblock2(Vector2f(blockk[1].getPosition().x + 70, blockk[1].getPosition().y + 15), Vector2f(10, 40));
    colright rightblock(Vector2f(blockk[0].getPosition().x + 70, blockk[0].getPosition().y + 15), Vector2f(10, 40));
    colright lower_left_tile_rect3(Vector2f(tile[1].getPosition().x + 500, tile[1].getPosition().y - 300), Vector2f(5, 400));
    colright upper_middle_tile_rect3(Vector2f(tile[2].getPosition().x + 480, tile[2].getPosition().y + 9), Vector2f(5, 65));
    colright upper_middle_tile_rect3_(Vector2f(tile[3].getPosition().x + 930, tile[3].getPosition().y + 9), Vector2f(5, 65));
    colright lower_middle_tile_rect3(Vector2f(tile[4].getPosition().x + 480, tile[4].getPosition().y + 9), Vector2f(5, 65));


    //left rectangles (used in collision)
    colleft rightmid2(Vector2f(tile[7].getPosition().x + 160, tile[7].getPosition().y), Vector2f(5, 80));  //right top
    colleft rightmid(Vector2f(tile[6].getPosition().x + 160, tile[6].getPosition().y + 0), Vector2f(5, 80)); //right bottom 
    colleft leftblock2(Vector2f(blockk[1].getPosition().x + 20, blockk[1].getPosition().y + 15), Vector2f(10, 40));
    colleft leftblock(Vector2f(blockk[0].getPosition().x + 20, blockk[0].getPosition().y + 15), Vector2f(10, 40));
    colleft upper_right_tile_rect4_(Vector2f(floorgate[0].getPosition().x + 190, floorgate[0].getPosition().y + 15), Vector2f(5, 95));
    upper_right_tile_rect4_.rotation(-30.0f);
    colleft upper_middle_tile_rect4(Vector2f(tile[2].getPosition().x + 130, tile[2].getPosition().y - 300), Vector2f(5, 400));
    colleft lower_middle_tile_rect4(Vector2f(tile[4].getPosition().x + 150, tile[4].getPosition().y + 9), Vector2f(5, 65));

    //transition
    transition start(0), end(1);


    //Skeletons
    enemy e1(Vector2f(100, 870), Vector2f(700, 200));
    enemy e2(Vector2f(330, 370), Vector2f(300, 200));
    enemy e3(Vector2f(1100, 870), Vector2f(700, 200));



    RectangleShape b1(Vector2f(5, 1080)), b2(Vector2f(5, 1900)), gate_rect(Vector2f(5, 130)), square_rect(Vector2f(7, 2)),
        square3_rect(Vector2f(5, 40)), square4_rect(Vector2f(5, 40)), screen_rect(Vector2f(1920, 1080));


    b1.setPosition(-20, 1); //left border
    b2.setPosition(1930, 1); // right border
    gate_rect.setPosition(1850, 75); //metal gate rect
    square_rect.setPosition(415, 865);
    square3_rect.setPosition(415, 890);
    square4_rect.setPosition(415, 890);
    screen_rect.setFillColor(sf::Color(0, 0, 0, color_cnt));



    sf::Clock player_switch1, player_switch2, movement_sound_switch, switch_time;


    while (win.isOpen())
    {
        Event event;
        while (win.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                win.close();
        }

        music.setVolume((abs(music_vol - 970) / 1260) * 100);
        coins_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        hearts_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        key_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        metal_gate_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        wooden_gate_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        button_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        jump_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        movement_sound.setVolume((abs(sfx_vol - 970) / 1260) * 5);
        damage_sound.setVolume((abs(sfx_vol - 970) / 1260) * 50);
        dead_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);

        if (dead)
        {
            health_bar1.setTexture(health_bar1_texture[health1]);
            health_bar2.setTexture(health_bar2_texture[health2]);
            if (color_cnt < 200)
                color_cnt += 2;
            screen_rect.setFillColor(sf::Color(0, 0, 0, color_cnt));
            if (color_cnt == 200) {
                if (gameOver_delay != 180) gameOver_delay++;
                else { indicator = 2; return; }
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Escape) || indicator != 1) { if (!indicator) { indicator = -1; goto render; } pauseMenu(indicator, 3); }

        if (indicator == 2 || indicator == 4) return;

        if (indicator == 3) soundControl(), indicator = 0;
        //fade to black at the end of the level
        if (complete) {
            end.update(3);
            if (end.retrn()) {
                return;
            }
        }
        if (indicator == 1) {
            if (!dead && !complete) {
                if (switch_time.getElapsedTime().asSeconds() > 0.08) {
                    coins_cnt++;
                    hearts_cnt++;
                    switch_time.restart();
                }
                coins_cnt %= 6;
                hearts_cnt %= 6;

                //coins & hearts animation 
                if (switch_time.getElapsedTime().asSeconds() > 0.08) {
                    coins_cnt++;
                    hearts_cnt++;
                    switch_time.restart();
                }
                if (Keyboard::isKeyPressed(Keyboard::Key::W)) {
                    sPlayer1.move(0, -5);
                    movement_sound.pause();
                    if (grounded1)
                        jump_sound.play();
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
                    sPlayer1.move(-5, 0);
                    ctrY = 3;
                    if (grounded1) {
                        if (movement_sound_switch.getElapsedTime().asSeconds() > 0.50) {
                            movement_sound.play();
                            movement_sound_switch.restart();
                        }
                    }

                    if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                        ctrX++;
                        player_switch1.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
                    sPlayer1.move(5, 0);
                    ctrY = 2;

                    if (grounded1) {
                        if (movement_sound_switch.getElapsedTime().asSeconds() > 0.50) {
                            movement_sound.play();
                            movement_sound_switch.restart();
                        }
                    }

                    if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                        ctrX++;
                        player_switch1.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::D)) {
                    sPlayer1.move(3, 3);
                    ctrY = 2;

                    if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                        ctrX++;
                        player_switch1.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::A)) {
                    sPlayer1.move(-3, 3);
                    ctrY = 3;

                    if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                        ctrX++;
                        player_switch1.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
                    sPlayer2.move(0, -5);
                    movement_sound.pause();
                    if (grounded2)
                        jump_sound.play();
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                    sPlayer2.move(-5, 0);
                    ctrY2 = 3;
                    if (grounded2) {
                        if (movement_sound_switch.getElapsedTime().asSeconds() > 0.50) {
                            movement_sound.play();
                            movement_sound_switch.restart();
                        }
                    }

                    if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                        ctrX2++;
                        player_switch2.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                    sPlayer2.move(5, 0);
                    ctrY2 = 2;
                    if (grounded2) {
                        if (movement_sound_switch.getElapsedTime().asSeconds() > 0.50) {
                            movement_sound.play();
                            movement_sound_switch.restart();
                        }
                    }

                    if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                        ctrX2++;
                        player_switch2.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                    sPlayer2.move(3, 3);
                    ctrY2 = 2;

                    if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                        ctrX2++;
                        player_switch2.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                    sPlayer2.move(-3, 3);
                    ctrY2 = 3;

                    if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                        ctrX2++;
                        player_switch2.restart();
                    }
                }

                ctrX %= 2;
                sPlayer1.setTextureRect(IntRect(ctrX * 32, ctrY * 34, 32, 28));

                ctrX2 %= 2;
                sPlayer2.setTextureRect(IntRect(ctrX2 * 32, ctrY2 * 33, 32, 28));


                //collision
                if (sPlayer1.getGlobalBounds().intersects(b1.getGlobalBounds())) {
                    sPlayer1.move(5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::A)) {
                        sPlayer1.move(3, 3);
                    }
                }

                if (sPlayer2.getGlobalBounds().intersects(b1.getGlobalBounds())) {
                    sPlayer2.move(5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                        sPlayer2.move(3, 3);
                    }
                }

                if (sPlayer1.getGlobalBounds().intersects(b2.getGlobalBounds())) {
                    sPlayer1.move(-5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::D)) {
                        sPlayer1.move(-3, 3);
                    }
                }

                if (sPlayer2.getGlobalBounds().intersects(b2.getGlobalBounds())) {
                    sPlayer2.move(-5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                        sPlayer2.move(-3, 3);
                    }
                }


                //collision with the spikes
                health_bar1.setTexture(health_bar1_texture[health1]);
                health_bar2.setTexture(health_bar2_texture[health2]);
                if (upperspike3.collision(sPlayer1) || upperspike2.collision(sPlayer1) || upperspike1.collision(sPlayer1) || upperspike0.collision(sPlayer1)) {
                    if (hb_delay1 == 60) {

                        if (health1 > 0) {
                            health1--;
                            damage_sound.play();
                        }
                        hb_delay1 = 0;
                    }
                    else
                        hb_delay1++;
                }
                else {
                    hb_delay1 = 60;
                }
                if (sPlayer1.getGlobalBounds().intersects(hearts[0].getGlobalBounds()) && health1 < 3) {
                    hearts[0].setScale(0, 0);
                    hearts_sound.play();
                    health1++;
                }

                if (sPlayer2.getGlobalBounds().intersects(hearts[1].getGlobalBounds()) && health2 < 3) {
                    hearts[1].setScale(0, 0);
                    hearts_sound.play();
                    health2++;
                }

                if (upperspike3.collision(sPlayer2) || upperspike2.collision(sPlayer2) || upperspike1.collision(sPlayer2) || upperspike0.collision(sPlayer2)) {
                    if (hb_delay2 == 60) {

                        if (health2 > 0) {
                            health2--;
                            damage_sound.play();
                        }
                        hb_delay2 = 0;
                    }
                    else
                        hb_delay2++;
                }
                else {
                    hb_delay2 = 60;
                }


                //coins & hearts animation 
                if (switch_time.getElapsedTime().asSeconds() > 0.08) {
                    coins_cnt++;
                    switch_time.restart();
                }
                coins_cnt %= 6;

                canonn1.update(b3, b2, sPlayer1, sPlayer2);
                canonn2.update(b3, b2, sPlayer1, sPlayer2);
                canonn3.update(b3, b2, sPlayer1, sPlayer2);
                canonn4.update(b3, b2, sPlayer1, sPlayer2);
                //the events the characters will do in the game
                if (sPlayer1.getGlobalBounds().intersects(key.getGlobalBounds())) {
                    key.setScale(0, 0);
                    keys[2] = true;
                    door.setTexture(opened_gate_texture);
                    door.setScale(1.2, 1.2);
                    door.setPosition(1750, 85);
                    gate.setPosition(1350, -200);
                    key_sound.play();
                    wooden_gate_sound.play();
                }

                if (sPlayer2.getGlobalBounds().intersects(key.getGlobalBounds())) {
                    key.setScale(0, 0);
                    keys[2] = true;
                    door.setTexture(opened_gate_texture);
                    door.setScale(1.2, 1.2);
                    door.setPosition(1750, 85);
                    gate.setPosition(1350, -200);
                    key_sound.play();
                    wooden_gate_sound.play();
                }

                if (sPlayer1.getGlobalBounds().intersects(gate.getGlobalBounds())) {
                    sPlayer1.move(-5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::D)) {
                        sPlayer1.move(-3, 3);
                    }
                }

                if (sPlayer2.getGlobalBounds().intersects(gate.getGlobalBounds())) {
                    sPlayer2.move(-5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                        sPlayer2.move(-3, 3);
                    }
                }


                score_num.setString("x" + to_string(coins_cntr));
                for (int i = 0; i < 8; i++) {
                    if (sPlayer1.getGlobalBounds().intersects(coins[i].getGlobalBounds()) || sPlayer2.getGlobalBounds().intersects(coins[i].getGlobalBounds())) {
                        coins[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }
                }



                //jumping & Collision with rectangles
                grounded1 = 0;
                grounded2 = 0;

                //collision with upper rects
                upperblock2.update(grounded1, grounded2, sPlayer1, sPlayer2);
                upperblock.update(grounded1, grounded2, sPlayer1, sPlayer2);
                upper_left_tile_rect___.update(grounded1, grounded2, sPlayer1, sPlayer2);
                upper_left_tile_rect__.update(grounded1, grounded2, sPlayer1, sPlayer2);
                upper_middle_tile_rect.update(grounded1, grounded2, sPlayer1, sPlayer2);
                upper_middle_tile_rect_.update(grounded1, grounded2, sPlayer1, sPlayer2);
                lower_middle_tile_rect.update(grounded1, grounded2, sPlayer1, sPlayer2);
                lower_middle_tile_rect_.update(grounded1, grounded2, sPlayer1, sPlayer2);

                //collision with lower rects
                underblock2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                underblock.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                upper_left_tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                lower_left_tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                upper_right_tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                lower_right_tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                upper_middle_tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                upper_middle_tile_rect2_.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                lower_middle_tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                lower_middle_tile_rect2_.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                if (moving_t.getpos().y > 200 && buttonPressed_)moving_t.update(grounded1, grounded2, sPlayer1, sPlayer2, 1, { 200 , 1000 });
                moving_tile.setPosition(moving_t.getpos());

                //right rectangles (used in collision)
                topright.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                rightmid2.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                rightmid.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                rightblock2.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                rightblock.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);

                lower_left_tile_rect3.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                upper_middle_tile_rect3.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                upper_middle_tile_rect3_.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                lower_middle_tile_rect3.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);

                //left rectangles (used in collision)
                leftmid2.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                leftmid.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                leftblock2.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                leftblock.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                upper_right_tile_rect4_.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                upper_middle_tile_rect4.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                lower_middle_tile_rect4.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);

                //transition
                start.update(3);

                //skeleton
                e1.update(sPlayer1.getPosition(), sPlayer2.getPosition());
                e1.collisionCheck(sPlayer1, sPlayer2);
                e2.update(sPlayer1.getPosition(), sPlayer2.getPosition());
                e2.collisionCheck(sPlayer1, sPlayer2);
                e3.update(sPlayer1.getPosition(), sPlayer2.getPosition());
                e3.collisionCheck(sPlayer1, sPlayer2);
                if (button_rect.collision(sPlayer1)) {

                    if (!sound_cnt) {
                        button_sound.play();
                        buttonPressed_ = 1;
                        sound_cnt++;
                    }
                }

                else if (button_rect.collision(sPlayer2)) {
                    button.setTexture(buttonpressed_texture);

                    if (!sound_cnt) {
                        button_sound.play();
                        buttonPressed_ = 1;
                        sound_cnt++;
                    }
                }

                if (!grounded1)
                {
                    velocityY1 -= 1;
                }

                if (grounded1)
                {
                    velocityY1 = 0;
                    if (Keyboard::isKeyPressed(Keyboard::Key::W)) {
                        velocityY1 = 15;
                    }
                }

                sPlayer1.move(0, -velocityY1);

                if (!grounded2) {
                    velocityY2 -= 1;
                }

                if (grounded2) {
                    velocityY2 = 0;
                    if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
                        velocityY2 = 15;
                    }
                }




                sPlayer2.move(0, -velocityY2);
                ctrX %= 2;
                sPlayer1.setTextureRect(IntRect(ctrX * 32, ctrY * 34, 32, 28));

                ctrX2 %= 2;
                sPlayer2.setTextureRect(IntRect(ctrX2 * 32, ctrY2 * 33, 32, 28));


                if (health1 == 0) {
                    sPlayer1.setTextureRect(IntRect(0, 11 * 32.3, 32, 28));
                    dead = true;
                }

                if (health2 == 0) {
                    sPlayer2.setTextureRect(IntRect(0, 4 * 32.4, 32, 28));
                    dead = true;
                }

                if (sPlayer1.getGlobalBounds().intersects(gate_rect.getGlobalBounds())
                    && sPlayer2.getGlobalBounds().intersects(gate_rect.getGlobalBounds())
                    && keys[2] == true && coins_cntr == 29) {

                    gate_pass[2] = true;
                    complete = 1;
                }
            }
        }

    render:;

        win.clear();
        win.draw(map);
        win.draw(moving_tile);
        win.draw(floor);
        for (int i = 0; i < 2; i++)
        {
            win.draw(floorgate[i]);
        }

        for (int i = 0; i < 8; i++)
        {
            win.draw(tile[i]);
        }

        for (int i = 0; i < 2; i++)
        {
            win.draw(blockk[i]);
        }
        for (int i = 0; i < 4; i++)

        {

            win.draw(spike[i]);

        }

        for (int i = 0; i < 8; i++)
        {
            if (!dead)
                coins[i].setTextureRect(IntRect(coins_cnt * coins_texture_size.x, 0, coins_texture_size.x, coins_texture_size.y));
            win.draw(coins[i]);
        }
        for (size_t i = 0; i < 2; i++) {
            if (!dead)
                hearts[i].setTextureRect(IntRect(hearts_cnt * hearts_texture_size.x, 0, hearts_texture_size.x, hearts_texture_size.y));
            win.draw(hearts[i]);
        }

        //draw
        win.draw(key);
        win.draw(button);
        win.draw(door);
        win.draw(gate);
        e1.draw();
        e2.draw();
        e3.draw();
        win.draw(sPlayer1);
        win.draw(sPlayer2);
        canonn1.draw();
        canonn2.draw();
        canonn3.draw();
        canonn4.draw();
        win.draw(health_bar1);
        win.draw(health_bar2);
        win.draw(score);
        win.draw(score_num);
        win.draw(screen_rect);

        if (color_cnt > 110) {
            win.draw(game_over);
            if (!sound_played) {
                music.pause();
                dead_sound.play();
                sound_played = true;
            }
        }
        end.draw();
        start.draw();
        win.display();
    }
}


void lvl2()
{
    //counters and booleans used throughout the level.for coins,hearts,player animations,speed and to check whether a player is on ground.
    int coins_cnt = 0, hearts_cnt = 0, ctrX = 0, ctrX2 = 0, ctrY = 0, ctrY2 = 0, coins_score = 0, hb_delay1 = 60, hb_delay2 = 60, sound_cnt = 0, gameOver_delay = 0, color_cnt = 0;
    float velocityY1 = 0, velocityY2 = 0;
    bool grounded1 = false, grounded2 = 0, button_is_pressed = 0, dead = 0, sound_played = 0, complete = 0;

    //Declaring sprites and textures

    Sprite background, tile1, tile2, tile3, tile4, tile5, tile6, tilethi, tilethi2, tilethi3, tilethi4, tile7_beside_key, key, coins[3],
        coins1[3], coins2[3], coins3[3], hearts[2], gate, sPlayer1, sPlayer2, cannons, spike1, spike2, button, HealthBarP1, HealthBarP2, score, game_over;


    Texture background_texture, player1_texture, player2_texture, tile1_texture, tileth_texture, tileth_2texture, tileth_3texture, tileth_4texture, key_texture, coins_texture,
        hearts_texture, tile2_texture, tile3_texture, tile4_texture, door_texture, tile_beside_key,
        gate_texture, opened_gate_texture, Player1, Player2, cannonT, Spike1, Spike2, Button, Button_On, Health_Bar1[4], Health_Bar2[4], Score, game_over_texture;



    //Loading textures from files
    background_texture.loadFromFile("map_vignette.png");
    Player1.loadFromFile("Player1.png");
    Player2.loadFromFile("Player2.png");
    tile1_texture.loadFromFile("ground.png");
    tile2_texture.loadFromFile("floor1.png");
    tile3_texture.loadFromFile("floor2.png");
    tile4_texture.loadFromFile("floor4.png");
    tileth_texture.loadFromFile("floor3.png");
    tileth_2texture.loadFromFile("floor3.png");
    tileth_3texture.loadFromFile("floor3.png");
    tileth_4texture.loadFromFile("floor3.png");
    key_texture.loadFromFile("key.png");
    coins_texture.loadFromFile("coins1.png");
    hearts_texture.loadFromFile("hearts.png");
    gate_texture.loadFromFile("gate.png");
    opened_gate_texture.loadFromFile("open_door.png");
    cannonT.loadFromFile("Canon.png");
    tile_beside_key.loadFromFile("floor4.png");
    Spike1.loadFromFile("Peaks.png");
    Spike2.loadFromFile("Peaks.png");
    Button.loadFromFile("Button.png");
    Button_On.loadFromFile("Button_Pressed.png");
    Health_Bar1[3].loadFromFile("P1_health_bar_full.png");
    Health_Bar1[2].loadFromFile("P1_health_bar_1.png");
    Health_Bar1[1].loadFromFile("P1_health_bar_2.png");
    Health_Bar1[0].loadFromFile("P1_health_bar_empty.png");
    Health_Bar2[3].loadFromFile("P2_health_bar_full.png");
    Health_Bar2[2].loadFromFile("P2_health_bar_1.png");
    Health_Bar2[1].loadFromFile("P2_health_bar_2.png");
    Health_Bar2[0].loadFromFile("P2_health_bar_empty.png");
    Score.loadFromFile("Score_Coin.png");
    game_over_texture.loadFromFile("Game_Over.png");
    //Border rectangles
    RectangleShape b1(Vector2f(5, 1080)), b2(Vector2f(5, 1900)), screen_rect(Vector2f(1920, 1080)), gate_rect(Vector2f(5, 130));
    screen_rect.setFillColor(sf::Color(0, 0, 0, color_cnt));
    //setting texture, position....etc
    background.setTexture(background_texture);


    b1.setPosition(-20, 1); //left border
    b2.setPosition(1930, 1); // right border
    gate_rect.setPosition(1850, 150);


    //ground
    tile1.setTexture(tile1_texture);
    tile1.setPosition(0, 980);


    //middle left
    tile2.setTexture(tile2_texture);
    tile2.setPosition(-150, 530);



    //lower right
    tile3.setTexture(tile3_texture);
    tile3.setPosition(1350.0f, 530.0f);

    //upper middle
    tile4.setTexture(tile4_texture);
    tile4.setPosition(800, 280);


    //upper right
    tile5.setTexture(tile3_texture);
    tile5.setPosition(1380, 280);


    //lower left
    tilethi.setTexture(tileth_texture);
    tilethi.setPosition(350, 830);


    //upper left
    tilethi2.setTexture(tileth_texture);
    tilethi2.setPosition(480, 350);


    //middle left
    tilethi4.setTexture(tileth_texture);
    tilethi4.setScale(Vector2f(0.7f, 0.7f));
    tilethi4.setPosition(680, 740);

    //beside key
    tile7_beside_key.setTexture(tile_beside_key);
    tile7_beside_key.setScale(Vector2f(0.5f, 0.5f));
    tile7_beside_key.setPosition(70, 300);

    //key texture and position
    key.setTexture(key_texture);
    key.setPosition(170.0f, 100.0f);
    key.setScale(0.14f, 0.15f);

    //gate texture and position
    gate.setTexture(gate_texture);
    gate.setPosition(1740.0f, 140.0f);
    gate.setScale(1.4f, 1.4f);

    //cannons texture and position
    cannons.setTexture(cannonT);
    cannons.setScale(1.8, 1.8);
    cannons.setPosition(1690, 440);


    //spikes
    spike1.setTexture(Spike1);
    spike1.setPosition(150, 290);
    spike1.setScale(2.5, 2.5);
    spike2.setTexture(Spike2);
    spike2.setPosition(240, 290);
    spike2.setScale(2.5, 2.5);


    //button
    button.setTexture(Button);
    button.setPosition(1800, 920);
    button.setScale(0.6f, 0.6f);

    //Health Bar P1 & P2

    HealthBarP1.setTexture(Health_Bar1[0]);
    HealthBarP1.setPosition(15.0f, -35.0f);
    HealthBarP1.setScale(1.4, 1.4);
    HealthBarP2.setTexture(Health_Bar2[0]);
    HealthBarP2.setPosition(1630.0f, -35.0f);
    HealthBarP2.setScale(1.4, 1.4);

    //Score
    score.setTexture(Score);
    score.scale(0.3f, 0.3f);
    score.setPosition(900, 20);
    Font font;
    font.loadFromFile("Amatic-Bold.ttf");
    Text score_num;
    score_num.setFont(font);
    score_num.setFillColor(sf::Color::White);
    score_num.setPosition(960, 15);
    score_num.setCharacterSize(50);


    //game over
    game_over.setTexture(game_over_texture);



    //Player1 texture,animations...etc
    sPlayer1.setTexture(Player1);
    sPlayer1.setTextureRect(IntRect(0, 0 * 35, 32, 40));
    sPlayer1.setScale(3, 3);
    sPlayer1.setPosition(15.0f, 830.0f);



    //Player2 texture,animations...etc
    sPlayer2.setTexture(Player2);
    sPlayer2.setTextureRect(IntRect(0, 0 * 36, 32, 40));
    sPlayer2.setScale(3, 3);
    sPlayer2.setPosition(100.0f, 830.0f);


    //Loop to set textures,positions,animations,scale to coins
    Vector2u coins_texture_size = coins_texture.getSize();
    coins_texture_size.x /= 6;
    for (size_t i = 0; i < 3; i++) {
        //lower left tile coins
        coins[i].setTexture(coins_texture);
        coins[i].setPosition(540.0f + (i * 90), 300.0f);
        coins[i].setTextureRect(IntRect(0, 0, coins_texture_size.x, coins_texture_size.y));
        coins[i].setScale(0.2f, 0.2f);

        //lower middle tile coins
        coins1[i].setTexture(coins_texture);
        coins1[i].setPosition(1100.0f + (i * 70), 910.0f);
        coins1[i].setTextureRect(IntRect(0, 0, coins_texture_size.x, coins_texture_size.y));
        coins1[i].setScale(0.2f, 0.2f);

        //upper Right tile coins
        coins2[i].setTexture(coins_texture);
        coins2[i].setPosition(1500.0f + (i * 70), 470.0f);
        coins2[i].setTextureRect(IntRect(0, 0, coins_texture_size.x, coins_texture_size.y));
        coins2[i].setScale(0.2f, 0.2f);

        //Lower Right tile coins
        coins3[i].setTexture(coins_texture);
        coins3[i].setPosition(1500.0f + (i * 70), 910.0f);
        coins3[i].setTextureRect(IntRect(0, 0, coins_texture_size.x, coins_texture_size.y));
        coins3[i].setScale(0.2f, 0.2f);
    }

    //Loop to set textures,positions,animations,scale to hearts
    Vector2u hearts_texture_size = hearts_texture.getSize();
    hearts_texture_size.x /= 6;
    for (size_t i = 0; i < 2; i++) {
        hearts[i].setTexture(hearts_texture);
        hearts[i].setPosition(1550.0f + (i * 90), 230.0f);
        hearts[i].setTextureRect(IntRect(0, 0, hearts_texture_size.x, hearts_texture_size.y));
        hearts[i].setScale(0.5f, 0.5f);
    }


    //Skeletons                   position                                    range of view            
    enemy e1(Vector2f(680, tile1.getPosition().y - 130), Vector2f(1000.0f, 150.0f));

    enemy e3(Vector2f(tilethi2.getPosition().x, tilethi2.getPosition().y - 120), Vector2f(280.0f, 150.0f));





    //rectangles above the tiles (used in collision)

    //                             position                                                        size
    collup upper_left_tile_rect(Vector2f(tile2.getPosition().x - 20, tile2.getPosition().y), Vector2f(560, 10));
    collup lower_left_tile_rect(Vector2f(tile5.getPosition().x + 200, tile5.getPosition().y), Vector2f(660, 10));
    collup upper_right_tile_rect(Vector2f(tile3.getPosition().x + 200, tile3.getPosition().y), Vector2f(435, 10));
    collup lower_right_tile_rect(Vector2f(tilethi.getPosition().x + 35, tilethi.getPosition().y), Vector2f(280, 10));
    collup upper_middle_tile_rect(Vector2f(tile4.getPosition().x + 170, tile4.getPosition().y), Vector2f(300, 10));
    collup upper_left_tilethi(Vector2f(tilethi2.getPosition().x + 40, tilethi2.getPosition().y), Vector2f(280, 5));
    collup ground_rect(Vector2f(tile1.getPosition()), Vector2f(1920, 10));
    collup beside_key1(Vector2f(tile7_beside_key.getPosition().x + 120, tile7_beside_key.getPosition().y), Vector2f(100, 5));
    collup button_coll(Vector2f(button.getPosition().x + 5, button.getPosition().y + 50), Vector2f(50, 10));

    //rectangles under the tiles (used in collision)
    //                                     position                                                size
    colldown upper_left_tile_rect2(Vector2f(tile2.getPosition().x, tile2.getPosition().y + 80), Vector2f(545, 10));
    colldown lower_left_tile_rect2(Vector2f(tile5.getPosition().x + 190, tile5.getPosition().y + 80), Vector2f(435, 10));
    colldown upper_right_tile_rect2(Vector2f(tile3.getPosition().x + 180, tile3.getPosition().y + 80), Vector2f(425, 10));
    colldown lower_right_tile_rect2(Vector2f(tilethi.getPosition().x + 30, tilethi.getPosition().y + 60), Vector2f(290, 10));
    colldown upper_middle_tile_rect2(Vector2f(tile4.getPosition().x + 170, tile4.getPosition().y + 80), Vector2f(300, 10));
    colldown middle_Left_tile2(Vector2f(tilethi4.getPosition().x + 40, tilethi4.getPosition().y + 30), Vector2f(220, 5));
    colldown upper_left_tilethi2(Vector2f(tilethi2.getPosition().x + 40, tilethi2.getPosition().y + 60), Vector2f(250, 5));
    colldown beside_key2(Vector2f(tile7_beside_key.getPosition().x + 120, tile7_beside_key.getPosition().y + 30), Vector2f(100, 5));

    //rectangles collision to the Right of the tiles
    //                                           position                                                     size
    colright upper_left_tile_rect3(Vector2f(tile2.getPosition().x + 550, tile2.getPosition().y + 10), Vector2f(10, 100));
    upper_left_tile_rect3.rotation(45.0f);
    colright lower_right_tile_rect3(Vector2f(tilethi.getPosition().x + 320, tilethi.getPosition().y), Vector2f(10, 70));
    colright upper_middle_tile_rect3(Vector2f(tile4.getPosition().x + 490, tile4.getPosition().y), Vector2f(10, 100));
    colright middle_Left_tile3(Vector2f(tilethi4.getPosition().x + 170, tilethi4.getPosition().y), Vector2f(10, 40));
    colright upper_left_tilethi3(Vector2f(tilethi2.getPosition().x + 330, tilethi2.getPosition().y), Vector2f(10, 70));
    colright beside_key3(Vector2f(tile7_beside_key.getPosition().x + 230, tile7_beside_key.getPosition().y), Vector2f(10, 40));

    //rectangles collision to the left of the tiles
    //                                          position                                                      size
    colleft lower_left_tile_rect4(Vector2f(tile5.getPosition().x + 150, tile5.getPosition().y + 20), Vector2f(10, 100));
    lower_left_tile_rect4.rotation(-45.0f);
    colleft upper_right_tile_rect4(Vector2f(tile3.getPosition().x + 180, tile3.getPosition().y + 30), Vector2f(5, 70));
    upper_right_tile_rect4.rotation(-45.0f);
    colleft lower_right_tile_rect4(Vector2f(tilethi.getPosition().x + 20, tilethi.getPosition().y + 10), Vector2f(10, 50));
    colleft upper_middle_tile_rect4(Vector2f(tile4.getPosition().x + 160, tile4.getPosition().y + 20), Vector2f(10, 70));
    colleft middle_Left_tile4(Vector2f(tilethi4.getPosition().x + 20, tilethi4.getPosition().y + 5), Vector2f(10, 30));
    colleft upper_left_tilethi4(Vector2f(tilethi2.getPosition().x + 30, tilethi2.getPosition().y), Vector2f(10, 60));
    colleft beside_key4(Vector2f(tile7_beside_key.getPosition().x + 100, tile7_beside_key.getPosition().y + 10), Vector2f(10, 40));

    //Spikes collision
    collup upper_left_spikes(Vector2f(spike1.getPosition()), Vector2f(150, 5));

    //Moving Tile.
    collup2 lower_Middle_tile(Vector2f(830, 740), Vector2f(140, 10), 5, 3.3);

    //Canon
    kaboom middle_right_canon(Vector2f(cannons.getPosition().x + 100, cannons.getPosition().y + 10), -1);

    //transition
    transition start(0), end(1);

    //Sounds
    Music music;
    music.openFromFile("SoundTrack.wav");
    music.play();
    music.setVolume(15.0f);


    Sound coins_sound;
    SoundBuffer coins_sound_buffer;
    coins_sound_buffer.loadFromFile("coin sound effect.wav");
    coins_sound.setBuffer(coins_sound_buffer);


    Sound hearts_sound;
    SoundBuffer hearts_sound_buffer;
    hearts_sound_buffer.loadFromFile("Heart Taken.wav");
    hearts_sound.setBuffer(hearts_sound_buffer);


    Sound key_sound;
    SoundBuffer key_sound_buffer;
    key_sound_buffer.loadFromFile("Key is taken.wav");
    key_sound.setBuffer(key_sound_buffer);
    key_sound.setVolume(200.0f);


    Sound wooden_gate_sound;
    SoundBuffer wooden_gate_sound_buffer;
    wooden_gate_sound_buffer.loadFromFile("Wooden Door Opening.wav");
    wooden_gate_sound.setBuffer(wooden_gate_sound_buffer);


    Sound jump_sound;
    SoundBuffer jump_sound_buffer;
    jump_sound_buffer.loadFromFile("Jump_Sound.wav");
    jump_sound.setBuffer(jump_sound_buffer);


    Sound movement_sound;
    SoundBuffer movement_sound_buffer;
    movement_sound_buffer.loadFromFile("Movement.wav");
    movement_sound.setBuffer(movement_sound_buffer);
    movement_sound.setVolume(1.0f);


    Sound damage_sound;
    SoundBuffer damage_sound_buffer;
    damage_sound_buffer.loadFromFile("Damage Taken.wav");
    damage_sound.setBuffer(damage_sound_buffer);


    Sound dead_sound;
    SoundBuffer dead_sound_buffer;
    dead_sound_buffer.loadFromFile("Game Over.wav");
    dead_sound.setBuffer(dead_sound_buffer);

    Sound button_sound;
    SoundBuffer button_sound_buffer;
    button_sound_buffer.loadFromFile("Button Pushed.wav");
    button_sound.setBuffer(button_sound_buffer);
    button_sound.setVolume(15.0f);


    Clock switch_time, player_switch1, player_switch2, movement;

    //Game loop
    while (win.isOpen()) {
        //Event to close the window
        Event evnt;
        while (win.pollEvent(evnt)) {
            switch (evnt.type) {
            case Event::Closed:
                win.close();
                break;
            }
        }

        music.setVolume((abs(music_vol - 970) / 1260) * 100);
        coins_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        hearts_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        key_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        wooden_gate_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        jump_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        movement_sound.setVolume((abs(sfx_vol - 970) / 1260) * 5);
        damage_sound.setVolume((abs(sfx_vol - 970) / 1260) * 50);
        dead_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        button_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);

        if (dead)
        {
            HealthBarP1.setTexture(Health_Bar1[health1]);
            HealthBarP2.setTexture(Health_Bar2[health2]);
            if (color_cnt < 200)
                color_cnt += 2;
            screen_rect.setFillColor(sf::Color(0, 0, 0, color_cnt));
            if (color_cnt == 200) {
                if (gameOver_delay != 180) gameOver_delay++;
                else { indicator = 2; return; }
            }
        }


        if (Keyboard::isKeyPressed(Keyboard::Escape) || indicator != 1) { if (!indicator) { indicator = -1; goto render; } pauseMenu(indicator, 2); }

        if (indicator == 2 || indicator == 4) return;

        if (indicator == 3) soundControl(), indicator = 0;
        //fade to black at the end of the level
        if (complete) {
            end.update(2);
            if (end.retrn()) {
                return;
            }
        }
        if (indicator == 1) {
            if (!dead && !complete) {

                grounded1 = 0, grounded2 = 0;
                //collision with upper rects
                upper_left_tile_rect.update(grounded1, grounded2, sPlayer1, sPlayer2);
                lower_left_tile_rect.update(grounded1, grounded2, sPlayer1, sPlayer2);
                upper_right_tile_rect.update(grounded1, grounded2, sPlayer1, sPlayer2);
                lower_right_tile_rect.update(grounded1, grounded2, sPlayer1, sPlayer2);
                upper_middle_tile_rect.update(grounded1, grounded2, sPlayer1, sPlayer2);
                ground_rect.update(grounded1, grounded2, sPlayer1, sPlayer2);
                upper_left_tilethi.update(grounded1, grounded2, sPlayer1, sPlayer2);
                beside_key1.update(grounded1, grounded2, sPlayer1, sPlayer2);
                button_coll.update(grounded1, grounded2, sPlayer1, sPlayer2);


                //collision with lower rects
                upper_left_tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                lower_left_tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                upper_right_tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                lower_right_tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                upper_middle_tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                middle_Left_tile2.update(velocityY1, velocityY2, sPlayer1, sPlayer2, 1, { lower_Middle_tile.getpos().x - 40,lower_Middle_tile.getpos().y + 50 });
                upper_left_tilethi2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                beside_key2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);


                //collision to the right of the tiles
                upper_left_tile_rect3.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                lower_right_tile_rect3.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                upper_middle_tile_rect3.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                middle_Left_tile3.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2, 1, { lower_Middle_tile.getpos().x + 160,lower_Middle_tile.getpos().y + 20 });
                upper_left_tilethi3.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                beside_key3.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);


                //collision to the left of the tiles        
                lower_left_tile_rect4.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                upper_right_tile_rect4.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                lower_right_tile_rect4.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                upper_middle_tile_rect4.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                middle_Left_tile4.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2, 1, { lower_Middle_tile.getpos().x - 30,lower_Middle_tile.getpos().y + 20 });
                upper_left_tilethi4.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                beside_key4.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);


                //Moving tile
                lower_Middle_tile.update(grounded1, grounded2, sPlayer1, sPlayer2, { {540,1300} ,  {550,800} });
                tilethi4.setPosition(lower_Middle_tile.getpos().x - 60, lower_Middle_tile.getpos().y);

                //transition
                start.update(2);


                //Spikes
                upper_left_spikes.update(grounded1, grounded2, sPlayer1, sPlayer2);
                HealthBarP1.setTexture(Health_Bar1[health1]);
                HealthBarP2.setTexture(Health_Bar2[health2]);
                if (upper_left_spikes.collision(sPlayer1) && !button_is_pressed) {
                    if (hb_delay1 == 60) {

                        if (health1 > 0) {
                            health1--;
                            damage_sound.play();
                        }
                        hb_delay1 = 0;
                    }
                    else
                        hb_delay1++;
                }
                else {
                    hb_delay1 = 60;
                }
                if (upper_left_spikes.collision(sPlayer2) && !button_is_pressed) {
                    if (hb_delay2 == 60) {

                        if (health2 > 0) {
                            health2--;
                            damage_sound.play();
                        }
                        hb_delay2 = 0;
                    }
                    else
                        hb_delay2++;
                }
                else {
                    hb_delay2 = 60;
                }


                //Skeletons
                e1.collisionCheck(sPlayer1, sPlayer2);
                e1.update(sPlayer1.getPosition(), sPlayer2.getPosition());
                e3.collisionCheck(sPlayer1, sPlayer2);
                e3.update(sPlayer1.getPosition(), sPlayer2.getPosition());


                //canons
                middle_right_canon.update(b1, b2, sPlayer1, sPlayer2);


                //coins & hearts animations counter. clock starts from 0 and starts counting,once it reaches > 0.08 seconds the counters increase by 1 up until it reaches 6 (the number of photos in the sprite sheet).
                // then we make each counter modulus 6 as to reset it to 0 and start over. if you increase 0.08,the animation becomes slower.
                if (switch_time.getElapsedTime().asSeconds() > 0.08) {
                    coins_cnt++;
                    hearts_cnt++;
                    switch_time.restart();//resets the clock back to 0.
                }
                coins_cnt %= 6;
                hearts_cnt %= 6;


                //the events the characters will do in the game(player movements..etc).also it increases the animation counter for both players on x and y axis.
                //for example, if the player clicks A player 1 will move to the left so ctrY goes to row 3 (remember it's 0 based!) where the walking to the left
                //animations happens and starts increasing the counter on x once it reaches 2 we modulus it by 2 and restart over.
                if (Keyboard::isKeyPressed(Keyboard::Key::W)) {
                    sPlayer1.move(0, -5);
                    if (grounded1) {
                        jump_sound.play();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
                    sPlayer1.move(-5, 0);
                    ctrY = 3;

                    if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                        ctrX++;
                        player_switch1.restart();
                    }

                    if (movement.getElapsedTime().asSeconds() > 0.5) {
                        movement_sound.play();
                        movement.restart();
                    }
                }


                if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
                    sPlayer1.move(5, 0);
                    ctrY = 2;

                    if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                        ctrX++;
                        player_switch1.restart();
                    }
                    if (movement.getElapsedTime().asSeconds() > 0.5) {
                        movement_sound.play();
                        movement.restart();
                    }
                }


                if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::D)) {
                    sPlayer1.move(3, 3);
                    ctrY = 2;
                    movement_sound.pause();
                    if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                        ctrX++;
                        player_switch1.restart();
                    }
                }


                if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::A)) {
                    sPlayer1.move(-3, 3);
                    ctrY = 3;
                    movement_sound.pause();
                    if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                        ctrX++;
                        player_switch1.restart();
                    }
                }


                if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
                    sPlayer2.move(0, -5);
                    if (grounded2) {
                        jump_sound.play();
                    }
                }


                if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                    sPlayer2.move(-5, 0);
                    ctrY2 = 3;

                    if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                        ctrX2++;
                        player_switch2.restart();
                    }
                    if (movement.getElapsedTime().asSeconds() > 0.5) {
                        movement_sound.play();
                        movement.restart();
                    }
                }


                if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                    sPlayer2.move(5, 0);
                    ctrY2 = 2;

                    if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                        ctrX2++;
                        player_switch2.restart();
                    }
                    if (movement.getElapsedTime().asSeconds() > 0.5) {
                        movement_sound.play();
                        movement.restart();
                    }
                }


                if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                    sPlayer2.move(3, 3);
                    ctrY2 = 2;
                    movement_sound.pause();
                    if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                        ctrX2++;
                        player_switch2.restart();

                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                    sPlayer2.move(-3, 3);
                    ctrY2 = 3;
                    movement_sound.pause();
                    if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                        ctrX2++;
                        player_switch2.restart();
                    }
                }

                ctrX %= 2;
                sPlayer1.setTextureRect(IntRect(ctrX * 32, ctrY * 34, 32, 28));

                ctrX2 %= 2;
                sPlayer2.setTextureRect(IntRect(ctrX2 * 32, ctrY2 * 33, 32, 28));




                //Collisions with window border
                if (sPlayer1.getGlobalBounds().intersects(b1.getGlobalBounds())) {
                    sPlayer1.move(5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::A)) {
                        sPlayer1.move(3, 3);
                    }
                }

                if (sPlayer1.getGlobalBounds().intersects(b2.getGlobalBounds())) {
                    sPlayer1.move(-5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::D)) {
                        sPlayer1.move(-3, 3);
                    }
                }

                if (sPlayer2.getGlobalBounds().intersects(b1.getGlobalBounds())) {
                    sPlayer2.move(5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                        sPlayer2.move(3, 3);
                    }
                }

                if (sPlayer2.getGlobalBounds().intersects(b2.getGlobalBounds())) {
                    sPlayer2.move(-5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                        sPlayer2.move(-3, 3);
                    }
                }

                //Collision with key
                if (sPlayer1.getGlobalBounds().intersects(key.getGlobalBounds())) {
                    gate.setTexture(opened_gate_texture);
                    gate.setScale(1.43f, 1.43f);
                    key.setScale(0, 0);
                    key_sound.play();
                    wooden_gate_sound.play();
                    keys[1] = true;
                }

                if (sPlayer2.getGlobalBounds().intersects(key.getGlobalBounds())) {
                    gate.setTexture(opened_gate_texture);
                    gate.setScale(1.43f, 1.43f);
                    key.setScale(0, 0);
                    key_sound.play();
                    wooden_gate_sound.play();
                    keys[1] = true;
                }

                //Collision with hearts
                if (sPlayer1.getGlobalBounds().intersects(hearts[0].getGlobalBounds()) && health1 < 3) {
                    hearts[0].setScale(0, 0);
                    hearts_sound.play();
                    health1++;
                }

                if (sPlayer2.getGlobalBounds().intersects(hearts[1].getGlobalBounds()) && health2 < 3) {
                    hearts[1].setScale(0, 0);
                    hearts_sound.play();
                    health2++;
                }

                //loop to detect collision with coins
                score_num.setString("x" + to_string(coins_cntr));
                for (size_t i = 0; i < 3; i++) {
                    if (sPlayer1.getGlobalBounds().intersects(coins[i].getGlobalBounds())) {
                        coins[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }

                    if (sPlayer1.getGlobalBounds().intersects(coins1[i].getGlobalBounds())) {
                        coins1[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }

                    if (sPlayer1.getGlobalBounds().intersects(coins2[i].getGlobalBounds())) {
                        coins2[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }

                    if (sPlayer1.getGlobalBounds().intersects(coins3[i].getGlobalBounds())) {
                        coins3[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }

                    if (sPlayer2.getGlobalBounds().intersects(coins[i].getGlobalBounds())) {
                        coins[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }

                    if (sPlayer2.getGlobalBounds().intersects(coins1[i].getGlobalBounds())) {
                        coins1[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }

                    if (sPlayer2.getGlobalBounds().intersects(coins2[i].getGlobalBounds())) {
                        coins2[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }

                    if (sPlayer2.getGlobalBounds().intersects(coins3[i].getGlobalBounds())) {
                        coins3[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }
                }

                //collision with button
                if (button_coll.collision(sPlayer1)) {
                    button.setTexture(Button_On);
                    spike1.setPosition(150, 320);
                    spike2.setPosition(240, 320);
                    upper_left_spikes.setpos(Vector2f(10000, 10000));
                    if (!sound_cnt) {
                        button_sound.play();
                        sound_cnt++;
                    }
                }

                else if (button_coll.collision(sPlayer2)) {
                    button.setTexture(Button_On);
                    spike1.setPosition(150, 320);
                    spike2.setPosition(240, 320);
                    upper_left_spikes.setpos(Vector2f(10000, 10000));
                    if (sound_cnt == 0) {
                        button_sound.play();
                        sound_cnt++;
                    }
                }

                if (!button_coll.collision(sPlayer1) && !button_coll.collision(sPlayer2)) {
                    button.setTexture(Button);
                    spike1.setPosition(150, 290);
                    spike2.setPosition(240, 290);
                    upper_left_spikes.setpos(spike1.getPosition());
                    if (sound_cnt == 1) {
                        button_sound.play();
                    }
                    sound_cnt = 0;
                }

                if (sPlayer1.getGlobalBounds().intersects(gate_rect.getGlobalBounds())
                    && sPlayer2.getGlobalBounds().intersects(gate_rect.getGlobalBounds())
                    && keys[1] == true && coins_cntr == 21) {

                    gate_pass[1] = true;
                    complete = 1;
                }


                //gravity of player 1
                if (!grounded1)
                {
                    velocityY1 -= 1;
                }

                if (grounded1)
                {
                    velocityY1 = 0;
                    if (Keyboard::isKeyPressed(Keyboard::Key::W)) {
                        velocityY1 = 15;
                    }
                }

                sPlayer1.move(0, -velocityY1);

                //gravity of player 2
                if (!grounded2) {
                    velocityY2 -= 1;
                }

                if (grounded2) {
                    velocityY2 = 0;
                    if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
                        velocityY2 = 15;
                    }
                }

                sPlayer2.move(0, -velocityY2);

                if (health1 == 0) {
                    sPlayer1.setTextureRect(IntRect(0, 11 * 32.3, 32, 28));
                    dead = true;
                }

                if (health2 == 0) {
                    sPlayer2.setTextureRect(IntRect(0, 4 * 32.4, 32, 28));
                    dead = true;
                }
            }
        }
    render:;

        //drawing and displaying objects
        win.clear();

        win.draw(background);
        e1.draw();
        e3.draw();
        win.draw(key);
        win.draw(tile1);
        win.draw(tile2);
        win.draw(tilethi);
        win.draw(tilethi2);
        win.draw(tilethi3);
        win.draw(tilethi4);
        win.draw(tile3);
        win.draw(tile4);
        win.draw(tile5);
        win.draw(tile6);
        middle_right_canon.draw();
        win.draw(cannons);
        win.draw(spike1);
        win.draw(spike2);
        win.draw(tile7_beside_key);
        win.draw(gate);
        win.draw(button);
        win.draw(score);
        win.draw(score_num);
        win.draw(sPlayer1);
        win.draw(sPlayer2);
        win.draw(HealthBarP1);
        win.draw(HealthBarP2);
        //Loop to make coin animations
        for (int i = 0; i < 3; i++) {
            if (!dead)
            {
                coins[i].setTextureRect(IntRect(coins_cnt * coins_texture_size.x, 0, coins_texture_size.x, coins_texture_size.y));
                coins1[i].setTextureRect(IntRect(coins_cnt * coins_texture_size.x, 0, coins_texture_size.x, coins_texture_size.y));
                coins2[i].setTextureRect(IntRect(coins_cnt * coins_texture_size.x, 0, coins_texture_size.x, coins_texture_size.y));
                coins3[i].setTextureRect(IntRect(coins_cnt * coins_texture_size.x, 0, coins_texture_size.x, coins_texture_size.y));
            }
            win.draw(coins[i]);
            win.draw(coins1[i]);
            win.draw(coins2[i]);
            win.draw(coins3[i]);
        }

        for (int i = 0; i < 2; i++) {
            if (!dead)
                hearts[i].setTextureRect(IntRect(hearts_cnt * hearts_texture_size.x, 0, hearts_texture_size.x, hearts_texture_size.y));
            win.draw(hearts[i]);
        }



        win.draw(screen_rect);

        if (color_cnt > 110) {
            win.draw(game_over);
            if (!sound_played) {
                music.pause();
                dead_sound.play();
                sound_played = true;
            }
        }
        start.draw();
        end.draw();
        win.display();
    }
}


void lvl1()
{
    int coins_cnt = 0, hearts_cnt = 0, ctrX = 0, ctrX2 = 0, ctrY = 0, ctrY2 = 0, sound_cnt = 0, hb_delay1 = 60, hb_delay2 = 60,
        coins_score = 0, gameOver_delay = 0;
    float velocityY1 = 0, velocityY2 = 0, color_cnt = 0;
    bool grounded1 = true, grounded2 = true, dead = false, sound_played = false, complete = 0;


    //Declaring sprites and textures
    Sprite background, tile1, tile2, tile3, tile4, tile5, tile6, tile7, tile8, tile9, key, coins[3],
        coins1[3], coins2[3], button, hearts[2], door, gate, spikes1, spikes2, spikes3, spikes4, spikes6, sPlayer1,
        sPlayer2, health_bar1, health_bar2, score, game_over;


    Texture background_texture, player1_texture, player2_texture, tile1_texture, key_texture, coins_texture,
        button_texture, buttonpressed_texture, hearts_texture, tile2_texture, tile3_texture, tile4_texture, door_texture,
        tile8_texture, gate_texture, opened_gate_texture, spikes_texture, Player1, Player2, tile9_texture, health_bar1_texture[4],
        health_bar2_texture[4], score_texture, game_over_texture;


    background_texture.loadFromFile("map_vignette.png");
    Player1.loadFromFile("Player1.png");
    Player2.loadFromFile("Player2.png");
    tile1_texture.loadFromFile("ground.png");
    tile2_texture.loadFromFile("floor1.png");
    tile3_texture.loadFromFile("floor2.png");
    tile4_texture.loadFromFile("floor4.png");
    tile8_texture.loadFromFile("floor5.png");
    tile9_texture.loadFromFile("small.png");
    key_texture.loadFromFile("key.png");
    coins_texture.loadFromFile("coins1.png");
    button_texture.loadFromFile("Button.png");
    buttonpressed_texture.loadFromFile("Button_Pressed.png");
    hearts_texture.loadFromFile("Hearts.png");
    door_texture.loadFromFile("Gate1.png");
    gate_texture.loadFromFile("Gate.png");
    opened_gate_texture.loadFromFile("open_door.png");
    spikes_texture.loadFromFile("Peaks.png");
    health_bar1_texture[3].loadFromFile("P1_health_bar_full.png");
    health_bar1_texture[2].loadFromFile("P1_health_bar_1.png");
    health_bar1_texture[1].loadFromFile("P1_health_bar_2.png");
    health_bar1_texture[0].loadFromFile("P1_health_bar_empty.png");
    health_bar2_texture[3].loadFromFile("P2_health_bar_full.png");
    health_bar2_texture[2].loadFromFile("P2_health_bar_1.png");
    health_bar2_texture[1].loadFromFile("P2_health_bar_2.png");
    health_bar2_texture[0].loadFromFile("P2_health_bar_empty.png");
    score_texture.loadFromFile("Score_Coin.png");
    game_over_texture.loadFromFile("Game_Over.png");


    RectangleShape b1(Vector2f(5, 1080)), b2(Vector2f(5, 1900)), gate_rect(Vector2f(5, 130)), square_rect(Vector2f(7, 2)),
        square3_rect(Vector2f(5, 40)), square4_rect(Vector2f(5, 40)), screen_rect(Vector2f(1920, 1080));


    b1.setPosition(-20, 1); //left border
    b2.setPosition(1930, 1); // right border
    gate_rect.setPosition(1850, 220); //metal gate rect
    square_rect.setPosition(415, 865);
    square3_rect.setPosition(415, 890);
    square4_rect.setPosition(415, 890);
    screen_rect.setFillColor(sf::Color(0, 0, 0, color_cnt));


    //setting texture, position, etc ...
    background.setTexture(background_texture);


    //ground
    tile1.setTexture(tile1_texture);
    tile1.setPosition(0.0f, 960.0f);


    //upper left tile
    tile2.setTexture(tile2_texture);
    tile2.setPosition(-140.0f, 344.0f);


    //upper right tile
    tile3.setTexture(tile3_texture);
    tile3.setPosition(1340.0f, 344.0f);


    //upper middle tiles
    tile4.setTexture(tile4_texture);
    tile4.setPosition(870.0f, 480.0f);


    //lower left tile
    tile5.setTexture(tile2_texture);
    tile5.setPosition(-140.0f, 628.0f);


    //lower middle tiles
    tile6.setTexture(tile4_texture);
    tile6.setPosition(420.0f, 760.0f);



    //lower right tile
    tile7.setTexture(tile3_texture);
    tile7.setPosition(1340.0f, 628.0f);


    //the tile above the gate
    tile8.setTexture(tile8_texture);
    tile8.setPosition(1530.0f, 425.0f);
    tile8.setScale(0.8f, 0.8f);


    //left square tile
    tile9.setTexture(tile9_texture);
    tile9.setPosition(380, 862);
    tile9.setScale(0.8f, 0.8f);


    key.setTexture(key_texture);
    key.setPosition(1780.0f, 580.0f);
    key.setScale(0.14f, 0.15f);


    button.setPosition(200.0f, 285.0f);
    button.setScale(0.6f, 0.6f);
    collup button_rect(Vector2f(button.getPosition().x + 30, button.getPosition().y + 40), Vector2f(1, 5));


    //the door in front of the key (metal gate)
    door.setTexture(door_texture);
    door.setScale(0.7f, 0.7f);


    //the portal (wooden gate)
    gate.setTexture(gate_texture);
    gate.setPosition(1740.0f, 208.0f);
    gate.setScale(1.4f, 1.4f);


    //upper left tile spikes
    spikes1.setTexture(spikes_texture);
    spikes1.setPosition(300.0f, 332.0f);
    spikes1.setScale(2.5f, 2.5f);


    //upper middle tiles spikes
    spikes3.setTexture(spikes_texture);
    spikes3.setScale(2.5f, 2.5f);
    spikes2.setTexture(spikes_texture);
    spikes2.setScale(2.5f, 2.5f);


    //lower middle tiles spikes
    spikes6.setTexture(spikes_texture);
    spikes6.setPosition(690.0f, 748.0f);
    spikes6.setScale(2.5f, 2.5f);


    //lower left tile spikes
    spikes4.setTexture(spikes_texture);
    spikes4.setPosition(240.0f, 615.0f);
    spikes4.setScale(2.5f, 2.5f);


    //player1
    sPlayer1.setTexture(Player1);
    sPlayer1.setTextureRect(IntRect(0, 0 * 35, 32, 40));
    sPlayer1.setScale(3, 3);
    sPlayer1.setPosition(15.0f, 860.0f);


    //health bar 1
    health_bar1.setPosition(15.0f, -35.0f);
    health_bar1.setScale(1.4, 1.4);


    //player2
    sPlayer2.setTexture(Player2);
    sPlayer2.setTextureRect(IntRect(0, 0 * 36, 32, 40));
    sPlayer2.setScale(3, 3);
    sPlayer2.setPosition(100.0f, 860.0f);


    //health bar 2
    health_bar2.setPosition(1630.0f, -35.0f);
    health_bar2.setScale(1.4, 1.4);


    //game over
    game_over.setTexture(game_over_texture);


    //score
    score.setTexture(score_texture);
    score.scale(0.3f, 0.3f);
    score.setPosition(900, 20);
    Font font;
    font.loadFromFile("Amatic-Bold.ttf");
    Text score_num;
    score_num.setFont(font);
    score_num.setFillColor(sf::Color::White);
    score_num.setPosition(960, 15);
    score_num.setCharacterSize(50);


    //coins
    Vector2u coins_texture_size = coins_texture.getSize();
    coins_texture_size.x /= 6;
    for (int i = 0; i < 3; i++) {
        //lower left tile coins
        coins[i].setTexture(coins_texture);
        coins[i].setPosition(20.0f + (i * 55), 580.0f);
        coins[i].setTextureRect(IntRect(0, 0, coins_texture_size.x, coins_texture_size.y));
        coins[i].setScale(0.2f, 0.2f);

        //upper left tile coins
        coins1[i].setTexture(coins_texture);
        coins1[i].setPosition(20.0f + (i * 55), 300.0f);
        coins1[i].setTextureRect(IntRect(0, 0, coins_texture_size.x, coins_texture_size.y));
        coins1[i].setScale(0.2f, 0.2f);

        //upper middle tile coins
        coins2[i].setTexture(coins_texture);
        coins2[i].setPosition(890.0f + (i * 55), 380.0f);
        coins2[i].setTextureRect(IntRect(0, 0, coins_texture_size.x, coins_texture_size.y));
        coins2[i].setScale(0.2f, 0.2f);
    }


    //hearts
    Vector2u hearts_texture_size = hearts_texture.getSize();
    hearts_texture_size.x /= 6;
    for (int i = 0; i < 2; i++) {
        hearts[i].setTexture(hearts_texture);
        hearts[i].setPosition(1520.0f + (i * 90), 290.0f);
        hearts[i].setTextureRect(IntRect(0, 0, hearts_texture_size.x, hearts_texture_size.y));
        hearts[i].setScale(0.5f, 0.5f);
    }


    //rectangles above the tiles (used in collision)
    collup upper_left_tile_rect(Vector2f(tile2.getPosition()), Vector2f(550, 10));
    collup lower_left_tile_rect(Vector2f(tile5.getPosition()), Vector2f(550, 10));
    collup upper_right_tile_rect(Vector2f(tile3.getPosition().x + 200, tile3.getPosition().y), Vector2f(435, 10));
    collup lower_right_tile_rect(Vector2f(tile7.getPosition().x + 210, tile7.getPosition().y), Vector2f(425, 10));
    collup upper_middle_tile_rect_(Vector2f(tile4.getPosition().x + 190, tile4.getPosition().y), Vector2f(260, 10), 3);
    collup lower_middle_tile_rect(Vector2f(tile6.getPosition().x + 190, tile6.getPosition().y), Vector2f(260, 10), -3);
    collup ground_rect(Vector2f(tile1.getPosition()), Vector2f(1920, 10));
    collup lower_middle_tile_spikes(Vector2f(spikes6.getPosition()), Vector2f(3, 5));
    collup upper_middle_tile_spikes1(Vector2f(spikes2.getPosition()), Vector2f(3, 5));
    collup upper_middle_tile_spikes2(Vector2f(spikes3.getPosition()), Vector2f(3, 5));
    collup upper_left_tile_spikes(Vector2f(spikes1.getPosition().x + 30, spikes1.getPosition().y + 10), Vector2f(3, 5));
    collup lower_left_tile_spikes(Vector2f(spikes4.getPosition().x + 30, spikes4.getPosition().y + 10), Vector2f(3, 5));

    //rectangles under the tiles (used in collision)
    colldown upper_left_tile_rect2(Vector2f(tile2.getPosition().x, tile2.getPosition().y + 80), Vector2f(535, 10));
    colldown lower_left_tile_rect2(Vector2f(tile5.getPosition().x, tile5.getPosition().y + 80), Vector2f(545, 10));
    colldown upper_right_tile_rect2(Vector2f(tile3.getPosition().x + 180, tile3.getPosition().y + 110), Vector2f(425, 10));
    colldown lower_right_tile_rect2(Vector2f(tile7.getPosition().x + 180, tile7.getPosition().y + 80), Vector2f(425, 10));
    colldown upper_middle_tile_rect2(Vector2f(tile4.getPosition().x + 180, tile4.getPosition().y + 80), Vector2f(260, 10));
    colldown lower_middle_tile_rect2(Vector2f(tile6.getPosition().x + 180, tile6.getPosition().y + 80), Vector2f(260, 10));

    //right rectangles (used in collision)
    colright upper_left_tile_rect3(Vector2f(tile2.getPosition().x + 550, tile2.getPosition().y + 7), Vector2f(5, 80));
    upper_left_tile_rect3.rotation(30);
    colright lower_left_tile_rect3(Vector2f(tile5.getPosition().x + 550, tile5.getPosition().y + 7), Vector2f(5, 80));
    lower_left_tile_rect3.rotation(30);
    colright upper_middle_tile_rect3(Vector2f(tile4.getPosition().x + 480, tile4.getPosition().y + 9), Vector2f(5, 60));
    colright lower_middle_tile_rect3(Vector2f(tile6.getPosition().x + 480, tile6.getPosition().y + 9), Vector2f(5, 60));

    //left rectangles (used in collision)
    colleft upper_right_tile_rect4(Vector2f(tile3.getPosition().x + 190, tile3.getPosition().y + 15), Vector2f(5, 95));
    upper_right_tile_rect4.rotation(-30.0f);
    colleft lower_right_tile_rect4(Vector2f(tile7.getPosition().x + 180, tile7.getPosition().y + 13), Vector2f(5, 80));
    lower_right_tile_rect4.rotation(-30.0f);
    colleft upper_middle_tile_rect4(Vector2f(tile4.getPosition().x + 165, tile4.getPosition().y + 9), Vector2f(5, 60));
    colleft lower_middle_tile_rect4(Vector2f(tile6.getPosition().x + 150, tile6.getPosition().y + 9), Vector2f(5, 60));


    //transition
    transition start(0), end(1);

    //Background music
    Music music;
    music.openFromFile("SoundTrack.wav");
    music.play();
    music.setVolume(15.0f);

    //Sounds
    Sound coins_sound;
    SoundBuffer coins_sound_buffer;
    coins_sound_buffer.loadFromFile("coin sound effect.wav");
    coins_sound.setBuffer(coins_sound_buffer);
    coins_sound.setVolume(10.0f);

    Sound hearts_sound;
    SoundBuffer hearts_sound_buffer;
    hearts_sound_buffer.loadFromFile("Heart Taken.wav");
    hearts_sound.setBuffer(hearts_sound_buffer);

    Sound key_sound;
    SoundBuffer key_sound_buffer;
    key_sound_buffer.loadFromFile("Key is taken.wav");
    key_sound.setBuffer(key_sound_buffer);
    key_sound.setVolume(200.0f);

    Sound metal_gate_sound;
    SoundBuffer metal_gate_sound_buffer;
    metal_gate_sound_buffer.loadFromFile("Metal_Door_Opening.wav");
    metal_gate_sound.setBuffer(metal_gate_sound_buffer);

    Sound wooden_gate_sound;
    SoundBuffer wooden_gate_sound_buffer;
    wooden_gate_sound_buffer.loadFromFile("Wooden Door Opening.wav");
    wooden_gate_sound.setBuffer(wooden_gate_sound_buffer);

    Sound button_sound;
    SoundBuffer button_sound_buffer;
    button_sound_buffer.loadFromFile("Button Pushed.wav");
    button_sound.setBuffer(button_sound_buffer);

    Sound jump_sound;
    SoundBuffer jump_sound_buffer;
    jump_sound_buffer.loadFromFile("Jump_Sound.wav");
    jump_sound.setBuffer(jump_sound_buffer);
    jump_sound.setVolume(30.0f);

    Sound movement_sound;
    SoundBuffer movement_sound_buffer;
    movement_sound_buffer.loadFromFile("Movement.wav");
    movement_sound.setBuffer(movement_sound_buffer);
    movement_sound.setVolume(1.0f);

    Sound damage_sound;
    SoundBuffer damage_sound_buffer;
    damage_sound_buffer.loadFromFile("Damage Taken.wav");
    damage_sound.setBuffer(damage_sound_buffer);

    Sound dead_sound;
    SoundBuffer dead_sound_buffer;
    dead_sound_buffer.loadFromFile("Game Over.wav");
    dead_sound.setBuffer(dead_sound_buffer);


    sf::Clock switch_time, player_switch1, player_switch2, movement_sound_switch;


    //Game loop
    while (win.isOpen()) {

        Event evnt;
        while (win.pollEvent(evnt)) {
            switch (evnt.type) {
            case Event::Closed:
                win.close();
                break;
            }
        }

        music.setVolume((abs(music_vol - 970) / 1260) * 100);
        coins_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        hearts_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        key_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        metal_gate_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        wooden_gate_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        button_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        jump_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);
        movement_sound.setVolume((abs(sfx_vol - 970) / 1260) * 5);
        damage_sound.setVolume((abs(sfx_vol - 970) / 1260) * 50);
        dead_sound.setVolume((abs(sfx_vol - 970) / 1260) * 100);

        if (dead)
        {
            if (color_cnt < 200)
                color_cnt += 2;
            screen_rect.setFillColor(sf::Color(0, 0, 0, color_cnt));
            if (color_cnt == 200) {
                if (gameOver_delay != 180) gameOver_delay++;
                else { indicator = 2; return; }
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Escape) || indicator != 1) { if (!indicator) { indicator = -1; goto render; } pauseMenu(indicator, 1); }

        if (indicator == 2 || indicator == 4) return;

        if (indicator == 3) soundControl(), indicator = 0;

        if (complete) {
            end.update(1);
            if (end.retrn()) {
                return;
            }
        }
        if (indicator == 1) {
            if (!dead && !complete)
            {
                //coins & hearts animation 
                if (switch_time.getElapsedTime().asSeconds() > 0.08) {
                    coins_cnt++;
                    hearts_cnt++;
                    switch_time.restart();
                }
                coins_cnt %= 6;
                hearts_cnt %= 6;


                //the events the characters will do in the game
                if (Keyboard::isKeyPressed(Keyboard::Key::W)) {
                    sPlayer1.move(0, -5);
                    movement_sound.pause();
                    if (grounded1)
                        jump_sound.play();
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
                    sPlayer1.move(-5, 0);
                    ctrY = 3;
                    if (grounded1) {
                        if (movement_sound_switch.getElapsedTime().asSeconds() > 0.50) {
                            movement_sound.play();
                            movement_sound_switch.restart();
                        }
                    }

                    if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                        ctrX++;
                        player_switch1.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
                    sPlayer1.move(5, 0);
                    ctrY = 2;

                    if (grounded1) {
                        if (movement_sound_switch.getElapsedTime().asSeconds() > 0.50) {
                            movement_sound.play();
                            movement_sound_switch.restart();
                        }
                    }

                    if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                        ctrX++;
                        player_switch1.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::D)) {
                    sPlayer1.move(3, 3);
                    ctrY = 2;

                    if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                        ctrX++;
                        player_switch1.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::A)) {
                    sPlayer1.move(-3, 3);
                    ctrY = 3;

                    if (player_switch1.getElapsedTime().asSeconds() > 0.04) {
                        ctrX++;
                        player_switch1.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
                    sPlayer2.move(0, -5);
                    movement_sound.pause();
                    if (grounded2)
                        jump_sound.play();
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                    sPlayer2.move(-5, 0);
                    ctrY2 = 3;
                    if (grounded2) {
                        if (movement_sound_switch.getElapsedTime().asSeconds() > 0.50) {
                            movement_sound.play();
                            movement_sound_switch.restart();
                        }
                    }

                    if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                        ctrX2++;
                        player_switch2.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                    sPlayer2.move(5, 0);
                    ctrY2 = 2;
                    if (grounded2) {
                        if (movement_sound_switch.getElapsedTime().asSeconds() > 0.50) {
                            movement_sound.play();
                            movement_sound_switch.restart();
                        }
                    }

                    if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                        ctrX2++;
                        player_switch2.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                    sPlayer2.move(3, 3);
                    ctrY2 = 2;

                    if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                        ctrX2++;
                        player_switch2.restart();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                    sPlayer2.move(-3, 3);
                    ctrY2 = 3;

                    if (player_switch2.getElapsedTime().asSeconds() > 0.04) {
                        ctrX2++;
                        player_switch2.restart();
                    }
                }

                ctrX %= 2;
                sPlayer1.setTextureRect(IntRect(ctrX * 32, ctrY * 34, 32, 28));

                ctrX2 %= 2;
                sPlayer2.setTextureRect(IntRect(ctrX2 * 32, ctrY2 * 33, 32, 28));


                //Collision
                if (sPlayer1.getGlobalBounds().intersects(b1.getGlobalBounds())) {
                    sPlayer1.move(5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::A)) {
                        sPlayer1.move(3, 3);
                    }
                }

                if (sPlayer2.getGlobalBounds().intersects(b1.getGlobalBounds())) {
                    sPlayer2.move(5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                        sPlayer2.move(3, 3);
                    }
                }

                if (sPlayer1.getGlobalBounds().intersects(b2.getGlobalBounds())) {
                    sPlayer1.move(-5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::D)) {
                        sPlayer1.move(-3, 3);
                    }
                }

                if (sPlayer2.getGlobalBounds().intersects(b2.getGlobalBounds())) {
                    sPlayer2.move(-5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                        sPlayer2.move(-3, 3);
                    }
                }


                if (sPlayer1.getGlobalBounds().intersects(key.getGlobalBounds())) {
                    key.setScale(0, 0);
                    keys[0] = true;
                    gate.setTexture(opened_gate_texture);
                    gate.setScale(1.43f, 1.43f);
                    gate.setPosition(1740.0f, 203.0f);
                    key_sound.play();
                    wooden_gate_sound.play();
                }

                if (sPlayer2.getGlobalBounds().intersects(key.getGlobalBounds())) {
                    key.setScale(0, 0);
                    keys[0] = true;
                    gate.setTexture(opened_gate_texture);
                    gate.setScale(1.43f, 1.43f);
                    gate.setPosition(1740.0f, 203.0f);
                    key_sound.play();
                    wooden_gate_sound.play();
                }

                if (sPlayer1.getGlobalBounds().intersects(hearts[0].getGlobalBounds()) && health1 < 3) {
                    hearts[0].setScale(0, 0);
                    hearts_sound.play();
                    health1++;
                }

                if (sPlayer2.getGlobalBounds().intersects(hearts[1].getGlobalBounds()) && health2 < 3) {
                    hearts[1].setScale(0, 0);
                    hearts_sound.play();
                    health2++;
                }

                // collision with metal gate
                if (sPlayer1.getGlobalBounds().intersects(door.getGlobalBounds()) && sPlayer1.getPosition().x < (1660.0f)) {
                    sPlayer1.move(-5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::D)) {
                        sPlayer1.move(-3, 3);
                    }
                }

                if (sPlayer2.getGlobalBounds().intersects(door.getGlobalBounds()) && sPlayer2.getPosition().x < (1660.0f)) {
                    sPlayer2.move(-5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                        sPlayer2.move(-3, 3);
                    }
                }

                if (sPlayer1.getGlobalBounds().intersects(door.getGlobalBounds()) && sPlayer1.getPosition().x > (1720.0f)) {
                    sPlayer1.move(5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::A)) {
                        sPlayer1.move(3, 3);
                    }
                }

                if (sPlayer2.getGlobalBounds().intersects(door.getGlobalBounds()) && sPlayer2.getPosition().x > (1720.0f)) {
                    sPlayer2.move(5, 0);

                    if (Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                        sPlayer2.move(3, 3);
                    }
                }


                button.setTexture(button_texture);
                door.setPosition(1600.0f, 455.0f);
                if (button_rect.collision(sPlayer1)) {
                    button.setTexture(buttonpressed_texture);
                    door.setPosition(1600.0f, 360.0f);
                    if (!sound_cnt) {
                        button_sound.play();
                        metal_gate_sound.play();
                        sound_cnt++;
                    }
                }

                else if (button_rect.collision(sPlayer2)) {
                    button.setTexture(buttonpressed_texture);
                    door.setPosition(1600.0f, 360.0f);
                    if (sound_cnt == 0) {
                        button_sound.play();
                        metal_gate_sound.play();
                        sound_cnt++;
                    }
                }

                if (!button_rect.collision(sPlayer1) && !button_rect.collision(sPlayer2)) {
                    if (sound_cnt == 1) {
                        button_sound.play();
                        metal_gate_sound.play();
                    }
                    sound_cnt = 0;
                }


                score_num.setString("x" + to_string(coins_cntr));
                for (int i = 0; i < 3; i++) {
                    if (sPlayer1.getGlobalBounds().intersects(coins[i].getGlobalBounds())) {
                        coins[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }

                    if (sPlayer1.getGlobalBounds().intersects(coins1[i].getGlobalBounds())) {
                        coins1[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }

                    if (sPlayer1.getGlobalBounds().intersects(coins2[i].getGlobalBounds())) {
                        coins2[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }

                    if (sPlayer2.getGlobalBounds().intersects(coins[i].getGlobalBounds())) {
                        coins[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }

                    if (sPlayer2.getGlobalBounds().intersects(coins1[i].getGlobalBounds())) {
                        coins1[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }

                    if (sPlayer2.getGlobalBounds().intersects(coins2[i].getGlobalBounds())) {
                        coins2[i].setScale(0, 0);
                        coins_sound.play();
                        coins_cntr++;
                    }

                }

                if (health1 == 0) {
                    sPlayer1.setTextureRect(IntRect(0, 11 * 32.3, 32, 28));
                    dead = true;
                }

                if (health2 == 0) {
                    sPlayer2.setTextureRect(IntRect(0, 4 * 32.4, 32, 28));
                    dead = true;
                }

                if (sPlayer1.getGlobalBounds().intersects(gate_rect.getGlobalBounds())
                    && sPlayer2.getGlobalBounds().intersects(gate_rect.getGlobalBounds())
                    && keys[0] == true && coins_cntr == 9) {

                    gate_pass[0] = true;
                    complete = 1;
                }


                //jumping & Collision with rectangles
                grounded1 = 0;
                grounded2 = 0;

                //collision with upper rects
                upper_left_tile_rect.update(grounded1, grounded2, sPlayer1, sPlayer2);
                lower_left_tile_rect.update(grounded1, grounded2, sPlayer1, sPlayer2);
                upper_right_tile_rect.update(grounded1, grounded2, sPlayer1, sPlayer2);
                lower_right_tile_rect.update(grounded1, grounded2, sPlayer1, sPlayer2);

                upper_middle_tile_rect_.update(grounded1, grounded2, sPlayer1, sPlayer2, 1, { 440 , 780 });
                tile4.setPosition({ upper_middle_tile_rect_.getpos().x - 190, upper_middle_tile_rect_.getpos().y });
                spikes3.setPosition({ upper_middle_tile_rect_.getpos().x + 160, upper_middle_tile_rect_.getpos().y - 10 });
                spikes2.setPosition({ upper_middle_tile_rect_.getpos().x - 20, upper_middle_tile_rect_.getpos().y - 10 });

                lower_middle_tile_rect.update(grounded1, grounded2, sPlayer1, sPlayer2, 1, { 440 , 780 });
                tile6.setPosition({ lower_middle_tile_rect.getpos().x - 190, lower_middle_tile_rect.getpos().y });
                spikes6.setPosition({ lower_middle_tile_rect.getpos().x + 90, lower_middle_tile_rect.getpos().y - 10 });
                ground_rect.update(grounded1, grounded2, sPlayer1, sPlayer2);



                //collision with lower rects
                upper_left_tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                lower_left_tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                upper_right_tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                lower_right_tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2);
                upper_middle_tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2, 1, { upper_middle_tile_rect_.getpos().x, upper_middle_tile_rect_.getpos().y + 70 });
                lower_middle_tile_rect2.update(velocityY1, velocityY2, sPlayer1, sPlayer2, 1, { lower_middle_tile_rect.getpos().x, lower_middle_tile_rect.getpos().y + 70 });


                //right rectangles (used in collision)
                upper_left_tile_rect3.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                lower_left_tile_rect3.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                upper_middle_tile_rect3.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2, 1, { upper_middle_tile_rect_.getpos().x + 265 , upper_middle_tile_rect_.getpos().y + 10 });
                lower_middle_tile_rect3.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2, 1, { lower_middle_tile_rect.getpos().x + 265 , lower_middle_tile_rect.getpos().y + 10 });


                //left rectangles (used in collision)
                upper_right_tile_rect4.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                lower_right_tile_rect4.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2);
                upper_middle_tile_rect4.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2, 1, { upper_middle_tile_rect_.getpos().x - 10 , upper_middle_tile_rect_.getpos().y + 10 });
                lower_middle_tile_rect4.update(sPlayer1, sPlayer2, velocityY1, velocityY2, grounded1, grounded2, 1, { lower_middle_tile_rect.getpos().x - 10 , lower_middle_tile_rect.getpos().y + 10 });


                //collision with the spikes
                health_bar1.setTexture(health_bar1_texture[health1]);
                health_bar2.setTexture(health_bar2_texture[health2]);
                lower_middle_tile_spikes.setpos(Vector2f(spikes6.getPosition().x + 30, spikes6.getPosition().y + 10));
                upper_middle_tile_spikes1.setpos(Vector2f(spikes2.getPosition().x + 30, spikes2.getPosition().y + 10));
                upper_middle_tile_spikes2.setpos(Vector2f(spikes3.getPosition().x + 30, spikes3.getPosition().y + 10));

                //transition
                start.update(1);

                if (lower_middle_tile_spikes.collision(sPlayer1) || upper_middle_tile_spikes1.collision(sPlayer1) || upper_middle_tile_spikes2.collision(sPlayer1) || upper_left_tile_spikes.collision(sPlayer1) || lower_left_tile_spikes.collision(sPlayer1)) {
                    if (hb_delay1 == 60) {

                        if (health1 > 0) {
                            health1--;
                            damage_sound.play();
                        }
                        hb_delay1 = 0;
                    }
                    else
                        hb_delay1++;
                }
                else {
                    hb_delay1 = 60;
                }

                if (lower_middle_tile_spikes.collision(sPlayer2) || upper_middle_tile_spikes1.collision(sPlayer2) || upper_middle_tile_spikes2.collision(sPlayer2) || upper_left_tile_spikes.collision(sPlayer2) || lower_left_tile_spikes.collision(sPlayer2)) {
                    if (hb_delay2 == 60) {

                        if (health2 > 0) {
                            health2--;
                            damage_sound.play();
                        }
                        hb_delay2 = 0;
                    }
                    else
                        hb_delay2++;
                }
                else {
                    hb_delay2 = 60;
                }

                //upper rectangles of the square tile
                if (square_rect.getGlobalBounds().intersects(sPlayer1.getGlobalBounds())) {
                    grounded1 = true;
                }

                if (square_rect.getGlobalBounds().intersects(sPlayer2.getGlobalBounds())) {
                    grounded2 = true;
                }

                //right rectangle of the square tile
                if ((Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::A)) || (Keyboard::isKeyPressed(Keyboard::Key::A) && !grounded1))
                {
                    if (sPlayer1.getGlobalBounds().intersects(square3_rect.getGlobalBounds()))
                    {
                        sPlayer1.move(5, 0);
                        velocityY1 = -10;

                    }
                }

                if ((Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Left)) || (Keyboard::isKeyPressed(Keyboard::Key::Left) && !grounded2)) {

                    if (sPlayer2.getGlobalBounds().intersects(square3_rect.getGlobalBounds()))
                    {
                        sPlayer2.move(5, 0);
                        velocityY2 = -10;

                    }
                }

                if ((Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::A)) || (Keyboard::isKeyPressed(Keyboard::Key::A)))
                {
                    if (sPlayer1.getGlobalBounds().intersects(square3_rect.getGlobalBounds()))
                    {
                        sPlayer1.move(5, 0);
                    }
                }

                if ((Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Left)) || (Keyboard::isKeyPressed(Keyboard::Key::Left))) {

                    if (sPlayer2.getGlobalBounds().intersects(square3_rect.getGlobalBounds()))
                    {
                        sPlayer2.move(5, 0);
                    }
                }

                //left rectangles of the square tiles
                if ((Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::D)) || (Keyboard::isKeyPressed(Keyboard::Key::D) && !grounded1))
                {
                    if (sPlayer1.getGlobalBounds().intersects(square4_rect.getGlobalBounds()))
                    {
                        sPlayer1.move(-5, 0);
                        velocityY1 = -10;

                    }
                }

                if ((Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Right)) || (Keyboard::isKeyPressed(Keyboard::Key::Right) && !grounded2)) {

                    if (sPlayer2.getGlobalBounds().intersects(square4_rect.getGlobalBounds()))
                    {
                        sPlayer2.move(-5, 0);
                        velocityY2 = -10;

                    }
                }

                if ((Keyboard::isKeyPressed(Keyboard::Key::W) && Keyboard::isKeyPressed(Keyboard::Key::D)) || (Keyboard::isKeyPressed(Keyboard::Key::D)))
                {
                    if (sPlayer1.getGlobalBounds().intersects(square4_rect.getGlobalBounds()))
                    {
                        sPlayer1.move(-5, 0);
                    }
                }

                if ((Keyboard::isKeyPressed(Keyboard::Key::Up) && Keyboard::isKeyPressed(Keyboard::Key::Right)) || (Keyboard::isKeyPressed(Keyboard::Key::Right)))
                {

                    if (sPlayer2.getGlobalBounds().intersects(square4_rect.getGlobalBounds()))
                    {
                        sPlayer2.move(-5, 0);
                    }
                }


                if (!grounded1)
                {
                    velocityY1 -= 1;
                }

                if (grounded1)
                {
                    velocityY1 = 0;
                    if (Keyboard::isKeyPressed(Keyboard::Key::W)) {
                        velocityY1 = 15;
                    }
                }

                sPlayer1.move(0, -velocityY1);

                if (!grounded2) {
                    velocityY2 -= 1;
                }

                if (grounded2) {
                    velocityY2 = 0;
                    if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
                        velocityY2 = 15;
                    }
                }

                sPlayer2.move(0, -velocityY2);
            }
        }

    render:;
        //drawing and displaying objects
        win.clear();
        win.draw(background);
        win.draw(door);
        win.draw(tile1);
        win.draw(tile2);
        win.draw(tile8);
        win.draw(tile3);
        win.draw(tile4);
        win.draw(tile5);
        win.draw(tile6);
        win.draw(tile7);
        win.draw(tile9);
        win.draw(gate);
        win.draw(key);
        win.draw(button);
        win.draw(spikes1);
        win.draw(spikes2);
        win.draw(spikes3);
        win.draw(spikes4);
        win.draw(spikes6);
        win.draw(health_bar1);
        win.draw(health_bar2);
        win.draw(score);
        win.draw(score_num);

        for (int i = 0; i < 3; i++) {
            if (!dead)
            {
                coins[i].setTextureRect(IntRect(coins_cnt * coins_texture_size.x, 0, coins_texture_size.x, coins_texture_size.y));
                coins1[i].setTextureRect(IntRect(coins_cnt * coins_texture_size.x, 0, coins_texture_size.x, coins_texture_size.y));
                coins2[i].setTextureRect(IntRect(coins_cnt * coins_texture_size.x, 0, coins_texture_size.x, coins_texture_size.y));
            }
            win.draw(coins[i]);
            win.draw(coins1[i]);
            win.draw(coins2[i]);
        }

        for (int i = 0; i < 2; i++) {
            if (!dead)
                hearts[i].setTextureRect(IntRect(hearts_cnt * hearts_texture_size.x, 0, hearts_texture_size.x, hearts_texture_size.y));
            win.draw(hearts[i]);
        }

        win.draw(sPlayer1);
        win.draw(sPlayer2);
        win.draw(screen_rect);

        if (color_cnt > 110) {
            win.draw(game_over);
            if (!sound_played) {
                music.pause();
                dead_sound.play();
                sound_played = true;
            }
        }
        end.draw();
        start.draw();
        win.display();
    }
}


void game()
{
    int lst1, lst2;
    while (true)
    {
        //update
        while (!keys[0] || !gate_pass[0]) {
            if (indicator == 2) health1 = 3, health2 = 3;
            if (indicator != 4) {
                indicator = 1, coins_cntr = 0, lvl1();
            }
            else { indicator = 1; goto x; }
        }
        lst1 = health1, lst2 = health2;
        while (!gate_pass[1] || !keys[1]) {
            if (indicator == 2) health1 = lst1, health2 = lst2;
            if (indicator != 4) {
                indicator = 1, coins_cntr = 9, lvl2();
            }
            else { indicator = 1; goto x; }
        }
        lst1 = health1, lst2 = health2;
        while (!gate_pass[2] || !keys[2]) {
            if (indicator == 2) health1 = lst1, health2 = lst2;
            if (indicator != 4) {
                indicator = 1, coins_cntr = 21, lvl3();
            }
            else { indicator = 1; goto x; }
        }
        lst1 = health1, lst2 = health2;
        while (!gate_pass[3] || !keys[3]) {
            if (indicator == 2) health1 = lst1, health2 = lst2;
            if (indicator != 4) {
                indicator = 1, coins_cntr = 29, lvl4();
            }
            else { indicator = 1; goto x; }
        }
        lst1 = health1, lst2 = health2;
        while (!gate_pass[4] || !keys[4]) {
            if (indicator == 2) health1 = lst1, health2 = lst2;
            if (indicator != 4) {
                indicator = 1, coins_cntr = 41, LvlBoss();
            }
            else { indicator = 1; goto x; }
        }
        for (auto& i : keys) i = 0;
        for (auto& i : gate_pass) i = 0;

        lvl6(); goto x;
    }
x:;
    background_music.play();
}


void Controls()
{
    Text controls[11];
    RectangleShape collisionBox;
    //controls declration
    {
        for (auto& i : controls)
        {
            i.setFont(font);
            i.setFillColor(Color::White);
            i.setCharacterSize(80);
        }
        //setting string
        controls[0].setString("              PLAYER 1             ");
        controls[1].setString("A/D");
        controls[2].setString("W");
        controls[3].setString("              PLAYER 2             ");
        controls[4].setString("RIGHT/LEFT ARROWS");
        controls[5].setString("UP ARROW");
        controls[6].setString("JUMP");
        controls[7].setString("MOVE");
        controls[8].setString("MOVE");
        controls[9].setString("JUMP");
        controls[10].setString("Back");
        //setting positions

        controls[0].setPosition(550 + 250, 100);
        controls[1].setPosition(350 + 250, 200);
        controls[8].setPosition(1000 + 250, 200);
        controls[2].setPosition(350 + 250, 300);
        controls[6].setPosition(1000 + 250, 300);
        controls[3].setPosition(550 + 250, 450);
        controls[4].setPosition(350 + 250, 550);
        controls[7].setPosition(1000 + 250, 550);
        controls[5].setPosition(350 + 250, 650);
        controls[9].setPosition(1000 + 250, 650);
        controls[10].setPosition(960, 870);
        collisionBox.setPosition(controls[10].getPosition().x, controls[10].getPosition().y + 15);
        collisionBox.setSize(Vector2f(90.f, 60.f));
    }
    while (true) {

        while (win.pollEvent(ev))
        {
            switch (ev.type)
            {
            case Event::Closed:
                background_music.stop();
                win.close();
            case Event::KeyReleased:
                if (ev.key.code == Keyboard::Escape)
                    goto back;
            default:
                break;
            }
        }

        Vector2i mousePos = Mouse::getPosition(win);
        Vector2f mousePosVeiw = win.mapPixelToCoords(mousePos);
        //registering mouse clicks
        {
            if (collisionBox.getGlobalBounds().contains(mousePosVeiw))
            {
                controls[10].setFillColor(Color::Yellow);

                if (Mouse::isButtonPressed(Mouse::Left))
                {
                    menu_sounds[0].play();
                    goto back;
                }
            }
            else
                controls[10].setFillColor(Color::White);
        }

        win.clear();
        win.draw(background);
        for (auto& i : controls)
            win.draw(i);
        win.display();
    }
back:;
}


void settings()
{
    Text settings[3];

    settings[0].setString("CONTROLS");
    settings[0].setFont(font);
    settings[0].setFillColor(Color::White);
    settings[0].setPosition(Vector2f(1070 + 450, 580));
    settings[0].setCharacterSize(70);

    settings[1].setString("VOLUME");
    settings[1].setFont(font);
    settings[1].setFillColor(Color::White);
    settings[1].setPosition(Vector2f(1150 + 450, 685));
    settings[1].setCharacterSize(70);

    settings[2].setString("BACK");
    settings[2].setFont(font);
    settings[2].setFillColor(Color::White);
    settings[2].setPosition(Vector2f(1230 + 465, 790));
    settings[2].setCharacterSize(70);

    RectangleShape settings_collision[3];
    {
        int i = 0;
        for (auto& e : settings_collision)
        {
            e.setPosition(settings[i].getPosition().x + 5, settings[i].getPosition().y + 5);
            e.setFillColor(Color(255, 255, 255, 0));
            i++;
        }
        settings_collision[2].setPosition(settings[2].getPosition().x, settings[2].getPosition().y + 15);
        settings_collision[0].setSize(Vector2f(160.f, 60.f));
        settings_collision[1].setSize(Vector2f(160.f, 60.f));
        settings_collision[2].setSize(Vector2f(90.f, 40.f));
    }

    //game loop
    while (true)
    {
        while (win.pollEvent(ev))
        {
            switch (ev.type)
            {
            case Event::Closed:
                background_music.stop();
                win.close();

            default:
                break;
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
        {
            break;
        }

        Vector2i mousePos = Mouse::getPosition(win);
        Vector2f mousePosVeiw = win.mapPixelToCoords(mousePos);
        //registering mouse clicks
        {
            if (settings_collision[0].getGlobalBounds().contains(mousePosVeiw))
            {
                settings[0].setFillColor(Color::Yellow);

                if (Mouse::isButtonPressed(Mouse::Left))
                {
                    menu_sounds[0].play();
                    Controls();
                }
            }
            else
                settings[0].setFillColor(Color::White);

            if (settings_collision[1].getGlobalBounds().contains(mousePosVeiw))
            {
                settings[1].setFillColor(Color::Yellow);

                if (Mouse::isButtonPressed(Mouse::Left))
                {
                    menu_sounds[0].play();
                    soundControl();
                }
            }
            else
                settings[1].setFillColor(Color::White);

            if (settings_collision[2].getGlobalBounds().contains(mousePosVeiw))
            {
                settings[2].setFillColor(Color::Yellow);

                if (Mouse::isButtonPressed(Mouse::Left))
                {
                    menu_sounds[0].play();
                    goto there;
                }
            }
            else
                settings[2].setFillColor(Color::White);
        }

        //render
        win.clear();
        win.draw(background);
        for (auto& i : settings)
            win.draw(i);
        win.display();
    }
there:;
}


void MainMenu()
{
    background_music.play();
    // menu decleration
    Mouse mouse;
    {
        menu[0].setString("START");
        menu[0].setFont(font);
        menu[0].setFillColor(Color::White);
        menu[0].setPosition(Vector2f(1170 + 440, 630));
        menu[0].setCharacterSize(80);

        menu[1].setString("SETTINGS");
        menu[1].setFont(font);
        menu[1].setFillColor(Color::White);
        menu[1].setPosition(Vector2f(1150 + 430, 735));
        menu[1].setCharacterSize(80);

        menu[2].setString("EXIT");
        menu[2].setFont(font);
        menu[2].setFillColor(Color::White);
        menu[2].setPosition(Vector2f(1190 + 440, 840));
        menu[2].setCharacterSize(80);
    }

    //menu collision boxes decleration
    RectangleShape menuCollision[3];
    {
        int i = 0;
        for (auto& e : menuCollision)
        {
            e.setPosition(menu[i].getPosition().x, menu[i].getPosition().y + 5);
            i++;
        }
        menuCollision[1].setPosition(menu[1].getPosition().x, menu[1].getPosition().y + 15);
        menuCollision[0].setSize(Vector2f(95.f, 60.f));
        menuCollision[1].setSize(Vector2f(130.f, 50.f));
        menuCollision[2].setSize(Vector2f(80.f, 60.f));
    }

    while (win.isOpen())
    {
        //update
        while (win.pollEvent(ev))
        {
            switch (ev.type)
            {
            case Event::Closed:
                background_music.stop();
                win.close();

            default:
                break;
            }
        }

        //menu button animation and clicks
        Vector2i mousePos = Mouse::getPosition(win);
        Vector2f mousePosVeiw = win.mapPixelToCoords(mousePos);
        {
            if (menuCollision[0].getGlobalBounds().contains(mousePosVeiw))
            {
                menu[0].setFillColor(Color::Yellow);
                if (Mouse::isButtonPressed(Mouse::Left))
                {
                    menu_sounds[0].play();
                    background_music.stop();
                    health1 = 3;
                    health2 = 3;
                    game();
                }
            }
            else
                menu[0].setFillColor(Color::White);
            if (menuCollision[1].getGlobalBounds().contains(mousePosVeiw))
            {
                menu[1].setFillColor(Color::Yellow);
                if (Mouse::isButtonPressed(Mouse::Left)) {
                    menu_sounds[0].play();
                    settings();
                }
            }
            else
                menu[1].setFillColor(Color::White);
            if (menuCollision[2].getGlobalBounds().contains(mousePosVeiw))
            {
                menu[2].setFillColor(Color::Yellow);
                if (Mouse::isButtonPressed(Mouse::Left))
                {
                    menu_sounds[0].play();
                    background_music.stop();
                    win.close();
                }
            }
            else
                menu[2].setFillColor(Color::White);
        }

        //render
        win.clear();
        //win.setView(view);
        win.draw(background);
        for (auto& e : menu)
            win.draw(e);
        win.display();
    }
}


int main()
{
    //loading font
    font.loadFromFile("Amatic-Bold.ttf");
    //setting background
    background_tex.loadFromFile("menu.jpg");
    background.setTexture(background_tex);
    background.setPosition(0.f, 0.f);
    background.setScale(0.9, 0.9);

    win.setFramerateLimit(30);

    background_buffer.loadFromFile("alone with the darkness.wav");
    background_music.setBuffer(background_buffer);
    background_music.setLoop(true);
    background_music.setVolume(100.f);

    menu_buffer[0].loadFromFile("Copy of 1.wav");
    menu_buffer[1].loadFromFile("Copy of 2.wav");
    menu_sounds[0].setBuffer(menu_buffer[0]);
    menu_sounds[0].setVolume(100.f);
    menu_sounds[1].setBuffer(menu_buffer[1]);
    menu_sounds[1].setVolume(100.f);

    MainMenu();

    return 0;
}