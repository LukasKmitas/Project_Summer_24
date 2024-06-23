#include "Game.h"
#include <iostream>


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
				m_options.handleMouseMove(mousePos);
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
							// Start Game
							SoundManager::getInstance().stopMusic("MenuMusic");
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
			// Start Game
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

	m_window.display();
}

