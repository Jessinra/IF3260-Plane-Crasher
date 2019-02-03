#include <assert.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <ctime>
#include <iostream>
#include <utility>
#include "Master.hpp"
#include "MoveableObject.hpp"
#include "Object.hpp"

using namespace std;

/* Aku sedih */
int deg = 0;
bool keepreading;

void *readinput(void *threadid)
{
    char c;
    while (keepreading)
    {
        c = getchar();
        if (c == 'd' && deg <= 100)
        {
            deg += 10;
        }
        else if (c == 'a' && deg >= -100)
        {
            deg -= 10;
        }
        usleep(10000);
    }
    pthread_exit(NULL);
}
/* Aku sedih */

class Runner : public Master
{
  protected:
    Object pesawat, meriam, peluru, puing1, puing2, puing3;

  public:
    Runner(int h = 700, int w = 1000) : Master(h, w)
    {
        pesawat = Object(0, 0, "object_plane.txt");
        meriam = Object(0, 0, "object_gun.txt");
        peluru = Object(0, 0, "object_star.txt");
        puing1 = Object(0, 0, "object_plane_part1.txt");
        puing2 = Object(0, 0, "object_plane_part2.txt");
        puing3 = Object(0, 0, "object_plane_part3.txt");
    }

    void start()
    {
        int sudut_meriam = 0;
        float titik_acuan_x, titik_acuan_y;
        titik_acuan_x = xend / 2.0;
        titik_acuan_y = yend - 2;

        pesawat.setPos(Pixel(xend, 0));
        meriam.setPos(Pixel((xend - meriam.getWidth()) / 2.0,
                            yend - meriam.getHeight() - 2));
        peluru.setPos(
            Pixel((xend - peluru.getWidth()) / 2.0,
                  yend - meriam.getHeight() - peluru.getHeight() - 2));
        vector<MoveableObject> planes;
        vector<MoveableObject> debris;
        vector<MoveableObject> bullets;
        MoveableObject cannon = meriam;
        planes.push_back(MoveableObject(-1, 0, 1, pesawat));
        bullets.push_back(MoveableObject(0, -1, 2, peluru));

        Object *solidObject;
        for (int i = 1;; i = (i + 1) % 1000)
        {
            // draw
            clearWindow();

            for (MoveableObject &movableObject : planes)
            {
                drawObject(movableObject);
                drawSolidObject(&movableObject);
            }
            for (MoveableObject &movableObject : debris)
            {
                drawObject(movableObject);
                drawSolidObject(&movableObject);
            }
            for (MoveableObject &movableObject : bullets)
            {
                drawObject(movableObject);
                drawSolidObject(&movableObject);
            }

            drawObject(cannon);
            drawSolidObject(&cannon);

            // move and rotate :/
            if (deg != 0)
            {
                if (deg > 0)
                {
                    if (sudut_meriam <= 40)
                    {
                        sudut_meriam += 10;
                        cannon = MoveableObject(meriam);
                        cannon.selfRotation(titik_acuan_x, titik_acuan_y,
                                            sudut_meriam);
                        deg -= 10;
                    }
                    else
                    {
                        deg = 0;
                    }
                }
                else
                {
                    if (sudut_meriam >= -40)
                    {
                        sudut_meriam -= 10;
                        cannon = MoveableObject(meriam);
                        cannon.selfRotation(titik_acuan_x, titik_acuan_y,
                                            sudut_meriam);
                        deg += 10;
                    }
                    else
                    {
                        deg = 0;
                    }
                }
            }
            vector<char> checkp(planes.size(), 1);
            vector<char> checkd(debris.size(), 1);
            vector<MoveableObject> tmpp; // plane
            vector<MoveableObject> tmpb; // bullet
            vector<MoveableObject> tmpd; // debris
            for (int j = 0; j < planes.size(); ++j)
            {
                planes[j].move();
                if (planes[j].outOfWindow(yend, xend))
                {
                    checkp[j] = 0;
                }
            }
            for (int j = 0; j < debris.size(); ++j)
            {
                debris[j].move();
                if (debris[j].outOfWindow(yend, xend))
                {
                    checkd[j] = 0;
                }
            }
            for (int j = 0; j < bullets.size(); ++j)
            {
                bullets[j].move();
            }

            // very slow shit
            for (const MoveableObject &objb : bullets)
            {
                bool bisa = true;
                for (int j = 0; j < planes.size(); ++j)
                {
                    if (overlap(planes[j], objb))
                    {
                        // isi pecahan
                        if (checkp[j])
                        {
                            MoveableObject sp = puing1;
                            sp.setPos(planes[j].getRefPos());
                            sp.setVector((planes[j].getDx() < 0 ? -1 : 1) *
                                             sin(60 * PI / 180),
                                         cos(60 * PI / 180));
                            tmpd.push_back(sp);
                            sp = puing2;
                            sp.setPos(Pixel(planes[j].getRefPos().getX() + 100,
                                            planes[j].getRefPos().getY() + 50));
                            sp.setVector((planes[j].getDx() < 0 ? -1 : 1) *
                                             sin(45 * PI / 180),
                                         cos(45 * PI / 180));
                            tmpd.push_back(sp);
                            sp = puing3;
                            sp.setPos(Pixel(planes[j].getRefPos().getX() + 300,
                                            planes[j].getRefPos().getY()));
                            sp.setVector((planes[j].getDx() < 0 ? -1 : 1) *
                                             sin(30 * PI / 180),
                                         cos(30 * PI / 180));
                            tmpd.push_back(sp);
                            checkp[j] = 0;
                        }
                        bisa = false;
                    }
                }
                for (int j = 0; j < debris.size(); ++j)
                {
                    debris[j].move();
                    if (overlap(debris[j], objb))
                    {
                        checkd[j] = 0;
                        bisa = false;
                    }
                }
                if (bisa && !objb.outOfWindow(yend, xend))
                {
                    tmpb.push_back(objb);
                }
            }
            for (int j = 0; j < planes.size(); ++j)
            {
                if (checkp[j])
                    tmpp.push_back(planes[j]);
            }
            for (int j = 0; j < debris.size(); ++j)
            {
                if (checkd[j])
                    tmpd.push_back(debris[j]);
            }
            planes = tmpp;
            bullets = tmpb;
            debris = tmpd;

            if (i % 200 == 0)
            {
                MoveableObject tmp = MoveableObject(peluru);
                tmp.setSpeed(2);
                tmp.selfRotation(titik_acuan_x, titik_acuan_y, sudut_meriam);
                tmp.setVector(sin(sudut_meriam * PI / 180),
                              -cos(sudut_meriam * PI / 180));
                bullets.push_back(tmp);
            }
            if (i % 500 == 0)
            {
                planes.push_back(MoveableObject(-1, 0, 1, pesawat));
            }

            usleep(6000);
        }
    }

    bool overlap(const Object &p, const Object &q)
    {
        int a, b, c, d, e, f, g, h;
        a = p.getRefPos().getX();
        b = p.getRefPos().getY();
        c = a + p.getWidth() - 1;
        d = b + p.getHeight() - 1;
        e = q.getRefPos().getX();
        f = q.getRefPos().getY();
        g = e + q.getWidth() - 1;
        h = f + q.getHeight() - 1;
        if (a > g || e > c)
            return false;
        if (b > h || f > d)
            return false;
        return true;
    }
};

int main()
{
    /* non-newline input */
    struct termios org_opts, new_opts;
    int res = 0;
    res = tcgetattr(STDIN_FILENO, &org_opts);
    assert(res == 0);
    memcpy(&new_opts, &org_opts, sizeof(new_opts));
    new_opts.c_lflag &=
        ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);

    /* Multithreading part */
    pthread_t thread;
    int rc, id = 0;
    keepreading = true;
    rc = pthread_create(&thread, NULL, readinput, (void *)id);

    Runner run;
    run.start();

    /* close hehe */
    pthread_exit(NULL);
    res = tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
    assert(res == 0);
    return 0;
}