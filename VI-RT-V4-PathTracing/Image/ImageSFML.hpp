//
//  ImageSFML.hpp
//  VI-RT-V4-PathTracing
//
//  Created by Luis Paulo Santos on 09/03/2025.
//

#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <thread>
#include <chrono>

class ImageWindow {
private:
    int width, height;
    sf::RenderWindow window;
    sf::Texture texture;
    sf::Sprite sprite;
    std::vector<sf::Uint8> pixels;

public:
    // Constructor
    ImageWindow(int w, int h) : width(w), height(h), window(sf::VideoMode(w, h), "Live Image Display") {
        texture.create(width, height);
        sprite.setTexture(texture);
        pixels.resize(width * height * 4, 255); // Initialize white image (RGBA)
    }

    // Function to update image with a 2D matrix (RGB values in range 0-1)
    void updateImage(const std::vector<std::vector<std::array<float, 3>>>& imageMatrix) {
        if (imageMatrix.size() != height || imageMatrix[0].size() != width) {
            throw std::runtime_error("Image matrix size does not match window dimensions.");
        }

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int index = (y * width + x) * 4;
                pixels[index] = static_cast<sf::Uint8>(imageMatrix[y][x][0] * 255); // Red
                pixels[index + 1] = static_cast<sf::Uint8>(imageMatrix[y][x][1] * 255); // Green
                pixels[index + 2] = static_cast<sf::Uint8>(imageMatrix[y][x][2] * 255); // Blue
                pixels[index + 3] = 255; // Alpha (fully opaque)
            }
        }
        texture.update(pixels.data());
    }

    // Function to handle events and redraw the image
    bool updateAndDraw() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false;
            }
        }

        window.clear();
        window.draw(sprite);
        window.display();
        return true;
    }

    // Check if the window is still open
    bool isOpen() {
        return window.isOpen();
    }

    // Function to close the window
    void close() {
        window.close();
    }
};

// Example usage:
int main() {
    int width = 400, height = 300;
    ImageWindow imgWindow(width, height);

    std::vector<std::vector<std::array<float, 3>>> imageMatrix(height, std::vector<std::array<float, 3>>(width));

    int frame = 0;
    while (imgWindow.isOpen()) {
        // Create a simple changing pattern
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                imageMatrix[y][x] = {
                    (float(x) / width),
                    (float(y) / height),
                    0.5f * (1 + sin(frame * 0.1f))  // Blue oscillates over time
                };
            }
        }

        imgWindow.updateImage(imageMatrix);
        if (!imgWindow.updateAndDraw()) break; // Render and handle events

        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // Limit FPS ~60
        frame++; // Increment frame count for animation
    }

    return 0;
}
