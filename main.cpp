#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>

using namespace sf;

//window size
int width = 1024;
int height = 768;
int roadW = 2000;
int segL = 200;
float camD = 0.9;

struct Line
{
    float x,y,z;
    float X,Y,W;
    //curve is for the curve; adding it to struct line
    float scale, curve;

    Line() {x,y,z=0;}

    //from world to screen
    void project(int camX, int camY,int camZ)
    {
        scale = camD/(z-camZ);
        X = (1 + scale*(x - camX)) * width/2;
        Y = (1 - scale*(y - camY)) * height/2;
        W = scale * roadW * width/2;
    }
};

//this draws the shape
void drawQuad(RenderWindow &w, Color c, int x1, int y1, int w1, int x2, int y2, int w2)
{   
    //giving the shape its form
    ConvexShape shape(4);
    shape.setFillColor(c);
    shape.setPoint(0, Vector2f(x1-w1,y1));
    shape.setPoint(1, Vector2f(x2-w2,y2));
    shape.setPoint(2, Vector2f(x2+w2,y2));
    shape.setPoint(3, Vector2f(x1+w1,y1));
    w.draw(shape);
}

int main()
{
    //render the window
    RenderWindow app(VideoMode(width,height), "Racing!");
    app.setFramerateLimit(60);

    //set the background

    Texture bg;
    bg.loadFromFile("bg.png");
    bg.setRepeated(true);
    Sprite sBackground(bg);

    //I guess add lines?

    std::vector<Line> lines;

    // the for cycle should be the total length of the road
    //x
    for (int i = 0; i<1600;i++)
    {
        Line line;
        line.z =i*segL;
        //tell at what pos the curve will b Added
        if (i>300 && i<700) line.curve=0.5; // also curve is the angle of the curve; FLOAT
        //tell at what pos the hill will b Added
        if (i>750) line.y = sin(i/30.0)*1500;

        lines.push_back(line);
    }

    int N = lines.size();
    int pos = 0;
    int playerX = 0;


    while(app.isOpen())
    {
        Event e;
        while(app.pollEvent(e))
        {
            if(e.type == Event::Closed)
            app.close();
        }

    if (Keyboard::isKeyPressed(Keyboard::Up)) pos +=200;
    if (Keyboard::isKeyPressed(Keyboard::Down)) pos -=200;
    if (Keyboard::isKeyPressed(Keyboard::Right)) playerX +=200;
    if (Keyboard::isKeyPressed(Keyboard::Left)) playerX -=200;

    app.clear();
    int startPos = pos/segL;
    //this for the hills?
    int camH = 1500 + 1 + lines[startPos].y;
    //this for the curves?
    float x=0, dx=0;
    //this for the hill height?
    int maxy = height;

    //draw road//
    for(int n=startPos; n<startPos+300;n++)
    {
        Line &l = lines[n%N];
        l.project(playerX - x, camH,pos);
        //adding this for the curves ?
        x+=dx;
        dx+=l.curve;

        //what was (l.Y) ?

        if (l.Y>=maxy) continue;
        maxy =  l.Y;

        Color grass = (n/3)%2?Color(16,200,16):Color(0,154,0);
        Color rumble = (n/3)%2?Color(255,255,255):Color(0,0,0);
        Color road = (n/3)%2?Color(107,107,107):Color(105,105,105);

        Line p = lines[(n-1)%N];

        drawQuad(app, grass, 0, p.Y, width,0, l.Y, width);
        drawQuad(app, rumble, p.X, p.Y, p.W*1.2,l.X, l.Y, l.W*1.2);
        drawQuad(app,road, p.X,p.Y,p.W,l.X,l.Y,l.W);
    }
    app.display();
    }
    return 0;
}