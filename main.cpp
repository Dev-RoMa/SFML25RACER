#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <math.h>

using namespace sf;

// Window size
int width = 1024;
int height = 768;
int roadW = 2000;
int segL = 200;
float camD = 0.9;

struct Line
{
    float x, y, z;
    float X, Y, W;
    float scale, curve;

    Line() { x = y = z = 0; }

    // Method to project the line to 2D space
    void project(int camX, int camY, int camZ)
    {
        scale = camD / (z - camZ);
        X = (1 + scale * (x - camX)) * width / 2;
        Y = (1 - scale * (y - camY)) * height / 2;
        W = scale * roadW * width / 2;
    }
};

// Function to draw the shape
void drawQuad(RenderWindow &w, Color c, int x1, int y1, int w1, int x2, int y2, int w2)
{
    ConvexShape shape(4);
    shape.setFillColor(c);
    shape.setPoint(0, Vector2f(x1 - w1, y1));
    shape.setPoint(1, Vector2f(x2 - w2, y2));
    shape.setPoint(2, Vector2f(x2 + w2, y2));
    shape.setPoint(3, Vector2f(x1 + w1, y1));
    w.draw(shape);
}

int main()
{
    RenderWindow app(VideoMode(width, height), "Racing!");
    app.setFramerateLimit(60);

    // Load the background image
    Image bgImage;
    if (!bgImage.loadFromFile("bg.png")) {
        std::cerr << "Failed to load background image" << std::endl;
        return -1; // Return error code
    }
    
    // Convert the background image to a texture
    Texture bgTexture;
    if (!bgTexture.loadFromImage(bgImage)) {
        std::cerr << "Failed to load background texture" << std::endl;
        return -1; // Return error code
    }
    
    // Create a sprite from the background texture
    Sprite bgSprite(bgTexture);

    // Load the player image
    Image playerImage;
    if (!playerImage.loadFromFile("player.png")) {
        std::cerr << "Failed to load player image" << std::endl;
        return -1; // Return error code
    }
    
    // Convert the player image to a texture
    Texture playerTexture;
    if (!playerTexture.loadFromImage(playerImage)) {
        std::cerr << "Failed to load player texture" << std::endl;
        return -1; // Return error code
    }
    
    // Create a sprite from the player texture
    Sprite playerSprite(playerTexture);

    // Resize the player sprite to make it smaller
    playerSprite.setScale(0.5f, 0.5f); // Adjust scale as needed

    // Initialize lines vector
    std::vector<Line> lines;

    // Generate lines for the road
    for (int i = 0; i < 2000; i++)
    {
        Line line;
        line.z = i * segL;

        if (i > 300 && i < 699)
            line.curve = 0.5;
        if (i > 700)
            line.curve = 0;
        //JUST IF U WANT A HILL DECOMMENT THESE
        //if (i > 750 && i < 800)
        //    line.y = sin(i / 30.0) * 1500;

        lines.push_back(line);
    }

    int N = lines.size();
    int pos = 0;
    int playerX = 0;

    while (app.isOpen())
    {
        Event e;
        while (app.pollEvent(e))
        {
            if (e.type == Event::Closed)
                app.close();
        }

        // Handle player input
        if (Keyboard::isKeyPressed(Keyboard::Up))
            pos += 200;
        if (Keyboard::isKeyPressed(Keyboard::Down))
            pos -= 200;
        if (Keyboard::isKeyPressed(Keyboard::Right))
            playerX += 200;
        if (Keyboard::isKeyPressed(Keyboard::Left))
            playerX -= 200;

        // Ensure player position stays within bounds
        if (pos < 0)
            pos = 0;
        else if (pos >= 1600 * segL)
            pos = 0; // Reset to starting position

        app.clear();

        // Draw the sprite as the background
        bgSprite.setPosition(0, 0);
        app.draw(bgSprite);

        int startPos = pos / segL;
        int camH = 1500 + 1 + lines[startPos].y;
        float x = 0, dx = 0;
        int maxy = height;

        // Draw road
        for (int n = startPos; n < startPos + 300; n++)
        {
            Line &l = lines[n % N];
            l.project(playerX - x, camH, pos);
            x += dx;
            dx += l.curve;

            if (l.Y >= maxy)
                continue;
            maxy = l.Y;

            Color grass = (n / 3) % 2 ? Color(16, 200, 16) : Color(0, 154, 0);
            Color rumble = (n / 3) % 2 ? Color(255, 255, 255) : Color(0, 0, 0);
            Color road = (n / 3) % 2 ? Color(107, 107, 107) : Color(105, 105, 105);

            Line p = lines[(n - 1) % N];

            drawQuad(app, grass, 0, p.Y, width, 0, l.Y, width);
            drawQuad(app, rumble, p.X, p.Y, p.W * 1.2, l.X, l.Y, l.W * 1.2);
            drawQuad(app, road, p.X, p.Y, p.W, l.X, l.Y, l.W);
        }

        // Center the player sprite
        FloatRect playerBounds = playerSprite.getLocalBounds();
        playerSprite.setOrigin(playerBounds.width / 2, playerBounds.height / 2);
        playerSprite.setPosition(width / 2, height / 1.2); // Center the sprite

        // Draw the player sprite
        app.draw(playerSprite);

        app.display();
    }

    return 0;
}
