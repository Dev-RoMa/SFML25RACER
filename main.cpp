#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cstdlib>

using namespace sf;
using namespace std;

// Window and camera parameters
int width = 1024;
int height = 768;
int roadW = 2000;
int segL = 200;
float camD = 0.9;

struct Line {
    float x, y, z, X, Y, W, scale, curve;
    Line() { x = y = z = X = Y = W = scale = curve = 0; }

    void project(int camX, int camY, int camZ) {
        scale = camD / (z - camZ);
        X = (1 + scale * (x - camX)) * width / 2;
        Y = (1 - scale * (y - camY)) * height / 2;
        W = scale * roadW * width / 2;
    }
};

// Function to draw quads on the road
void drawQuad(RenderWindow &w, Color c, int x1, int y1, int w1, int x2, int y2, int w2) {
    ConvexShape shape(4);
    shape.setFillColor(c);
    shape.setPoint(0, Vector2f(x1 - w1, y1));
    shape.setPoint(1, Vector2f(x2 - w2, y2));
    shape.setPoint(2, Vector2f(x2 + w2, y2));
    shape.setPoint(3, Vector2f(x1 + w1, y1));
    w.draw(shape);
}

// Texture manager to load textures once and reuse them
class TextureManager {
public:
    bool loadTexture(const string &name, const string &filename) {
        Texture texture;
        if (!texture.loadFromFile(filename)) {
            cerr << "Failed to load " << filename << endl;
            return false;
        }
        textures[name] = texture;
        return true;
    }

    Texture &getTexture(const string &name) {
        return textures[name];
    }

private:
    map<string, Texture> textures;
};

int main() {
    RenderWindow app(VideoMode(width, height), "Racing!");
    app.setFramerateLimit(60);

    TextureManager textureManager;

    // Load textures only once
    if (!textureManager.loadTexture("background", "bg.png") ||
        !textureManager.loadTexture("playerS", "cars/00_it/IT_S.png") ||
        !textureManager.loadTexture("playerT", "cars/00_it/IT_T.png")) {
        return -1; // Exit if any texture fails to load
    }

    // Create background and player sprites
    Sprite bgSprite(textureManager.getTexture("background"));
    Sprite playerStraightSprite(textureManager.getTexture("playerS"));
    Sprite playerTurningSprite(textureManager.getTexture("playerT"));
    playerStraightSprite.setScale(0.5f, 0.5f);
    playerTurningSprite.setScale(0.5f, 0.5f);

    // Center the player sprites
    FloatRect playerBounds = playerStraightSprite.getLocalBounds();
    playerStraightSprite.setOrigin(playerBounds.width / 2, playerBounds.height / 2);
    playerTurningSprite.setOrigin(playerBounds.width / 2, playerBounds.height / 2);

    // Initialize lines for the road
    vector<Line> lines;
    for (int i = 0; i < 2000; i++) {
        Line line;
        line.z = i * segL;
        line.curve = (i > 300 && i < 699) ? 0.5f : 0.0f;
        lines.push_back(line);
    }

    int N = lines.size();
    int pos = 0, playerX = 0;

    while (app.isOpen()) {
        Event e;
        while (app.pollEvent(e)) {
            if (e.type == Event::Closed) app.close();

            // Check for Escape key to close the window
            if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape) {
                system("menu.exe");
                app.close(); // Close the window
                continue; // Skip the rest of the loop
            }
        }

        // Handle player input
        bool turningLeft = false;
        bool turningRight = false;
        if (Keyboard::isKeyPressed(Keyboard::Up)) pos += 200;
        if (Keyboard::isKeyPressed(Keyboard::Down)) pos -= 200;
        if (Keyboard::isKeyPressed(Keyboard::Right)) {
            playerX += 200;
            turningRight = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::Left)) {
            playerX -= 200;
            turningLeft = true;
        }

        pos = (pos < 0) ? 0 : ((pos >= 1600 * segL) ? 0 : pos);

        app.clear();
        bgSprite.setPosition(0, 0);
        app.draw(bgSprite);

        int startPos = pos / segL;
        int camH = 1500 + lines[startPos].y;
        float x = 0, dx = 0;
        int maxy = height;

        // Draw road
        for (int n = startPos; n < startPos + 300; n++) {
            Line &l = lines[n % N];
            l.project(playerX - x, camH, pos);
            x += dx;
            dx += l.curve;

            if (l.Y >= maxy) continue;
            maxy = l.Y;

            Color grass = (n / 3) % 2 ? Color(16, 200, 16) : Color(0, 154, 0);
            Color rumble = (n / 3) % 2 ? Color(255, 255, 255) : Color(0, 0, 0);
            Color road = (n / 3) % 2 ? Color(107, 107, 107) : Color(105, 105, 105);

            Line p = lines[(n - 1) % N];

            drawQuad(app, grass, 0, p.Y, width, 0, l.Y, width);
            drawQuad(app, rumble, p.X, p.Y, p.W * 1.2, l.X, l.Y, l.W * 1.2);
            drawQuad(app, road, p.X, p.Y, p.W, l.X, l.Y, l.W);
        }

        // Draw the player sprite based on turning state
        if (turningLeft) {
            playerTurningSprite.setScale(0.5f, 0.5f); // Normal orientation for left turn
            playerTurningSprite.setPosition(width / 2, height / 1.2);
            app.draw(playerTurningSprite);
        } else if (turningRight) {
            playerTurningSprite.setScale(-0.5f, 0.5f); // Flip horizontally for right turn
            playerTurningSprite.setPosition(width / 2, height / 1.2);
            app.draw(playerTurningSprite);
        } else {
            playerStraightSprite.setPosition(width / 2, height / 1.2);
            app.draw(playerStraightSprite);
        }

        app.display();
    }

    return 0;
}
