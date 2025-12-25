#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <string>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <ctime>

/*
FLOW:
- main() starts multiple game sessions using GameManager
- GameManager creates a new Game per request in a separate thread
- Game initializes Board, Dice and Players
- Players take turns rolling dice
- Player position is updated
- Board checks for Snake or Ladder and returns final position
- Game continues until a player reaches last cell
*/

/*
ENTITIES & DATA MEMBERS:
Player
-name
-position

Dice

Snake : BoardEntity
-head
-tail

Ladder : BoardEntity
-bottom
-top

Board
-size
-unordered_map<int,int> jumps
-entities

Game
-board
-dice
-players
-status
-winner

GameManager (Singleton)
-mutex mtx
*/


using namespace std;

/* ===================== ENUM ===================== */

enum class GameStatus {
    NOT_STARTED,
    RUNNING,
    FINISHED
};

/* ===================== PLAYER ===================== */

class Player {
public:
    string name;
    int position;

    Player(const string& name) : name(name), position(0) {}
};

/* ===================== DICE ===================== */

class Dice {
public:
    int roll() {
        return rand() % 6 + 1;
    }
};

/* ===================== BOARD ENTITY (INTERFACE) ===================== */

class BoardEntity {
public:
    virtual int getStart() const = 0;
    virtual int getEnd() const = 0;
    virtual ~BoardEntity() {}
};

/* ===================== SNAKE ===================== */

class Snake : public BoardEntity {
    int head;
    int tail;

public:
    Snake(int head, int tail) : head(head), tail(tail) {}

    int getStart() const override {
        return head;
    }

    int getEnd() const override {
        return tail;
    }
};

/* ===================== LADDER ===================== */

class Ladder : public BoardEntity {
    int bottom;
    int top;

public:
    Ladder(int bottom, int top) : bottom(bottom), top(top) {}

    int getStart() const override {
        return bottom;
    }

    int getEnd() const override {
        return top;
    }
};

/* ===================== BOARD ===================== */

class Board {
    int size;
    unordered_map<int, int> jumps;
    vector<BoardEntity*> entities;

public:
    Board(int size = 100) : size(size) {}

    void addEntity(BoardEntity* entity) {
        entities.push_back(entity);
        jumps[entity->getStart()] = entity->getEnd();
    }

    int getFinalPosition(int pos) const {
        auto it = jumps.find(pos);
        return (it != jumps.end()) ? it->second : pos;
    }

    int getSize() const {
        return size;
    }

    ~Board() {
        for (auto e : entities) {
            delete e;
        }
    }
};

/* ===================== GAME ===================== */

class Game {
    Board board;
    Dice dice;
    queue<Player*> players;
    GameStatus status;
    Player* winner;

public:
    Game(const vector<string>& playerNames)
        : status(GameStatus::NOT_STARTED), winner(nullptr) {

        for (const auto& name : playerNames) {
            players.push(new Player(name));
        }

        // Snakes
        board.addEntity(new Snake(99, 54));
        board.addEntity(new Snake(70, 55));
        board.addEntity(new Snake(52, 42));

        // Ladders
        board.addEntity(new Ladder(2, 38));
        board.addEntity(new Ladder(15, 26));
        board.addEntity(new Ladder(8, 31));
    }

    void takeTurn(Player* player) {
        int roll = dice.roll();
        int next = player->position + roll;

        if (next > board.getSize()) {
            cout << player->name << " rolled " << roll
                 << " but stays at " << player->position << endl;
            return;
        }

        cout << player->name << " rolled " << roll
             << " moved from " << player->position
             << " to " << next;

        int finalPos = board.getFinalPosition(next);
        if (finalPos != next) {
            cout << " -> " << finalPos;
        }
        cout << endl;

        player->position = finalPos;

        if (finalPos == board.getSize()) {
            status = GameStatus::FINISHED;
            winner = player;
        }
    }

    void play() {
        status = GameStatus::RUNNING;

        while (status == GameStatus::RUNNING) {
            Player* current = players.front();
            players.pop();

            takeTurn(current);

            if (status == GameStatus::FINISHED) {
                cout << "Winner is: " << current->name << "\n\n";
                break;
            }

            players.push(current);
        }

        cleanup();
    }

};

/* ===================== GAME MANAGER (SINGLETON) ===================== */

class GameManager {
    GameManager() {}          // private constructor
    mutex mtx;

public:
    // delete copy operations
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    static GameManager* getInstance() {
        static GameManager instance;   
        return &instance;
    }

    void runGame(vector<string> players) {
        Game game(players);
        game.play();
    }
    
    void startGame(const vector<string>& players) {
        lock_guard<mutex> lock(mtx);
        thread(runGame, players).detach();
    }
};


/* ===================== DEMO ===================== */

int main() {
    srand(time(nullptr));

    GameManager* manager = GameManager::getInstance();

    manager->startGame({"Alice", "Bob"});
    manager->startGame({"Tom", "Jerry", "Spike"});

    this_thread::sleep_for(chrono::seconds(5));
    return 0;
}

