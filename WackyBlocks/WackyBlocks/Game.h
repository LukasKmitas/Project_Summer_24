#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <thread>
#include <memory>
#include <iostream>
#include <unordered_map>
#include "GameStates.h"
#include "ServerData.h"
#include "MainMenu.h"
#include "LevelEditor.h"
#include "Options.h"
#include "LightHelper.h"
#include "Player.h"
#include "Enemy.h"
#include "EvilEye.h"

struct ShopItem
{
	sf::RectangleShape button;
	sf::Text text;
	int cost = 0;
	bool purchased = false;
};

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
	Player m_otherPlayer;

	std::vector<Block> m_gameBlocks;
	std::vector<std::unique_ptr<Enemy>> m_enemies;

	void processEvents();
	void processKeys(sf::Event t_event);
	void update(sf::Time t_deltaTime);
	void render();

	void loadBackground();
	void loadLevel(const std::string& m_fileName);
	void createLightMap();
	void initLevelAssets();
	void initShop();
	void updateCurrencyText();
	void updatePortalAnimation();
	void updateShopAnimation();
	void updateShopTextAnimation();
	void renderLevelAssets();

	// Server stuff
	void initNetwork();
	void listenForServerMessages();
	void startHostSessions();
	void searchForGames();
	void handleServerSessionResponse();
	void joinSession(const std::string& m_hostID);
	void sendLevelToServer();
	void sendPlayerUpdate();

	std::vector<std::string> split(const std::string& m_string, char m_delimiter);

	sf::RenderWindow m_window;
	sf::Font m_font;
	sf::Texture m_backgroundTexture;
	sf::Sprite m_backgroundSprite;

	// Shop stuff
	sf::Text m_shopText;
	sf::Text m_currencyText;
	sf::Text m_shopTitleText;
	std::vector<ShopItem> m_shopItems;
	sf::RectangleShape m_shopUIBackground;
	sf::RectangleShape m_scrollBar;
	sf::CircleShape m_scrollBarHandle;
	int m_currency = 1000;
	float m_scrollOffset = 0.0f;
	float m_scrollMaxOffset = 0.0f;
	bool m_shopOpen = false;
	bool m_showShopText = false;

	// Animation stuff
	// Portal
	sf::Texture m_portalTexture;
	sf::Sprite m_portalSprite;
	sf::IntRect m_portalFrameRect;
	sf::Clock m_portalClock;
	int m_portalCurrentFrame = 0;
	// Shop
	sf::Texture m_shopTexture;
	sf::Sprite m_shopSprite;
	sf::IntRect m_shopFrameRect;
	sf::Clock m_shopClock;
	sf::Clock m_shopTextClock;
	int m_shopCurrentFrame = 0;
	float m_shopTextYOffset = 0.0f;
	float m_shopTextYDirection = 1.0f;

	// Light stuff
	sf::RenderTexture m_lightMapTexture;
	sf::Sprite m_lightMapSprite;
	std::vector<Edge> m_edges;
	sf::Vector2f m_lightSource;
	float m_lightRadius = 600.0f;

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
