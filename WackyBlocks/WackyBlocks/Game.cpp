#include "Game.h"

Game::Game() :
	m_window{ sf::VideoMode{ SCREEN_WIDTH, SCREEN_HEIGHT, 32U }, "WackyBlocks" },
	m_exitGame{false},
	m_currentState{ GameState::MAIN_MENU },
	m_currentMode{ GameMode::NONE },
	m_socket{ std::make_unique<sf::TcpSocket>() }
{
	if (!m_font.loadFromFile("Assets\\Fonts\\ariblk.ttf"))
	{
		std::cout << "Error loading font" << std::endl;
	}
	initNetwork();
	m_otherPlayer.setOtherPlayerColor();
	if (!m_lightMapTexture.create(SCREEN_WIDTH, SCREEN_HEIGHT))
	{
		std::cout << "Error creating light map texture" << std::endl;
	}
	loadBackground();
	initLevelAssets();
	initShop();
	m_lightMapSprite.setTexture(m_lightMapTexture.getTexture());
}

Game::~Game()
{
}

void Game::run()
{	
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	const float fps{ 60.0f };
	sf::Time timePerFrame = sf::seconds(1.0f / fps); // 60 fps
	while (m_window.isOpen())
	{
		processEvents();
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			if (m_window.hasFocus())
			{
				processEvents();
				update(timePerFrame);
			}
		}
		if (m_window.hasFocus())
		{
			render();
		}
	}
}

void Game::processEvents()
{
	sf::Event newEvent;
	while (m_window.pollEvent(newEvent))
	{
		if ( sf::Event::Closed == newEvent.type) 
		{
			m_exitGame = true;
		}
		if (sf::Event::KeyPressed == newEvent.type)
		{
			processKeys(newEvent);
		}
		if (sf::Event::MouseMoved == newEvent.type)
		{
			sf::Vector2f mousePos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));

			if (m_currentState == GameState::MAIN_MENU)
			{
				m_mainMenu.handleMouseHover(mousePos);
			}
			else if (m_currentState == GameState::LEVEL_EDITOR)
			{
				m_levelEditor.handleMouseHover(mousePos);
			}
			else if (m_currentState == GameState::OPTIONS)
			{
				m_options.handleMouseHover(mousePos);
			}
			else if (m_currentState == GameState::GAMEPLAY)
			{
				if (m_shopOpen)
				{
					if (m_isDraggingScrollBarHandle)
					{
						float newScrollHandleY = mousePos.y - m_scrollBarHandleOffsetY;
						newScrollHandleY = std::max(m_scrollBar.getPosition().y, std::min(newScrollHandleY, m_scrollBar.getPosition().y + m_scrollBar.getSize().y - 20));
						m_scrollBarHandle.setPosition(m_scrollBar.getPosition().x + 10, newScrollHandleY);

						float scrollRatio = (newScrollHandleY - m_scrollBar.getPosition().y) / (m_scrollBar.getSize().y - 20);
						m_scrollOffset = scrollRatio * m_scrollMaxOffset;

						for (int i = 0; i < m_shopItems.size(); ++i)
						{
							float newY = SCREEN_HEIGHT / 2 - 100 + i * 60 - m_scrollOffset;
							m_shopItems[i].button.setPosition(SCREEN_WIDTH / 2 - 150, newY);
							m_shopItems[i].text.setPosition(m_shopItems[i].button.getPosition().x + 10, m_shopItems[i].button.getPosition().y + 10);
						}
					}
					else
					{
						bool itemHovered = false;
						for (auto& item : m_shopItems)
						{
							if (item.button.getGlobalBounds().contains(mousePos) &&
								item.button.getPosition().y > m_shopUIBackground.getPosition().y - m_shopUIBackground.getSize().y / 2 + 45 &&
								item.button.getPosition().y < m_shopUIBackground.getPosition().y + m_shopUIBackground.getSize().y / 2 - 40)
							{
								itemHovered = true;
								if (!item.purchased)
								{
									item.button.setFillColor(sf::Color(150, 150, 150, 255));
								}
								m_shopItemDescriptionText.setString(item.description);
							}
							else
							{
								if (!item.purchased)
								{
									item.button.setFillColor(sf::Color(100, 100, 100, 200));
								}
							}
						}
						if (!itemHovered)
						{
							m_shopItemDescriptionText.setString("");
						}
					}
				}
			}
		}
		if (sf::Event::MouseButtonPressed == newEvent.type)
		{
			sf::Vector2f mousePos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
			if (newEvent.mouseButton.button == sf::Mouse::Left)
			{
				if (m_currentState == GameState::MAIN_MENU)
				{
					int clickedItem = m_mainMenu.handleClick(mousePos);
					if (clickedItem != -1)
					{
						SoundManager::getInstance().playSound("buttonClick");
						switch (clickedItem)
						{
						case 0:
							m_mainMenu.showSubmenu();
							break;
						case 1:
							// Level Editor
							m_currentState = GameState::LEVEL_EDITOR;
							m_mainMenu.clearParticles();
							break;
						case 2:
							m_currentState = GameState::OPTIONS;
							m_mainMenu.clearParticles();
							break;
						case 3:
							m_exitGame = true;
							break;
						case 4:
							// Single Player
							m_mainMenu.showLevelSelection();
							m_currentMode = GameMode::SINGLE_PLAYER;
							break;
						case 5:
							// Multiplayer
							m_mainMenu.showMultiplayerOptions();
							m_currentMode = GameMode::MULTIPLAYER;
							break;
						case 6:
							// Continue to Single Player
							loadLevel(m_mainMenu.getSelectedLevelFile());
							m_currentState = GameState::GAMEPLAY;
							m_mainMenu.clearParticles();
							break;
						case 7:
							// Host Game
							m_mainMenu.showHostGameScreen();
							startHostSessions();
							break;
						case 8:
							// Join Game
							m_mainMenu.showJoinGameScreen();
							searchForGames();
							break;
						case 9:
							// Start Game after Host
							sendLevelToServer();
							m_currentState = GameState::GAMEPLAY;
							break;
						case 10:
							// Refresh join game sessions
							searchForGames();
							break;
						case 11:
							// Join that session
							joinSession(m_mainMenu.getSelectedSession());
							break;
						default:
							break;
						}
					}
				}
				else if (m_currentState == GameState::LEVEL_EDITOR)
				{
					m_levelEditor.handleMouseClick(mousePos);
				}
				else if (m_currentState == GameState::OPTIONS)
				{
					m_options.handleMouseClick(mousePos);
				}
				else if (m_currentState == GameState::GAMEPLAY)
				{
					if (m_shopOpen && m_scrollBarHandle.getGlobalBounds().contains(mousePos))
					{
						m_isDraggingScrollBarHandle = true;
						m_scrollBarHandleOffsetY = mousePos.y - m_scrollBarHandle.getPosition().y;
					}
					else if (m_shopOpen && isMouseOverShop(mousePos))
					{
						for (auto& item : m_shopItems)
						{
							if (item.button.getGlobalBounds().contains(mousePos) &&
								item.button.getPosition().y > m_shopUIBackground.getPosition().y - m_shopUIBackground.getSize().y / 2 + 45 &&
								item.button.getPosition().y < m_shopUIBackground.getPosition().y + m_shopUIBackground.getSize().y / 2 - 40)
							{
								if (m_currency >= item.cost && !item.purchased)
								{
									m_currency -= item.cost;
									updateCurrencyText();
									item.purchased = true;
									item.button.setFillColor(sf::Color::Green);
									std::cout << "Purchased: " << item.text.getString().toAnsiString() << std::endl;
									
									if (item.text.getString().toAnsiString().find("Health Upgrade") != std::string::npos)
									{
										m_player.upgradeHealth();
									}
									else if (item.text.getString().toAnsiString().find("Ammo Upgrade") != std::string::npos)
									{
										m_player.upgradeAmmo();
									}
									else if (item.text.getString().toAnsiString().find("Double Jump") != std::string::npos)
									{
										m_player.upgradeDoubleJump();
									}
									else if (item.text.getString().toAnsiString().find("Energy Wave Attack") != std::string::npos)
									{
										m_player.upgradeEnergyWaveAttack();
									}
									else if (item.text.getString().toAnsiString().find("Extra bullet x1") != std::string::npos)
									{
										m_player.upgradeBullets();
									}
									else if (item.text.getString().toAnsiString().find("Extra bullet x2") != std::string::npos)
									{
										m_player.upgradeBullets();
									}
								}
								else if (!item.purchased)
								{
									std::cout << "Not enough currency!" << std::endl;
								}
							}
						}
					}
					else
					{
						m_player.shootBullet(mousePos);
					}
				}
			}
			if (newEvent.mouseButton.button == sf::Mouse::Right)
			{
				if (m_currentState == GameState::MAIN_MENU)
				{
					m_mainMenu.handleRightClick(mousePos);
				}
				else if (m_currentState == GameState::OPTIONS)
				{
					m_options.handleRightClick(mousePos);
				}
			}
		}
		if (sf::Event::MouseButtonReleased == newEvent.type)
		{
			if (newEvent.mouseButton.button == sf::Mouse::Left)
			{
				if (m_currentState == GameState::LEVEL_EDITOR)
				{
					m_levelEditor.handleMouseRelease();
					if (m_levelEditor.isBackButtonPressed())
					{
						m_levelEditor.reset();
						m_mainMenu.loadLevelFiles();
						m_currentState = GameState::MAIN_MENU;
					}
				}
				else if (m_currentState == GameState::OPTIONS)
				{
					m_options.handleMouseRelease();

					if (m_options.isBackButtonPressed())
					{
						m_options.reset();
						m_options.clearParticles();
						m_currentState = GameState::MAIN_MENU;
					}
				}
				else if (m_currentState == GameState::GAMEPLAY)
				{
					m_isDraggingScrollBarHandle = false;
				}
			}
		}
		if (newEvent.type == sf::Event::TextEntered)
		{
			if (m_currentState == GameState::LEVEL_EDITOR)
			{
				m_levelEditor.handleTextInput(newEvent);
			}
		}
	}
}

void Game::processKeys(sf::Event t_event)
{
	if (sf::Keyboard::Escape == t_event.key.code)
	{
		m_exitGame = true;
	}
	else if (sf::Keyboard::E == t_event.key.code && m_showShopText)
	{
		m_shopOpen = !m_shopOpen;
	}

	if (m_currentState == GameState::LEVEL_EDITOR)
	{
		m_levelEditor.handleKeyInput(t_event);
	}
}

void Game::update(sf::Time t_deltaTime)
{
	if (m_exitGame)
	{
		m_window.close();
	}

	if (m_currentState == GameState::MAIN_MENU)
	{
		m_mainMenu.update(t_deltaTime);
	}
	else if (m_currentState == GameState::LEVEL_EDITOR)
	{
		m_levelEditor.update(t_deltaTime);
	}
	else if (m_currentState == GameState::OPTIONS)
	{
		m_options.update(t_deltaTime);
	}
	else if (m_currentState == GameState::GAMEPLAY)
	{
		if (m_currentMode == GameMode::SINGLE_PLAYER)
		{
			m_player.update(t_deltaTime, m_gameBlocks);
			updateBulletCollisionsForEnemy();
			if (m_shopOpen && !m_isDraggingScrollBarHandle)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				{
					m_scrollOffset = std::max(0.0f, m_scrollOffset - 1.0f);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
				{
					m_scrollOffset = std::min(m_scrollMaxOffset, m_scrollOffset + 1.0f);
				}

				for (int i = 0; i < m_shopItems.size(); ++i)
				{
					float newY = SCREEN_HEIGHT / 2 - 100 + i * 60 - m_scrollOffset;
					m_shopItems[i].button.setPosition(SCREEN_WIDTH / 2 - 150, newY);
					m_shopItems[i].text.setPosition(m_shopItems[i].button.getPosition().x + 10, m_shopItems[i].button.getPosition().y + 10);
				}

				float scrollHandleY = m_scrollBar.getPosition().y + (m_scrollOffset / m_scrollMaxOffset) * (m_scrollBar.getSize().y - 20);
				m_scrollBarHandle.setPosition(m_scrollBar.getPosition().x + 10, scrollHandleY);
			}

			if (m_player.isNearShop(m_shopSprite.getPosition()))
			{
				m_showShopText = true;
				updateShopTextAnimation();
			}
			else
			{
				m_showShopText = false;
				m_shopOpen = false;
			}

			for (auto& enemy : m_enemies)
			{
				enemy->update(t_deltaTime, m_player);
			}
			updateBlocks();
		}
		else if (m_currentMode == GameMode::MULTIPLAYER)
		{
			m_player.update(t_deltaTime, m_gameBlocks);
			sendPlayerUpdate();
			for (auto& enemy : m_enemies)
			{
				enemy->update(t_deltaTime, m_player);
			}
		}
		updatePortalAnimation();
		updateShopAnimation();
		createLightMap();
	}
}

void Game::render()
{
	m_window.clear(sf::Color::Black);

	if (m_currentState == GameState::MAIN_MENU)
	{
		m_mainMenu.render(m_window);
	}
	else if (m_currentState == GameState::LEVEL_EDITOR)
	{
		m_levelEditor.render(m_window);
	}
	else if (m_currentState == GameState::OPTIONS)
	{
		m_options.render(m_window);
	}
	else if (m_currentState == GameState::GAMEPLAY)
	{
		m_window.draw(m_backgroundSprite);
		renderLevelAssets();
		if (m_currentMode == GameMode::SINGLE_PLAYER)
		{
			m_player.render(m_window);
		}
		else if (m_currentMode == GameMode::MULTIPLAYER)
		{
			m_player.render(m_window);
			m_otherPlayer.render(m_window);
		}
		for (auto& enemy : m_enemies)
		{
			enemy->render(m_window);
		}
		//m_window.draw(m_lightMapSprite, sf::BlendMultiply);
		m_player.renderHealthUI(m_window);
		m_window.draw(m_currencyText);
		if (m_showShopText && !m_shopOpen)
		{
			m_window.draw(m_shopText);
		}
		if (m_shopOpen)
		{
			m_window.draw(m_shopUIBackground);
			m_window.draw(m_shopDescriptionBackground);
			m_window.draw(m_lineBetween);
			m_window.draw(m_shopTitleText);
			m_window.draw(m_shopItemDescriptionTitleText);
			m_window.draw(m_shopItemDescriptionText);
			for (const auto& item : m_shopItems)
			{
				if (item.button.getPosition().y > m_shopUIBackground.getPosition().y - m_shopUIBackground.getSize().y / 2 + 45 &&
					item.button.getPosition().y < m_shopUIBackground.getPosition().y + m_shopUIBackground.getSize().y / 2 - 40)
				{
					m_window.draw(item.button);
					m_window.draw(item.text);
				}
			}
			m_window.draw(m_scrollBar);
			m_window.draw(m_scrollBarHandle);
		}
	}

	m_window.display();
}

void Game::loadBackground()
{
	if (!m_backgroundTexture.loadFromFile("Assets/Images/World/dirtBackground.png"))
	{
		std::cout << "Error loading background texture" << std::endl;
	}
	m_backgroundSprite.setTexture(m_backgroundTexture);
	m_backgroundSprite.setScale
	(
		m_window.getSize().x / static_cast<float>(m_backgroundTexture.getSize().x),
		m_window.getSize().y / static_cast<float>(m_backgroundTexture.getSize().y)
	);
}

void Game::loadLevel(const std::string& m_fileName)
{
	m_gameBlocks.clear();
	m_enemies.clear();

	std::ifstream inputFile("Assets\\SaveFiles\\" + m_fileName);
	if (!inputFile.is_open())
	{
		std::cout << "Error opening file: " << m_fileName << std::endl;
		return;
	}

	std::cout << "Opening save file: " << m_fileName << std::endl;

	std::string fileContent((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
	inputFile.close();

	auto readValue = [](const std::string& key, const std::string& content) -> std::string
		{
			auto start = content.find(key) + key.length() + 1;
			auto end = content.find_first_of(",}", start);
			return content.substr(start, end - start);
		};

	auto readFloat = [](const std::string& value) -> float 
		{
			return std::stof(value);
		};

	auto readInt = [](const std::string& value) -> int 
		{
			return std::stoi(value);
		};

	auto readBool = [](const std::string& value) -> bool 
		{
			std::string trimmedValue = value;
			trimmedValue.erase(remove_if(trimmedValue.begin(), trimmedValue.end(), isspace), trimmedValue.end());
			std::transform(trimmedValue.begin(), trimmedValue.end(), trimmedValue.begin(), ::tolower);

			return (trimmedValue == "true" || trimmedValue == "1");
		};

	auto readBlockType = [](const std::string& value) -> BlockType 
		{
			return static_cast<BlockType>(std::stoi(value));
		};

	auto start = fileContent.find("[");
	if (start == std::string::npos)
	{
		std::cout << "No blocks found in the save file." << std::endl;
		return;
	}

	start += 1;
	auto end = fileContent.find("]", start);
	std::string blocksContent = fileContent.substr(start, end - start);
	std::string::size_type blockStart = 0, blockEnd = 0;

	while ((blockStart = blocksContent.find("{", blockEnd)) != std::string::npos)
	{
		blockEnd = blocksContent.find("}", blockStart);
		std::string blockContent = blocksContent.substr(blockStart, blockEnd - blockStart + 1);

		Block block;
		block.type = readBlockType(readValue("\"type\"", blockContent));
		block.health = readInt(readValue("\"health\"", blockContent));
		block.damage = readInt(readValue("\"damage\"", blockContent));
		block.shape.setPosition(readFloat(readValue("\"x\"", blockContent)), readFloat(readValue("\"y\"", blockContent)));
		block.shape.setSize(sf::Vector2f(readFloat(readValue("\"width\"", blockContent)), readFloat(readValue("\"height\"", blockContent))));
		block.shape.setOrigin(block.shape.getSize().x / 2, block.shape.getSize().y / 2);

		std::string traversableValue = readValue("\"traversable\"", blockContent);
		block.traversable = readBool(traversableValue);

		if (block.type == BlockType::PLAYER)
		{
			m_player.setPosition(block.shape.getPosition().x, block.shape.getPosition().y - (block.shape.getSize().y / 2));
		}
		else if (block.type == BlockType::EVIL_EYE)
		{
			m_evilEyePositions.push_back(block.shape.getPosition());
		}
		else if (block.type == BlockType::MUSHROOM)
		{
			m_mushroomPositions.push_back(block.shape.getPosition());
		}
		else if (block.type == BlockType::SKELETON)
		{
			m_skeletonPositions.push_back(block.shape.getPosition());
		}
		else if (block.type == BlockType::GOBLIN)
		{
			m_goblinPositions.push_back(block.shape.getPosition());
		}
		else if (block.type == BlockType::ENEMY_DEMON_BOSS)
		{
			m_demonBossPositions.push_back(block.shape.getPosition());
		}
		else if (block.type == BlockType::END_GATE)
		{
			m_portalSprite.setPosition(block.shape.getPosition());
			m_portalSprite.setOrigin(m_portalSprite.getGlobalBounds().width / 2, m_portalSprite.getGlobalBounds().height / 2);
			m_portalSprite.setScale(
				block.shape.getSize().x / m_portalFrameRect.width,
				block.shape.getSize().y / m_portalFrameRect.height
			);
		}
		else if (block.type == BlockType::SHOP)
		{
			m_shopSprite.setPosition(block.shape.getPosition());
			m_shopSprite.setOrigin(m_shopSprite.getGlobalBounds().width / 2, m_shopSprite.getGlobalBounds().height / 2);
			m_shopSprite.setScale(
				block.shape.getSize().x / m_shopFrameRect.width,
				block.shape.getSize().y / m_shopFrameRect.height
			);
		}
		else
		{
			sf::Texture* texture = new sf::Texture();
			bool textureLoaded = false;

			switch (block.type)
			{
			case BlockType::DIRT:
				textureLoaded = texture->loadFromFile("Assets\\Images\\World\\dirt01.png");
				block.shape.setTexture(texture);
				break;
			case BlockType::GRANITE:
				textureLoaded = texture->loadFromFile("Assets\\Images\\World\\granite.png");
				block.shape.setTexture(texture);
				break;
			case BlockType::STONE:
				textureLoaded = texture->loadFromFile("Assets\\Images\\World\\stone.png");
				block.shape.setTexture(texture);
				break;
			case BlockType::SAND:
				textureLoaded = texture->loadFromFile("Assets\\Images\\World\\sand.png");
				block.shape.setTexture(texture);
				break;
			case BlockType::WATER:
				block.shape.setFillColor(sf::Color(0, 0, 255));
				textureLoaded = true;
				break;
			case BlockType::LAVA:
				block.shape.setFillColor(sf::Color(255, 70, 0));
				textureLoaded = true;
				break;
			case BlockType::TRAP_SPIKE:
				textureLoaded = texture->loadFromFile("Assets\\Images\\World\\spike.png");
				block.shape.setTexture(texture);
				break;
			case BlockType::TRAP_BARREL:
				textureLoaded = texture->loadFromFile("Assets\\Images\\World\\barrel.png");
				block.shape.setTexture(texture);
				break;
			case BlockType::HEALTH_PACK:
				textureLoaded = texture->loadFromFile("Assets\\Images\\World\\healthPack.png");
				block.shape.setTexture(texture);
				break;
			case BlockType::AMMO_PACK:
				textureLoaded = texture->loadFromFile("Assets\\Images\\World\\ammo.png");
				block.shape.setTexture(texture);
				break;
			case BlockType::TORCH:
				textureLoaded = texture->loadFromFile("Assets\\Images\\World\\torch.png");
				block.shape.setTexture(texture);
				break;
			default:
				break;
			}

			if (textureLoaded || block.shape.getFillColor() != sf::Color::Transparent)
			{
				m_gameBlocks.push_back(block);
				std::cout << "Loaded block: " << static_cast<int>(block.type) << " at (" << block.shape.getPosition().x << ", " << block.shape.getPosition().y << ")\n";
			}
			else
			{
				std::cout << "Failed to load texture for block type: " << static_cast<int>(block.type) << std::endl;
				delete texture;
			}
		}	
	}

	for (const auto& pos : m_evilEyePositions)
	{
		m_enemies.push_back(std::make_unique<EvilEye>(pos, m_gameBlocks));
	}

	for (const auto& pos : m_mushroomPositions)
	{
		m_enemies.push_back(std::make_unique<CorruptedMushroom>(pos, m_gameBlocks));
	}

	for (const auto& pos : m_skeletonPositions)
	{
		m_enemies.push_back(std::make_unique<Skeleton>(pos, m_gameBlocks));
	}

	for (const auto& pos : m_goblinPositions)
	{
		m_enemies.push_back(std::make_unique<Goblin>(pos, m_gameBlocks));
	}

	for (const auto& pos : m_demonBossPositions)
	{
		m_enemies.push_back(std::make_unique<DemonKing>(pos, m_gameBlocks));
	}

	std::vector<sf::RectangleShape> shapes;
	for (const auto& block : m_gameBlocks)
	{
		if (block.type == BlockType::DIRT || 
			block.type == BlockType::GRANITE || 
			block.type == BlockType::STONE ||
			block.type == BlockType::SAND ||
			block.type == BlockType::SHOP ||
			block.type == BlockType::LAVA
			)
		{
			shapes.push_back(block.shape);
		}
	}
	m_edges = calculateEdges(shapes);
}

void Game::createLightMap()
{
	m_lightMapTexture.clear(sf::Color::Black);

    std::unordered_set<const sf::RectangleShape*> shadowedBlocks;

    // Add torch lights
    for (const auto& block : m_gameBlocks)
    {
        if (block.type == BlockType::TORCH)
        {
            // Calculate light polygon for torch
            auto torchLight = calculateLightPolygon(block.shape.getPosition(), m_torchLightRadius, torchLightIntensity, m_edges);
            m_lightMapTexture.draw(&torchLight[0], torchLight.size(), sf::TrianglesFan, sf::BlendAdd);

            // Calculate shadows for the torch
            for (const auto& shadowBlock : m_gameBlocks)
            {
                if (shadowBlock.type == BlockType::DIRT ||
                    shadowBlock.type == BlockType::GRANITE ||
                    shadowBlock.type == BlockType::STONE ||
                    shadowBlock.type == BlockType::SAND ||
                    shadowBlock.type == BlockType::SHOP ||
                    shadowBlock.type == BlockType::LAVA)
                {
                    auto shadowPolygon = calculateShadowPolygon(block.shape.getPosition(), shadowBlock.shape, m_shadowDistance, m_torchLightRadius, m_player.getPosition(), m_playerLightRadius);
                    if (!shadowPolygon.empty())
                    {
                        shadowedBlocks.insert(&shadowBlock.shape);
                        m_lightMapTexture.draw(&shadowPolygon[0], shadowPolygon.size(), sf::Quads, sf::BlendMultiply);
                    }
                }
            }
        }
    }

    // Add player light
    m_lightSource = m_player.getPosition();
    auto playerLight = calculateLightPolygon(m_lightSource, m_playerLightRadius, playerLightIntensity, m_edges);
    m_lightMapTexture.draw(&playerLight[0], playerLight.size(), sf::TrianglesFan, sf::BlendAdd);

    // Shadows from the players light source
    for (const auto& block : m_gameBlocks)
    {
        if ((block.type == BlockType::DIRT ||
            block.type == BlockType::GRANITE ||
            block.type == BlockType::STONE ||
            block.type == BlockType::SAND ||
            block.type == BlockType::SHOP ||
            block.type == BlockType::LAVA) &&
            shadowedBlocks.find(&block.shape) == shadowedBlocks.end())
        {
            auto shadowPolygon = calculateShadowPolygon(m_lightSource, block.shape, m_shadowDistance, m_playerLightRadius, sf::Vector2f(-5000,-5000), m_playerLightRadius);
            if (!shadowPolygon.empty())
            {
                m_lightMapTexture.draw(&shadowPolygon[0], shadowPolygon.size(), sf::Quads, sf::BlendMultiply);
            }
        }
    }

	m_lightMapTexture.display();
}

void Game::initLevelAssets()
{
	if (!m_portalTexture.loadFromFile("Assets\\Images\\World\\portalRings2.png"))
	{
		std::cout << "Error loading portal texture" << std::endl;
	}
	m_portalFrameRect = sf::IntRect(0, 0, 32, 32);
	m_portalSprite.setTexture(m_portalTexture);
	m_portalSprite.setTextureRect(m_portalFrameRect);

	if (!m_shopTexture.loadFromFile("Assets\\Images\\World\\shop_anim.png"))
	{
		std::cout << "Error loading shop texture" << std::endl;
	}
	m_shopFrameRect = sf::IntRect(0, 0, 118, 128);
	m_shopSprite.setTexture(m_shopTexture);
	m_shopSprite.setTextureRect(m_shopFrameRect);

	m_shopText.setFont(m_font);
	m_shopText.setString("Press E");
	m_shopText.setCharacterSize(24);
	m_shopText.setFillColor(sf::Color::White);
}

void Game::initShop()
{
	m_currencyText.setFont(m_font);
	m_currencyText.setCharacterSize(25);
	m_currencyText.setFillColor(sf::Color::White);
	m_currencyText.setPosition(m_window.getSize().x - 250, 10);
	updateCurrencyText();

	// Shop UI
	m_shopUIBackground.setSize(sf::Vector2f(400, 300));
	m_shopUIBackground.setFillColor(sf::Color(0, 0, 0, 200));
	m_shopUIBackground.setOutlineColor(sf::Color::White);
	m_shopUIBackground.setOutlineThickness(2);
	m_shopUIBackground.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	m_shopUIBackground.setOrigin(m_shopUIBackground.getSize().x / 2, m_shopUIBackground.getSize().y / 2);

	m_shopTitleText.setFont(m_font);
	m_shopTitleText.setString("SHOP");
	m_shopTitleText.setStyle(sf::Text::Underlined);
	m_shopTitleText.setCharacterSize(24);
	m_shopTitleText.setFillColor(sf::Color::White);
	m_shopTitleText.setPosition(SCREEN_WIDTH / 2 - m_shopTitleText.getGlobalBounds().width / 2, SCREEN_HEIGHT / 2 - 130);
	
	m_lineBetween.setSize(sf::Vector2f(400, 5));
	m_lineBetween.setFillColor(sf::Color(180, 180, 180, 255));
	m_lineBetween.setPosition(m_shopUIBackground.getPosition().x, m_shopUIBackground.getPosition().y + 152);
	m_lineBetween.setOrigin(m_lineBetween.getSize().x / 2, m_lineBetween.getSize().y / 2);

	m_shopDescriptionBackground.setSize(sf::Vector2f(400, 100));
	m_shopDescriptionBackground.setFillColor(sf::Color(0, 0, 0, 200));
	m_shopDescriptionBackground.setOutlineColor(sf::Color::White);
	m_shopDescriptionBackground.setOutlineThickness(2);
	m_shopDescriptionBackground.setPosition(m_lineBetween.getPosition().x, m_lineBetween.getPosition().y + 50);
	m_shopDescriptionBackground.setOrigin(m_shopDescriptionBackground.getSize().x / 2, m_shopDescriptionBackground.getSize().y / 2);

	m_shopItemDescriptionTitleText.setFont(m_font);
	m_shopItemDescriptionTitleText.setString("Item Description");
	m_shopItemDescriptionTitleText.setCharacterSize(15);
	m_shopItemDescriptionTitleText.setFillColor(sf::Color::White);
	m_shopItemDescriptionTitleText.setPosition(m_lineBetween.getPosition().x, m_lineBetween.getPosition().y + 10);
	m_shopItemDescriptionTitleText.setOrigin(m_shopItemDescriptionTitleText.getGlobalBounds().width / 2, m_shopItemDescriptionTitleText.getGlobalBounds().height / 2);

	m_shopItemDescriptionText.setFont(m_font);
	m_shopItemDescriptionText.setCharacterSize(15);
	m_shopItemDescriptionText.setFillColor(sf::Color::White);
	m_shopItemDescriptionText.setPosition(m_lineBetween.getPosition().x - 185, m_lineBetween.getPosition().y + 35);
	
	// shop items
	std::vector<std::pair<std::string, int>> items =
	{
		{"Health Upgrade", 100},
		{"Ammo Upgrade", 100},
		{"Double Jump", 80},
		{"Energy Wave Attack", 200},
		{"Extra bullet x1", 250},
		{"Extra bullet x2", 250},
		{"Small Shield", 130},
	};

	std::vector<std::string> descriptions = 
	{
		"Increases your health capacity",
		"Increases your ammo capacity",
		"Allows you to jump twice in the air",
		"Releases a powerful energy wave\nfrom slash attacks",
		"Gives a extra bullet when you fire",
		"Gives a extra bullet when you fire",
		"Grants a small shield for protection"
	};

	for (int i = 0; i < items.size(); ++i)
	{
		ShopItem shopItem;
		shopItem.button.setSize(sf::Vector2f(300, 40));
		shopItem.button.setFillColor(sf::Color(100, 100, 100, 200));
		shopItem.button.setPosition(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 100 + i * 60);

		shopItem.text.setFont(m_font);
		shopItem.text.setString(items[i].first + " - " + std::to_string(items[i].second) + " GOLD");
		shopItem.text.setCharacterSize(15);
		shopItem.text.setFillColor(sf::Color::White);
		shopItem.text.setPosition(shopItem.button.getPosition().x + 10, shopItem.button.getPosition().y + 10);

		shopItem.cost = items[i].second;
		shopItem.description = descriptions[i];

		m_shopItems.push_back(shopItem);
	}

	m_scrollBar.setSize(sf::Vector2f(20, 200));
	m_scrollBar.setFillColor(sf::Color::Transparent);
	m_scrollBar.setOutlineColor(sf::Color::White);
	m_scrollBar.setOutlineThickness(1.2f);
	m_scrollBar.setPosition(SCREEN_WIDTH / 2 + 170, SCREEN_HEIGHT / 2 - 100);

	m_scrollBarHandle.setRadius(10);
	m_scrollBarHandle.setFillColor(sf::Color::White);
	m_scrollBarHandle.setOrigin(10, 10);
	m_scrollBarHandle.setPosition(m_scrollBar.getPosition().x + 10, m_scrollBar.getPosition().y);

	m_scrollMaxOffset = std::max(0.0f, (items.size() * 60.0f) - m_shopUIBackground.getSize().y + 40);
}

void Game::updateCurrencyText()
{
	m_currencyText.setString("Gold: " + std::to_string(m_currency));
}

void Game::updatePortalAnimation()
{
	sf::Time frameTime = sf::seconds(0.1f);

	if (m_portalClock.getElapsedTime() >= frameTime)
	{
		m_portalCurrentFrame = (m_portalCurrentFrame + 1) % 5;
		m_portalFrameRect.left = m_portalCurrentFrame * 32;
		m_portalSprite.setTextureRect(m_portalFrameRect);
		m_portalClock.restart();
	}
}

void Game::updateShopAnimation()
{
	sf::Time frameTime = sf::seconds(0.1f);

	if (m_shopClock.getElapsedTime() >= frameTime)
	{
		m_shopCurrentFrame = (m_shopCurrentFrame + 1) % 6;
		m_shopFrameRect.left = m_shopCurrentFrame * 118;
		m_shopSprite.setTextureRect(m_shopFrameRect);
		m_shopClock.restart();
	}
}

void Game::updateShopTextAnimation()
{
	m_shopTextYOffset += m_shopTextYDirection * 0.5f;
	if (m_shopTextYOffset > 5.0f || m_shopTextYOffset < -5.0f)
	{
		m_shopTextYDirection *= -1.0f;
	}
	m_shopText.setPosition
	(
		m_shopSprite.getPosition().x - m_shopText.getGlobalBounds().width / 2,
		m_shopSprite.getPosition().y - m_shopSprite.getGlobalBounds().height / 2 - 30 + m_shopTextYOffset
	);
}

void Game::renderLevelAssets()
{
	for (const auto& block : m_gameBlocks)
	{
		m_window.draw(block.shape);
	}
	m_window.draw(m_portalSprite);
	m_window.draw(m_shopSprite);
}

void Game::updateBlocks()
{
	for (auto& block : m_gameBlocks)
	{
		float healthPercentage = block.getHealthPercentage();

		sf::Color flashColor = block.shape.getFillColor();
		sf::Time flashInterval;

		if (healthPercentage < 20.0f)
		{
			flashColor = sf::Color::Red;
			flashInterval = sf::milliseconds(200);
		}
		else if (healthPercentage < 40.0f)
		{
			flashColor = sf::Color(255, 165, 0);
			flashInterval = sf::milliseconds(500);
		}
		else
		{
			flashColor = sf::Color::White;
			block.shape.setFillColor(flashColor);
			continue;
		}

		// Handle flashing effect
		if (block.flashClock.getElapsedTime() >= flashInterval)
		{
			block.flashState = !block.flashState;
			block.shape.setFillColor(block.flashState ? flashColor : sf::Color::White);
			block.flashClock.restart();
		}
	}
}

void Game::updateBulletCollisionsForEnemy()
{
	for (auto bulletIt = m_player.getBullets().begin(); bulletIt != m_player.getBullets().end();)
	{
		Bullet& bullet = *bulletIt;
		bool collision = false;

		for (auto& enemy : m_enemies)
		{
			if (!enemy->isDead() && bullet.shape.getGlobalBounds().intersects(enemy->getCollisionBoundingBox()))
			{
				collision = true;
				enemy->takeDamage(6);
				break;
			}
		}

		if (collision)
		{
			bulletIt = m_player.getBullets().erase(bulletIt);
		}
		else
		{
			++bulletIt;
		}
	}
}

void Game::initNetwork()
{
	if (m_socket->connect(SERVER_IP, PORT) == sf::Socket::Done)
	{
		std::cout << "Server connected" << std::endl;
		std::string request = "init";
		m_socket->send(request.c_str(), request.size());

		char buffer[2000];
		std::size_t received;
		if (m_socket->receive(buffer, sizeof(buffer), received) == sf::Socket::Done)
		{
			std::string clientID(buffer, received);
			std::cout << "Received client ID: " << clientID << std::endl;
			m_mainMenu.setClientID(clientID);
		}
	}
	else
	{
		std::cout << "Failed to connect to server" << std::endl;
	}
}

void Game::listenForServerMessages()
{
	m_isListening = true;
	while (m_isListening)
	{
		char buffer[2000];
		std::size_t received;
		if (m_socket->receive(buffer, sizeof(buffer), received) == sf::Socket::Done)
		{
			std::string response(buffer, received);

			if (response.substr(0, 13) == "playerJoined:")
			{
				std::string clientID = response.substr(13);
				m_mainMenu.setPlayerJoined(true);
				std::cout << "A player has joined the session: " << clientID << std::endl;
			}
			else if (response.substr(0, 14) == "levelSelected:")
			{
				std::string levelName = response.substr(14);
				loadLevel(levelName); // Load the level on the client side (Join person)
				std::cout << "Level selected: " << levelName << std::endl;
				m_currentState = GameState::GAMEPLAY;
			}
			else if (response.substr(0, 13) == "playerUpdate:")
			{
				auto tokens = split(response, ':');
				if (tokens.size() == 5)
				{
					float x = std::stof(tokens[1]);
					float y = std::stof(tokens[2]);
					int currentFrame = std::stoi(tokens[3]);
					PlayerState state = static_cast<PlayerState>(std::stoi(tokens[4]));

					if (currentFrame >= 0 && currentFrame < m_otherPlayer.getFrameCountForState(state))
					{
						m_otherPlayer.setPosition(x, y);
						m_otherPlayer.updateFacingDirection(x);
						m_otherPlayer.setCurrentFrame(currentFrame);
						m_otherPlayer.setState(state);
					}
				}
			}
		}
	}
}

void Game::startHostSessions()
{
	std::string request = "host";
	m_socket->send(request.c_str(), request.size());
	m_isHosting = true;
	std::cout << "Hosting session..." << std::endl;
	m_listenThread = std::thread(&Game::listenForServerMessages, this);
}

void Game::searchForGames()
{
	std::string request = "searchForGame";
	m_socket->send(request.c_str(), request.size());
	m_clientThread = std::thread(&Game::handleServerSessionResponse, this);
	m_clientThread.detach();
}

void Game::handleServerSessionResponse()
{
	char buffer[2000];
	std::size_t received;
	if (m_socket->receive(buffer, sizeof(buffer), received) == sf::Socket::Done)
	{
		std::string sessions(buffer, received);
		std::cout << "Received sessions:\n" << sessions << std::endl;
		m_mainMenu.updateSessionList(sessions);
	}
	else 
	{
		std::cerr << "Failed to receive session list" << std::endl;
	}
}

void Game::joinSession(const std::string& m_hostID)
{
	std::string request = "join:" + m_hostID + ":" + m_mainMenu.getClientID();
	m_socket->send(request.c_str(), request.size());
	m_isClient = true;
	m_mainMenu.showWaitingForHostScreen();
	m_listenThread = std::thread(&Game::listenForServerMessages, this);
}

void Game::sendLevelToServer()
{
	std::string selectedLevel = m_mainMenu.getSelectedLevelFile();
	std::string request = "levelSelected:" + selectedLevel;
	m_socket->send(request.c_str(), request.size());
	loadLevel(selectedLevel); // this loads on the host side
}

void Game::sendPlayerUpdate()
{
	sf::Vector2f position = m_player.getPosition();
	int currentFrame = m_player.getCurrentFrame();
	PlayerState state = m_player.getState();

	std::string playerInformation = "playerUpdate:" + std::to_string(position.x) + ":" + std::to_string(position.y) + ":" + std::to_string(currentFrame) + ":" + std::to_string(static_cast<int>(state));
	m_socket->send(playerInformation.c_str(), playerInformation.size());
}

bool Game::isMouseOverShop(const sf::Vector2f& m_mousePos)
{
	for (const auto& item : m_shopItems)
	{
		if (item.button.getGlobalBounds().contains(m_mousePos))
		{
			return true;
		}
	}
	if (m_shopUIBackground.getGlobalBounds().contains(m_mousePos) 
		|| m_scrollBar.getGlobalBounds().contains(m_mousePos) 
		|| m_scrollBarHandle.getGlobalBounds().contains(m_mousePos)
		|| m_shopDescriptionBackground.getGlobalBounds().contains(m_mousePos)
		|| m_lineBetween.getGlobalBounds().contains(m_mousePos)
		)
	{
		return true;
	}
	return false;
}

std::vector<std::string> Game::split(const std::string& m_string, char m_delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(m_string);
	while (std::getline(tokenStream, token, m_delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}
