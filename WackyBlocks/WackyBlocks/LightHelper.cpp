#include "LightHelper.h"

std::vector<Edge> calculateEdges(const std::vector<sf::RectangleShape>& m_shapes)
{
    std::vector<Edge> edges;
    for (const auto& shape : m_shapes)
    {
        sf::FloatRect bounds = shape.getGlobalBounds();
        edges.push_back({ sf::Vector2f(bounds.left, bounds.top), sf::Vector2f(bounds.left + bounds.width, bounds.top) });
        edges.push_back({ sf::Vector2f(bounds.left + bounds.width, bounds.top), sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height) });
        edges.push_back({ sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height), sf::Vector2f(bounds.left, bounds.top + bounds.height) });
        edges.push_back({ sf::Vector2f(bounds.left, bounds.top + bounds.height), sf::Vector2f(bounds.left, bounds.top) });
    }
    return edges;
}

sf::Vector2f normalize(const sf::Vector2f& m_vec)
{
    float length = std::sqrt(m_vec.x * m_vec.x + m_vec.y * m_vec.y);
    return sf::Vector2f(m_vec.x / length, m_vec.y / length);
}

std::vector<sf::Vertex> calculateLightPolygon(const sf::Vector2f& m_lightPos, const std::vector<Edge>& m_edges, float m_lightRadius)
{
    std::vector<sf::Vertex> vertices;
    std::vector<sf::Vector2f> points;

    for (const auto& edge : m_edges)
    {
        points.push_back(edge.m_start);
        points.push_back(edge.m_end);
    }

    // Add the four corners of the screen
    points.push_back(sf::Vector2f(0, 0));
    points.push_back(sf::Vector2f(SCREEN_WIDTH, 0));
    points.push_back(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
    points.push_back(sf::Vector2f(0, SCREEN_HEIGHT));

    std::vector<std::pair<sf::Vector2f, float>> directions;
    for (const auto& point : points)
    {
        sf::Vector2f direction = normalize(point - m_lightPos);
        directions.push_back({ direction, std::atan2(direction.y, direction.x) });
    }

    std::sort(directions.begin(), directions.end(),
        [](const std::pair<sf::Vector2f, float>& a, const std::pair<sf::Vector2f, float>& b)
        {
            return a.second < b.second;
        });

    vertices.push_back(sf::Vertex(m_lightPos, sf::Color(255, 255, 255, 255)));
    for (const auto& dir : directions)
    {
        sf::Vector2f farPoint = m_lightPos + dir.first * m_lightRadius;
        vertices.push_back(sf::Vertex(farPoint, sf::Color(255, 255, 200, 0)));
    }

    // Add the first point again to close the fan
    vertices.push_back(vertices[1]);

    return vertices;
}
