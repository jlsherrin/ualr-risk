#ifndef TERRITORY_H
#define TERRITORY_H

#include <string>
#include <vector>

using namespace std;

class Territory{
	/** A Territory instance represents a single territory in the game.

		Each Territory stores a Territory-code, continent-code, owner-code, 
		number of armies, and number of fortifyable armies.  As well, each 
		Territory stores an array containing references to all adjoining Territory's.

		When first initialized by the game world, each Territory will be given 
		a permanent Territory-code, continent-code and adjoining-list. The 
		owner-code and number of armies are set to -1.

		The Territory-code is a unique number used to identify countries. 
		The array returned by the Board.getTerritorries() will always be ordered
		by Territory-code.
	*/

public:
	/*	Constructor(s)	*/
	Territory();
	Territory(string);
	
	/*	Accesors  */
	int getTerritoryCode();
	string getName();
	string getContinent();
	int getOwner();
	int getNumUnits();
	int getNumMoveableUnits();
	int getNumNeighbors();
	string getNeighbor(int);


	/*	Manipulators  */
	void setTerritoryCode(int);
	void setContinent(string);
	void setOwner(int);
	void addUnit();
	void deleteUnit();
	void addNeighbor(Territory);
	void setNumMoveableUnits(int);
	
	/*	Queries  */
	bool isOccupied();
	bool isNeighborOf(string);
	Territory getWeakestEnemyNeighbor(); 
	Territory getWeakestEnemyNeighborInContinent( string cont ); 
	int getNumberEnemyNeighbors(); 
	int getNumberPlayerNeighbors(int player);
	vector<int> getFriendlyNeighborList();
	

	/*	Combat Methods */
	//vector<int> attack(int);
	//vector<int> defend(int);	
	

private:
	//Members
	int code;				// TODO NEW (add method(s) and other stuff)
	string name;
	string continent;
	int owner;
	int numUnits;
	int numMoveableUnits;		// TODO NEW (add method(s) and other stuff)
	vector<Territory> neighbors;	

	//Methods
	//vector<int> volley(int);	// TODO move to engine?
	//int shot();					// TODO move to engine?

};

#endif
