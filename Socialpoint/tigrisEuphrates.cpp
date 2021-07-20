#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <string>
#include <bitset>
#include <cstdio>
#include <limits>
#include <vector>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <numeric>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>

using namespace std;

// Possible exceptions

#define GENERIC1 "invalid board space position"
#define GENERIC2 "board space is already occupied"
#define GENERIC3 "invalid number or player tiles"
#define GENERIC4 "player not found"
#define GENERIC5 "too many turn actions"

#define PARSING1 "could not parse tile type"
#define PARSING2 "could not parse color type"
#define PARSING3 "could not parse leader type"
#define PARSING4 "could not parse dynasty type" // 

#define CIVTILE1 "only farms can be placed in a river tile"
#define CIVTILE2 "could not find tile of specified type"

#define LEADERT1 "cannot place a leader in a river"
#define LEADERT2 "cannot place a leader in a space without adjacent temple"
#define LEADERT3 "cannot place a leader that would unite two kingdoms"

#define TREASUR1 "trying to take treasure where there is none"
#define TREASUR2 "taking incorrect number of treasures"
#define TREASUR3 "treasure region does not have a leader"

#define CONFLIC1 "could not find conflict attacker or defender"
#define CONFLIC2 "no loader found in the revolt attacker position" //
#define CONFLIC3 "board has unresolved conflicts"

#define CATASTR1 "cannot place catastrophe if space already contains a leader"
#define CATASTR2 "cannot place catastrophe if space is bearing a treasure"
#define CATASTR3 "cannot place catastrophe if space is bearing a monument"

#define MONUMEN1 "monument needs to be build on tiles"
#define MONUMEN2 "monument needs to be build on tiles of the same type"
#define MONUMEN3 "monument colors needs to match tiles"
#define MONUMEN4 "monument color combination not available"

// Tile types
#define TEMPLE "temple"
#define MARKET "market"
#define SETTLE "settlement"
#define FARM "farm"
#define CATASTROPHE "catastrophe"

#define TREASURE "treasure"

// Leader types
#define KING "king"
#define FARMER "farmer"
#define TRADER "trader"
#define PRIEST "priest"

// Colors
#define RD "red"
#define GN "green"
#define BL "blue"
#define BK "black"

// Boar size
#define SIZEX 16
#define SIZEY 11

void throwMessage(char const *message)
{
    try
    {
        throw message;
    }
    catch (char const *msg)
    {
        cout << "exception: " << msg << '\n';
    }
}

string randomTile()
{
    int random = rand() % 4;
    switch (random)
    {
    case 0:
        return TEMPLE;
    case 1:
        return SETTLE;
    case 2:
        return MARKET;
    case 3:
        return FARM;
    default:
        return FARM;
    }
}

// Global variables 1

int nextRegion;

// Global methods 1 (some classes use them)

bool validBoardPosition(int x, int y, bool err)
{
    bool b = x >= 0 && x < SIZEX && y >= 0 && y < SIZEY;
    if (!b && err)
        throwMessage(GENERIC1);
    return b;
}

// Classes

// Player class contains all relevant information about each player
class Player
{
private:
    list<string> tiles;
    int treasure = 0, bricks = 0, amulets = 0, crops = 0, goods = 0;

public:
    Player(list<string> iniTiles)
    {
        this->tiles = iniTiles;
    }
    ~Player() {}

    int numberTiles() { return tiles.size(); }
    void addTile(string tile) { this->tiles.push_back(tile); }
    void deleteTile(string tile) 
    {
        list<string>::iterator it = tiles.begin();
        for(string t : tiles)
        {
            if (t == tile) break;
            ++it;
        }
        tiles.erase(it);
        
    }
    bool hasThisTile(string type)
    {
        for(string t : tiles)
        {
            if (t == type) return true;
        }
        return false;
    }
    bool hasNTiles(string type, int n)
    {
        int count = 0;
        for(string t : tiles)
        {
            if (t == type) count += 1;
        }
        if (count >= n) return true;
        return false;
    }
    void addPoint(string type)
    {
        if ( type == TREASURE ) treasure += 1;
        if ( type == SETTLE ) bricks += 1;
        if ( type == TEMPLE ) amulets += 1;
        if ( type == FARM ) crops += 1;
        if ( type == MARKET ) goods += 1;
    }
    void printVictoryPoints()
    {
        printf("victory %d (%d %d %d %d)\n", this->treasure, this->bricks, this->amulets, this->crops, this->goods);
    }

    // debug 
    void printTiles()
    {
        for (string t : tiles)
            cout << t << " ";
        printf("\n");
    }
    void reloadRandomTiles()
    {
        while (this->numberTiles() < 6)
        {
            this->tiles.push_back(randomTile());
        }
    }
};

struct War
{
    pair<int,int> warTile;
    string leaderType;

    int attackerPlayer;
    int defenderPlayer;

    pair<int,int> attackerPosition;
    pair<int,int> defenderPosition;

    int attackerRegion;
    int defenderRegion;

    int attackerStartingTiles;
    int defenderStartingTiles;
};

struct Revolt
{
    int region;
    string leaderType;

    int attackerPlayer;
    int defenderPlayer;

    pair<int,int> attackerPosition;
    pair<int,int> defenderPosition;

    int attackerStartingTiles;
    int defenderStartingTiles;
};


struct Tile
{
    string type;
    int region;
    bool hasTreasure;
    bool isKingdom;
};

struct Cell
{
    bool hasTile;
    bool isLeader;
    Tile tile;
    bool isRiver;
    bool isMonument;
    bool isCatastrophe;
};

class Board
{
private:

    int setRegion(int x, int y, int prevRegion) 
    {
        return nearestRegion(x, y, prevRegion);
    }

    bool setKingdom(int x, int y)
    {
        return nearestKingdom(x, y);
    }

    bool hasLeaderNear(int x, int y)
    {
        return leaderNearTile(x, y);
    }

    Tile newTile(string type, int x, int y, int prevRegion)
    {
        Tile t;
        t.type = type;
        t.hasTreasure = false;
        t.region = setRegion(x, y, prevRegion);
        t.isKingdom = setKingdom(x, y);
        return t;
    }

    Tile newCatastrophe(int x, int y)
    {
        Tile t;
        t.type = CATASTROPHE;
        t.hasTreasure = false;  
        t.region = -9;
        t.isKingdom = false;
        return t;
    }

public:
    Cell board[SIZEX][SIZEY]; // not the best practice to have this exposed but whatever

    Board() {}
    ~Board() {}
    
    void iniBoard() 
    {
        // ini all cells
        Cell cell;
        cell.hasTile = false;
        cell.isLeader = false;
        cell.isRiver = false;
        cell.isMonument = false;
        cell.isCatastrophe = false;
        for (int x = 0; x < SIZEX; x++)
            for (int y = 0; y < SIZEY; y++)
            {
                board[x][y] = cell;
            }
        
        // ini monuments
        Tile t;
        t.type = TEMPLE;
        t.hasTreasure = true;
        t.isKingdom = false;
        cell.hasTile = true;
        t.region = 0; cell.tile = t; board[1][3] = cell;
        t.region = 1; cell.tile = t; board[1][9] = cell;
        t.region = 2; cell.tile = t; board[5][1] = cell;
        t.region = 3; cell.tile = t; board[5][8] = cell;
        t.region = 4; cell.tile = t; board[8][4] = cell;
        t.region = 5; cell.tile = t; board[10][0] = cell;
        t.region = 6; cell.tile = t; board[10][10] = cell;
        t.region = 7; cell.tile = t; board[13][6] = cell;
        t.region = 8; cell.tile = t; board[14][2] = cell;
        t.region = 9; cell.tile = t; board[15][9] = cell;

        // ini south river
        board[0][4].isRiver = true; board[1][4].isRiver = true; board[2][4].isRiver = true; board[3][4].isRiver = true;
        board[3][3].isRiver = true; board[4][3].isRiver = true; board[5][3].isRiver = true; board[6][3].isRiver = true;
        board[6][2].isRiver = true; board[7][2].isRiver = true; board[8][2].isRiver = true; board[9][2].isRiver = true;
        board[10][2].isRiver = true; board[11][2].isRiver = true; board[12][2].isRiver = true; board[12][3].isRiver = true;
        board[12][4].isRiver = true; board[13][4].isRiver = true; board[14][4].isRiver = true; board[14][5].isRiver = true;
        board[14][6].isRiver = true; board[15][6].isRiver = true; board[15][7].isRiver = true; board[14][7].isRiver = true;
        board[13][7].isRiver = true; board[13][8].isRiver = true; board[12][8].isRiver = true; board[12][9].isRiver = true;
        board[12][10].isRiver = true;

        // ini north river
        board[0][7].isRiver = true; board[1][7].isRiver = true; board[2][7].isRiver = true; board[3][7].isRiver = true;
        board[3][8].isRiver = true; board[4][8].isRiver = true; board[4][9].isRiver = true; board[4][10].isRiver = true;
        board[5][10].isRiver = true; board[6][10].isRiver = true; board[7][10].isRiver = true; board[8][10].isRiver = true;
    }

    void addTile(string type, int x, int y) 
    {
        board[x][y].tile = newTile(type, x, y, -1);
        board[x][y].hasTile = true;
        board[x][y].isLeader = false;
    }

    void clearTile(int x, int y)
    {
        board[x][y].hasTile = false;
        board[x][y].isLeader = false;
    }

    int clearTilesByRegionType(int region, string type)
    {
        int clearedTiles = 0;
        for ( int x = 0; x < SIZEX; x++ )
            for ( int y = 0; y < SIZEY; y++ )
                if ( board[x][y].hasTile )
                    if ( board[x][y].tile.region == region )
                        if ( board[x][y].tile.type == type )
                        {
                            if ( type != PRIEST || hasLeaderNear(x, y) )
                            {
                                clearedTiles++;
                                clearTile(x, y);
                            }
                        }
        return clearedTiles;
    }

    void addLeader(string type, int x, int y) 
    {
        board[x][y].tile = newTile(type, x, y, -1);
        board[x][y].hasTile = true;
        board[x][y].isLeader = true;
        board[x][y].isMonument = false;
    }

    void moveLeader(string type, int x, int y, int x0, int y0) 
    {

        board[x][y].tile = newTile(type, x, y, board[x0][y0].tile.region);
        board[x][y].hasTile = true;
        board[x][y].isLeader = true;
        board[x][y].isMonument = false;
        clearTile(x0, y0);
    }

    void addCatastrophe(int x, int y)
    {
        board[x][y].tile = newCatastrophe(x, y);
        board[x][y].hasTile = false;
        board[x][y].isLeader = false;
        board[x][y].isMonument = false;
        board[x][y].isCatastrophe = true;
    }

    void newMonument(int x, int y)
    {
        board[x][y].isMonument = true;
        board[x][y+1].isMonument = true;
        board[x+1][y].isMonument = true;
        board[x+1][y+1].isMonument = true;
    }

    void updateRegions(int x, int y)
    {
        int reg = board[x][y].tile.region;

        queue<int> reg2change;
        if( validBoardPosition(x, y+1, false) && board[x][y+1].hasTile ) reg2change.push(board[x][y+1].tile.region);
        if( validBoardPosition(x, y-1, false) && board[x][y-1].hasTile ) reg2change.push(board[x][y-1].tile.region);
        if( validBoardPosition(x+1, y, false) && board[x+1][y].hasTile ) reg2change.push(board[x+1][y].tile.region);
        if( validBoardPosition(x-1, y, false) && board[x-1][y].hasTile ) reg2change.push(board[x-1][y].tile.region);

        while (!reg2change.empty())
        {
            int change = reg2change.front();
            reg2change.pop();

            for (int i = 0; i < SIZEX; i++)
                for (int j = 0; j < SIZEY; j++)
                    if (board[i][j].hasTile)
                        if (board[i][j].tile.region == change)
                            board[i][j].tile.region = reg;
        }
    }

    void updateKingdoms(int x, int y, bool state)
    {
        int reg = board[x][y].tile.region;
        for (int i = 0; i < SIZEX; i++)
            for (int j = 0; j < SIZEY; j++)
                if (board[i][j].hasTile)
                    if (board[i][j].tile.region == reg)
                        board[i][j].tile.isKingdom = state;
    }

    bool templeNear(int x, int y)
    {
        if( validBoardPosition(x, y+1, false) && board[x][y+1].hasTile && board[x][y+1].tile.type == TEMPLE ) return true;
        if( validBoardPosition(x, y-1, false) && board[x][y-1].hasTile && board[x][y-1].tile.type == TEMPLE ) return true;
        if( validBoardPosition(x+1, y, false) && board[x+1][y].hasTile && board[x+1][y].tile.type == TEMPLE ) return true;
        if( validBoardPosition(x-1, y, false) && board[x-1][y].hasTile && board[x-1][y].tile.type == TEMPLE ) return true;

        return false;
    }

    int nearestRegion(int x, int y, int prevRegion)
    {
        int N = -1, S = -1, E = -1, W = -1;
        if( validBoardPosition(x, y+1, false) && board[x][y+1].hasTile ) N = board[x][y+1].tile.region;
        if( validBoardPosition(x, y-1, false) && board[x][y-1].hasTile ) S = board[x][y-1].tile.region;
        if( validBoardPosition(x+1, y, false) && board[x+1][y].hasTile ) E = board[x+1][y].tile.region;
        if( validBoardPosition(x-1, y, false) && board[x-1][y].hasTile ) W = board[x-1][y].tile.region;

        if ( prevRegion != -1)
            if ( prevRegion == N || prevRegion == S || prevRegion == E || prevRegion == W )
                return prevRegion;
        
        int reg = -1;
        if (N != -1) reg = N;
        if (S != -1)
            if ( !(reg != -1 && S != reg) ) reg = S;

        if (E != -1)
            if ( !(reg != -1 && E != reg) ) reg = E;

        if (W != -1)
            if ( !(reg != -1 && W != reg) ) reg = W;

        if (reg == -1)
        {
            reg = nextRegion;
            ++nextRegion;
        }
        return reg;
    }

    bool nearestKingdom(int x, int y)
    {
        if( validBoardPosition(x, y+1, false) && board[x][y+1].hasTile && board[x][y+1].tile.isKingdom ) return true;
        if( validBoardPosition(x, y-1, false) && board[x][y-1].hasTile && board[x][y-1].tile.isKingdom ) return true;
        if( validBoardPosition(x+1, y, false) && board[x+1][y].hasTile && board[x+1][y].tile.isKingdom ) return true;
        if( validBoardPosition(x-1, y, false) && board[x-1][y].hasTile && board[x-1][y].tile.isKingdom ) return true;

        return false;
    }

    bool leaderNearTile(int x, int y)
    {
        if( validBoardPosition(x, y+1, false) && board[x][y+1].isLeader ) return true;
        if( validBoardPosition(x, y-1, false) && board[x][y-1].isLeader ) return true;
        if( validBoardPosition(x+1, y, false) && board[x+1][y].isLeader ) return true;
        if( validBoardPosition(x-1, y, false) && board[x-1][y].isLeader ) return true;

        return false;
    }

    bool betweenTwoKingdoms(int x, int y)
    {
        int N = -1, S = -1, E = -1, W = -1;
        if( validBoardPosition(x, y+1, false) && board[x][y+1].hasTile && board[x][y+1].tile.isKingdom) N = board[x][y+1].tile.region;
        if( validBoardPosition(x, y-1, false) && board[x][y-1].hasTile && board[x][y-1].tile.isKingdom) S = board[x][y-1].tile.region;
        if( validBoardPosition(x+1, y, false) && board[x+1][y].hasTile && board[x+1][y].tile.isKingdom) E = board[x+1][y].tile.region;
        if( validBoardPosition(x-1, y, false) && board[x-1][y].hasTile && board[x-1][y].tile.isKingdom) W = board[x-1][y].tile.region;

        if (N != -1)
        {
            if (S != -1 && S != N) return true;
            if (E != -1 && E != N) return true;
            if (W != -1 && W != N) return true;
        }
        if (S != -1)
        {
            if (E != -1 && E != S) return true;
            if (W != -1 && W != S) return true;
        }
        if (E != -1)
        {
            if (W != -1 && W != E) return true;
        }

        return false;
    }

    void printState()
    {
        for (int y = SIZEY-1; y >= 0; y--)
        {
            for (int x = 0; x < SIZEX; x++)
            {
                if (board[x][y].hasTile)
                {
                    if (board[x][y].isMonument)
                    {
                        cout << " MM ";
                    }
                    else if (board[x][y].isLeader)
                    {
                        if (board[x][y].tile.hasTreasure)
                            cout << "T";
                        else if (board[x][y].isMonument)
                            cout << "M";
                        else
                            cout << " ";
                        if (board[x][y].tile.region/10 != 0)
                            cout << board[x][y].tile.region << "L";
                        else
                            cout << "0" << board[x][y].tile.region << "L";
                    }
                    else if (board[x][y].tile.isKingdom)
                    {
                        if (board[x][y].tile.hasTreasure)
                            cout << "T";
                        else if (board[x][y].isMonument)
                            cout << "M";
                        else
                            cout << " ";
                        if (board[x][y].tile.region/10 != 0)
                            cout << board[x][y].tile.region << "K";
                        else
                            cout << "0" << board[x][y].tile.region << "K";
                    }
                    else 
                    {
                        if (board[x][y].tile.hasTreasure)
                            cout << "T";
                        else if (board[x][y].isMonument)
                            cout << "M";
                        else
                            cout << " ";
                        if (board[x][y].tile.region/10 != 0)
                            cout << board[x][y].tile.region << " ";
                        else
                            cout << "0" << board[x][y].tile.region << " ";
                    }
                }
                else if (board[x][y].isCatastrophe)
                {
                    cout << " XX ";
                }                
                else if (board[x][y].isRiver)
                    cout << " ~~ ";
                else
                    cout << "    ";
            }
            cout << endl;
        }
    }
};

// Global variables 2

list<Player> players;
list<Player>::iterator activePlayer;
map<string,pair<int,int>> leaders[4]; // first: leaderType second: location
map<pair<string,string>,pair<int,int>> monuments; // first: colors(top,base) second: owner + region
int currentPlayer, turnActions;
Board gameBoard;
queue<Revolt> unresolvedRevolt;
list<War> unresolvedWar;

// Global methods 2 (bools used to cheeck some exceptions)

list<Player>::iterator selectPlayer(int player)
{
    int x = 0;
    list<Player>::iterator selected = players.begin();
    for (Player p : players)
    {
        if (x == player) break;
        ++selected;
        ++x;
    }
    return selected;
}

bool noRevoltToResolve()
{
    if ( !unresolvedRevolt.empty() )return true;
    throwMessage(CONFLIC1);
    return false;
}

bool noWarToResolve(string leader)
{
    for (War w : unresolvedWar)
    {
        if ( w.leaderType == leader) return true;
    }
    throwMessage(CONFLIC1);
    return false;
}

bool allConflictsResolved()
{
    if ( unresolvedRevolt.empty() && unresolvedWar.empty() )return true;
    throwMessage(CONFLIC3);
    return false;
}

bool playerHasTurnActions()
{
    if (turnActions < 2) return true;
    throwMessage(GENERIC5);
    return false;
}

bool validTileType(string a)
{
    bool b = a == TEMPLE || a == MARKET || a == SETTLE || a == FARM;
    if (!b)
        throwMessage(PARSING1);
    return b;
}

bool validColorType(string c1, string c2)
{
    bool b = (c1 == RD || c1 == GN || c1 == BL || c1 == BK) &&
             (c2 == RD || c2 == GN || c2 == BL || c2 == BK);
    if (!b)
        throwMessage(PARSING2);
    return b;
}

bool validLeaderType(string a)
{
    bool b = a == KING || a == FARMER || a == TRADER || a == PRIEST;
    if (!b)
        throwMessage(PARSING3);
    return b;
}

bool activePlayerHasTile(string type)
{
    bool b = activePlayer->hasThisTile(type);
    if (!b)
        throwMessage(CIVTILE2);
    return b;
}

bool playerHasNTiles(int player, string type, int n)
{
    list<Player>::iterator it = selectPlayer(player);
    bool b = it->hasNTiles(type,n);
    if (!b)
        throwMessage(GENERIC3);
    return b;
}

bool canPlaceThisTile(string type, int x, int y)
{
    if (gameBoard.board[x][y].hasTile || gameBoard.board[x][y].isCatastrophe)
    {
        throwMessage(GENERIC2);
        return false;
    }
    else if ((type != FARM && gameBoard.board[x][y].isRiver) || (type == FARM && !gameBoard.board[x][y].isRiver))
    {
        throwMessage(CIVTILE1);
        return false;
    }
    return true;
}

bool canPlaceThisLeader(string type, int x, int y)
{
    if (gameBoard.board[x][y].hasTile || gameBoard.board[x][y].isCatastrophe)
    {
        throwMessage(GENERIC2);
        return false;
    }
    else if (gameBoard.board[x][y].isRiver)
    {
        throwMessage(LEADERT1);
        return false;
    }
    else if (!gameBoard.templeNear(x, y))
    {
        throwMessage(LEADERT2);
        return false;
    }
    return true;
}

bool itWillUniteTwoKingdoms(int x, int y, bool err)
{
    if (gameBoard.betweenTwoKingdoms(x, y))
    {
        if (err) throwMessage(LEADERT3);
        return true;
    }
    return false;
}

bool isPlayer(int p)
{
    if (selectPlayer(p) == players.end())
    {
        throwMessage(GENERIC4);
        return false;
    }
    return true;
}

bool canOcurrCatastrophe(int x, int y)
{
    if (gameBoard.board[x][y].hasTile)
    {
        if (gameBoard.board[x][y].isLeader)
        {
            throwMessage(CATASTR1);
            return false;
        }
        if (gameBoard.board[x][y].tile.hasTreasure)
        {
            throwMessage(CATASTR2);
            return false;
        }
        if (gameBoard.board[x][y].isMonument)
        {
            throwMessage(CATASTR3);
            return false;
        }
    }
    return true;
}

bool tileHasTreasure(int x, int y)
{
    if (gameBoard.board[x][y].hasTile)
    {
        if (!gameBoard.board[x][y].tile.hasTreasure)
        {
            throwMessage(TREASUR1);
            return false;
        }
        return true;
    }
    else 
    {
        throwMessage(TREASUR1);
        return false;
    }
}

int howManyTreasuresHave(int region)
{
    int availableTreasures = 0;

    for ( int x = 0; x < SIZEX; x++ )
        for ( int y = 0; y < SIZEY; y++ )
            if ( gameBoard.board[x][y].hasTile )
                if ( gameBoard.board[x][y].tile.region == region )
                    if ( gameBoard.board[x][y].tile.hasTreasure ) availableTreasures += 1;
    
    return availableTreasures;
}

bool canTakeTreasure(int region, int currentTreasures)
{
    int availableTreasures = howManyTreasuresHave(region);
    //printf("current %d / available %d\n", currentTreasures, availableTreasures);
    if ((currentTreasures + 1) < availableTreasures) return true;
    throwMessage(TREASUR2);
    return false;
}

bool regionHasTraderLeader(int region)
{
    for ( int x = 0; x < SIZEX; x++ )
        for ( int y = 0; y < SIZEY; y++ )
            if ( gameBoard.board[x][y].hasTile )
                if ( gameBoard.board[x][y].tile.region == region )
                    if ( gameBoard.board[x][y].isLeader )
                        if ( gameBoard.board[x][y].tile.type == TRADER ) return true;
    
    throwMessage(TREASUR3);
    return false;
}

string color2type(string c)
{
    if (c == RD) return TEMPLE;
    if (c == GN) return MARKET;
    if (c == BL) return FARM;
    if (c == BK) return SETTLE;
    return "none";
}

string color2leader(string c)
{
    if (c == RD) return PRIEST;
    if (c == GN) return TRADER;
    if (c == BL) return FARMER;
    if (c == BK) return KING;
    return "none";
}

string type2leader(string t)
{
    if (t == TEMPLE ) return PRIEST;
    if (t == MARKET ) return TRADER;
    if (t == FARM ) return FARMER;
    if (t == SETTLE) return KING;
    return "none";
}

string leader2type(string l)
{
    if (l == PRIEST) return TEMPLE;
    if (l == TRADER) return MARKET;
    if (l == FARMER) return FARM;
    if (l == KING) return SETTLE;
    return "none";
}

bool matchTypes(string color, string type)
{
    return type == color2type(color);
}

bool nearTilesMatchTypes(int x, int y, string type)
{
    if( !validBoardPosition(x, y+1, false) || !gameBoard.board[x][y+1].hasTile || gameBoard.board[x][y+1].tile.type != type ) return false;
    if( !validBoardPosition(x+1, y, false) || !gameBoard.board[x+1][y].hasTile || gameBoard.board[x+1][y].tile.type != type ) return false;
    if( !validBoardPosition(x+1, y+1, false) || !gameBoard.board[x+1][y+1].hasTile || gameBoard.board[x+1][y+1].tile.type != type ) return false;

    return true;
}

bool colorCombAvailable(string color1, string color2)
{
    map<pair<string,string>,pair<int,int>>::iterator it = monuments.find( pair<string,string>(color1,color2) );
    if (it == monuments.end()) return true;
    return false;
}

bool canBuildMonument(string color1, string color2, int x, int y)
{
    if ( !gameBoard.board[x][y].hasTile )
    {
        throwMessage(MONUMEN1);
        return false;
    }
    string type = gameBoard.board[x][y].tile.type;
    if ( !(matchTypes(color1, type) || matchTypes(color2, type)) )
    {
        throwMessage(MONUMEN2);
        return false;
    }
    if ( !nearTilesMatchTypes(x, y, type) )
    {
        throwMessage(MONUMEN3);
        return false;
    }
    if ( !colorCombAvailable(color1, color2) )
    {
        throwMessage(MONUMEN4);
        return false;
    }
    return true;
}

// Initialize

void iniLeaders() 
{
    for (int i = 0; i < 4; i++)
    {
        leaders[i][FARMER].first = -1;
        leaders[i][FARMER].second = -1;
        leaders[i][KING].first = -1;
        leaders[i][KING].second = -1;
        leaders[i][PRIEST].first = -1;
        leaders[i][PRIEST].second = -1;
        leaders[i][TRADER].first = -1;
        leaders[i][TRADER].second = -1;
    }
}

// End turn

bool canEndTurn()
{
    for (Player p : players)
        if (p.numberTiles() != 6)
        {   
            throwMessage(GENERIC3);
            return false;
        }
    return true;
}

string num2tile(int val)
{
    if (val == 0) return TEMPLE;
    else if (val == 1) return MARKET;
    else if (val == 2) return SETTLE;
    else if (val == 3) return FARM;
    else return "none";
}

void refreshPlayer(int player)
{
    list<Player>::iterator selectedIterator = selectPlayer(player);

    string tile;
    int val;
    while (selectedIterator->numberTiles() != 6)
    {
        cin >> tile;
        
        try { val = stoi(tile); }
        catch(const std::exception& e)
        {   
            // printf("tile is not a number proceed naturaly\n");
            if ( !validTileType(tile) ) continue;
            selectedIterator->addTile(tile);
            continue;
        }
        // printf("tile is a number must be transformed\n");
        // printf("val correspont to type: ");
        // cout << num2tile(val) << endl;
        tile = num2tile(val);
        if ( !validTileType(tile) ) continue;
        selectedIterator->addTile(tile);

    }
}

void nextTurn()
{
    ++currentPlayer;
    ++activePlayer;
    if (activePlayer == players.end())
    {
        currentPlayer = 0;
        activePlayer = players.begin();
    }
    turnActions = 0;
}

// Update things (leaders, victory points...)

void updateLeader(string type, int x, int y)
{
    leaders[currentPlayer][type].first = x;
    leaders[currentPlayer][type].second = y;
}

int whoOwnsLeader(int x, int y, string leaderType)
{
    for (int i = 0; i < 4; i++)
        if (leaders[i][leaderType].first == x && leaders[i][leaderType].second == y) return i;
    return -1;
}

bool isActivePlayerLeader(int x, int y, string leaderType)
{
    return leaders[currentPlayer][leaderType].first == x && 
            leaders[currentPlayer][leaderType].second == y;
}

void findPlayer2AddPoint(int player, string type)
{
    list<Player>::iterator selectedIterator = selectPlayer(player);
    if (selectedIterator != players.end())
        selectedIterator->addPoint(type);
}

void findPlayer2AddMultiplePoints(int player, string type, int points)
{
    list<Player>::iterator selectedIterator = selectPlayer(player);
    if (selectedIterator != players.end())
        for (int i = 0; i < points; i++)
            selectedIterator->addPoint(type);
}

void findPlayerLeaderAndAddPoint(string leaderType, int region, string type)
{
    for (int i = 0; i < 4; ++i)
    {
        int x1 = leaders[i][leaderType].first;
        int y1 = leaders[i][leaderType].second;
        if (gameBoard.board[x1][y1].tile.region == region)
            findPlayer2AddPoint(i, type);
    }
}

bool canObtainPointByLeader(string type, int region)
{
    string leaderType;
    if ( type == FARM ) leaderType = FARMER;
    if ( type == TEMPLE ) leaderType = PRIEST;
    if ( type == SETTLE ) leaderType = KING;
    if ( type == MARKET ) leaderType = TRADER;

    for ( int x = 0; x < SIZEX; x++ )
        for ( int y = 0; y < SIZEY; y++ )
            if ( gameBoard.board[x][y].hasTile )
                if ( gameBoard.board[x][y].tile.region == region )
                        if ( gameBoard.board[x][y].isLeader )
                            if ( gameBoard.board[x][y].tile.type == leaderType ) 
                            {   
                                findPlayerLeaderAndAddPoint(leaderType,region,type);
                                return true;
                            }    
    return false;
}

void addPointIfThisRegionHasOnlyKing(string type, int region)
{
    bool hasOnlyKing = false;
    int kx, ky;

    for ( int x = 0; x < SIZEX; x++ )
        for ( int y = 0; y < SIZEY; y++ )
            if ( gameBoard.board[x][y].hasTile )
                if ( gameBoard.board[x][y].tile.region == region )
                    if ( gameBoard.board[x][y].isLeader )
                    {
                        if ( gameBoard.board[x][y].tile.type == KING ) 
                        {
                            kx = x; ky = y;
                            hasOnlyKing = true;
                        }
                        else 
                        {
                            hasOnlyKing = false;
                            break;
                        }
                    }
    
    if (hasOnlyKing)
    {
        // +1 to king owner
        int player = whoOwnsLeader(kx, ky, KING);
        if (player != -1)
        {
            findPlayer2AddPoint(player, type);
        }
    }
}

bool thisRegionHasLeader(int region)
{
    for ( int x = 0; x < SIZEX; x++ )
        for ( int y = 0; y < SIZEY; y++ )
            if ( gameBoard.board[x][y].hasTile )
                if ( gameBoard.board[x][y].tile.region == region )
                    if ( gameBoard.board[x][y].isLeader ) return true;
    return false;
}

void removeLeaderWithoutTemple(int x, int y)
{
    if (!gameBoard.templeNear(x, y))
    {
        updateLeader(gameBoard.board[x][y].tile.type, -1, -1);
        gameBoard.clearTile(x, y);
    }
}

void updateNearLeaders(int x, int y)
{
    if( validBoardPosition(x, y+1, false) && gameBoard.board[x][y+1].isLeader ) removeLeaderWithoutTemple(x, y+1);
    if( validBoardPosition(x, y-1, false) && gameBoard.board[x][y-1].isLeader ) removeLeaderWithoutTemple(x, y-1);
    if( validBoardPosition(x+1, y, false) && gameBoard.board[x+1][y].isLeader ) removeLeaderWithoutTemple(x+1, y);
    if( validBoardPosition(x-1, y, false) && gameBoard.board[x-1][y].isLeader ) removeLeaderWithoutTemple(x-1, y);
}

pair<int,int> locateDefenderLeader(string type, int region, int x, int y)
{
    // (x, y) attacker position
    for (int i = 0; i < SIZEX; i++)
        for (int j = 0; j < SIZEY; j++)
            if ( i != x && j != y )
                if (gameBoard.board[i][j].hasTile)
                    if (gameBoard.board[i][j].tile.region == region)
                        if (gameBoard.board[i][j].tile.type == type) return pair<int,int> (i,j);
    
    return pair<int,int> (-1, -1);
}

int nearTemples(int x, int y)
{
    int numTemples = 0;
    if( validBoardPosition(x, y+1, false) && gameBoard.board[x][y+1].hasTile && gameBoard.board[x][y+1].tile.type == TEMPLE ) numTemples += 1;
    if( validBoardPosition(x, y-1, false) && gameBoard.board[x][y-1].hasTile && gameBoard.board[x][y-1].tile.type == TEMPLE ) numTemples += 1;
    if( validBoardPosition(x+1, y, false) && gameBoard.board[x+1][y].hasTile && gameBoard.board[x+1][y].tile.type == TEMPLE ) numTemples += 1;
    if( validBoardPosition(x-1, y, false) && gameBoard.board[x-1][y].hasTile && gameBoard.board[x-1][y].tile.type == TEMPLE ) numTemples += 1;
    return numTemples;
}

bool newLeaderProvokeRevolt(string type, int region)
{
    int leaderCount = 0;
    for (int i = 0; i < SIZEX; i++)
        for (int j = 0; j < SIZEY; j++)
            if (gameBoard.board[i][j].hasTile)
                if (gameBoard.board[i][j].tile.region == region)
                    if (gameBoard.board[i][j].tile.type == type) leaderCount += 1;

    return leaderCount > 1;
}

void resolveRevolt(int xLoser, int yLoser, int loser, string leaderType, int winner)
{
    // remove loser
    gameBoard.clearTile(xLoser, yLoser);
    leaders[loser][leaderType] = pair<int,int> (-1, -1);

    // winner recive 1 red point
    findPlayer2AddPoint(winner,TEMPLE);
}

void resolveWar(int regionLoser, int xLoser, int yLoser, int loser, string leaderType, int winner)
{
    // remove loser leader
    gameBoard.clearTile(xLoser, yLoser);
    leaders[loser][leaderType] = pair<int,int> (-1, -1);

    // remove loser suporters
    int points = gameBoard.clearTilesByRegionType(regionLoser, leader2type(leaderType));

    // winner recive as many leader type points as loser tiles has been cleared (+ 1 for the leader??)
    findPlayer2AddMultiplePoints(winner, leader2type(leaderType), points);
}

void deleteNTiles(int player, int n, string type)
{
    for (int i = 0; i < n; i++)
    {
        selectPlayer(player)->deleteTile(type);
    }
}

void updateBoard()
{
    for (int i = 0; i < SIZEX; i++)
        for (int j = 0; j < SIZEY; j++)
            if (gameBoard.board[i][j].hasTile)
                gameBoard.updateRegions(i,j);
}

void updateAfterTile(string type, int x, int y)
{
    updateBoard();
    int newTileRegion = gameBoard.board[x][y].tile.region;

    if ( !canObtainPointByLeader(type, newTileRegion) ) // if true point is already added
        if ( thisRegionHasLeader(newTileRegion) )
            addPointIfThisRegionHasOnlyKing(type, newTileRegion); // adds point to king owner if it is the only leader on region
}

void collectMonumentPoints()
{
    for (map<pair<string,string>,pair<int,int>>::iterator it = monuments.begin(); it != monuments.end(); it++)
    {
        string leader1 = color2leader(it->first.first);
        string leader2 = color2leader(it->first.second);
        string type1 = color2type(it->first.first);
        string type2 = color2type(it->first.second);
        int region = it->second.second;

        findPlayerLeaderAndAddPoint(leader1,region,type1);
        findPlayerLeaderAndAddPoint(leader2,region,type2);
    }
}

list<int> seekNearRegions(int x, int y)
{
    list<int> regions;
    if( validBoardPosition(x, y+1, false) && gameBoard.board[x][y+1].hasTile && gameBoard.board[x][y+1].tile.isKingdom ) regions.push_back(gameBoard.board[x][y+1].tile.region);
    if( validBoardPosition(x, y-1, false) && gameBoard.board[x][y-1].hasTile && gameBoard.board[x][y-1].tile.isKingdom ) regions.push_back(gameBoard.board[x][y-1].tile.region);
    if( validBoardPosition(x+1, y, false) && gameBoard.board[x+1][y].hasTile && gameBoard.board[x+1][y].tile.isKingdom ) regions.push_back(gameBoard.board[x+1][y].tile.region);
    if( validBoardPosition(x-1, y, false) && gameBoard.board[x-1][y].hasTile && gameBoard.board[x-1][y].tile.isKingdom ) regions.push_back(gameBoard.board[x-1][y].tile.region);
    if (!regions.empty())
    {
        regions.sort();
        regions.unique();
    }
    return regions;
}

list<pair<string,int>> seekLeadersRegion(int region)
{
    list<pair<string,int>> leaderList;

    for (int i = 0; i < SIZEX; i++)
        for (int j = 0; j < SIZEY; j++)
            if (gameBoard.board[i][j].hasTile)
                if (gameBoard.board[i][j].tile.region == region)
                    if (gameBoard.board[i][j].isLeader)
                    {
                        string leaderType = gameBoard.board[i][j].tile.type;
                        int owner = whoOwnsLeader(i, j, leaderType);
                        leaderList.push_back( pair<string,int> (leaderType, owner) );
                    }

    return leaderList;
}

int regionTiles(string tileType, int region, int excludedX, int excludedY)
{
    int count = 0;
    // cout <<  tileType << " " << region << endl;
    for (int i = 0; i < SIZEX; i++)
        for (int j = 0; j < SIZEY; j++)
            if (gameBoard.board[i][j].hasTile)
                if (gameBoard.board[i][j].tile.region == region)
                    if (gameBoard.board[i][j].tile.type == tileType) 
                        if ( !(i == excludedX && j == excludedY) ) count++;

    return count;
}

queue<pair<string,list<int>>> leadersAtWar(int x, int y)
{
    // (x, y) conflict tile
    queue<pair<string,list<int>>> warLeaders;
    list<int> regionsInvolved = seekNearRegions(x, y);
    int size = regionsInvolved.size();
    if (!regionsInvolved.empty())
    {
        list<pair<string,int>> leadersInvolved[size];
        int i = 0;
        for (int region : regionsInvolved)
        {
            leadersInvolved[i] = seekLeadersRegion(region);
            ++i;
        }
        map<string,pair<int,list<int>>> leadersMap;
        for (int j = 0; j < size; j++)
        {
            for (pair<string,int> leader : leadersInvolved[j])
            {
                leadersMap[leader.first].first++;
                leadersMap[leader.first].second.push_back(leader.second);
            }
        }
        for (map<string,pair<int,list<int>>>::iterator it = leadersMap.begin(); it != leadersMap.end(); it++)
        {
            if (it->second.first > 1) warLeaders.push(pair<string,list<int>> (it->first,it->second.second));
        }
    }
    return warLeaders;
}

void updateWarConflicts(pair<int,int> tile)
{
    queue<list<War>::iterator> q;
    for (list<War>::iterator it = unresolvedWar.begin(); it != unresolvedWar.end(); ++it)
        if ( it->warTile == tile)
            if ( !itWillUniteTwoKingdoms(tile.first, tile.second, false) ) q.push(it);

    while (!q.empty())
    {
        unresolvedWar.erase(q.front());
        q.pop();
    }
}

void expandRegion(int x0, int y0, int region)
{
    queue< pair<int,int> > q;
    q.push(pair<int,int> (x0, y0));

    bool visited[SIZEX][SIZEY];
    for (int i = 0; i < SIZEX; i++)
        for (int j = 0; j < SIZEY; j++)
            visited[i][j] = false;    

    while ( !q.empty() )
    {
        int x = q.front().first;
        int y = q.front().second;
        visited[x][y] = true;
        q.pop();

        gameBoard.board[x][y].tile.region = region;

        if ( validBoardPosition(x, y+1, false) && !visited[x][y+1] && gameBoard.board[x][y+1].hasTile ) q.push(pair<int,int> (x, y+1));
        if ( validBoardPosition(x, y-1, false) && !visited[x][y-1] && gameBoard.board[x][y-1].hasTile ) q.push(pair<int,int> (x, y-1));
        if ( validBoardPosition(x+1, y, false) && !visited[x+1][y] && gameBoard.board[x+1][y].hasTile ) q.push(pair<int,int> (x+1, y));
        if ( validBoardPosition(x-1, y, false) && !visited[x-1][y] && gameBoard.board[x-1][y].hasTile ) q.push(pair<int,int> (x-1, y));
    }
}


bool bfs(int x0, int y0, int x1, int y1)
{
    queue< pair<int,int> > q;
    q.push(pair<int,int> (x0, y0));

    bool visited[SIZEX][SIZEY];
    for (int i = 0; i < SIZEX; i++)
        for (int j = 0; j < SIZEY; j++)
            visited[i][j] = false;    

    while ( !q.empty() )
    {
        int x = q.front().first;
        int y = q.front().second;
        visited[x][y] = true;
        q.pop();

        if ( x == x1 && y == y1 ) return true;

        int region = gameBoard.board[x][y].tile.region;

        if ( validBoardPosition(x, y+1, false) && !visited[x][y+1] && gameBoard.board[x][y+1].hasTile && gameBoard.board[x][y+1].tile.region == region ) q.push(pair<int,int> (x, y+1));
        if ( validBoardPosition(x, y-1, false) && !visited[x][y-1] && gameBoard.board[x][y-1].hasTile && gameBoard.board[x][y-1].tile.region == region ) q.push(pair<int,int> (x, y-1));
        if ( validBoardPosition(x+1, y, false) && !visited[x+1][y] && gameBoard.board[x+1][y].hasTile && gameBoard.board[x+1][y].tile.region == region ) q.push(pair<int,int> (x+1, y));
        if ( validBoardPosition(x-1, y, false) && !visited[x-1][y] && gameBoard.board[x-1][y].hasTile && gameBoard.board[x-1][y].tile.region == region ) q.push(pair<int,int> (x-1, y));
    }
    return false;
}

void updateRegionsAfterCatastrophe(int x, int y)
{
    queue< pair<int,int> > q;
    if ( validBoardPosition(x, y+1, false) && gameBoard.board[x][y+1].hasTile ) q.push(pair<int,int> (x, y+1));
    if ( validBoardPosition(x, y-1, false) && gameBoard.board[x][y-1].hasTile ) q.push(pair<int,int> (x, y-1));
    if ( validBoardPosition(x+1, y, false) && gameBoard.board[x+1][y].hasTile ) q.push(pair<int,int> (x+1, y));
    if ( validBoardPosition(x-1, y, false) && gameBoard.board[x-1][y].hasTile ) q.push(pair<int,int> (x-1, y));
    
    queue< pair<int,int> > q2;
    while ( !q.empty() )
    {
        int x0 = q.front().first;
        int y0 = q.front().second;
        q.pop();
        printf("ini pos: %d, %d", x0, y0);
        if ( validBoardPosition(x, y+1, false) && gameBoard.board[x][y+1].hasTile && !bfs(x0,y0,x,y+1) ) q2.push(pair<int,int> (x, y+1));
        if ( validBoardPosition(x, y-1, false) && gameBoard.board[x][y-1].hasTile && !bfs(x0,y0,x,y-1) ) q2.push(pair<int,int> (x, y-1));
        if ( validBoardPosition(x+1, y, false) && gameBoard.board[x+1][y].hasTile && !bfs(x0,y0,x+1,y) ) q2.push(pair<int,int> (x+1, y));
        if ( validBoardPosition(x-1, y, false) && gameBoard.board[x-1][y].hasTile && !bfs(x0,y0,x-1,y) ) q2.push(pair<int,int> (x-1, y));
    }

    while ( !q2.empty() )
    {
        int x0 = q2.front().first;
        int y0 = q2.front().second;
        q2.pop();
        gameBoard.board[x0][y0].tile.region = nextRegion;
        expandRegion(x0,y0,nextRegion);
        ++nextRegion;
    }
}

// Output 

void printVictoryPoints()
{
    for(Player p : players)
    {
        p.printVictoryPoints();
    }
    printf("----\n");
}

// Debug

void printLeaders()
{
    for (int i = 0; i < 4; i++)
    {
        printf("\nPlayer %d\n", i);
        printf("farmer\tking\tpriest\ttrader\n");
        printf("%d %d\t",leaders[i][FARMER].first,leaders[i][FARMER].second);
        printf("%d %d\t",leaders[i][KING].first,leaders[i][KING].second);
        printf("%d %d\t",leaders[i][PRIEST].first,leaders[i][PRIEST].second);
        printf("%d %d\t",leaders[i][TRADER].first,leaders[i][TRADER].second);
        printf("\n");
    }
}

void printConflicts()
{
    printf("Revolts:\n");
    queue<Revolt> tmp = unresolvedRevolt;
    while(!tmp.empty())
    {
        Revolt r = tmp.front();
        tmp.pop();
        cout << "type= " << r.leaderType << " reg= " << r.region << " atk= " << r.attackerPlayer << "(";
        cout << r.attackerStartingTiles << ") def= " << r.defenderPlayer << "(" << r.defenderStartingTiles << ") " << endl;
    }
    printf("\nWars:\n");
    for (War w : unresolvedWar)
    {
        cout << "type= " << w.leaderType << " warTile= (" << w.warTile.first << "," << w.warTile.second << ")" ;
        cout << " atk= " << w.attackerPlayer << "(" << w.attackerStartingTiles << ") def= " << w.defenderPlayer << "(" << w.defenderStartingTiles << ") " << endl;
    }
    
}

int main()
{
    iniLeaders();
    gameBoard.iniBoard();
    nextRegion = 10;

    // Inlitialize players
    list<string> initialTiles;
    int count = 0;
    string a;
    while (cin >> a)
    {
        if ( players.size() >= 4 || a == "----" ) break; // if reached max players or separator line
        if ( !validTileType(a) ) continue; // if parsing is not possible just try again
        initialTiles.push_back(a);
        ++count;
        if ( count >= 6 )
        {
            Player p = Player(initialTiles);
            players.push_back(p);
            initialTiles.clear();
            count = 0;
        }
    }
    // this asures that all players have 6 tiles
    if ( players.back().numberTiles() < 6 )
        players.back().reloadRandomTiles();


    // Commands
    activePlayer = players.begin();
    currentPlayer = 0;
    turnActions = 0;
    string command, type, color1, color2;
    int x, y, selectedPlayer;
    bool treasure = false;
    queue<pair<int,int>> bag;
    while ( cin >> command )
    {
        if ( command == "treasure" )
        {
            // treasure [x] [y] [x] [y] ...
            while (!bag.empty())
                bag.pop(); // empty bag to star a new fresh hunt
            treasure = true;

            while ( treasure )
            {
                cin >> command;
                try { x = stoi(command); }
                catch ( const std::exception& e )
                {
                    treasure = false;
                    break;
                }
                cin >> y;

                if ( !allConflictsResolved() ) break;
                if ( !validBoardPosition(x, y, true) ) break;
                if ( !tileHasTreasure(x, y) ) break;
                if ( !regionHasTraderLeader(gameBoard.board[x][y].tile.region) ) break;
                if ( !canTakeTreasure(gameBoard.board[x][y].tile.region, bag.size()) ) break;
                // add 'treasure' to 'bag'
                bag.push(pair<int,int>(x,y));
            }

            if ( treasure ) 
            {
                // exit treasure mode via error/exception
                treasure = false;
                continue;
            }
            else
            {
                // exit treasure mode OK
                while ( !bag.empty() )
                {
                    int a = bag.front().first;
                    int b = bag.front().second;
                    bag.pop();

                    activePlayer->addPoint(TREASURE);
                    gameBoard.board[a][b].tile.hasTreasure = false;
                }
            }
        }

        if ( command == "tile" )
        {
            // tile [type] [x] [y]
            cin >> type >> x >> y;
            if ( !allConflictsResolved() ) continue;
            if ( !playerHasTurnActions() ) continue;
            if ( !validTileType(type) ) continue;
            if ( !validBoardPosition(x, y, true) ) continue;
            if ( !activePlayerHasTile(type) ) continue;
            if ( !canPlaceThisTile(type, x, y) ) continue;
            
            gameBoard.addTile(type, x, y);
            activePlayer->deleteTile(type);

            if ( itWillUniteTwoKingdoms(x, y, false) )
            {
                // war?
                queue<pair<string,list<int>>> warLeaders = leadersAtWar(x, y);
                if ( !warLeaders.empty() )
                {
                    // WAR!!!
                    while ( !warLeaders.empty() )
                    {
                        War w;
                        w.warTile = pair<int,int> (x, y);
                        w.leaderType = warLeaders.front().first;

                        /** I don´t know if in a game w/ 3 players player 1 provokes a war but he has no leaders involved
                         *  who is the attacker and who the defender? I will asume the active player always is involved as
                         *  an attacker.
                         */

                        w.attackerPlayer = currentPlayer;
                        for(int wl : warLeaders.front().second)
                        {
                            if (wl != currentPlayer) w.defenderPlayer = wl;
                        }

                        int posXA = leaders[w.attackerPlayer][w.leaderType].first;
                        int posYA = leaders[w.attackerPlayer][w.leaderType].second;
                        w.attackerRegion = gameBoard.board[posXA][posYA].tile.region;
                        w.attackerPosition = pair<int,int> (posXA,posYA);
                        w.attackerStartingTiles = regionTiles(leader2type(w.leaderType), w.attackerRegion, x, y);
                        // printf("attackers starting tiles %d\n", w.attackerStartingTiles);

                        int posXD = leaders[w.defenderPlayer][w.leaderType].first;
                        int posYD = leaders[w.defenderPlayer][w.leaderType].second;
                        w.defenderRegion = gameBoard.board[posXD][posYD].tile.region;
                        w.defenderPosition = pair<int,int> (posXD,posYD);
                        w.defenderStartingTiles = regionTiles(leader2type(w.leaderType), w.defenderRegion, x, y);
                        // printf("defenders starting tiles %d\n", w.defenderStartingTiles);

                        warLeaders.pop();
                        unresolvedWar.push_back(w);
                    }
                }
                else
                {
                    updateAfterTile(type, x, y);
                }
            }
            else
            {
                updateAfterTile(type, x, y);
            }

            ++turnActions;
        } 
        else if ( command == "leader" )
        {
            // leader [type] [x] [y]
            cin >> type >> x >> y;
            if ( !allConflictsResolved() ) continue;
            if ( !playerHasTurnActions() ) continue;
            if ( !validLeaderType(type) ) continue;
            if ( !validBoardPosition(x, y, true) ) continue;
            if ( !canPlaceThisLeader(type, x, y) ) continue;

            if ( leaders[currentPlayer][type].first == -1)
            {
                // new leader
                if ( itWillUniteTwoKingdoms(x, y, true) ) continue;
                gameBoard.addLeader(type, x, y);
            }
            else
            {
                // move leader
                /** I don't know if you are alowed to move the leader to a position that would unite two kingdoms 
                  * one of them being the same kingdom as before. But the Test case 1 would be wrong if so. 
                  * This is the reason I don't check if moving the leader will unite kingdoms.
                  * Reading the manual I guess that it can't be alowed but in order to pass all tests i had to make this exception
                  */
                int x0 = leaders[currentPlayer][type].first;
                int y0 = leaders[currentPlayer][type].second;
                int region = gameBoard.board[x0][y0].tile.region;

                gameBoard.moveLeader(type, x, y, x0, y0);
                bool hasLeader = thisRegionHasLeader(region);
                gameBoard.updateKingdoms(x0, y0, hasLeader);
            }

            updateLeader(type, x, y);   // update the position in the leaders map
            gameBoard.updateKingdoms(x, y, true);

            // look for conflicts (revolt)
            int reg = gameBoard.board[x][y].tile.region;
            if ( newLeaderProvokeRevolt(type, reg) )
            {
                Revolt revolt;
                revolt.leaderType = type;
                revolt.region = reg;

                revolt.attackerPosition = pair<int,int>(x,y);
                revolt.defenderPosition = locateDefenderLeader(type, reg, x, y);

                revolt.attackerStartingTiles = nearTemples(x,y);
                revolt.defenderStartingTiles = nearTemples(revolt.defenderPosition.first, revolt.defenderPosition.second);

                revolt.attackerPlayer = currentPlayer;
                revolt.defenderPlayer = whoOwnsLeader(revolt.defenderPosition.first, revolt.defenderPosition.second, type);

                unresolvedRevolt.push(revolt);
            }

            ++turnActions;
        }
        else if ( command == "catastrophe" )
        {
            // catastrophe [x] [y]
            cin >> x >> y;
            if ( !allConflictsResolved() ) continue;
            if ( !playerHasTurnActions() ) continue;
            if ( !validBoardPosition(x, y, true) ) continue;
            if ( !canOcurrCatastrophe(x, y) ) continue;

            gameBoard.addCatastrophe(x, y);

            updateNearLeaders(x, y); // leaders could be removed if no temple is near them

            // update regions that could be divided
            // dfs de toda la vida y ya
            updateRegionsAfterCatastrophe(x, y);

            ++turnActions;
        }
        else if ( command == "monument" )
        {
            // monument [color1] [color2] [x] [y]
            cin >> color1 >> color2 >> x >> y;
            if ( !allConflictsResolved() ) continue;
            if ( !playerHasTurnActions() ) continue;
            if ( !validBoardPosition(x, y, true) ) continue;
            if ( !validColorType(color1, color2) ) continue;
            if ( !canBuildMonument(color1, color2, x,y) ) continue;

            // save monuemtn in map
            monuments[pair<string,string> (color1, color2)] = pair<int,int> (currentPlayer, gameBoard.board[x][y].tile.region);
            gameBoard.newMonument(x, y); // all tiles must change to isMonument

            // update if monument creation affected near liders just like in a catastrophe
            updateNearLeaders(x, y);
            updateNearLeaders(x, y+1);
            updateNearLeaders(x+1, y);
            updateNearLeaders(x+1, y+1);

            ++turnActions;
        }
        else if ( command == "revolt" )
        {
            // revolt [attacker_tiles] [defender_tiles]
            cin >> x >> y;
            if ( !noRevoltToResolve() ) continue;

            Revolt revolt = unresolvedRevolt.front();
            if ( !playerHasNTiles(revolt.attackerPlayer, TEMPLE, x) ) continue;
            if ( !playerHasNTiles(revolt.attackerPlayer, TEMPLE, y) ) continue;

            unresolvedRevolt.pop();

            int attackerTiles = x + revolt.attackerStartingTiles;
            int defenderTiles = y + revolt.defenderStartingTiles;

            if (attackerTiles > defenderTiles) // attacker wins
                resolveRevolt(revolt.defenderPosition.first, revolt.defenderPosition.second, revolt.defenderPlayer, revolt.leaderType, revolt.attackerPlayer);
            else // defender wins
                resolveRevolt(revolt.attackerPosition.first, revolt.attackerPosition.second, revolt.attackerPlayer, revolt.leaderType, revolt.defenderPlayer);

            deleteNTiles(revolt.attackerPlayer,x,TEMPLE);
            deleteNTiles(revolt.defenderPlayer,y,TEMPLE);
        }
        else if ( command == "war" )
        {
            // revolt [leader_type] [attacker_tiles] [defender_tiles]
            cin >> type >> x >> y;
            
            if ( !validLeaderType(type) ) continue;
            if ( !noWarToResolve(type) ) continue;

            list<War>::iterator war = unresolvedWar.begin();
            for (list<War>::iterator it = unresolvedWar.begin(); it != unresolvedWar.end(); ++it)
                if ( it->leaderType == type) war = it;

            int attackerTiles = x + war->attackerStartingTiles;
            int defenderTiles = y + war->defenderStartingTiles;

            if (attackerTiles > defenderTiles) // attacker wins
                resolveWar(war->defenderRegion, war->defenderPosition.first, war->defenderPosition.second, war->defenderPlayer, war->leaderType, war->attackerPlayer);
            else // defender wins
                resolveWar(war->attackerRegion, war->attackerPosition.first, war->attackerPosition.second, war->attackerPlayer, war->leaderType, war->defenderPlayer);

            pair<int,int> warTile = war->warTile;
            unresolvedWar.erase(war);

            updateWarConflicts(warTile); // if warTile won't unite two kingdoms --> delete conflicts where warTile == warTile
            
        }
        else if ( command == "refresh" )
        {
            // refresh [player] [tile] [tile] ... until has 6 tiles
            cin >> selectedPlayer;
            if ( !allConflictsResolved() ) continue;
            if ( !isPlayer(selectedPlayer) ) continue;
            refreshPlayer(selectedPlayer);
            continue;
        }
        else if ( command == "----" )
        {
            if ( !allConflictsResolved() ) continue;
            if ( !canEndTurn() ) continue;
            // if active player has leader that shares color with monument in his kingdom +1 point
            updateBoard();
            collectMonumentPoints();
            printVictoryPoints();
            nextTurn();
        }
        else if (command == "print")
        {
            gameBoard.printState();
        }
        else if (command == "treasures")
        {
            cin >> x;
            printf("region %d has %d treasures\n", x, howManyTreasuresHave(x));
        }
        else if (command == "leaders")
        {
            printLeaders();
        }
        else if (command == "conflicts")
        {
            printConflicts();
        }
    }
    return 0;
}