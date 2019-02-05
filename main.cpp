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
int shoot = 0;
bool keepreading;

void *readinput(void *threadid){
    char c;
    while (keepreading){
        c = getchar();
        if(c == '\n'){
            shoot++;
        }
        else if (c == 'd' && deg <= 100){
            deg += 10;
        }
        else if (c == 'a' && deg >= -100){
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
    Object revpesawat, revpuing1, revpuing2, revpuing3, explosion;

  public:
    Runner(int h = 700, int w = 1000) : Master(h, w){
        revpesawat = Object(0, 0, "Asset/object_plane.txt");
        revpesawat.reverseHorizontal();
        pesawat = Object(0, 0, "Asset/object_plane.txt");
        meriam = Object(0, 0, "Asset/object_gun.txt");
        peluru = Object(0, 0, "Asset/object_star.txt");
        puing1 = Object(0, 0, "Asset/object_plane_part1.txt");
        puing2 = Object(0, 0, "Asset/object_plane_part2.txt");
        puing3 = Object(0, 0, "Asset/object_plane_part3.txt");
        revpuing1 = Object(0, 0, "Asset/object_plane_part1.txt");
        revpuing1.reverseHorizontal();
        revpuing2 = Object(0, 0, "Asset/object_plane_part2.txt");
        revpuing2.reverseHorizontal();
        revpuing3 = Object(0, 0, "Asset/object_plane_part3.txt");
        revpuing3.reverseHorizontal();
        explosion = Object(0, 0, "Asset/object_ledakan.txt");
    }

    void start(){
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
        vector<MoveableObject> planes, rplanes;
        vector<MoveableObject> debris;
        vector<MoveableObject> bullets;
        vector<MoveableObject> explosions;
        MoveableObject cannon = meriam;
        planes.push_back(MoveableObject(-1, 0, 1, pesawat));

        for (int i = 1;; i = (i + 1) % 500){
            // draw
            clearWindow();

            for (MoveableObject &movableObject : planes){
                drawObject(movableObject);
                drawSolidObject(movableObject);
            }
            for (MoveableObject &movableObject : rplanes){
                drawObject(movableObject);
                drawSolidObject(movableObject);
            }
            for (MoveableObject &movableObject : debris){
                drawObject(movableObject);
                drawSolidObject(movableObject);
            }
            for (MoveableObject &movableObject : bullets){
                drawObject(movableObject);
                drawSolidObject(movableObject);
            }
            for (MoveableObject &explosion : explosions) {
                drawObject(explosion);
                drawSolidObject(explosion);
            }

            drawObject(cannon);
            drawSolidObject(cannon);

            // move and rotate :/
            if (deg != 0){
                if (deg > 0){
                    if (sudut_meriam <= 40){
                        sudut_meriam += 10;
                        cannon = MoveableObject(meriam);
                        cannon.selfRotation(titik_acuan_x, titik_acuan_y,
                                            sudut_meriam);
                        deg -= 10;
                    }
                    else{
                        deg = 0;
                    }
                }
                else{
                    if (sudut_meriam >= -40){
                        sudut_meriam -= 10;
                        cannon = MoveableObject(meriam);
                        cannon.selfRotation(titik_acuan_x, titik_acuan_y,
                                            sudut_meriam);
                        deg += 10;
                    }
                    else{
                        deg = 0;
                    }
                }
            }
            vector<char> checkr(rplanes.size(), 1);
            vector<char> checkp(planes.size(), 1);
            vector<char> checkd(debris.size(), 1);
            vector<char> checke(explosions.size(), 1);
            vector<MoveableObject> tmpr; // rplane
            vector<MoveableObject> tmpp; // plane
            vector<MoveableObject> tmpb; // bullet
            vector<MoveableObject> tmpd; // debris
            vector<MoveableObject> tmpe; // explosion
            for (int j = 0; j < planes.size(); ++j){
                planes[j].move();
                // if (planes[j].outOfWindow(yend, xend)){
                //     checkp[j] = 0;
                // }
            }
            for (int j = 0; j < rplanes.size(); ++j){
                rplanes[j].move();
                // if (rplanes[j].outOfWindow(yend, xend)){
                //     checkr[j] = 0;
                // }
            }
            for (int j = 0; j < debris.size(); ++j){
                debris[j].move();
                if (debris[j].outOfWindow(yend, xend)){
                    checkd[j] = 0;
                }
            }
            for(int j = 0; j < explosions.size(); ++j) {
                int middleX_explosion = explosions[j].getRefPos().getX() + explosions[j].getWidth()/2;
                int middleY_explosion = explosions[j].getRefPos().getY() + explosions[j].getHeight()/2;
                explosions[j].selfDilated(middleX_explosion, middleY_explosion, 1);
                if (explosions[j].outOfWindow(yend, xend)) {
                    checke[j] = 0;
                }
            }
            for (int j = 0; j < bullets.size(); ++j){
                bullets[j].move();
            }

            // very slow shit
            for (const MoveableObject &objb : bullets){
                bool bisa = true;
                for (int j = 0; j < planes.size(); ++j){
                    if (overlap(planes[j], objb)){
                        // // isi pecahan
                        // // if (checkp[j]){
                        //     MoveableObject sp = puing1;
                        //     sp.setPos(planes[j].getRefPos());
                        //     sp.setVector((planes[j].getDx() < 0 ? -1 : 1) *
                        //                      sin(60 * PI / 180),
                        //                  cos(60 * PI / 180));
                        //     tmpd.push_back(sp);
                        //     sp = puing2;
                        //     sp.setPos(Pixel(planes[j].getRefPos().getX() + 100,
                        //                     planes[j].getRefPos().getY() + 50));
                        //     sp.setVector((planes[j].getDx() < 0 ? -1 : 1) *
                        //                      sin(45 * PI / 180),
                        //                  cos(45 * PI / 180));
                        //     tmpd.push_back(sp);
                        //     sp = puing3;
                        //     sp.setPos(Pixel(planes[j].getRefPos().getX() + 300,
                        //                     planes[j].getRefPos().getY()));
                        //     sp.setVector((planes[j].getDx() < 0 ? -1 : 1) *
                        //                      sin(30 * PI / 180),
                        //                  cos(30 * PI / 180));
                        //     tmpd.push_back(sp);
                        //     checkp[j] = 0;
                        // // }
                        // bisa = false;
                        MoveableObject epl = explosion;
                        epl.setPos(planes[j].getRefPos().getX() + planes[j].getWidth()/2,
                            planes[j].getRefPos().getY() + planes[j].getHeight()/2);
                        tmpe.push_back(epl);
                        checkp[j] = 0;
                        bisa = false;
                    }
                }
                for (int j = 0; j < rplanes.size(); ++j){
                    if (overlap(rplanes[j], objb)){
                        // isi pecahan
                        // if (checkp[j]){
                            MoveableObject sp = revpuing1;
                            sp.setPos(Pixel(rplanes[j].getRefPos().getX() + 150,
                                            rplanes[j].getRefPos().getY()));
                            sp.setVector((rplanes[j].getDx() < 0 ? -1 : 1) *
                                             sin(60 * PI / 180),
                                         cos(60 * PI / 180));
                            tmpd.push_back(sp);
                            sp = revpuing2;
                            sp.setPos(Pixel(rplanes[j].getRefPos().getX() + 100,
                                            rplanes[j].getRefPos().getY() + 50));
                            sp.setVector((rplanes[j].getDx() < 0 ? -1 : 1) *
                                             sin(45 * PI / 180),
                                         cos(45 * PI / 180));
                            tmpd.push_back(sp);
                            sp = revpuing3;
                            sp.setPos(Pixel(rplanes[j].getRefPos().getX(),
                                            rplanes[j].getRefPos().getY()));
                            sp.setVector((rplanes[j].getDx() < 0 ? -1 : 1) *
                                             sin(30 * PI / 180),
                                         cos(30 * PI / 180));
                            tmpd.push_back(sp);
                            checkr[j] = 0;
                        // }
                        bisa = false;
                    }
                }
                for (int j = 0; j < debris.size(); ++j){
                    debris[j].move();
                    if (overlap(debris[j], objb)){
                        checkd[j] = 0;
                        bisa = false;
                    }
                }
                if (bisa && !objb.outOfWindow(yend, xend)){
                    tmpb.push_back(objb);
                }
            }
            for (int j = 0; j < planes.size(); ++j){
                if (checkp[j]){
                    Pixel p = planes[j].getPos();
                    if((int)p.getX() <= 0 && (int)p.getY() == 0){
                        tmpr.push_back(MoveableObject(1, 0, 1, revpesawat));
                    }
                    else{
                        tmpp.push_back(planes[j]);
                    }
                }
            }
            for (int j = 0; j < rplanes.size(); ++j){
                if (checkr[j]){
                    Pixel p = rplanes[j].getPos();
                    if((int)p.getX() >= xend - rplanes[j].getWidth() && (int)p.getY() == 0){
                        MoveableObject tmp = MoveableObject(-11, 0, 1, pesawat);
                        tmp.setPos(Pixel(xend - rplanes[j].getWidth(), 0));
                        tmpp.push_back(tmp);
                    }
                    else{
                        tmpr.push_back(rplanes[j]);
                    }
                }
            }
            for (int j = 0; j < debris.size(); ++j){
                if (checkd[j])
                    tmpd.push_back(debris[j]);
            }
            for (int j = 0; j < explosions.size(); ++j) {
                if (checke[j]) 
                    tmpe.push_back(explosions[j]);
            }
            rplanes = tmpr;
            planes = tmpp;
            bullets = tmpb;
            debris = tmpd;
            explosions = tmpe;

            if (shoot > 0){
                MoveableObject tmp = MoveableObject(peluru);
                tmp.setSpeed(2);
                tmp.selfRotation(titik_acuan_x, titik_acuan_y, sudut_meriam);
                tmp.setVector(sin(sudut_meriam * PI / 180),
                              -cos(sudut_meriam * PI / 180));
                bullets.push_back(tmp);
                shoot --;
            }
            if (i == 0){
                planes.push_back(MoveableObject(-1, 0, 1, pesawat));
            }

            if(planes.size() + rplanes.size() >= 8){ // stop game
                break;
            }

            usleep(5000);
        }
        puts("GAME OVER!! YOU LOSE");
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

int main(){
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
    keepreading = false;

    /* close */
    pthread_exit(NULL);
    res = tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
    assert(res == 0);
    return 0;
}