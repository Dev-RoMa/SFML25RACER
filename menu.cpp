#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib> // For system()
#include <thread>   // For std::thread

using namespace sf;

void launchGame() {
    // Launch the sfml-app.exe in a separate thread
    system("sfml-app.exe");
}

int main()
{
    // Window setup
    RenderWindow window(VideoMode(1024, 768), "Main Menu");

    // Load images
    Texture startTexture;
    if (!startTexture.loadFromFile("ui/menu/start_white.png")) {
        std::cerr << "Failed to load start_white.png" << std::endl;
        return -1;
    }

    Texture exitTexture;
    if (!exitTexture.loadFromFile("ui/menu/exit_white.png")) {
        std::cerr << "Failed to load exit_white.png" << std::endl;
        return -1;
    }

    // Create sprites for menu options
    Sprite startSprite(startTexture);
    Sprite exitSprite(exitTexture);

    // Scale and position sprites (adjust as needed)
    startSprite.setScale(0.5f, 0.5f);
    startSprite.setPosition(50, 50);

    exitSprite.setScale(0.5f, 0.5f);
    exitSprite.setPosition(100, 300);

    // Menu selection state
    int op_menu = 1; // Start at "Start" option

    // Main loop
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            // Close the window when the close event is triggered
            if (event.type == Event::Closed)
                window.close();

            // Handle key press events for navigation
            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Down) {
                    op_menu = 2; // Select "Exit" when Down is pressed
                } else if (event.key.code == Keyboard::Up) {
                    op_menu = 1; // Select "Start" when Up is pressed
                } else if (event.key.code == Keyboard::Enter || event.key.code == Keyboard::Space) {
                    if (op_menu == 1) {
                        std::cout << "Start selected!" << std::endl;
                        // Launch sfml-app.exe in a separate thread
                        std::thread gameThread(launchGame);
                        gameThread.detach(); // Detach the thread to allow it to run independently
                        window.close(); // Close the window after launching the game
                    } else if (op_menu == 2) {
                        std::cout << "Exit selected!" << std::endl;
                        window.close(); // Close window if "Exit" is selected
                    }
                }
            }
        }

        // Visual indication of selected option
        if (op_menu == 1) {
            startSprite.setColor(Color::Yellow); // Highlight Start
            exitSprite.setColor(Color::White);
        } else if (op_menu == 2) {
            startSprite.setColor(Color::White);
            exitSprite.setColor(Color::Yellow); // Highlight Exit
        }

        // Clear and draw menu
        window.clear();
        window.draw(startSprite);
        window.draw(exitSprite);
        window.display();
    }

    return 0;
}
