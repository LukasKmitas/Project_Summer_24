#pragma once
#ifndef LEVELEDITOR_H
#define LEVELEDITOR_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include "Global.h"
#include "SoundManager.h"
#include "Block.h"

enum class TabType
{
    BLOCKS,
    TRAPS,
    ENEMIES,
    ESSENTIALS,
    PICKUPS
};

class LevelEditor
{
public:

    LevelEditor();
    ~LevelEditor();

    void update(sf::Time t_deltaTime);
    void render(sf::RenderWindow& m_window);

    void handleMouseClick(sf::Vector2f m_mousePos);
    void handleMouseHover(sf::Vector2f m_mousePos);
    void handleMouseRelease();
    void handleTextInput(sf::Event m_event);

    void reset();
    bool isBackButtonPressed() const;

private:

    void loadTextures();
    void setupUI();
    void setupGrid();
    void initTileBlocks();
    void toggleSlider();
    void updateSliderPosition(sf::Time t_deltaTime);
    void updateTabName();
    void updateTileBlocks();
    void selectBlock(sf::Vector2f m_mousePos);
    void placeBlock(sf::Vector2f m_mousePos);
    void deleteBlock(sf::Vector2f m_mousePos);
    void toggleDeleteMode();

    void setupSaveUI();
    void handleSaveBoxClick(sf::Vector2f m_mousePos);
    void saveToFile(const std::string& m_fileName);
    void showWarning(const std::string& m_message);
    void updateWarning(sf::Time t_deltaTime);
    void updateSaveUI();
    bool hasEssentialAssets(const std::vector<BlockType>& m_requiredAssets) const;
    bool hasPlayer() const;
    bool hasEndGate() const;

    TabType m_currentTab;
    BlockType getBlockTypeForCurrentTab(int m_index) const;

    sf::Font m_font;
    sf::Text m_openSliderText;
    sf::Text m_tabNameText;
    sf::Text m_nextTabText;
    sf::Text m_prevTabText;
    sf::Text m_backButtonText;
    sf::Text m_saveButtonText;
    sf::Text m_deleteButtonText;

    sf::RectangleShape m_openSliderButton;
    sf::RectangleShape m_sliderPanel;
    sf::RectangleShape m_nextTabButton;
    sf::RectangleShape m_prevTabButton;
    sf::RectangleShape m_backButton;
    sf::RectangleShape m_saveButton;
    sf::RectangleShape m_deleteButton;

    float m_sliderPosition;

    bool m_backButtonPressed = false;
    bool m_saveButtonPressed = false;
    bool m_backButtonHovered = false;
    bool m_saveButtonHovered = false;
    bool m_openSliderButtonHovered = false;
    bool m_nextTabButtonHovered = false;
    bool m_prevTabButtonHovered = false;
    bool m_deleteButtonHovered = false;
    bool m_sliderOpen = false;
    bool m_deleteMode = false;

    static const int GRID_ROWS = 8;
    static const int GRID_COLS = 4;
    sf::RectangleShape m_grid[GRID_ROWS][GRID_COLS];
    std::vector<sf::RectangleShape> m_tileBlocks;

    // World blocks
    sf::Texture m_dirtTexture;
    sf::Texture m_graniteTexture;
    sf::Texture m_stoneTexture;
    sf::Texture m_sandTexture;
    // Traps blocks
    sf::Texture m_spikeTexture;
    sf::Texture m_barrelTexture;
    // Enemy blocks
    sf::Texture m_skeletonTexture;
    sf::Texture m_evilEyeTexture;
    sf::Texture m_goblinTexture;
    sf::Texture m_mushroomTexture;
    sf::Texture m_demonBossTexture;
    // Essetial blocks
    sf::Texture m_playerTexture;
    sf::Texture m_endGateTexture;
    // Misc blocks
    sf::Texture m_healthPackTexture;
    sf::Texture m_ammoPackTexture;

    std::vector<Block> m_mapBlocks;
    BlockType m_selectedBlockType = BlockType::NONE;
    int m_selectedBlockIndex = -1;

    // UI components for saving the map
    sf::RectangleShape m_saveBox;
    sf::RectangleShape m_fileNameBox;
    sf::RectangleShape m_saveConfirmButton;
    sf::Text m_savePromptText;
    sf::Text m_fileNameText;
    sf::Text m_saveConfirmButtonText;
    sf::Text m_warningText;

    sf::Time m_warningDisplayTime;
    sf::Time m_warningDuration = sf::seconds(3);

    std::string m_fileNameInput;
    bool m_showSaveBox = false;
    bool m_showWarning = false;

};

#endif
