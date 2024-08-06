#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Global.h"

struct Edge
{
    sf::Vector2f m_start;
    sf::Vector2f m_end;
};

std::vector<Edge> calculateEdges(const std::vector<sf::RectangleShape>& m_shapes);
sf::Vector2f normalize(const sf::Vector2f& m_vec);
std::vector<sf::Vertex> calculateLightPolygon(const sf::Vector2f& m_lightPos, float m_lightRadius, float m_intensity, const std::vector<Edge>& m_edges);
std::vector<sf::Vertex> calculateShadowPolygon(const sf::Vector2f& m_lightPos, const sf::RectangleShape& m_block, float m_shadowDistance, float m_lightRadius, const sf::Vector2f& playerLightPos, float playerRadius);
