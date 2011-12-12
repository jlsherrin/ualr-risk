#include <vector>
#include <cstdlib>
#include "Territory.h"

using namespace std;

Territory::Territory(){

}

Territory::Territory(string _name){
	code = -1;
	name = _name;
	continent = "undefined";
	owner = -1;
	numUnits = 0;
	numMoveableUnits = 0;
}

int Territory::getTerritoryCode(){
	return(code);
}

string Territory::getContinent(){
	return(continent);
}

string Territory::getName(){
	return(name);
}

int Territory::getOwner(){
	return(owner);
}

int Territory::getNumUnits(){
	return(numUnits);
}

int Territory::getNumMoveableUnits(){
	return(numMoveableUnits);
}

bool Territory::isOccupied(){
	bool occupied = true;
	if(owner == -1){
		occupied = false;
	}
	return(occupied);
}

void Territory::setTerritoryCode(int n){
	code = n;
}

void Territory::setContinent(string _continent){
	continent = _continent;
}

void Territory::setOwner(int _owner){
	owner = _owner;
}

void Territory::addNeighbor(Territory _neighbor){
	neighbors.push_back(_neighbor);
}

void Territory::addUnit(){
	numUnits++;
}

void Territory::deleteUnit(){
	if(numUnits>0){
		numUnits--;
	}
}

void Territory::setNumMoveableUnits(int n){
	numMoveableUnits = n;
}

int Territory::getNumNeighbors(){
	return(neighbors.size());
}

string Territory::getNeighbor(int nbr){
	return(neighbors[nbr].getName());
}


bool Territory::isNeighborOf(string nbr){
	
	bool result = false;

	for(unsigned i=0;i<neighbors.size();i++){
		if(neighbors[i].getName() == nbr){
			result = true;
		}
	}

	return(result);

}

//vector<int> Territory::attack(int maxDice){
//	
//	return(volley(min(maxDice,numUnits-1)));
//
//}
//
//vector<int> Territory::defend(int maxDice){
//
//	return(volley(min(maxDice,numUnits)));
//
//}
//
//vector<int> Territory::volley(int nVolleys){
//
//	vector<int> volleys;
//	for(int i=0;i<nVolleys;i++){
//		volleys.push_back(shot());
//	}
//	return(volleys);
//
//}
//
//int Territory::shot(){
//	return(6*(double)rand()/RAND_MAX+1);
//}

Territory Territory::getWeakestEnemyNeighbor(){
	/** Returns a reference to the weakest neighbor that is owned by another player, 
	 *	or null if there are no enemy neighbors.*/
	
	vector<Territory> enemies;
	Territory weakest(neighbors[0]);

	for ( unsigned i = 1; i < neighbors.size();i++){
		if (neighbors[i].getOwner() != getOwner() && neighbors[i].getNumUnits() < weakest.getNumUnits()){
			weakest = neighbors[i];
		}
	}
	return weakest;
}
Territory Territory::getWeakestEnemyNeighborInContinent( string cont ){
	/** Operates like getWeakestEnemyNeighbor but limits its search to the given continent. 
		returns the weakest enemy country in specified continent
		returns null if there are no matches*/
	
	vector<Territory> enemies;
	Territory weakest(neighbors[0]);
	for ( unsigned i = 1; i < neighbors.size();i++){
		if (neighbors[i].getOwner() != getOwner() 
			&& neighbors[i].getContinent() == getContinent()
			&& neighbors[i].getNumUnits() < weakest.getNumUnits()){
			weakest = neighbors[i];
		}
	}
	return weakest;
}

int Territory::getNumberEnemyNeighbors(){
	/** Returns the number of neighbor countries owned by players that don't own this Country. */
	int numEnemies = 0;

	for (unsigned i=0; i <neighbors.size();i++){
		if (neighbors[i].getOwner() != getOwner()){
			numEnemies++;
		}
	}
	return numEnemies;
}

int Territory::getNumberPlayerNeighbors(int player){
	/** Returns the number of adjacent countries owned by <i>player</i>.*/
	int numPlayer = 0;

	for (unsigned i=0; i <neighbors.size();i++){
		if (neighbors[i].getOwner() == player){
			numPlayer++;
		}
	}
	return numPlayer;
}
	
vector<int> Territory::getFriendlyNeighborList(){
	/** Return an int array containing the country codes of all the neigbors 
		of this country that are owned by the same player. 
		Useful to determine where this country can fortify to. */
	vector<int> friendlies;	
	for (unsigned i=0; i <neighbors.size();i++){
		if (neighbors[i].getOwner() == getOwner()){
			friendlies.push_back(neighbors[i].getTerritoryCode());
		}
	}
	return friendlies;
}

