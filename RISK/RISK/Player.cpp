#include <string>

#include "Player.h"

using namespace std;

Player::Player(){

}

void Player::setGUI(GUI * g){
	gui = g;
	
}

int Player::pickCountry(){
	return board->searchTerritoryName(getCommand()).getTerritoryCode();
}

void Player::placeInitialArmies(){
	board->placeArmies(1,board->searchTerritoryName(getCommand()));
}
void Player::cardsPhase() {
		
}

void Player::placeArmies(int){
		
}

void Player::attackPhase(){
		
}

int Player::moveArmiesIn(int, int){
	//board->playHumanTurn();	
	return 0;
}

void Player::fortifyPhase(){
		
}

string Player::getCommand(){
	string command("");
	while (command == ""){
		if(board->gui->observeEvent()){

			//GUI transmits quit event
			if(board->gui->quitGame()){

				//board->quit=true;

			//Otherwise, GUI transmits commands
			}else{

				/*
				 * Receive Command from GUI to Modify the Game State
				 */
				command = (board->gui->getCommand());

				//board->updateGUI();
			}
		}
	}
	return command;
}

