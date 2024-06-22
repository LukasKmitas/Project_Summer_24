#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>

#include "MainMenu.h"
#include "Options.h"

enum class GameState
{
	MAIN_MENU,
	OPTIONS
};

class Game
{
public:
	Game();
	~Game();
	
	void run();

private:

	MainMenu m_mainMenu; 
	Options m_options;
	GameState m_currentState;

	void processEvents();
	void processKeys(sf::Event t_event);
	void update(sf::Time t_deltaTime);
	void render();

	sf::RenderWindow m_window; 

	sf::Font m_ArialBlackfont;

	bool m_exitGame; 

};

#endif 

