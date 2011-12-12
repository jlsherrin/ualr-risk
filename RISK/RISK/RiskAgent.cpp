#include <vector>

#include "RiskAgent.h"
#include "Card.h"
#include "Territory.h"

using namespace std;

//Cards Methods
void RiskAgent::cashCardsIfPossible(vector<Card> cards){
	Card hand(0,0);
	if (cards.size() > 0) {
		if (hand.containsASet(cards)){
			vector<Card> set = hand.getBestSet( cards, ID, board->gameBoard );
			board->cashCards(set[0], set[1], set[2]);
		}
	}
}

void RiskAgent::setPrefs(int i, Board * b){
	this->ID = i;
	this->board = b;
}

//Place Methods
void RiskAgent::setGoalToLeastBordersCont(){

}
int RiskAgent::pickCountryInSmallContinent(){
	return 0;
}
int RiskAgent::pickCountryInContinent(int continent){
	return 0;
}
int RiskAgent::pickCountryTouchingUs(){
	return 0;
}

int RiskAgent::pickCountryNearCluster( Territory root) {
	return 0;
}

int RiskAgent::getEasiestContToTake(){
	return 0;
}

bool RiskAgent::weOwnContsArround(Territory center){
	return 0;
}

void RiskAgent::placeArmiesToTakeCont( int numberOfArmies, int wantCont ){

}

void RiskAgent::placeArmiesOnClusterBorder( int numberOfArmies, Territory root ){

}

//Attack Methods

//Fortify Method
void RiskAgent::fortifyCluster( Territory root ){

}
	
bool RiskAgent::ownNeighbor(string){
	return 0;
}
