#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;

class GameImpl
{
public:
    GameImpl(int nRows, int nCols);
    int rows() const;
    int cols() const;
    bool isValid(Point p) const;
    Point randomPoint() const;
    bool addShip(int length, char symbol, string name);
    int nShips() const;
    int shipLength(int shipId) const;
    char shipSymbol(int shipId) const;
    string shipName(int shipId) const;
    Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);

private:
    void turn(Player* myTurn, Player* opponent, Board& opponentBoard, bool& shotHit, bool& shipDestroyed, int& shipId);

    int m_rows;
    int m_cols;
    int m_ships;

    struct Ship {
        Ship() : m_length(0), m_symbol(0), m_name("") {};
        Ship(int nLength, char nSymbol, string nName) : m_length(nLength), m_symbol(nSymbol), m_name(nName) {};

        int m_length;
        char m_symbol;
        string m_name;
    };

    vector<Ship> shipArr;
};

GameImpl::GameImpl(int nRows, int nCols) : m_rows(nRows), m_cols(nCols)
{

}

int GameImpl::rows() const
{
    return m_rows;  
}

int GameImpl::cols() const
{
    return m_cols;
}

bool GameImpl::isValid(Point p) const
{
    return p.r >= 0 && p.r < rows() && p.c >= 0 && p.c < cols();
}

Point GameImpl::randomPoint() const
{
    return Point(randInt(rows()), randInt(cols()));
}

//The ship has the indicated length, which must
//be positive and must allow the ship to fit on the board. When the board is displayed, the
//indicated symbol will be displayed in the positions occupied by that ship; that symbol
//must be a printable character other than X, o, and . (which are used for other purposes in
//the display), and must not be the same as for any other ship. (A printable character is one
//for which the <cctype> function isprint returns a non - zero value.) The indicated
//name is what will be used to denote the ship when writing messages
bool GameImpl::addShip(int length, char symbol, string name)
{
    shipArr.push_back(Ship(length, symbol, name));
    m_ships++;

    return true;
}

int GameImpl::nShips() const
{
    return m_ships;
}

int GameImpl::shipLength(int shipId) const
{
    return shipArr[shipId].m_length;
}

char GameImpl::shipSymbol(int shipId) const
{
    return shipArr[shipId].m_symbol;
}

string GameImpl::shipName(int shipId) const
{
    return shipArr[shipId].m_name;
}

/* a. Display the second player's board (since the first player will be attacking
        that board).If the first player is human, do not show undamaged segments
        on the opponent's board, since that would be cheating. If the first player is
        not human, then show the entire opponent's board, since we're just an
        onlooker to what the computer player will decide for itself.
      b.Make the first player's attack.
      c.Display the result of the attack.
      d.Repeat these steps with the roles of the first and second player reversed.*/
void GameImpl::turn(Player* myTurn, Player* opponent, Board& opponentBoard, bool& shotHit, bool& shipDestroyed, int& shipId) {
    cout << myTurn->name() << "'s turn. Board for " << opponent->name() << ":" << endl;
    opponentBoard.display(myTurn->isHuman());

    Point atk = myTurn->recommendAttack();
    if (!opponentBoard.attack(atk, shotHit, shipDestroyed, shipId)) { //here
        cout << myTurn->name() << " wasted a shot at (" << atk.r << "," << atk.c << ")." << endl;
        myTurn->recordAttackResult(atk, false, false, false, shipId);
        opponent->recordAttackByOpponent(atk);
    }
    else {
        cout << myTurn->name() << " attacked (" << atk.r << "," << atk.c << ") and ";
        if (shotHit && shipDestroyed) {
            cout << "destroyed the " << shipName(shipId);
        }
        else if (shotHit && !shipDestroyed) {
            cout << "hit something";
        }
        else {
            cout << "missed";
        }

        cout << ", resulting in:" << endl;
        opponentBoard.display(myTurn->isHuman());

        myTurn->recordAttackResult(atk, true, shotHit, shipDestroyed, shipId);
        opponent->recordAttackByOpponent(atk);
    }
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{
    if (!p1->placeShips(b1) || !p2->placeShips(b2)) {
        return nullptr;
    }

    while (!b1.allShipsDestroyed() && !b2.allShipsDestroyed())
    {
        bool shotHit = false;
        bool shipDestroyed = false;
        int shipId = -1;
         
        //take turn
        turn(p1, p2, b2, shotHit, shipDestroyed, shipId);

        if (b2.allShipsDestroyed()) {
            //p1 win
            cout << p1->name() << " wins!" << endl;
            if (p2->isHuman())
            {
                cout << "Here's where " << p1->name() << "'s ships were:" << endl;
                b1.display(false);
            }
            return p1; //end
        }

        if (shouldPause) {
            cout << "Press enter to continue: ";
            cin.ignore(10000, '\n');
        }

        turn(p2, p1, b1, shotHit, shipDestroyed, shipId);

        if (b1.allShipsDestroyed()) {
            //p2 win
            cout << p2->name() << " wins!" << endl;
            if (p1->isHuman()) {
                cout << "Here's where " << p2->name() << "'s ships were:" << endl;
                b2.display(false);
            }
            return p2;
        }

        if (shouldPause) {
            cout << "Press enter to continue: ";
            cin.ignore(10000, '\n');
        }
    }

    return p1;
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
    if (nRows < 1 || nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1 || nCols > MAXCOLS)
    {
        cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
        exit(1);
    }
    m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
    delete m_impl;
}

int Game::rows() const
{
    return m_impl->rows();
}

int Game::cols() const
{
    return m_impl->cols();
}

bool Game::isValid(Point p) const
{
    return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
    return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
    if (length < 1)
    {
        cout << "Bad ship length " << length << "; it must be >= 1" << endl;
        return false;
    }
    if (length > rows() && length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
            << endl;
        return false;
    }
    if (!isascii(symbol) || !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
            << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X' || symbol == '.' || symbol == 'o')
    {
        cout << "Character " << symbol << " must not be used as a ship symbol"
            << endl;
        return false;
    }
    int totalOfLengths = 0;
    for (int s = 0; s < nShips(); s++)
    {
        totalOfLengths += shipLength(s);
        if (shipSymbol(s) == symbol)
        {
            cout << "Ship symbol " << symbol
                << " must not be used for more than one ship" << endl;
            return false;
        }
    }
    if (totalOfLengths + length > rows() * cols())
    {
        cout << "Board is too small to fit all ships" << endl;
        return false;
    }
    return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
    return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
    assert(shipId >= 0 && shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0 && shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0 && shipId < nShips());
    return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
    if (p1 == nullptr || p2 == nullptr || nShips() == 0)
        return nullptr;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}

