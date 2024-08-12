#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std;

//screen size
int width = 1024;
int height = 768;
//road stuff
int roadW= 2000;
int segL = 200; //segment length
float camD = 0.84; //camera depth

struct Line
{
    float x,y,z;
    float X,Y,W;
    float scale,curve;

    Line() {curve=x=y=z=0;}

    //world to screen coords
    void project(int camX, int camY, int camZ){
        scale = camD/(z-camZ);
        X = (1 + scale*(x - camX)) * width/2;
        Y = (1 - scale*(y - camY)) * height/2;
        W = scale * roadW * width/2;
    }
};

// Function to draw a quad
void drawQuad(RenderWindow &w, Color c, int x1, int y1, int w1, int x2, int y2, int w2){
    ConvexShape shape(4);
    shape.setFillColor(c);
    shape.setPoint(0, Vector2f(x1 - w1, y1));
    shape.setPoint(1, Vector2f(x1 + w1, y1));
    shape.setPoint(2, Vector2f(x2 + w2, y2));
    shape.setPoint(3, Vector2f(x2 - w2, y2));
    w.draw(shape);
}

int main()
{
    //Set render w size and w title + limit framerate at 60 fps
    RenderWindow app(VideoMode(width, height), "Racing!");
    app.setFramerateLimit(60);

    //Background
    Texture bg;
    bg.loadFromFile("bg.png");
    bg.setRepeated(true);
    Sprite sBackground(bg);

    std::vector<Line> lines;

    //this is giving the road shape
    for (int i=0;i<1600;i++)
    {
        Line line;
        line.z =i*segL;
        //curves
        if (i>300 && i<700) line.curve=0.5;
        lines.push_back(line);
    }

    int N = lines.size();
    int pos = 0;
    int playerX = 0;

    while (app.isOpen()){
        Event e;
        while(app.pollEvent(e))
        {
            if(e.type == Event::Closed)
                app.close();
        }

        //Forward Backwards
        if (Keyboard::isKeyPressed(Keyboard::Up)) pos +=200;
        if (Keyboard::isKeyPressed(Keyboard::Down)) pos -=200;
        //left right player movment
        if (Keyboard::isKeyPressed(Keyboard::Right)) playerX+=200;
        if (Keyboard::isKeyPressed(Keyboard::Left)) playerX-=200;


        app.clear();
        int startPos = pos/segL;
        float x=0, dx=0;
        //////////////////////DRAW ROAD///////////////////////////////////
        for(int n=startPos; n<startPos+300; n++){
            Line &l = lines[n%N];
            l.project(playerX - x, 1500, pos);
            x+=dx;
            dx+=l.curve;

            Color grass = (n/3)%2?Color(16,200,16):Color(0,154,0);
            Color rumble = (n/3)%2?Color(255,255,255):Color(0,0,0);
            Color road = (n/3)%2?Color(107,107,107):Color(105,105,105);

            Line p = lines[(n-1)%N]; //prev line

            drawQuad(app, grass, 0, p.Y, width, 0, l.Y, width);
            drawQuad(app, rumble,p.X, p.Y, p.W*1.2, l.X, l.Y, l.W*1.2);
            drawQuad(app, road, p.X, p.Y, p.W, l.X, l.Y, l.W);
            //THIS IS EATING TOO MUCH MEMORY WTF
            //std::cout << "Line " << n % N << ": X=" << l.X << ", Y=" << l.Y << ", W=" << l.W << std::endl;

        }
        //drawQuad(app, Color::Green, 500, 500, 200, 500, 300, 100);
        app.display();
    }
    return 0;
}
