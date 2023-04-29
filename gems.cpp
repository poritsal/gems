#include "gems.h"
#include <random>
#include <cassert>
#include <vector>

GameBoard::GameBoard()
    : score(0)
    , width(800)
    , height(900)
    , selectedX(-1)
    , selectedY(-1)
{
    // initialization
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 4);

    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            board[x][y].setSize(sf::Vector2f(cellSize - 1, cellSize - 1));
            board[x][y].setPosition(2 + x * cellSize, 2 + y * cellSize);
            boardData[x][y] = dist(gen);
            bonusData[x][y] = Bonus::NONE;
            brushData[x][y] = -1;
        }
    }
    prepareBoard();

    // Font download
    if (!font.loadFromFile("arial.ttf")) {
        assert(0);
    }

    // Initializing Text to Display Points
    scoreText.setFont(font);
    scoreText.setCharacterSize(90);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setString("Score: " + std::to_string(score));
    scoreText.setPosition(0, 800);
}


void GameBoard::drawCells(sf::RenderWindow& window) {
    // Drawing the playing field and chips on the screen
    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            board[x][y].setFillColor(colors[boardData[x][y]]);
            window.draw(board[x][y]);

            // Draw a bonus in the center of the cell
            switch (bonusData[x][y])
            {
            case Bonus::NONE:
                break;
            case Bonus::BOMB:
            {
                sf::CircleShape circle(cellSize / 4);
                circle.setFillColor(sf::Color::Black);
                circle.setPosition(2 + x * cellSize + cellSize / 2 - cellSize / 4, 2 + y * cellSize + cellSize / 2 - cellSize / 4);
                window.draw(circle);
                break;
            }
            case Bonus::BRUSH:
            {
                sf::RectangleShape brush;
                brush.setSize(sf::Vector2f(cellSize / 2, cellSize / 2));
                brush.setFillColor(colors[brushData[x][y]]);
                brush.setPosition(2 + x * cellSize + cellSize / 2 - cellSize / 4, 2 + y * cellSize + cellSize / 2 - cellSize / 4);
                brush.setOutlineThickness(-3);
                brush.setOutlineColor(sf::Color::Black);
                window.draw(brush);
                break;
            }
            default:
                assert(0);
            }
        }
    }
    window.draw(scoreText);
}

void GameBoard::drawGrid(sf::RenderWindow& window)
{
    // Create an array of vertices
    sf::VertexArray grid(sf::Lines);

    // Determine the number of lines horizontally and vertically
    int numLinesX = width / cellSize + 1;
    int numLinesY = height / cellSize + 1;

    // Filling the array of vertices
    for (int i = 0; i < numLinesX; i++) {
        int x = 1 + i * cellSize;
        grid.append(sf::Vertex(sf::Vector2f(x, 1), sf::Color::White));
        grid.append(sf::Vertex(sf::Vector2f(x, 801), sf::Color::White));
    }
    for (int i = 0; i < numLinesY; i++) {
        int y = 1 + i * cellSize;
        grid.append(sf::Vertex(sf::Vector2f(1, y), sf::Color::White));
        grid.append(sf::Vertex(sf::Vector2f(width, y), sf::Color::White));
    }

    // Draw the grid on the screen
    window.draw(grid);
}

void GameBoard::drawInter(sf::RenderWindow& window)
{
    window.clear(sf::Color::Black);
    drawGrid(window);
    drawCells(window);
    window.display();
}

void GameBoard::swapCells(int x1, int y1, int x2, int y2) {
    // Swap cell data
    int temp = boardData[x1][y1];
    boardData[x1][y1] = boardData[x2][y2];
    boardData[x2][y2] = temp;

    // Update board visuals
    board[x1][y1].setFillColor(colors[boardData[x1][y1]]);
    board[x2][y2].setFillColor(colors[boardData[x2][y2]]);
}

bool GameBoard::checkCombo()
{
    // Check for horizontal matches
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH - 2; x++) {
            if (boardData[x][y] == boardData[x + 1][y] && boardData[x][y] == boardData[x + 2][y]) {
                return true;
            }
        }
    }

    // Check for vertical matches
    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT - 2; y++) {
            if (boardData[x][y] == boardData[x][y + 1] && boardData[x][y] == boardData[x][y + 2]) {
                return true;
            }
        }
    }
    return false;
}

void GameBoard::touchBoard(sf::RenderWindow& window)
{
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    int x = (mousePos.x) / cellSize;
    int y = (mousePos.y) / cellSize;

    if (x < BOARD_WIDTH && y < BOARD_HEIGHT) {
        if (selectedX == -1 && selectedY == -1) {
            // No cell is currently selected, so select this one
            selectedX = x;
            selectedY = y;
            board[selectedX][selectedY].setOutlineThickness(-3);
            board[selectedX][selectedY].setOutlineColor(sf::Color::Black);
        }
        else if (selectedX == x && selectedY == y) {
            // This cell is already selected, so deselect it
            selectedX = -1;
            selectedY = -1;
            board[x][y].setOutlineThickness(0);
        }
        else if ((selectedX == x && abs(selectedY - y) == 1) || (selectedY == y && abs(selectedX - x) == 1)) {
            // Two cells are selected and adjacent, so swap them
            board[selectedX][selectedY].setOutlineThickness(0);
            swapCells(selectedX, selectedY, x, y);
            if (checkCombo() == false) {
                drawInter(window);
                sf::sleep(sf::milliseconds(500));
                swapCells(selectedX, selectedY, x, y);
            }
            else {
                gameCore(window);
            }
            selectedX = -1;
            selectedY = -1;
        }
        else {
            // Invalid selection, so deselect the current cell and select the new one
            board[selectedX][selectedY].setOutlineThickness(0);
            selectedX = x;
            selectedY = y;
            board[selectedX][selectedY].setOutlineThickness(-3);
            board[selectedX][selectedY].setOutlineColor(sf::Color::Black);
        }
    }
}

void GameBoard::horBonus(std::vector<int>& comb, int y, sf::RenderWindow& window)
{
    for (int i = 0; i < comb.size(); i++) {
        switch (bonusData[comb[i]][y])
        {
        case Bonus::NONE:
            break;
        case Bonus::BOMB:
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(0, 7);
            for (size_t i = 0; i < 4; i++) {
                boardData[dist(gen)][dist(gen)] = -1;
            }
            score += 50;
            bonusData[comb[i]][y] = Bonus::NONE;
            scoreText.setString("Score: " + std::to_string(score));
            drawInter(window);
            sf::sleep(sf::milliseconds(1000));
            break;
        }
        case Bonus::BRUSH:
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> loc(1, 2);
            if (loc(gen) == 1) {
                int x1 = comb[i] - 1;
                int y1 = y - 1;
                int x2 = comb[i] + 1;
                int y2 = y + 1;
                if (x1 < BOARD_WIDTH && x1 >= 0 && y1 < BOARD_HEIGHT && y1 >= 0) {
                    boardData[x1][y1] = brushData[comb[i]][y];
                }
                if (x2 < BOARD_WIDTH && x2 >= 0 && y2 < BOARD_HEIGHT && y2 >= 0) {
                    boardData[x2][y2] = brushData[comb[i]][y];
                }
            }
            else {
                int x1 = comb[i] - 1;
                int y1 = y + 1;
                int x2 = comb[i] + 1;
                int y2 = y - 1;
                if (x1 < BOARD_WIDTH && x1 >= 0 && y1 < BOARD_HEIGHT && y1 >= 0) {
                    boardData[x1][y1] = brushData[comb[i]][y];
                }
                if (x2 < BOARD_WIDTH && x2 >= 0 && y2 < BOARD_HEIGHT && y2 >= 0) {
                    boardData[x2][y2] = brushData[comb[i]][y];
                }
            }
            bonusData[comb[i]][y] = Bonus::NONE;
            brushData[comb[i]][y] = -1;
            scoreText.setString("Score: " + std::to_string(score));
            drawInter(window);
            sf::sleep(sf::milliseconds(1000));
            break;
        }
        default:
            assert(0);
        }
    }
}

void GameBoard::vertBonus(std::vector<int>& comb, int x, sf::RenderWindow& window)
{
    for (int i = 0; i < comb.size(); i++) {
        switch (bonusData[x][comb[i]])
        {
        case Bonus::NONE:
            break;
        case Bonus::BOMB:
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(0, 7);
            for (size_t i = 0; i < 4; i++) {
                boardData[dist(gen)][dist(gen)] = -1;
            }
            score += 50;
            bonusData[x][comb[i]] = Bonus::NONE;
            scoreText.setString("Score: " + std::to_string(score));
            drawInter(window);
            sf::sleep(sf::milliseconds(1000));
            break;
        }
        case Bonus::BRUSH:
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> loc(1, 2);
            if (loc(gen) == 1) {
                int x1 = x - 1;
                int y1 = comb[i] - 1;
                int x2 = x + 1;
                int y2 = comb[i] + 1;
                if (x1 < BOARD_WIDTH && x1 >= 0 && y1 < BOARD_HEIGHT && y1 >= 0) {
                    boardData[x1][y1] = brushData[x][comb[i]];
                }
                if (x2 < BOARD_WIDTH && x2 >= 0 && y2 < BOARD_HEIGHT && y2 >= 0) {
                    boardData[x2][y2] = brushData[x][comb[i]];
                }
            }
            else {
                int x1 = x - 1;
                int y1 = comb[i] - 1;
                int x2 = x + 1;
                int y2 = comb[i] + 1;
                if (x1 < BOARD_WIDTH && x1 >= 0 && y1 < BOARD_HEIGHT && y1 >= 0) {
                    boardData[x1][y1] = brushData[x][comb[i]];
                }
                if (x2 < BOARD_WIDTH && x2 >= 0 && y2 < BOARD_HEIGHT && y2 >= 0) {
                    boardData[x2][y2] = brushData[x][comb[i]];
                }
            }
            bonusData[x][comb[i]] = Bonus::NONE;
            brushData[x][comb[i]] = -1;
            scoreText.setString("Score: " + std::to_string(score));
            drawInter(window);
            sf::sleep(sf::milliseconds(1000));
            break;
        }
        default:
            assert(0);
        }
    }
}

void GameBoard::bonusDrop()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> prob(1, 100);
    std::uniform_int_distribution<> type(1, 2);
    std::uniform_int_distribution<> dist(0, 7);
    std::uniform_int_distribution<> clr(0, 4);
    unsigned p = prob(gen);
    unsigned t = type(gen);
    if (p > 0 && p < 11) {
        if (t == 1) {
            bonusData[dist(gen)][dist(gen)] = Bonus::BOMB;
        }
        else {
            int xc = dist(gen);
            int yc = dist(gen);
            bonusData[xc][yc] = Bonus::BRUSH;
            brushData[xc][yc] = clr(gen);
        }
    }
}

void GameBoard::shiftCells()
{
    for (int x = 0; x < BOARD_WIDTH; x++) {
        int shift = 0;
        for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
            if (boardData[x][y] == -1) {
                shift++;
                continue;
            }
            if (shift > 0) {
                boardData[x][y + shift] = boardData[x][y];
                boardData[x][y] = -1;
            }
        }
    }
}

void GameBoard::gameCore(sf::RenderWindow& window)
{
    // Check for horizontal matches
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH - 2; x++) {
            if (boardData[x][y] == boardData[x + 1][y] && boardData[x][y] == boardData[x + 2][y]) {
                std::vector<int> comb;
                comb.push_back(x);
                comb.push_back(x + 1);
                comb.push_back(x + 2);
                score += 30;
                int color = boardData[x][y];

                for (int i = x - 1; i >= 0; i--) {
                    if (boardData[i][y] != color) break;
                    comb.push_back(i);
                    score += 10;
                }

                for (int i = x + 3; i < BOARD_WIDTH; i++) {
                    if (boardData[i][y] != color) break;
                    comb.push_back(i);
                    score += 10;
                }

                horBonus(comb, y, window);

                for (size_t i = 0; i < comb.size(); i++) {
                    boardData[comb[i]][y] = -1;
                }

                scoreText.setString("Score: " + std::to_string(score));
                drawInter(window);
                sf::sleep(sf::milliseconds(500));

                bonusDrop();

                shiftCells();

                refillBoard();
                gameCore(window);
            }
        }
    }

    // Check for vertical matches
    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT - 2; y++) {
            if (boardData[x][y] == boardData[x][y + 1] && boardData[x][y] == boardData[x][y + 2]) {
                std::vector<int> comb;
                comb.push_back(y);
                comb.push_back(y + 1);
                comb.push_back(y + 2);
                int color = boardData[x][y];
                score += 30;

                for (int i = y - 1; i >= 0; i--) {
                    if (boardData[x][i] != color) break;
                    comb.push_back(i);
                    score += 10;
                }

                for (int i = y + 3; i < BOARD_WIDTH; i++) {
                    if (boardData[x][i] != color) break;
                    comb.push_back(i);
                    score += 10;
                }

                vertBonus(comb, x, window);

                for (size_t i = 0; i < comb.size(); i++) {
                    boardData[x][comb[i]] = -1;
                }

                scoreText.setString("Score: " + std::to_string(score));
                drawInter(window);
                sf::sleep(sf::milliseconds(500));

                bonusDrop();

                shiftCells();

                refillBoard();
                gameCore(window);
            }
        }
    }
}

void GameBoard::prepareBoard()
{
    // Check for horizontal matches
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH - 2; x++) {
            if (boardData[x][y] == boardData[x + 1][y] && boardData[x][y] == boardData[x + 2][y]) {

                int color = boardData[x][y];
                boardData[x][y] = -1;
                boardData[x + 1][y] = -1;
                boardData[x + 2][y] = -1;

                for (int i = x - 1; i >= 0; i--) {
                    if (boardData[i][y] != color) break;
                    boardData[i][y] = -1;
                }

                for (int i = x + 3; i < BOARD_WIDTH; i++) {
                    if (boardData[i][y] != color) break;
                    boardData[i][y] = -1;
                }

                // Remove matched cells and shift the remaining ones down
                for (int x = 0; x < BOARD_WIDTH; x++) {
                    int shift = 0;
                    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
                        if (boardData[x][y] == -1) {
                            shift++;
                            continue;
                        }
                        if (shift > 0) {
                            boardData[x][y + shift] = boardData[x][y];
                            boardData[x][y] = -1;
                        }
                    }
                }

                refillBoard();
                prepareBoard();
            }
        }
    }

    // Check for vertical matches
    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT - 2; y++) {
            if (boardData[x][y] == boardData[x][y + 1] && boardData[x][y] == boardData[x][y + 2]) {

                int color = boardData[x][y];
                boardData[x][y] = -1;
                boardData[x][y + 1] = -1;
                boardData[x][y + 2] = -1;

                for (int i = y - 1; i >= 0; i--) {
                    if (boardData[x][i] != color) break;
                    boardData[x][i] = -1;
                }

                for (int i = y + 3; i < BOARD_HEIGHT; i++) {
                    if (boardData[x][i] != color)
                        boardData[x][i] = -1;
                }

                // Remove matched cells and shift the remaining ones down
                for (int x = 0; x < BOARD_WIDTH; x++) {
                    int shift = 0;
                    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
                        if (boardData[x][y] == -1) {
                            shift++;
                            continue;
                        }
                        if (shift > 0) {
                            boardData[x][y + shift] = boardData[x][y];
                            boardData[x][y] = -1;
                        }
                    }
                }
                refillBoard();
                prepareBoard();
            }
        }
    }
}

void GameBoard::refillBoard() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 4);

    // Finding empty cells and filling them with new random cells
    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            if (boardData[x][y] == -1) {
                boardData[x][y] = dist(gen);
                board[x][y].setFillColor(colors[boardData[x][y]]);
            }
        }
    }
}