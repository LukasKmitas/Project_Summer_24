#include "Server.h"

Server::Server()
{
}

Server::~Server()
{
}

void Server::run()
{
    if (m_listener.listen(PORT) != sf::Socket::Done)
    {
        std::cerr << "Failed to bind listener socket to port " << PORT << std::endl;
        return;
    }
    std::cout << "Server is listening on port " << PORT << std::endl;

    std::thread connectionThread(&Server::handleConnections, this);
    connectionThread.detach();

    std::string command;
    while (m_running)
    {
        std::cin >> command;
        if (command == "exit")
        {
            m_running = false;
            m_listener.close();
            break;
        }
    }
}

void Server::handleConnections()
{
    while (m_running)
    {
        std::unique_ptr<sf::TcpSocket> client = std::make_unique<sf::TcpSocket>();
        if (m_listener.accept(*client) == sf::Socket::Done)
        {
            std::cout << "New client connected: " << client->getRemoteAddress() << std::endl;
            std::thread clientThread(&Server::handleClient, this, client.get());
            clientThread.detach();

            std::lock_guard<std::mutex> lock(m_mutex);
            m_clients.push_back(std::move(client));
        }
    }
}

void Server::handleClient(sf::TcpSocket* m_client)
{
    char buffer[128];
    std::size_t received;
    if (m_client->receive(buffer, sizeof(buffer), received) == sf::Socket::Done)
    {
        std::string request(buffer, received);
        if (request == "host")
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            Session session = { m_client->getRemoteAddress(), "Host_" + std::to_string(m_sessions.size() + 1) };
            m_sessions.push_back(session);
        }
        else if (request == "join")
        {
            sendAvailableSessions(m_client);
        }
    }
}

void Server::sendAvailableSessions(sf::TcpSocket* m_client)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string sessions;
    for (const auto& session : m_sessions)
    {
        sessions += session.m_name + " (" + session.m_address.toString() + ")\n";
    }
    m_client->send(sessions.c_str(), sessions.size());
}
