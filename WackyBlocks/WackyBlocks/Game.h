#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <thread>
#include <memory>
#include <iostream>
#include <unordered_map>
#include "Global.h"
#include "Block.h"
#include "GameStates.h"
#include "ServerData.h"
#include "MainMenu.h"
#include "LevelEditor.h"
#include "Options.h"
#include "Player.h"

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

	std::vector<Block> m_gameBlocks;

	void processEvents();
	void processKeys(sf::Event t_event);
	void update(sf::Time t_deltaTime);
	void render();

	void loadLevel(const std::string& m_fileName);

	void initNetwork();
	void listenForServerMessages();
	void startHostPreparations();
	void searchForGames();
	void handleServerSessionResponse();
	void joinSession(const std::string& m_hostID);
	void sendLevelToServer();

	std::vector<std::string> split(const std::string& m_string, char m_delimiter);

	sf::RenderWindow m_window; 
	sf::Font m_ArialBlackfont;

	// Multiplayer
	std::unique_ptr<sf::TcpSocket> m_socket;
	std::thread m_serverThread;
	std::thread m_clientThread;
	std::thread m_listenThread;
	bool m_isHosting = false;
	bool m_isClient = false;
	bool m_gameStarted = false;
	bool m_isListening;

	bool m_exitGame; 

};

#endif 

