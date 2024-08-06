#include "LevelEditor.h"

LevelEditor::LevelEditor()
    : m_sliderOpen(false), m_sliderPosition(SCREEN_WIDTH)
{
    if (!m_font.loadFromFile("Assets\\Fonts\\ariblk.ttf"))
    {
        std::cout << "problem loading Font" << std::endl;
    }
    loadTextures();
    setupUI();
    setupGrid();
    initTileBlocks();
    setupSaveUI();
}

LevelEditor::~LevelEditor()
{
}

void LevelEditor::update(sf::Time t_deltaTime)
{
    updateSliderPosition(t_deltaTime);
    updateWarning(t_deltaTime);
}

void LevelEditor::render(sf::RenderWindow& m_window)
{
    drawWorldGrid(m_window);

    if (m_isBlockSelected)
    {
        m_window.draw(m_ghostTile);
    }

    // slider panel
    m_window.draw(m_sliderPanel);
    m_window.draw(m_tabNameText);
    m_window.draw(m_nextTabButton);
    m_window.draw(m_nextTabText);
    m_window.draw(m_prevTabButton);
    m_window.draw(m_prevTabText);
    m_window.draw(m_deleteButton);
    m_window.draw(m_deleteButtonText);

    // Slider grid
    for (int row = 0; row < GRID_ROWS; ++row)
    {
        for (int col = 0; col < GRID_COLS; ++col)
        {
            m_window.draw(m_grid[row][col]);
        }
    }

    // tile blocks in grid
    for (const auto& tile : m_tileBlocks)
    {
        m_window.draw(tile);
    }

    // placed blocks
    for (const auto& block : m_mapBlocks)
    {
        m_window.draw(block.shape);
    }

    // back button
    m_window.draw(m_backButton);
    m_window.draw(m_backButtonText);

    // save button
    m_window.draw(m_saveButton);
    m_window.draw(m_saveButtonText);

    // open slider button
    m_window.draw(m_openSliderButton);
    m_window.draw(m_openSliderText);

    // save UI
    if (m_showSaveBox)
    {
        m_window.draw(m_saveBox);
        m_window.draw(m_savePromptText);
        m_window.draw(m_fileNameBox);
        m_window.draw(m_fileNameText);
        m_window.draw(m_saveConfirmButton);
        m_window.draw(m_saveConfirmButtonText);
    }

    // Warning message
    if (m_showWarning)
    {
        m_window.draw(m_warningText);
    }
}

void LevelEditor::handleMouseClick(sf::Vector2f m_mousePos)
{
    if (m_showSaveBox)
    {
        handleSaveBoxClick(m_mousePos);
        return;
    }

    if (m_backButton.getGlobalBounds().contains(m_mousePos))
    {
        m_backButtonPressed = true;
        SoundManager::getInstance().playSound("buttonClick");
    }
    else if (m_saveButton.getGlobalBounds().contains(m_mousePos))
    {
        std::vector<BlockType> requiredAssets = { BlockType::PLAYER, BlockType::END_GATE };
        if (!hasEssentialAssets(requiredAssets))
        {
            if (!hasPlayer())
            {
                showWarning("Map must include a player to be saved");
            }
            else if (!hasEndGate())
            {
                showWarning("Map must include an end gate to be saved");
            }
        }
        else
        {
            m_showSaveBox = true;
        }
        SoundManager::getInstance().playSound("buttonClick");
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
            m_currentTab = TabType::MISCELLANEOUS;
        }
        else if (m_currentTab == TabType::MISCELLANEOUS)
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
            m_currentTab = TabType::MISCELLANEOUS;
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
        else if (m_currentTab == TabType::MISCELLANEOUS)
        {
            m_currentTab = TabType::ESSENTIALS;
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
        m_backButton.setFillColor(sf::Color(100, 100, 100));
    }
    else
    {
        m_backButtonHovered = false;
        m_backButton.setFillColor(sf::Color::Transparent);
    }

    // Save button hover effect
    if (m_saveButton.getGlobalBounds().contains(m_mousePos))
    {
        m_saveButtonHovered = true;
        m_saveButton.setFillColor(sf::Color(100, 100, 100));
    }
    else
    {
        m_saveButtonHovered = false;
        m_saveButton.setFillColor(sf::Color::Transparent);
    }

    // Open slider button hover effect
    if (m_openSliderButton.getGlobalBounds().contains(m_mousePos))
    {
        m_openSliderButtonHovered = true;
        m_openSliderButton.setFillColor(sf::Color(100, 100, 100));
    }
    else
    {
        m_openSliderButtonHovered = false;
        m_openSliderButton.setFillColor(sf::Color::Transparent);
    }

    // Next tab button hover effect
    if (m_nextTabButton.getGlobalBounds().contains(m_mousePos))
    {
        m_nextTabButtonHovered = true;
        m_nextTabButton.setFillColor(sf::Color(100, 100, 100));
    }
    else
    {
        m_nextTabButtonHovered = false;
        m_nextTabButton.setFillColor(sf::Color::Transparent);
    }

    // Previous tab button hover effect
    if (m_prevTabButton.getGlobalBounds().contains(m_mousePos))
    {
        m_prevTabButtonHovered = true;
        m_prevTabButton.setFillColor(sf::Color(100, 100, 100));
    }
    else
    {
        m_prevTabButtonHovered = false;
        m_prevTabButton.setFillColor(sf::Color::Transparent);
    }

    // Delete button hover effect
    if (m_deleteButton.getGlobalBounds().contains(m_mousePos))
    {
        m_deleteButtonHovered = true;
        m_deleteButton.setFillColor(sf::Color(100, 100, 100));
    }
    else
    {
        m_deleteButtonHovered = false;
        m_deleteButton.setFillColor(sf::Color::Transparent);
    }

    if (m_isBlockSelected)
    {
        float gridSize = 80.0f;
        float snappedX = std::floor(m_mousePos.x / gridSize) * gridSize + (gridSize / 2);
        float snappedY = std::floor(m_mousePos.y / gridSize) * gridSize + (gridSize / 2);
        m_ghostTile.setPosition(snappedX, snappedY);
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
    if (!m_dirtTexture.loadFromFile("Assets\\Images\\LevelEditor\\dirt01.png"))
    {
        std::cout << "Problem loading Dirt Texture" << std::endl;
    }
    if (!m_graniteTexture.loadFromFile("Assets\\Images\\LevelEditor\\granite.png"))
    {
        std::cout << "Problem loading granite Texture" << std::endl;
    }
    if (!m_stoneTexture.loadFromFile("Assets\\Images\\LevelEditor\\stone.png"))
    {
        std::cout << "Problem loading stone Texture" << std::endl;
    }
    if (!m_sandTexture.loadFromFile("Assets\\Images\\LevelEditor\\sand.png"))
    {
        std::cout << "Problem loading sand Texture" << std::endl;
    }
    // TRAPS
    if (!m_spikeTexture.loadFromFile("Assets\\Images\\LevelEditor\\spike.png"))
    {
        std::cout << "Problem loading Spike Texture" << std::endl;
    }
    if (!m_barrelTexture.loadFromFile("Assets\\Images\\LevelEditor\\barrel.png"))
    {
        std::cout << "Problem loading Barrel Texture" << std::endl;
    }
    // ENEMIES
    if (!m_skeletonTexture.loadFromFile("Assets\\Images\\LevelEditor\\skeletonIcon.png"))
    {
        std::cout << "Problem loading Basic Enemy Texture" << std::endl;
    }
    if (!m_evilEyeTexture.loadFromFile("Assets\\Images\\LevelEditor\\flyingEyeIcon.png"))
    {
        std::cout << "Problem loading Advanced Enemy Texture" << std::endl;
    }
    if (!m_goblinTexture.loadFromFile("Assets\\Images\\LevelEditor\\goblinIcon.png"))
    {
        std::cout << "Problem loading Advanced Enemy Texture" << std::endl;
    }
    if (!m_mushroomTexture.loadFromFile("Assets\\Images\\LevelEditor\\mushroomIcon.png"))
    {
        std::cout << "Problem loading Advanced Enemy Texture" << std::endl;
    }
    if (!m_demonBossTexture.loadFromFile("Assets\\Images\\LevelEditor\\demonBossIcon.png"))
    {
        std::cout << "Problem loading Advanced Enemy Texture" << std::endl;
    }
    // ESSENTIALS
    if (!m_playerTexture.loadFromFile("Assets\\Images\\LevelEditor\\player.png"))
    {
        std::cout << "Problem loading Player Texture" << std::endl;
    }
    if (!m_endGateTexture.loadFromFile("Assets\\Images\\LevelEditor\\portal.png"))
    {
        std::cout << "Problem loading End Gate Texture" << std::endl;
    }
    // MISCELLANEOUS
    if (!m_healthPackTexture.loadFromFile("Assets\\Images\\LevelEditor\\healthPack.png"))
    {
        std::cout << "Problem loading Health Pack Texture" << std::endl;
    }
    if (!m_ammoPackTexture.loadFromFile("Assets\\Images\\LevelEditor\\ammo.png"))
    {
        std::cout << "Problem loading Ammo Pack Texture" << std::endl;
    }
    if (!m_torchTexture.loadFromFile("Assets\\Images\\LevelEditor\\torch.png"))
    {
        std::cout << "Problem loading torch Texture" << std::endl;
    }
    if (!m_shopTexture.loadFromFile("Assets\\Images\\LevelEditor\\shop.png"))
    {
        std::cout << "Problem loading torch Texture" << std::endl;
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

void LevelEditor::initTileBlocks()
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

    setTileBlockPositions();

    m_ghostTile.setSize(sf::Vector2f(80.0f, 80.0f));
    m_ghostTile.setFillColor(sf::Color(255, 255, 255, 128));
    m_ghostTile.setOrigin(40, 40);
}

void LevelEditor::setTileBlockPositions()
{
    int tileIndex = 0;
    for (int row = 0; row < GRID_ROWS; ++row) 
    {
        for (int col = 0; col < GRID_COLS; ++col)
        {
            if (tileIndex < m_tileBlocks.size())
            {
                sf::FloatRect gridBounds = m_grid[row][col].getGlobalBounds();
                m_tileBlocks[tileIndex].setPosition
                (
                    gridBounds.left + gridBounds.width / 2,
                    gridBounds.top + gridBounds.height / 2
                );
                tileIndex++;
            }
        }
    }

    for (auto& block : m_tileBlocks)
    {
        sf::FloatRect bounds = block.getLocalBounds();
        block.setOrigin(bounds.width / 2, bounds.height / 2);
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
    // Or use setTileBlockPositions instead of the below code 
    int tileIndex = 0;
    for (int row = 0; row < GRID_ROWS; ++row) 
    {
        for (int col = 0; col < GRID_COLS; ++col) 
        {
            if (tileIndex < m_tileBlocks.size())
            {
                sf::FloatRect gridBounds = m_grid[row][col].getGlobalBounds();
                m_tileBlocks[tileIndex].setPosition
                (
                    gridBounds.left + gridBounds.width / 2,
                    gridBounds.top + gridBounds.height / 2
                );
                tileIndex++;
            }
        }
    }
    //setTileBlockPositions();

    if (m_isBlockSelected)
    {
        m_ghostTile.setRotation(m_tileBlocks[m_selectedBlockIndex].getRotation());
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
    else if (m_currentTab == TabType::MISCELLANEOUS)
    {
        m_tabNameText.setString("Miscellaneous");
    }
    std::cout << "Current tab: " << m_tabNameText.getString().toAnsiString() << std::endl;
}

void LevelEditor::updateTileBlocks()
{
    m_tileBlocks.clear();
    m_isBlockSelected = false;
    resetGhostTile();

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
        sf::RectangleShape enemySkeletonTile;
        enemySkeletonTile.setSize(sf::Vector2f(80.0f, 80.0f));
        enemySkeletonTile.setTexture(&m_skeletonTexture);
        m_tileBlocks.push_back(enemySkeletonTile);

        sf::RectangleShape enemyEvilEyeTile;
        enemyEvilEyeTile.setSize(sf::Vector2f(80.0f, 80.0f));
        enemyEvilEyeTile.setTexture(&m_evilEyeTexture);
        m_tileBlocks.push_back(enemyEvilEyeTile);

        sf::RectangleShape enemyGoblinTile;
        enemyGoblinTile.setSize(sf::Vector2f(80.0f, 80.0f));
        enemyGoblinTile.setTexture(&m_goblinTexture);
        m_tileBlocks.push_back(enemyGoblinTile);

        sf::RectangleShape mushroomTile;
        mushroomTile.setSize(sf::Vector2f(80.0f, 80.0f));
        mushroomTile.setTexture(&m_mushroomTexture);
        m_tileBlocks.push_back(mushroomTile);

        sf::RectangleShape demonBossTile;
        demonBossTile.setSize(sf::Vector2f(80.0f, 80.0f));
        demonBossTile.setTexture(&m_demonBossTexture);
        m_tileBlocks.push_back(demonBossTile);
    }
    else if (m_currentTab == TabType::ESSENTIALS)
    {
        sf::RectangleShape playerTile;
        playerTile.setSize(sf::Vector2f(80.0f, 80.0f));
        playerTile.setTexture(&m_playerTexture);
        m_tileBlocks.push_back(playerTile);

        sf::RectangleShape endGoal;
        endGoal.setSize(sf::Vector2f(80.0f, 80.0f));
        endGoal.setTexture(&m_endGateTexture);
        m_tileBlocks.push_back(endGoal);
    }
    else if (m_currentTab == TabType::MISCELLANEOUS)
    {
        sf::RectangleShape healthPackTile;
        healthPackTile.setSize(sf::Vector2f(80.0f, 80.0f));
        healthPackTile.setTexture(&m_healthPackTexture);
        m_tileBlocks.push_back(healthPackTile);

        sf::RectangleShape ammoPackTile;
        ammoPackTile.setSize(sf::Vector2f(80.0f, 80.0f));
        ammoPackTile.setTexture(&m_ammoPackTexture);
        m_tileBlocks.push_back(ammoPackTile);

        sf::RectangleShape torchTile;
        torchTile.setSize(sf::Vector2f(80.0f, 80.0f));
        torchTile.setTexture(&m_torchTexture);
        m_tileBlocks.push_back(torchTile);

        sf::RectangleShape shopTile;
        shopTile.setSize(sf::Vector2f(80.0f, 80.0f));
        shopTile.setTexture(&m_shopTexture);
        m_tileBlocks.push_back(shopTile);
    }

    setTileBlockPositions();
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
                m_isBlockSelected = false;
                resetGhostTile();
                std::cout << "Deselected block: " << m_tabNameText.getString().toAnsiString() << " #" << (i + 1) << std::endl;
                SoundManager::getInstance().playSound("buttonClick");
            }
            else
            {
                // Select the block
                m_selectedBlockType = getBlockTypeForCurrentTab(i);
                m_selectedBlockIndex = i;
                m_tileBlocks[i].setOutlineColor(sf::Color::Yellow);
                m_tileBlocks[i].setOutlineThickness(3.0f);
                m_isBlockSelected = true;
                
                auto texture = m_tileBlocks[i].getTexture();
                if (texture)
                {
                    m_ghostTile.setTexture(texture, true); // Use true to reset the texture rect
                }
                else 
                {
                    m_ghostTile.setTexture(nullptr);
                }

                // Deselect the delete button
                m_deleteMode = false;
                m_deleteButton.setOutlineColor(sf::Color::White);
                m_deleteButton.setOutlineThickness(2.0f);

                std::cout << "Selected block: " << m_tabNameText.getString().toAnsiString() << " #" << (i + 1) << std::endl;
                SoundManager::getInstance().playSound("buttonClick");
            }
        }
        else
        {
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
        float snappedX = std::floor(m_mousePos.x / gridSize) * gridSize + (gridSize / 2);
        float snappedY = std::floor(m_mousePos.y / gridSize) * gridSize + (gridSize / 2);

        // Check if a block already exists at this position and remove it
        auto it = std::remove_if(m_mapBlocks.begin(), m_mapBlocks.end(),
            [snappedX, snappedY](const Block& block) 
            {
                return block.shape.getPosition().x == snappedX && block.shape.getPosition().y == snappedY;
            });
        m_mapBlocks.erase(it, m_mapBlocks.end());

        Block newBlock;
        newBlock.type = m_selectedBlockType;
        newBlock.shape.setSize(sf::Vector2f(80, 80));
        newBlock.shape.setTexture(m_tileBlocks[m_selectedBlockIndex].getTexture());
        newBlock.shape.setRotation(m_tileBlocks[m_selectedBlockIndex].getRotation());
        newBlock.shape.setPosition(snappedX, snappedY);
        newBlock.shape.setOrigin(40, 40);

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
            newBlock.health = 120;
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
            newBlock.health = 60;
            newBlock.traversable = false;
            newBlock.damage = 15;
            break;
        case BlockType::WATER:
            newBlock.shape.setFillColor(sf::Color(0, 0, 255));
            newBlock.health = 100;
            newBlock.traversable = true;
            newBlock.damage = 0;
            break;
        case BlockType::LAVA:
            newBlock.shape.setFillColor(sf::Color(255, 69, 0));
            newBlock.health = 100;
            newBlock.traversable = true;
            newBlock.damage = 10;
            break;
        case BlockType::TRAP_SPIKE:
            newBlock.shape.setTexture(&m_spikeTexture);
            newBlock.health = 100;
            newBlock.traversable = true;
            newBlock.damage = 20;
            break;
        case BlockType::TRAP_BARREL:
            newBlock.shape.setTexture(&m_barrelTexture);
            newBlock.health = 100;
            newBlock.traversable = true;
            newBlock.damage = 0;
            break;
        case BlockType::SKELETON:
            newBlock.shape.setTexture(&m_skeletonTexture);
            newBlock.health = 100;
            newBlock.traversable = true;
            newBlock.damage = 5;
            break;
        case BlockType::EVIL_EYE:
            newBlock.shape.setTexture(&m_evilEyeTexture);
            newBlock.health = 100;
            newBlock.traversable = true;
            newBlock.damage = 10;
            break;
        case BlockType::GOBLIN:
            newBlock.shape.setTexture(&m_goblinTexture);
            newBlock.health = 100;
            newBlock.traversable = true;
            newBlock.damage = 14;
            break;
        case BlockType::MUSHROOM:
            newBlock.shape.setTexture(&m_mushroomTexture);
            newBlock.health = 100;
            newBlock.traversable = true;
            newBlock.damage = 8;
            break;
        case BlockType::ENEMY_DEMON_BOSS:
            newBlock.shape.setTexture(&m_demonBossTexture);
            newBlock.health = 100;
            newBlock.traversable = true;
            newBlock.damage = 25;
            break;
        case BlockType::HEALTH_PACK:
            newBlock.shape.setTexture(&m_healthPackTexture);
            newBlock.health = 100;
            newBlock.traversable = true;
            newBlock.damage = 0;
            break;
        case BlockType::AMMO_PACK:
            newBlock.shape.setTexture(&m_ammoPackTexture);
            newBlock.health = 100;
            newBlock.traversable = true;
            newBlock.damage = 0;
            break;
        case BlockType::TORCH:
            newBlock.shape.setTexture(&m_torchTexture);
            newBlock.health = 100;
            newBlock.traversable = true;
            newBlock.damage = 0;
            break;
        case BlockType::SHOP:
            newBlock.shape.setTexture(&m_shopTexture);
            newBlock.health = 100;
            newBlock.traversable = true;
            newBlock.damage = 0;
            break;
        case BlockType::PLAYER:
            newBlock.shape.setTexture(&m_playerTexture);
            newBlock.health = 100;
            newBlock.traversable = true;
            newBlock.damage = 0;
            break;
        case BlockType::END_GATE:
            newBlock.shape.setTexture(&m_endGateTexture);
            newBlock.health = 100;
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
    float snappedX = std::floor(m_mousePos.x / gridSize) * gridSize + (gridSize / 2);
    float snappedY = std::floor(m_mousePos.y / gridSize) * gridSize + (gridSize / 2);

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

void LevelEditor::toggleDeleteMode()
{
    m_deleteMode = !m_deleteMode;
    if (m_deleteMode)
    {
        m_deleteButton.setOutlineColor(sf::Color::Yellow);
        m_deleteButton.setOutlineThickness(3.0f);
        // Deselect the block in the slider
        if (m_selectedBlockIndex != -1) 
        {
            m_tileBlocks[m_selectedBlockIndex].setOutlineColor(sf::Color::Transparent);
            m_tileBlocks[m_selectedBlockIndex].setOutlineThickness(0.0f);
            m_selectedBlockType = BlockType::NONE;
            m_selectedBlockIndex = -1;
            m_isBlockSelected = false;
            resetGhostTile();
        }
        std::cout << "Delete mode enabled" << std::endl;
    }
    else
    {
        m_deleteButton.setOutlineColor(sf::Color::White);
        m_deleteButton.setOutlineThickness(2.0f);
        std::cout << "Delete mode disabled" << std::endl;
    }
}

void LevelEditor::rotateSelectedBlockLeft()
{
    if (m_selectedBlockIndex != -1) 
    {
        m_tileBlocks[m_selectedBlockIndex].rotate(-90);
    }
}

void LevelEditor::rotateSelectedBlockRight()
{
    if (m_selectedBlockIndex != -1) 
    {
        m_tileBlocks[m_selectedBlockIndex].rotate(90);
    }
}

void LevelEditor::drawWorldGrid(sf::RenderWindow& m_window)
{
    sf::Color gridColor(128, 128, 128);
    float gridSize = 80.0f;
    for (float x = 0; x < SCREEN_WIDTH; x += gridSize) 
    {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(x, 0), gridColor),
            sf::Vertex(sf::Vector2f(x, SCREEN_HEIGHT), gridColor)
        };
        m_window.draw(line, 2, sf::Lines);
    }
    for (float y = 0; y < SCREEN_HEIGHT; y += gridSize)
    {
        sf::Vertex line[] = 
        {
            sf::Vertex(sf::Vector2f(0, y), gridColor),
            sf::Vertex(sf::Vector2f(SCREEN_WIDTH, y), gridColor)
        };
        m_window.draw(line, 2, sf::Lines);
    }
}

void LevelEditor::resetGhostTile()
{
    m_ghostTile.setTexture(nullptr);
    m_ghostTile.setSize(sf::Vector2f(80.0f, 80.0f));
    m_ghostTile.setFillColor(sf::Color(255, 255, 255, 128));
    m_ghostTile.setRotation(0);
    m_ghostTile.setOrigin(40, 40);
}

void LevelEditor::setupSaveUI()
{
    // save box
    m_saveBox.setSize(sf::Vector2f(500.0f, 250.0f));
    m_saveBox.setFillColor(sf::Color(50, 50, 50, 200));
    m_saveBox.setOutlineColor(sf::Color::White);
    m_saveBox.setOutlineThickness(2.0f);
    m_saveBox.setPosition(SCREEN_WIDTH / 2 - 250.0f, SCREEN_HEIGHT / 2 - 100.0f);

    // save prompt text
    m_savePromptText.setFont(m_font);
    m_savePromptText.setString("Enter the file name below");
    m_savePromptText.setFillColor(sf::Color::White);
    m_savePromptText.setPosition(m_saveBox.getPosition().x + 50.0f, m_saveBox.getPosition().y + 20.0f);

    // file name box
    m_fileNameBox.setSize(sf::Vector2f(420.0f, 50.0f));
    m_fileNameBox.setFillColor(sf::Color::White);
    m_fileNameBox.setOutlineColor(sf::Color::Black);
    m_fileNameBox.setOutlineThickness(2.0f);
    m_fileNameBox.setPosition(m_saveBox.getPosition().x + 50.0f, m_saveBox.getPosition().y + 80.0f);

    // file name text
    m_fileNameText.setFont(m_font);
    m_fileNameText.setString("");
    m_fileNameText.setFillColor(sf::Color::Black);
    m_fileNameText.setPosition(m_saveBox.getPosition().x + 50.0f, m_saveBox.getPosition().y + 90.0f);

    // save confirm button
    m_saveConfirmButton.setSize(sf::Vector2f(100.0f, 50.0f));
    m_saveConfirmButton.setFillColor(sf::Color::Transparent);
    m_saveConfirmButton.setOutlineColor(sf::Color::Red);
    m_saveConfirmButton.setOutlineThickness(2.0f);
    m_saveConfirmButton.setPosition(m_saveBox.getPosition().x + 200.0f, m_saveBox.getPosition().y + 160.0f);

    // save confirm button text
    m_saveConfirmButtonText.setFont(m_font);
    m_saveConfirmButtonText.setString("Save");
    m_saveConfirmButtonText.setFillColor(sf::Color::White);
    m_saveConfirmButtonText.setPosition(m_saveConfirmButton.getPosition().x + 10.0f, m_saveConfirmButton.getPosition().y + 5.0f);

    // warning text
    m_warningText.setFont(m_font);
    m_warningText.setFillColor(sf::Color::Red);
    m_warningText.setOutlineColor(sf::Color::Yellow);
    m_warningText.setOutlineThickness(1);
    m_warningText.setCharacterSize(30);
    m_warningText.setPosition(SCREEN_WIDTH / 2, 50);
}

void LevelEditor::handleSaveBoxClick(sf::Vector2f m_mousePos)
{
    if (m_saveConfirmButton.getGlobalBounds().contains(m_mousePos) && !m_fileNameInput.empty())
    {
        std::vector<BlockType> requiredAssets = { BlockType::PLAYER, BlockType::END_GATE };
        if (!hasEssentialAssets(requiredAssets))
        {
            if (!hasPlayer())
            {
                showWarning("Map must include a <player> to be saved");
            }
            else if (!hasEndGate())
            {
                showWarning("Map must include an <end gate> to be saved");
            }
        }
        else
        {
            saveToFile(m_fileNameInput);
            m_showSaveBox = false;
            m_fileNameInput.clear();
            m_fileNameText.setString("");
        }
    }
}

void LevelEditor::handleTextInput(sf::Event m_event)
{
    if (m_showSaveBox)
    {
        if (m_event.type == sf::Event::TextEntered)
        {
            if (m_event.text.unicode == '\b') // Handle backspace
            {
                if (!m_fileNameInput.empty())
                {
                    m_fileNameInput.pop_back();
                }
            }
            else if (m_event.text.unicode < 128) // Handle ASCII characters
            {
                m_fileNameInput += static_cast<char>(m_event.text.unicode);
            }
            m_fileNameText.setString(m_fileNameInput);
            updateSaveUI();
        }
    }
}

void LevelEditor::handleKeyInput(sf::Event m_event)
{
    if (m_event.type == sf::Event::KeyPressed) 
    {
        if (m_event.key.code == sf::Keyboard::Q) 
        {
            rotateSelectedBlockLeft();
        }
        else if (m_event.key.code == sf::Keyboard::E)
        {
            rotateSelectedBlockRight();
        }
    }
}

void LevelEditor::saveToFile(const std::string& m_fileName)
{
    std::string filePath = "Assets\\SaveFiles\\" + m_fileName + ".json";
    std::ofstream outFile(filePath);

    if (!outFile.is_open())
    {
        std::cout << "Could not open file for writing: " << filePath << std::endl;
        showWarning("Failed to save map!");
        return;
    }

    std::ostringstream jsonStream;
    jsonStream << "{\n";
    jsonStream << "  \"blocks\":\n";
    jsonStream << "  [\n";

    for (int i = 0; i < m_mapBlocks.size(); ++i)
    {
        const auto& block = m_mapBlocks[i];
        jsonStream << "    {\n";
        jsonStream << "      \"type\": " << static_cast<int>(block.type) << ",\n";
        jsonStream << "      \"health\": " << block.health << ",\n";
        jsonStream << "      \"damage\": " << block.damage << ",\n";
        jsonStream << "      \"traversable\": " << (block.traversable ? "true" : "false") << ",\n";
        jsonStream << "      \"position\":\n";
        jsonStream << "      {\n";
        jsonStream << "        \"x\": " << block.shape.getPosition().x << ",\n";
        jsonStream << "        \"y\": " << block.shape.getPosition().y << "\n";
        jsonStream << "      }\n";
        jsonStream << "    }";
        if (i < m_mapBlocks.size() - 1)
        {
            jsonStream << ",";
        }
        jsonStream << "\n";
    }

    jsonStream << "  ]\n";
    jsonStream << "}";

    outFile << jsonStream.str();
    outFile.close();

    std::cout << "Map saved to " << filePath << std::endl;
    showWarning("Map saved successfully!");
}

void LevelEditor::showWarning(const std::string& m_message)
{
    m_warningText.setString(m_message);
    m_warningText.setPosition(SCREEN_WIDTH / 2 - m_warningText.getGlobalBounds().width / 2, 100);
    m_showWarning = true;
    m_warningDisplayTime = sf::Time::Zero;
}

void LevelEditor::updateWarning(sf::Time t_deltaTime)
{
    if (m_showWarning)
    {
        m_warningDisplayTime += t_deltaTime;
        if (m_warningDisplayTime > m_warningDuration)
        {
            m_showWarning = false;
        }
        else
        {
            float alpha = 255 * (1 - m_warningDisplayTime.asSeconds() / m_warningDuration.asSeconds());
            m_warningText.setFillColor(sf::Color(255, 0, 0, static_cast<sf::Uint8>(alpha)));
            m_warningText.move(0, -30 * t_deltaTime.asSeconds());
        }
    }
}

void LevelEditor::updateSaveUI()
{
    if (!m_fileNameInput.empty())
    {
        m_fileNameBox.setOutlineColor(sf::Color::Green);
        m_saveConfirmButton.setOutlineColor(sf::Color::Green);
        m_saveConfirmButtonText.setFillColor(sf::Color::Green);
    }
    else
    {
        m_fileNameBox.setOutlineColor(sf::Color::Red);
        m_saveConfirmButton.setOutlineColor(sf::Color::Red);
        m_saveConfirmButtonText.setFillColor(sf::Color::Red);
    }
}

bool LevelEditor::hasEssentialAssets(const std::vector<BlockType>& m_requiredAssets) const
{
    std::unordered_set<BlockType> presentAssets;

    for (const auto& block : m_mapBlocks)
    {
        presentAssets.insert(block.type);
    }

    for (const auto& required : m_requiredAssets)
    {
        if (presentAssets.find(required) == presentAssets.end())
        {
            return false;
        }
    }

    return true;
}

bool LevelEditor::hasPlayer() const
{
    for (const auto& block : m_mapBlocks)
    {
        if (block.type == BlockType::PLAYER)
        {
            return true;
        }
    }
    return false;
}

bool LevelEditor::hasEndGate() const
{
    for (const auto& block : m_mapBlocks)
    {
        if (block.type == BlockType::END_GATE)
        {
            return true;
        }
    }
    return false;
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
        case 0: return BlockType::SKELETON;
        case 1: return BlockType::EVIL_EYE;
        case 2: return BlockType::GOBLIN;
        case 3: return BlockType::MUSHROOM;
        case 4: return BlockType::ENEMY_DEMON_BOSS;
        default: return BlockType::NONE;
        }
    case TabType::ESSENTIALS:
        switch (m_index)
        {
        case 0: return BlockType::PLAYER;
        case 1: return BlockType::END_GATE;
        default: return BlockType::NONE;
        }
    case TabType::MISCELLANEOUS:
        switch (m_index)
        {
        case 0: return BlockType::HEALTH_PACK;
        case 1: return BlockType::AMMO_PACK;
        case 2: return BlockType::TORCH;
        case 3: return BlockType::SHOP;
        default: return BlockType::NONE;
        }
    default:
        break;
    }
}
