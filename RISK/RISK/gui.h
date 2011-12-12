#ifndef GUI_H
#define GUI_H

#include <string>

//GUI declarations
#include"SDL.h"
#include"SDL_image.h"
#include"SDL_ttf.h"

//Engine declarations
#include "engine.h"

class ActiveTerritory;
class Button;

using namespace std;

class GUI{

public:
	GUI();
	GUI(string);
	~GUI();
	bool observeEvent();
	bool quitGame();
	string getCommand();
	void displayGameState();
	void displayGameState(int*, int*, int, int, int, string, string, bool);

private:

	// 
	// PRIVATE METHODS
	//
	//Book-keeping functions
	void initGUI(string);
	SDL_Surface* loadImage(string, int);
	void loadImages();
	void loadTerritories(string);
	void cleanUp();

	//Define in-game functions
	SDL_Surface* renderPositionMessage(int, int, int, int, int);
	void applySurface( int, int, SDL_Surface*, SDL_Surface*, SDL_Rect* = NULL);

	// 
	// PRIVATE MEMBERS
	//

	bool quit;

	//Screen attributes
	int SCREEN_WIDTH;
	int SCREEN_HEIGHT;
	int SCREEN_BPP;

	//Dashboard Attributes
	int CAPTION_X;
	int CAPTION_Y;

	//Button Attributes
	int BUTTON_TEXT_OFFSET_X;
	int BUTTON_TEXT_OFFSET_Y;
	int BUTTON_W;
	int BUTTON_H;
	int FORTIFY_BUTTON_X;
	int FORTIFY_BUTTON_Y;
	int DONE_BUTTON_X;
	int DONE_BUTTON_Y;

	//Button states in the sprite sheet
	int CLIP_MOUSEOVER;
	int CLIP_MOUSEOUT;
	int CLIP_MOUSEDOWN;
	int CLIP_MOUSEUP;

	SDL_Surface* gridMessage;    //message that goes above the icon

	//The surfaces that will be used for images
	SDL_Surface** iconList;
	SDL_Surface* iconPlayer1;
	SDL_Surface* iconPlayer2;
	SDL_Surface* iconPlayer3;
	SDL_Surface* iconPlayer4;
	SDL_Surface* iconPlayer5;
	SDL_Surface* iconPlayer6;
	SDL_Surface* buttonSheet;
	SDL_Surface* background;
	SDL_Surface* screen;
	SDL_Surface* newScreen;

	//The event structure that will be used
	SDL_Event event;

	//The surfaces for the dashboard
	SDL_Surface* terrMessage;
	SDL_Surface* icon;
	SDL_Surface* phase;
	SDL_Surface* target;
	SDL_Surface* base;	
	SDL_Surface* vbanner;

	//The font that's going to be used
	TTF_Font *fontSmall;
	TTF_Font *fontNormal;
	TTF_Font *fontHuge;

	//The clip regions of the sprite sheet
	SDL_Rect clips[ 4 ];

	//Stores Active Territories
	int Nterritories;
	ActiveTerritory** territories;

	//Stores Buttons
	Button* fortify;
	Button* done;

	

};


//Define territory
class ActiveTerritory{

private:
    
	//Territory Name
	string terrName;

	//Territory Display
	SDL_Surface* terrMessage;
	SDL_Color terrMessageColor;
	TTF_Font* terrMessageFont;

	//The attributes of the button
    SDL_Rect box;

	//Display properties of the button
	string command;
	bool commandIssued;
	bool mouseOver;
	//an array of images that are the different colors of the territories
	
	int locationX;
	int locationY;
	SDL_Surface** territoryColor;
	
	

public:

    //Initialize the variables
    ActiveTerritory(string, TTF_Font*, int, int, int, int, int, int);
    
	//Interaction methods
	void handleEvents(SDL_Event);
	bool isCommandIssued();
	string getCommand();
	
	bool show();
	void cleanMsg();

	int getX();
	int getY();
	int getW();
	int getH();
	int getLocationX();
	int getLocationY();
	void setImage(SDL_Surface*, int);
	string getName();
	SDL_Surface* getColor(int);
	SDL_Surface* getSurface();
	void clearSurface();
	
};


//Define territory
class Button{

private:
    
	//Territory Name
	string buttonName;

	//Territory Display
	SDL_Surface* buttonMessage;
	SDL_Color buttonMessageColor;
	TTF_Font* buttonMessageFont;

	//The attributes of the button
    SDL_Rect box;

	//Display properties of the button
	string command;
	bool commandIssued;
	bool mouseOver;
	int phaseid;
	int clip;

	//Button states in the sprite sheet
	int CLIP_MOUSEOVER;
	int CLIP_MOUSEOUT;
	int CLIP_MOUSEDOWN;
	int CLIP_MOUSEUP;

public:

    //Initialize the variables
    Button(string, TTF_Font*, int, int, int, int);
    
	//Interaction methods
	void handleEvents(SDL_Event);
	bool isCommandIssued();
	string getCommand();
	
	bool show();
	void cleanMsg();

	int getX();
	int getY();
	int getW();
	int getH();
	string getName();
	SDL_Surface* getSurface();
	int getClip();
	void clearSurface();
	
};





#endif
