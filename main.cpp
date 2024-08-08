#include <SFML/Graphics.hpp>

using namespace sf;

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
    float scale;

    Line() {x=y=z=0;}

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
    RenderWindow app(VideoMode(width, height), "Racing!");
    app.setFramerateLimit(60);

    std::vector<Line> lines;

    for (int i=0;i<1600;i++)
    {
        Line line;
        line.z =i*segL;

        lines.push_back(line);
    }

    int N = lines.size();

    while (app.isOpen()){
        Event e;
        while(app.pollEvent(e))
        {
            if(e.type == Event::Closed)
                app.close();
        }

        app.clear();
        /////////////////////////////////////////////////////////
        for(int n=0;n<300; n++){
            Line &l = lines[n%N];
            l.project(0, 1500, 0);

            Color grass = (n/3)%2?Color(16,200,16):Color(0,154,0);
            Color rumble = (n/3)%2?Color(255,255,255):Color(0,0,0);
            Color road = (n/3)%2?Color(107,107,107):Color(105,105,105);

            Line p = lines[(n-1)%N]; //prev line

            drawQuad(app, grass, 0, p.Y, width, 0, l.Y, width);
            drawQuad(app, rumble,p.X, p.Y, p.W*1.2, l.X, l.Y, l.W*1.2);
            drawQuad(app, road, p.X, p.Y, p.W, l.X, l.Y, l.W);

        }
        //drawQuad(app, Color::Green, 500, 500, 200, 500, 300, 100);
        app.display();
    }
    return 0;
}
