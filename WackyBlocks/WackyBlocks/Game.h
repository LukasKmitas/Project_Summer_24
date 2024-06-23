#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>

#include "MainMenu.h"
#include "LevelEditor.h"
#include "Options.h"

enum class GameState
{
	MAIN_MENU,
	LEVEL_EDITOR,
	OPTIONS
};

class Game
{
public:
	Game();
	~Game();
	
	void run();

private:

	GameState m_currentState;
	MainMenu m_mainMenu;
	LevelEditor m_levelEditor;
	Options m_options;

	void processEvents();
	void processKeys(sf::Event t_event);
	void update(sf::Time t_deltaTime);
	void render();

	sf::RenderWindow m_window; 

	sf::Font m_ArialBlackfont;

	bool m_exitGame; 

};

#endif 

