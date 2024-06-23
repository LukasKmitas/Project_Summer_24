#include "LevelEditor.h"

LevelEditor::LevelEditor()
    : m_sliderOpen(false), m_sliderPosition(SCREEN_WIDTH)
{
    if (!m_font.loadFromFile("ASSETS/FONTS/ariblk.ttf"))
    {
        std::cout << "problem loading Font" << std::endl;
    }
    loadTextures();
    setupUI();
    setupGrid();
    createTileBlocks();
}

LevelEditor::~LevelEditor()
{
}

void LevelEditor::update(sf::Time t_deltaTime)
{
    updateSliderPosition(t_deltaTime);
}

void LevelEditor::render(sf::RenderWindow& m_window)
{
    // Render slider panel
    m_window.draw(m_sliderPanel);
    m_window.draw(m_tabNameText);
    m_window.draw(m_nextTabButton);
    m_window.draw(m_nextTabText);
    m_window.draw(m_prevTabButton);
    m_window.draw(m_prevTabText);
    m_window.draw(m_deleteButton);
    m_window.draw(m_deleteButtonText);

    // Render grid
    for (int row = 0; row < GRID_ROWS; ++row)
    {
        for (int col = 0; col < GRID_COLS; ++col)
        {
            m_window.draw(m_grid[row][col]);
        }
    }

    // Render tile blocks in grid
    for (const auto& tile : m_tileBlocks)
    {
        m_window.draw(tile);
    }

    // Render placed blocks
    for (const auto& block : m_mapBlocks)
    {
        m_window.draw(block.shape);
    }

    // Render back button
    m_window.draw(m_backButton);
    m_window.draw(m_backButtonText);

    // Render save button
    m_window.draw(m_saveButton);
    m_window.draw(m_saveButtonText);

    // Render open slider button
    m_window.draw(m_openSliderButton);
    m_window.draw(m_openSliderText);
}

void LevelEditor::handleMouseClick(sf::Vector2f m_mousePos)
{
    if (m_backButton.getGlobalBounds().contains(m_mousePos))
    {
        m_backButtonPressed = true;
        SoundManager::getInstance().playSound("buttonClick");
    }
    else if (m_saveButton.getGlobalBounds().contains(m_mousePos))
    {
        SoundManager::getInstance().playSound("buttonClick");
        std::cout << "Map saved" << std::endl;
    }
    else if (m_openSliderButton.getGlobalBounds().contains(m_mousePos))
    {
        toggleSlider();
        SoundManager::getInstance().playSound("buttonClick");
    }
    else if (m_nextTabButton.getGlobalBounds().contains(m_mousePos))
    {
        // Handle switching to the next tab
        if (m_currentTab == TabType::BLOCKS)
        {
            m_currentTab = TabType::TRAPS;
        }
        else if (m_currentTab == TabType::TRAPS)
        {
            m_currentTab = TabType::ENEMIES;
        }
        else if (m_currentTab == TabType::ENEMIES)
        {
            m_currentTab = TabType::ESSENTIALS;
        }
        else if (m_currentTab == TabType::ESSENTIALS)
        {
            m_currentTab = TabType::BLOCKS;
        }
        updateTabName();
        updateTileBlocks();
        SoundManager::getInstance().playSound("buttonClick");
        std::cout << "Next tab button clicked" << std::endl;
    }
    else if (m_prevTabButton.getGlobalBounds().contains(m_mousePos))
    {
        // Handle switching to the previous tab
        if (m_currentTab == TabType::BLOCKS)
        {
            m_currentTab = TabType::ESSENTIALS;
        }
        else if (m_currentTab == TabType::TRAPS)
        {
            m_currentTab = TabType::BLOCKS;
        }
        else if (m_currentTab == TabType::ENEMIES)
        {
            m_currentTab = TabType::TRAPS;
        }
        else if (m_currentTab == TabType::ESSENTIALS)
        {
            m_currentTab = TabType::ENEMIES;
        }
        updateTabName();
        updateTileBlocks();
        SoundManager::getInstance().playSound("buttonClick");
        std::cout << "Previous tab button clicked" << std::endl;
    }
    else if (m_deleteButton.getGlobalBounds().contains(m_mousePos))
    {
        toggleDeleteMode();
        SoundManager::getInstance().playSound("buttonClick");
    }
    else if (m_sliderPanel.getGlobalBounds().contains(m_mousePos))
    {
        selectBlock(m_mousePos);
    }
    else
    {
        if (m_deleteMode)
        {
            deleteBlock(m_mousePos);
        }
        else
        {
            placeBlock(m_mousePos);
        }
    }
}

void LevelEditor::handleMouseHover(sf::Vector2f m_mousePos)
{
    // Back button hover effect
    if (m_backButton.getGlobalBounds().contains(m_mousePos))
    {
        m_backButtonHovered = true;
        m_backButton.setFillColor(sf::Color(100, 100, 100));  // Highlight color
    }
    else
    {
        m_backButtonHovered = false;
        m_backButton.setFillColor(sf::Color::Transparent);  // Default color
    }

    // Save button hover effect
    if (m_saveButton.getGlobalBounds().contains(m_mousePos))
    {
        m_saveButtonHovered = true;
        m_saveButton.setFillColor(sf::Color(100, 100, 100));  // Highlight color
    }
    else
    {
        m_saveButtonHovered = false;
        m_saveButton.setFillColor(sf::Color::Transparent);  // Default color
    }

    // Open slider button hover effect
    if (m_openSliderButton.getGlobalBounds().contains(m_mousePos))
    {
        m_openSliderButtonHovered = true;
        m_openSliderButton.setFillColor(sf::Color(100, 100, 100));  // Highlight color
    }
    else
    {
        m_openSliderButtonHovered = false;
        m_openSliderButton.setFillColor(sf::Color::Transparent);  // Default color
    }

    // Next tab button hover effect
    if (m_nextTabButton.getGlobalBounds().contains(m_mousePos))
    {
        m_nextTabButtonHovered = true;
        m_nextTabButton.setFillColor(sf::Color(100, 100, 100));  // Highlight color
    }
    else
    {
        m_nextTabButtonHovered = false;
        m_nextTabButton.setFillColor(sf::Color::Transparent);  // Default color
    }

    // Previous tab button hover effect
    if (m_prevTabButton.getGlobalBounds().contains(m_mousePos))
    {
        m_prevTabButtonHovered = true;
        m_prevTabButton.setFillColor(sf::Color(100, 100, 100));  // Highlight color
    }
    else
    {
        m_prevTabButtonHovered = false;
        m_prevTabButton.setFillColor(sf::Color::Transparent);  // Default color
    }

    // Delete button hover effect
    if (m_deleteButton.getGlobalBounds().contains(m_mousePos))
    {
        m_deleteButtonHovered = true;
        m_deleteButton.setFillColor(sf::Color(100, 100, 100));  // Highlight color
    }
    else
    {
        m_deleteButtonHovered = false;
        m_deleteButton.setFillColor(sf::Color::Transparent);  // Default color
    }
}

void LevelEditor::handleMouseRelease()
{
}

void LevelEditor::reset()
{
    m_backButtonPressed = false;
    m_saveButtonPressed = false;
}

bool LevelEditor::isBackButtonPressed() const
{
    return m_backButtonPressed;
}

void LevelEditor::loadTextures()
{
    // BLOCKS
    if (!m_dirtTexture.loadFromFile("ASSETS\\IMAGES\\dirt01.png"))
    {
        std::cout << "Problem loading Dirt Texture" << std::endl;
    }
    if (!m_graniteTexture.loadFromFile("ASSETS\\IMAGES\\granite.png"))
    {
        std::cout << "Problem loading granite Texture" << std::endl;
    }
    if (!m_stoneTexture.loadFromFile("ASSETS\\IMAGES\\stone.png"))
    {
        std::cout << "Problem loading stone Texture" << std::endl;
    }
    if (!m_sandTexture.loadFromFile("ASSETS\\IMAGES\\sand.png"))
    {
        std::cout << "Problem loading sand Texture" << std::endl;
    }
    // TRAPS
    if (!m_spikeTexture.loadFromFile("ASSETS\\IMAGES\\spike.png"))
    {
        std::cout << "Problem loading Spike Texture" << std::endl;
    }
    if (!m_barrelTexture.loadFromFile("ASSETS\\IMAGES\\barrel.png"))
    {
        std::cout << "Problem loading Barrel Texture" << std::endl;
    }
    // ENEMIES
    if (!m_slimeTexture.loadFromFile("ASSETS\\IMAGES\\slime.png"))
    {
        std::cout << "Problem loading Basic Enemy Texture" << std::endl;
    }
    if (!m_evilEyeTexture.loadFromFile("ASSETS\\IMAGES\\evilEye.png"))
    {
        std::cout << "Problem loading Advanced Enemy Texture" << std::endl;
    }
    if (!m_squigTexture.loadFromFile("ASSETS\\IMAGES\\squig.png"))
    {
        std::cout << "Problem loading Advanced Enemy Texture" << std::endl;
    }
    // ESSENTIALS
    if (!m_playerTexture.loadFromFile("ASSETS\\IMAGES\\player.png"))
    {
        std::cout << "Problem loading Player Texture" << std::endl;
    }
    if (!m_healthPackTexture.loadFromFile("ASSETS\\IMAGES\\health_pack.png"))
    {
        std::cout << "Problem loading Health Pack Texture" << std::endl;
    }
}

void LevelEditor::setupUI()
{
    // Back button
    m_backButton.setSize(sf::Vector2f(100.0f, 50.0f));
    m_backButton.setFillColor(sf::Color::Transparent);
    m_backButton.setOutlineColor(sf::Color::White);
    m_backButton.setOutlineThickness(2.0f);
    m_backButton.setPosition(10.0f, 10.0f);

    m_backButtonText.setFont(m_font);
    m_backButtonText.setString("Back");
    m_backButtonText.setFillColor(sf::Color::White);
    m_backButtonText.setPosition(15.0f, 15.0f);

    // Save button
    m_saveButton.setSize(sf::Vector2f(100.0f, 50.0f));
    m_saveButton.setFillColor(sf::Color::Transparent);
    m_saveButton.setOutlineColor(sf::Color::White);
    m_saveButton.setOutlineThickness(2.0f);
    m_saveButton.setPosition(120.0f, 10.0f);

    m_saveButtonText.setFont(m_font);
    m_saveButtonText.setString("Save");
    m_saveButtonText.setFillColor(sf::Color::White);
    m_saveButtonText.setPosition(125.0f, 15.0f);

    // Open slider button
    m_openSliderButton.setSize(sf::Vector2f(50.0f, 50.0f));
    m_openSliderButton.setFillColor(sf::Color::Transparent);
    m_openSliderButton.setOutlineColor(sf::Color::White);
    m_openSliderButton.setOutlineThickness(2.0f);
    m_openSliderButton.setPosition(m_sliderPosition - 50.0f, 70.0f);

    m_openSliderText.setFont(m_font);
    m_openSliderText.setString("<");
    m_openSliderText.setFillColor(sf::Color::White);
    m_openSliderText.setPosition(m_sliderPosition - 35.0f, 75.0f);

    // Slider panel
    m_sliderPanel.setSize(sf::Vector2f(400.0f, SCREEN_HEIGHT));
    m_sliderPanel.setFillColor(sf::Color(50, 50, 50, 200));
    m_sliderPanel.setOutlineColor(sf::Color::White);
    m_sliderPanel.setOutlineThickness(2.0f);
    m_sliderPanel.setPosition(m_sliderPosition, 0);

    m_tabNameText.setFont(m_font);
    m_tabNameText.setString("Blocks");
    m_tabNameText.setFillColor(sf::Color::White);
    m_tabNameText.setPosition(m_sliderPosition + 150.0f, 100.0f);

    // Next tab button
    m_nextTabButton.setSize(sf::Vector2f(30.0f, 30.0f));
    m_nextTabButton.setFillColor(sf::Color::Transparent);
    m_nextTabButton.setOutlineColor(sf::Color::White);
    m_nextTabButton.setOutlineThickness(2.0f);
    m_nextTabButton.setPosition(m_sliderPosition + 350.0f, 150.0f);

    m_nextTabText.setFont(m_font);
    m_nextTabText.setString(">");
    m_nextTabText.setFillColor(sf::Color::White);
    m_nextTabText.setPosition(m_sliderPosition + 355.0f, 155.0f);

    // Previous tab button
    m_prevTabButton.setSize(sf::Vector2f(30.0f, 30.0f));
    m_prevTabButton.setFillColor(sf::Color::Transparent);
    m_prevTabButton.setOutlineColor(sf::Color::White);
    m_prevTabButton.setOutlineThickness(2.0f);
    m_prevTabButton.setPosition(m_sliderPosition + 20.0f, 150.0f);

    m_prevTabText.setFont(m_font);
    m_prevTabText.setString("<");
    m_prevTabText.setFillColor(sf::Color::White);
    m_prevTabText.setPosition(m_sliderPosition + 25.0f, 155.0f);

    // Delete button
    m_deleteButton.setSize(sf::Vector2f(120.0f, 50.0f));
    m_deleteButton.setFillColor(sf::Color::Transparent);
    m_deleteButton.setOutlineColor(sf::Color::White);
    m_deleteButton.setOutlineThickness(2.0f);
    m_deleteButton.setPosition(m_sliderPosition + 140.0f, 30.0f);

    m_deleteButtonText.setFont(m_font);
    m_deleteButtonText.setString("Delete");
    m_deleteButtonText.setFillColor(sf::Color::White);
    m_deleteButtonText.setPosition(m_sliderPosition + 150.0f, 35.0f);
}

void LevelEditor::setupGrid()
{
    // Setup grid
    float startX = m_sliderPosition + 20.0f;
    float startY = 200.0f;
    float cellWidth = 90.0f;
    float cellHeight = 90.0f;
    for (int row = 0; row < GRID_ROWS; ++row)
    {
        for (int col = 0; col < GRID_COLS; ++col)
        {
            m_grid[row][col].setSize(sf::Vector2f(cellWidth - 10.0f, cellHeight - 10.0f));
            m_grid[row][col].setFillColor(sf::Color::Transparent);
            m_grid[row][col].setOutlineColor(sf::Color::White);
            m_grid[row][col].setOutlineThickness(1.0f);
            m_grid[row][col].setPosition(startX + col * cellWidth, startY + row * cellHeight);
        }
    }
}

void LevelEditor::createTileBlocks()
{
    sf::RectangleShape dirtTile;
    dirtTile.setSize(sf::Vector2f(80.0f, 80.0f));
    dirtTile.setTexture(&m_dirtTexture);
    m_tileBlocks.push_back(dirtTile);

    sf::RectangleShape graniteTile;
    graniteTile.setSize(sf::Vector2f(80.0f, 80.0f));
    graniteTile.setTexture(&m_graniteTexture);
    m_tileBlocks.push_back(graniteTile);

    sf::RectangleShape stoneTile;
    stoneTile.setSize(sf::Vector2f(80.0f, 80.0f));
    stoneTile.setTexture(&m_stoneTexture);
    m_tileBlocks.push_back(stoneTile);

    sf::RectangleShape sandTile;
    sandTile.setSize(sf::Vector2f(80.0f, 80.0f));
    sandTile.setTexture(&m_sandTexture);
    m_tileBlocks.push_back(sandTile);

    sf::RectangleShape waterTile;
    waterTile.setSize(sf::Vector2f(80.0f, 80.0f));
    waterTile.setFillColor(sf::Color(0, 0, 255));
    m_tileBlocks.push_back(waterTile);

    sf::RectangleShape lavaTile;
    lavaTile.setSize(sf::Vector2f(80.0f, 80.0f));
    lavaTile.setFillColor(sf::Color(255, 69, 0));
    m_tileBlocks.push_back(lavaTile);

    int tileIndex = 0;
    for (int row = 0; row < GRID_ROWS; ++row)
    {
        for (int col = 0; col < GRID_COLS; ++col)
        {
            if (tileIndex < m_tileBlocks.size())
            {
                m_tileBlocks[tileIndex].setPosition(m_grid[row][col].getPosition());
                tileIndex++;
            }
        }
    }
}

void LevelEditor::toggleSlider()
{
    m_sliderOpen = !m_sliderOpen;
    std::cout << "Slider toggled: " << (m_sliderOpen ? "Open" : "Closed") << std::endl;
}

void LevelEditor::updateSliderPosition(sf::Time t_deltaTime)
{
    if (m_sliderOpen && m_sliderPosition > SCREEN_WIDTH - 400)
    {
        m_sliderPosition -= 300.0f * t_deltaTime.asSeconds();
        if (m_sliderPosition < SCREEN_WIDTH - 400)
        {
            m_sliderPosition = SCREEN_WIDTH - 400;
        }
    }
    else if (!m_sliderOpen && m_sliderPosition < SCREEN_WIDTH)
    {
        m_sliderPosition += 300.0f * t_deltaTime.asSeconds();
        if (m_sliderPosition > SCREEN_WIDTH)
        {
            m_sliderPosition = SCREEN_WIDTH;
        }
    }

    m_sliderPanel.setPosition(m_sliderPosition, 0);
    m_tabNameText.setPosition(m_sliderPosition + 150.0f, 100.0f);
    m_nextTabButton.setPosition(m_sliderPosition + 350.0f, 155.0f);
    m_nextTabText.setPosition(m_sliderPosition + 355.0f, 150.0f);
    m_prevTabButton.setPosition(m_sliderPosition + 20.0f, 155.0f);
    m_prevTabText.setPosition(m_sliderPosition + 25.0f, 150.0f);
    m_openSliderButton.setPosition(m_sliderPosition - 50.0f, 70.0f);
    m_openSliderText.setPosition(m_sliderPosition - 35.0f, 75.0f);
    m_deleteButton.setPosition(m_sliderPosition + 150.0f, 30.0f);
    m_deleteButtonText.setPosition(m_sliderPosition + 155.0f, 35.0f);

    // Update grid positions
    float startX = m_sliderPosition + 20.0f;
    float startY = 200.0f;
    float cellWidth = 90.0f;
    float cellHeight = 90.0f;
    for (int row = 0; row < GRID_ROWS; ++row)
    {
        for (int col = 0; col < GRID_COLS; ++col)
        {
            m_grid[row][col].setPosition(startX + col * cellWidth, startY + row * cellHeight);
        }
    }

    // Update tile block positions
    int tileIndex = 0;
    for (int row = 0; row < GRID_ROWS; ++row)
    {
        for (int col = 0; col < GRID_COLS; ++col)
        {
            if (tileIndex < m_tileBlocks.size())
            {
                m_tileBlocks[tileIndex].setPosition(m_grid[row][col].getPosition());
                tileIndex++;
            }
        }
    }
}

void LevelEditor::updateTabName()
{
    if (m_currentTab == TabType::BLOCKS)
    {
        m_tabNameText.setString("Blocks");
    }
    else if (m_currentTab == TabType::TRAPS)
    {
        m_tabNameText.setString("Traps");
    }
    else if (m_currentTab == TabType::ENEMIES)
    {
        m_tabNameText.setString("Enemies");
    }
    else if (m_currentTab == TabType::ESSENTIALS)
    {
        m_tabNameText.setString("Essentials");
    }
    std::cout << "Current tab: " << m_tabNameText.getString().toAnsiString() << std::endl;
}

void LevelEditor::updateTileBlocks()
{
    m_tileBlocks.clear();

    if (m_currentTab == TabType::BLOCKS)
    {
        sf::RectangleShape dirtTile;
        dirtTile.setSize(sf::Vector2f(80.0f, 80.0f));
        dirtTile.setTexture(&m_dirtTexture);
        m_tileBlocks.push_back(dirtTile);

        sf::RectangleShape graniteTile;
        graniteTile.setSize(sf::Vector2f(80.0f, 80.0f));
        graniteTile.setTexture(&m_graniteTexture);
        m_tileBlocks.push_back(graniteTile);

        sf::RectangleShape stoneTile;
        stoneTile.setSize(sf::Vector2f(80.0f, 80.0f));
        stoneTile.setTexture(&m_stoneTexture);
        m_tileBlocks.push_back(stoneTile);

        sf::RectangleShape sandTile;
        sandTile.setSize(sf::Vector2f(80.0f, 80.0f));
        sandTile.setTexture(&m_sandTexture);
        m_tileBlocks.push_back(sandTile);

        sf::RectangleShape waterTile;
        waterTile.setSize(sf::Vector2f(80.0f, 80.0f));
        waterTile.setFillColor(sf::Color(0, 0, 255));
        m_tileBlocks.push_back(waterTile);

        sf::RectangleShape lavaTile;
        lavaTile.setSize(sf::Vector2f(80.0f, 80.0f));
        lavaTile.setFillColor(sf::Color(255, 69, 0));
        m_tileBlocks.push_back(lavaTile);
    }
    else if (m_currentTab == TabType::TRAPS)
    {
        sf::RectangleShape spikeTile;
        spikeTile.setSize(sf::Vector2f(80.0f, 80.0f));
        spikeTile.setTexture(&m_spikeTexture);
        m_tileBlocks.push_back(spikeTile);

        sf::RectangleShape barrelTile;
        barrelTile.setSize(sf::Vector2f(80.0f, 80.0f));
        barrelTile.setTexture(&m_barrelTexture);
        m_tileBlocks.push_back(barrelTile);
    }
    else if (m_currentTab == TabType::ENEMIES)
    {
        sf::RectangleShape enemySlimeTile;
        enemySlimeTile.setSize(sf::Vector2f(80.0f, 80.0f));
        enemySlimeTile.setTexture(&m_slimeTexture);
        m_tileBlocks.push_back(enemySlimeTile);

        sf::RectangleShape enemyEvilEyeTile;
        enemyEvilEyeTile.setSize(sf::Vector2f(80.0f, 80.0f));
        enemyEvilEyeTile.setTexture(&m_evilEyeTexture);
        m_tileBlocks.push_back(enemyEvilEyeTile);

        sf::RectangleShape enemySquigTile;
        enemySquigTile.setSize(sf::Vector2f(80.0f, 80.0f));
        enemySquigTile.setTexture(&m_squigTexture);
        m_tileBlocks.push_back(enemySquigTile);
    }
    else if (m_currentTab == TabType::ESSENTIALS)
    {
        sf::RectangleShape playerTile;
        playerTile.setSize(sf::Vector2f(80.0f, 80.0f));
        playerTile.setTexture(&m_playerTexture);
        m_tileBlocks.push_back(playerTile);

        sf::RectangleShape healthPackTile;
        healthPackTile.setSize(sf::Vector2f(80.0f, 80.0f));
        healthPackTile.setTexture(&m_healthPackTexture);
        m_tileBlocks.push_back(healthPackTile);
    }

    // Assign tiles to grid
    int tileIndex = 0;
    for (int row = 0; row < GRID_ROWS; ++row)
    {
        for (int col = 0; col < GRID_COLS; ++col)
        {
            if (tileIndex < m_tileBlocks.size())
            {
                m_tileBlocks[tileIndex].setPosition(m_grid[row][col].getPosition());
                tileIndex++;
            }
        }
    }
}

void LevelEditor::selectBlock(sf::Vector2f m_mousePos)
{
    for (int i = 0; i < m_tileBlocks.size(); ++i)
    {
        if (m_tileBlocks[i].getGlobalBounds().contains(m_mousePos))
        {
            if (m_selectedBlockIndex == static_cast<int>(i))
            {
                // Deselect the block
                m_selectedBlockType = BlockType::NONE;
                m_selectedBlockIndex = -1;
                m_tileBlocks[i].setOutlineColor(sf::Color::Transparent);
                m_tileBlocks[i].setOutlineThickness(0.0f);
                std::cout << "Deselected block: " << m_tabNameText.getString().toAnsiString() << " #" << (i + 1) << std::endl;
            }
            else
            {
                // Select the block
                m_selectedBlockType = getBlockTypeForCurrentTab(i);
                m_selectedBlockIndex = i;
                m_tileBlocks[i].setOutlineColor(sf::Color::Yellow);
                m_tileBlocks[i].setOutlineThickness(3.0f);
                std::cout << "Selected block: " << m_tabNameText.getString().toAnsiString() << " #" << (i + 1) << std::endl;
                SoundManager::getInstance().playSound("buttonClick");
            }
        }
        else
        {
            // Remove highlight from other blocks
            m_tileBlocks[i].setOutlineColor(sf::Color::Transparent);
            m_tileBlocks[i].setOutlineThickness(0.0f);
        }
    }
}


void LevelEditor::placeBlock(sf::Vector2f m_mousePos)
{
    if (m_selectedBlockType != BlockType::NONE && !m_sliderPanel.getGlobalBounds().contains(m_mousePos))
    {
        // Snap the block to the grid
        float gridSize = 80.0f;
        float snappedX = std::floor(m_mousePos.x / gridSize) * gridSize;
        float snappedY = std::floor(m_mousePos.y / gridSize) * gridSize;

        // Check if a block already exists at this position and remove it
        auto it = std::remove_if(m_mapBlocks.begin(), m_mapBlocks.end(),
            [snappedX, snappedY](const Block& block) 
            {
                return block.shape.getPosition().x == snappedX && block.shape.getPosition().y == snappedY;
            });
        m_mapBlocks.erase(it, m_mapBlocks.end());

        Block newBlock;
        newBlock.type = m_selectedBlockType;
        newBlock.shape.setSize(sf::Vector2f(gridSize, gridSize));
        newBlock.shape.setPosition(snappedX, snappedY);

        switch (m_selectedBlockType)
        {
        case BlockType::DIRT:
            newBlock.shape.setTexture(&m_dirtTexture);
            newBlock.health = 100;
            newBlock.traversable = false;
            newBlock.damage = 0;
            break;
        case BlockType::GRANITE:
            newBlock.shape.setTexture(&m_graniteTexture);
            newBlock.health = 200;
            newBlock.traversable = false;
            newBlock.damage = 0;
            break;
        case BlockType::STONE:
            newBlock.shape.setTexture(&m_stoneTexture);
            newBlock.health = 150;
            newBlock.traversable = false;
            newBlock.damage = 0;
            break;
        case BlockType::SAND:
            newBlock.shape.setTexture(&m_sandTexture);
            newBlock.health = 50;
            newBlock.traversable = false;
            newBlock.damage = 0;
            break;
        case BlockType::WATER:
            newBlock.shape.setFillColor(sf::Color(0, 0, 255));
            newBlock.health = 0;
            newBlock.traversable = true;
            newBlock.damage = 0;
            break;
        case BlockType::LAVA:
            newBlock.shape.setFillColor(sf::Color(255, 69, 0));
            newBlock.health = 0;
            newBlock.traversable = false;
            newBlock.damage = 10;
            break;
        case BlockType::TRAP_SPIKE:
            newBlock.shape.setTexture(&m_spikeTexture);
            newBlock.health = 0;
            newBlock.traversable = false;
            newBlock.damage = 20;
            break;
        case BlockType::TRAP_BARREL:
            newBlock.shape.setTexture(&m_barrelTexture);
            newBlock.health = 100;
            newBlock.traversable = false;
            newBlock.damage = 0;
            break;
        case BlockType::SLIME:
            newBlock.shape.setTexture(&m_slimeTexture);
            newBlock.health = 50;
            newBlock.traversable = false;
            newBlock.damage = 5;
            break;
        case BlockType::EVIL_EYE:
            newBlock.shape.setTexture(&m_evilEyeTexture);
            newBlock.health = 70;
            newBlock.traversable = false;
            newBlock.damage = 10;
            break;
        case BlockType::SQUIG:
            newBlock.shape.setTexture(&m_squigTexture);
            newBlock.health = 100;
            newBlock.traversable = false;
            newBlock.damage = 14;
            break;
        case BlockType::PLAYER:
            newBlock.shape.setTexture(&m_playerTexture);
            newBlock.health = 100;
            newBlock.traversable = true;
            newBlock.damage = 0;
            break;
        case BlockType::HEALTH_PACK:
            newBlock.shape.setTexture(&m_healthPackTexture);
            newBlock.health = 0;
            newBlock.traversable = true;
            newBlock.damage = 0;
            break;
        default:
            break;
        }

        m_mapBlocks.push_back(newBlock);
        std::cout << "Placed block: " << m_tabNameText.getString().toAnsiString() << " #" << (m_selectedBlockIndex + 1) << " at: (" << snappedX << ", " << snappedY << ")" << std::endl;
    }
}

void LevelEditor::deleteBlock(sf::Vector2f m_mousePos)
{
    // Snap the block to the grid
    float gridSize = 80.0f;
    float snappedX = std::floor(m_mousePos.x / gridSize) * gridSize;
    float snappedY = std::floor(m_mousePos.y / gridSize) * gridSize;

    // Check if a block exists at this position and remove it
    auto it = std::remove_if(m_mapBlocks.begin(), m_mapBlocks.end(),
        [snappedX, snappedY](const Block& block) 
        {
            return block.shape.getPosition().x == snappedX && block.shape.getPosition().y == snappedY;
        });
    if (it != m_mapBlocks.end())
    {
        m_mapBlocks.erase(it, m_mapBlocks.end());
        std::cout << "Deleted block at: (" << snappedX << ", " << snappedY << ")" << std::endl;
    }
}

void LevelEditor::placeBlocksInRectangle(sf::FloatRect m_selectionRect)
{
    float gridSize = 80.0f;

    for (float x = m_selectionRect.left; x < m_selectionRect.left + m_selectionRect.width; x += gridSize)
    {
        for (float y = m_selectionRect.top; y < m_selectionRect.top + m_selectionRect.height; y += gridSize)
        {
            sf::Vector2f snappedPos(std::floor(x / gridSize) * gridSize, std::floor(y / gridSize) * gridSize);

            // Check if a block already exists at this position and remove it
            auto it = std::remove_if(m_mapBlocks.begin(), m_mapBlocks.end(),
                [snappedPos](const Block& block) 
                {
                    return block.shape.getPosition() == snappedPos;
                });
            m_mapBlocks.erase(it, m_mapBlocks.end());

            Block newBlock;
            newBlock.type = m_selectedBlockType;
            newBlock.shape.setSize(sf::Vector2f(gridSize, gridSize));
            newBlock.shape.setPosition(snappedPos);

            switch (m_selectedBlockType)
            {
            case BlockType::DIRT:
                newBlock.shape.setTexture(&m_dirtTexture);
                newBlock.health = 100;
                newBlock.traversable = false;
                newBlock.damage = 0;
                break;
            case BlockType::GRANITE:
                newBlock.shape.setTexture(&m_graniteTexture);
                newBlock.health = 200;
                newBlock.traversable = false;
                newBlock.damage = 0;
                break;
            case BlockType::STONE:
                newBlock.shape.setTexture(&m_stoneTexture);
                newBlock.health = 150;
                newBlock.traversable = false;
                newBlock.damage = 0;
                break;
            case BlockType::SAND:
                newBlock.shape.setTexture(&m_sandTexture);
                newBlock.health = 50;
                newBlock.traversable = false;
                newBlock.damage = 0;
                break;
            case BlockType::WATER:
                newBlock.shape.setFillColor(sf::Color(0, 0, 255));
                newBlock.health = 0;
                newBlock.traversable = true;
                newBlock.damage = 0;
                break;
            case BlockType::LAVA:
                newBlock.shape.setFillColor(sf::Color(255, 69, 0));
                newBlock.health = 0;
                newBlock.traversable = false;
                newBlock.damage = 10;
                break;
            case BlockType::TRAP_SPIKE:
                newBlock.shape.setTexture(&m_spikeTexture);
                newBlock.health = 0;
                newBlock.traversable = false;
                newBlock.damage = 20;
                break;
            case BlockType::TRAP_BARREL:
                newBlock.shape.setTexture(&m_barrelTexture);
                newBlock.health = 100;
                newBlock.traversable = false;
                newBlock.damage = 0;
                break;
            case BlockType::SLIME:
                newBlock.shape.setTexture(&m_slimeTexture);
                newBlock.health = 50;
                newBlock.traversable = false;
                newBlock.damage = 5;
                break;
            case BlockType::EVIL_EYE:
                newBlock.shape.setTexture(&m_evilEyeTexture);
                newBlock.health = 70;
                newBlock.traversable = false;
                newBlock.damage = 10;
                break;
            case BlockType::SQUIG:
                newBlock.shape.setTexture(&m_squigTexture);
                newBlock.health = 100;
                newBlock.traversable = false;
                newBlock.damage = 14;
                break;
            case BlockType::PLAYER:
                newBlock.shape.setTexture(&m_playerTexture);
                newBlock.health = 100;
                newBlock.traversable = true;
                newBlock.damage = 0;
                break;
            case BlockType::HEALTH_PACK:
                newBlock.shape.setTexture(&m_healthPackTexture);
                newBlock.health = 0;
                newBlock.traversable = true;
                newBlock.damage = 0;
                break;
            default:
                break;
            }

            m_mapBlocks.push_back(newBlock);
            std::cout << "Placed block at: (" << snappedPos.x << ", " << snappedPos.y << ")" << std::endl;
        }
    }
}

void LevelEditor::toggleDeleteMode()
{
    m_deleteMode = !m_deleteMode;
    if (m_deleteMode)
    {
        m_deleteButton.setOutlineColor(sf::Color::Yellow);
        m_deleteButton.setOutlineThickness(3.0f);
        std::cout << "Delete mode enabled" << std::endl;
    }
    else
    {
        m_deleteButton.setOutlineColor(sf::Color::White);
        m_deleteButton.setOutlineThickness(2.0f);
        std::cout << "Delete mode disabled" << std::endl;
    }
}

BlockType LevelEditor::getBlockTypeForCurrentTab(int m_index) const
{
    switch (m_currentTab)
    {
    case TabType::BLOCKS:
        switch (m_index)
        {
        case 0: return BlockType::DIRT;
        case 1: return BlockType::GRANITE;
        case 2: return BlockType::STONE;
        case 3: return BlockType::SAND;
        case 4: return BlockType::WATER;
        case 5: return BlockType::LAVA;
        default: return BlockType::NONE;
        }
    case TabType::TRAPS:
        switch (m_index)
        {
        case 0: return BlockType::TRAP_SPIKE;
        case 1: return BlockType::TRAP_BARREL;
        default: return BlockType::NONE;
        }
    case TabType::ENEMIES:
        switch (m_index)
        {
        case 0: return BlockType::SLIME;
        case 1: return BlockType::EVIL_EYE;
        case 2: return BlockType::SQUIG;
        default: return BlockType::NONE;
        }
    case TabType::ESSENTIALS:
        switch (m_index)
        {
        case 0: return BlockType::PLAYER;
        case 1: return BlockType::HEALTH_PACK;
        default: return BlockType::NONE;
        }
    default:
        return BlockType::NONE;
    }
}
