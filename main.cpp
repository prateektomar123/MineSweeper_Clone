#include <iostream>
#include <vector>
#include <random>
#include <ctime>
using namespace std;

class Cell {
private:
    bool isMine;
    bool isRevealed;
    int adjacentMines;

public:
    Cell() : isMine(false), isRevealed(false), adjacentMines(0) {}
    
    void setMine(bool mine) { isMine = mine; }
    bool hasMine() const { return isMine; }
    void reveal() { isRevealed = true; }
    bool isOpen() const { return isRevealed; }
    void setAdjacentMines(int count) { adjacentMines = count; }
    int getAdjacentMines() const { return adjacentMines; }
};

class Board {
private:
    vector<vector<Cell>> grid;
    int size;
    int numMines;
    bool firstMove;

    bool isValid(int x, int y) const {
        return x >= 1 && x <= size && y >= 1 && y <= size;
    }

    void calculateAdjacentMines() {
        for (int y = 1; y <= size; y++) {
            for (int x = 1; x <= size; x++) {
                if (!grid[y][x].hasMine()) {
                    int count = 0;
                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dx = -1; dx <= 1; dx++) {
                            if (dx == 0 && dy == 0) continue;
                            int nx = x + dx, ny = y + dy;
                            if (isValid(nx, ny) && grid[ny][nx].hasMine()) {
                                count++;
                            }
                        }
                    }
                    grid[y][x].setAdjacentMines(count);
                }
            }
        }
    }

    void placeMines(int firstX, int firstY) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(1, size);

        int minesPlaced = 0;
        while (minesPlaced < numMines) {
            int x = dis(gen);
            int y = dis(gen);
            if (!grid[y][x].hasMine() && (x != firstX || y != firstY)) {
                grid[y][x].setMine(true);
                minesPlaced++;
            }
        }
        calculateAdjacentMines();
    }

    void revealEmpty(int x, int y) {
        if (!isValid(x, y) || grid[y][x].isOpen()) return;
        
        grid[y][x].reveal();
        
        if (grid[y][x].getAdjacentMines() == 0) {
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;
                    revealEmpty(x + dx, y + dy);
                }
            }
        }
    }

public:
    Board(int n) : size(n), numMines(n * n / 6), firstMove(true) {
        if (n < 9) throw invalid_argument("Board size must be at least 9");
        grid.resize(n + 1, vector<Cell>(n + 1));
    }

    bool makeMove(int x, int y) {
        if (!isValid(x, y)) return false;
        
        if (firstMove) {
            placeMines(x, y);
            firstMove = false;
        }

        if (grid[y][x].hasMine()) {
            return false;
        }

        revealEmpty(x, y);
        return true;
    }

    bool checkWin() const {
        for (int y = 1; y <= size; y++) {
            for (int x = 1; x <= size; x++) {
                if (!grid[y][x].hasMine() && !grid[y][x].isOpen()) {
                    return false;
                }
            }
        }
        return true;
    }

    void display() const {
        cout << "  ";
        for (int x = 1; x <= size; x++) {
            cout << x << " ";
        }
        cout << "\n";
        
        for (int y = 1; y <= size; y++) {
            cout << y << " ";
            for (int x = 1; x <= size; x++) {
                if (grid[y][x].isOpen()) {
                    if (grid[y][x].getAdjacentMines() > 0) {
                        cout << grid[y][x].getAdjacentMines() << " ";
                    } else {
                        cout << "  ";
                    }
                } else {
                    cout << "| ";
                }
            }
            cout << "\n";
        }
    }

    void revealAll() const {
        cout << "  ";
        for (int x = 1; x <= size; x++) {
            cout << x << " ";
        }
        cout << "\n";
        
        for (int y = 1; y <= size; y++) {
            cout << y << " ";
            for (int x = 1; x <= size; x++) {
                if (grid[y][x].hasMine()) {
                    cout << "* ";
                } else if (grid[y][x].getAdjacentMines() > 0) {
                    cout << grid[y][x].getAdjacentMines() << " ";
                } else {
                    cout << "  ";
                }
            }
            cout << "\n";
        }
    }
};

class Game {
private:
    Board board;

public:
    Game(int size) : board(size) {}

    void play() {
        cout << "Welcome to Minesweeper!\n";
        cout << "Enter coordinates as 'x y' (1-based indexing)\n";

        while (true) {
            board.display();
            
            int x, y;
            cout << "Enter coordinates: ";
            cin >> x >> y;

            if (!board.makeMove(x, y)) {
                cout << "\nGame Over! You hit a mine!\n";
                board.revealAll();
                break;
            }

            if (board.checkWin()) {
                cout << "\nCongratulations! You won!\n";
                board.revealAll();
                break;
            }
        }
    }
};

int main() {
    int size;
    do {
        cout << "Enter board size (minimum 9): ";
        cin >> size;
    } while (size < 9);

    Game game(size);
    game.play();

    return 0;
}