#pragma once
#include <SFML/Graphics.hpp>
#include <random>
#include <cassert>
#include <vector>

enum class Bonus {
    NONE
    , BOMB
    , BRUSH
};

class GameBoard {
public:
    GameBoard();
    void drawInter(sf::RenderWindow& window);
    void touchBoard(sf::RenderWindow& window);
private:
    static const int BOARD_WIDTH = 8; // table size
    static const int BOARD_HEIGHT = 8;
    const int cellSize = 100; // cell size
    sf::RectangleShape board[BOARD_WIDTH][BOARD_HEIGHT];
    sf::Color colors[5] = { sf::Color::Red, sf::Color::Green, sf::Color::Blue,  sf::Color::Yellow, sf::Color::Magenta };
    int boardData[BOARD_WIDTH][BOARD_HEIGHT]; // colors of cells
    Bonus bonusData[BOARD_WIDTH][BOARD_HEIGHT]; // bonus information
    int brushData[BOARD_WIDTH][BOARD_HEIGHT]; // colors of brushes
    int score; // Points storage box
    sf::Font font; // Font to display text
    sf::Text scoreText; // Text to display points
    int selectedX; // Selected Cell Coordinates
    int selectedY;
    int width; // size of window
    int height;

    void drawCells(sf::RenderWindow& window);
    void swapCells(int x1, int y1, int x2, int y2);
    void drawGrid(sf::RenderWindow& window);
    bool checkCombo();
    void refillBoard();
    void gameCore(sf::RenderWindow& window);
    void prepareBoard();
    void horBonus(std::vector<int>& comb, int y, sf::RenderWindow& window);
    void vertBonus(std::vector<int>& comb, int x, sf::RenderWindow& window);
    void bonusDrop();
    void shiftCells();
};