#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <thread>
#include <memory>
#include <iostream>
#include "Global.h"
#include "Block.h"
#include "GameStates.h"
#include "MainMenu.h"
#include "LevelEditor.h"
#include "Options.h"
#include "Player.h"

const unsigned short PORT = 53000;
const sf::IpAddress SERVER_IP = "127.0.0.1"; // Server IP address

class Game
{
public:
	Game();
	~Game();
	
	void run();

private:

	GameState m_currentState;
	GameMode m_currentMode;
	MainMenu m_mainMenu;
	LevelEditor m_levelEditor;
	Options m_options;
	Player m_player;

	void processEvents();
	void processKeys(sf::Event t_event);
	void update(sf::Time t_deltaTime);
	void render();

	void loadLevel(const std::string& m_fileName);

	std::vector<Block> m_gameBlocks;

	sf::RenderWindow m_window; 
	sf::Font m_ArialBlackfont;

	// Multiplayer
	std::unique_ptr<sf::TcpSocket> m_socket;
	std::thread m_serverThread;
	std::thread m_clientThread;
	bool m_isHosting = false;
	bool m_isClient = false;
	bool m_gameStarted = false;

	void startHostSession();
	void searchHosts();

	bool m_exitGame; 

};

#endif 

