#include "Game.h"

Game::Game() :
	m_window{ sf::VideoMode{ SCREEN_WIDTH, SCREEN_HEIGHT, 32U }, "WackyBlocks" },
	m_exitGame{false},
	m_currentState{ GameState::MAIN_MENU }
{
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
			processEvents();
			update(timePerFrame);
		}
		render();
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
							m_mainMenu.showLevelSelection();
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
							std::cout << "SINGLE PLAYER MODE" << std::endl;
							loadLevel(m_mainMenu.getSelectedLevelFile());
							m_currentState = GameState::SINGLE_PLAYER;
							m_mainMenu.clearParticles();
							SoundManager::getInstance().stopMusic("MenuMusic");
							break;
						case 5:
							// Multiplayer
							std::cout << "MULTIPLAYER MODE" << std::endl;
							m_currentState = GameState::MULTIPLAYER;
							m_mainMenu.clearParticles();
							SoundManager::getInstance().stopMusic("MenuMusic");
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
	if (sf::Keyboard::Up == t_event.key.code)
	{
		m_mainMenu.moveUp();
	}
	if (sf::Keyboard::Down == t_event.key.code)
	{
		m_mainMenu.moveDown();
	}
	if (sf::Keyboard::Return == t_event.key.code)
	{
		int selectedScreen = m_mainMenu.getPressedItem();
		if (selectedScreen != -1)
		{
			SoundManager::getInstance().playSound("buttonClick");
		}
		switch (selectedScreen)
		{
		case 0:
			m_mainMenu.showLevelSelection();
			break;
		case 1:
			// Level Editor
			m_currentState = GameState::LEVEL_EDITOR;
			m_mainMenu.clearParticles();
			break;
		case 2:
			// Options
			m_currentState = GameState::OPTIONS;
			m_mainMenu.clearParticles();
			break;
		case 3:
			// Exit
			m_exitGame = true;
			break;
		case 4:
			// Single Player
			loadLevel(m_mainMenu.getSelectedLevelFile());
			m_currentState = GameState::SINGLE_PLAYER;
			m_mainMenu.clearParticles();
			break;
		case 5:
			// Multiplayer
			m_currentState = GameState::MULTIPLAYER;
			m_mainMenu.clearParticles();
			break;
		default:
			break;
		}
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
	else if (m_currentState == GameState::SINGLE_PLAYER)
	{

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
	else if (m_currentState == GameState::SINGLE_PLAYER)
	{
		for (const auto& block : m_gameBlocks)
		{
			m_window.draw(block.shape);
		}
	}
	else if (m_currentState == GameState::MULTIPLAYER)
	{
		for (const auto& block : m_gameBlocks)
		{
			m_window.draw(block.shape);
		}
	}

	m_window.display();
}

void Game::loadLevel(const std::string& m_fileName)
{
	m_gameBlocks.clear();

	std::ifstream inputFile("ASSETS\\SAVEFILES\\" + m_fileName);
	if (!inputFile.is_open())
	{
		std::cout << "Error opening file: " << m_fileName << std::endl;
		return;
	}

	std::cout << "Opening save file: " << m_fileName << std::endl;

	std::stringstream buffer;
	buffer << inputFile.rdbuf();
	std::string fileContent = buffer.str();
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
			return value == "true";
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
	start += std::string("[").length();
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
		block.traversable = readBool(readValue("\"traversable\"", blockContent));
		block.shape.setPosition(readFloat(readValue("\"x\"", blockContent)), readFloat(readValue("\"y\"", blockContent)));
		block.shape.setSize(sf::Vector2f(80, 80));

		sf::Texture* texture = new sf::Texture();
		bool textureLoaded = false;

		switch (block.type)
		{
		case BlockType::DIRT:
			textureLoaded = texture->loadFromFile("ASSETS\\IMAGES\\dirt01.png");
			block.shape.setTexture(texture);
			break;
		case BlockType::GRANITE:
			textureLoaded = texture->loadFromFile("ASSETS\\IMAGES\\granite.png");
			block.shape.setTexture(texture);
			break;
		case BlockType::STONE:
			textureLoaded = texture->loadFromFile("ASSETS\\IMAGES\\stone.png");
			block.shape.setTexture(texture);
			break;
		case BlockType::SAND:
			textureLoaded = texture->loadFromFile("ASSETS\\IMAGES\\sand.png");
			block.shape.setTexture(texture);
			break;
		case BlockType::WATER:
			block.shape.setFillColor(sf::Color(0, 0, 255));
			textureLoaded = true;
			break;
		case BlockType::LAVA:
			block.shape.setFillColor(sf::Color(255, 69, 0));
			textureLoaded = true;
			break;
		case BlockType::TRAP_SPIKE:
			textureLoaded = texture->loadFromFile("ASSETS\\IMAGES\\spike.png");
			block.shape.setTexture(texture);
			break;
		case BlockType::TRAP_BARREL:
			textureLoaded = texture->loadFromFile("ASSETS\\IMAGES\\barrel.png");
			block.shape.setTexture(texture);
			break;
		case BlockType::SLIME:
			textureLoaded = texture->loadFromFile("ASSETS\\IMAGES\\slime.png");
			block.shape.setTexture(texture);
			break;
		case BlockType::EVIL_EYE:
			textureLoaded = texture->loadFromFile("ASSETS\\IMAGES\\evilEye.png");
			block.shape.setTexture(texture);
			break;
		case BlockType::SQUIG:
			textureLoaded = texture->loadFromFile("ASSETS\\IMAGES\\squig.png");
			block.shape.setTexture(texture);
			break;
		case BlockType::ENEMY_BOSS:
			textureLoaded = texture->loadFromFile("ASSETS\\IMAGES\\boss1.png");
			block.shape.setTexture(texture);
			break;
		case BlockType::HEALTH_PACK:
			textureLoaded = texture->loadFromFile("ASSETS\\IMAGES\\healthPack.png");
			block.shape.setTexture(texture);
			break;
		case BlockType::AMMO_PACK:
			textureLoaded = texture->loadFromFile("ASSETS\\IMAGES\\ammoPack.png");
			block.shape.setTexture(texture);
			break;
		case BlockType::PLAYER:
			textureLoaded = texture->loadFromFile("ASSETS\\IMAGES\\player.png");
			block.shape.setTexture(texture);
			break;
		case BlockType::END_GATE:
			textureLoaded = texture->loadFromFile("ASSETS\\IMAGES\\portal.png");
			block.shape.setTexture(texture);
			break;
		default:
			delete texture; // Free the texture if not used
			continue;
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

