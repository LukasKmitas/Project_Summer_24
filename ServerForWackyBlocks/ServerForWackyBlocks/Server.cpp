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
        std::cout << "Failed to bind listener socket to port " << PORT << std::endl;
        return;
    }
    std::cout << "Server is starting..." << std::endl;
    std::cout << "Server is listening on port: " << PORT << std::endl;
    std::cout << "Press 'i' for more information" << std::endl;

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
        else if (command == "i")
        {
            displayInfo();
        }
        else if (command == "1")
        {
            displayAllClients();
        }
        else if (command == "2")
        {
            displayClientSessions();
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
            std::cout << "##       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~       ##" << std::endl;
            std::cout << "##             NEW CLIENT CONNECTED             ##" << std::endl;
            std::cout << "##    Address: " << client->getRemoteAddress() << " | Port: " << client->getRemotePort() << "      ##" << std::endl;
            std::cout << "##       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~       ##" << std::endl;
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

    while (m_client->receive(buffer, sizeof(buffer), received) == sf::Socket::Done) 
    {
        std::string request(buffer, received);

        if (request == "init")
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::string randomID = generateRandomID();
            Session session = { m_client->getRemoteAddress(), randomID, m_client, nullptr };
            m_sessions[randomID] = session;
            m_client->send(randomID.c_str(), randomID.size());
        }
        else if (request == "host") 
        {
            notifyServerOfNewHost(m_client);
        }
        else if (request == "searchForGame") 
        {
            sendAvailableSessions(m_client);
        }
        else if (request.substr(0, 5) == "join:")
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto tokens = split(request, ':');
            if (tokens.size() == 3)
            {
                std::string hostID = tokens[1];
                std::string clientID = tokens[2];
                auto it = m_sessions.find(hostID);
                if (it != m_sessions.end())
                {
                    it->second.m_playerTwoSocket = m_client; // Update Host session to have the second players (join person) TCP

                    // Find the joining clients session and update it with the hosts socket TCP
                    auto clientIt = m_sessions.find(clientID);
                    if (clientIt != m_sessions.end())
                    {
                        clientIt->second.m_playerTwoSocket = it->second.m_playerOneSocket;
                    }
                    else
                    {
                        std::cout << "Client session not found: " << clientID << std::endl;
                    }

                    notifyHostPlayerHasJoined(hostID, clientID);
                }
                else
                {
                    std::cout << "Host session not found: " << hostID << std::endl;
                }
            }
        }
        else if (request.substr(0, 14) == "levelSelected:")
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::string levelName = request.substr(14);
            for (auto& session : m_sessions)
            {
                if (session.second.m_playerOneSocket == m_client)
                {
                    notifyClients(session.first, levelName);
                    break;
                }
            }
        }
        else if (request.substr(0, 13) == "playerUpdate:")
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::string sessionID;
            for (const auto& pair : m_sessions)
            {
                if (pair.second.m_playerOneSocket == m_client)
                {
                    if (pair.second.m_playerTwoSocket != nullptr)
                    {
                        pair.second.m_playerTwoSocket->send(request.c_str(), request.size());
                    }
                    break;
                }
                else if (pair.second.m_playerTwoSocket == m_client)
                {
                    if (pair.second.m_playerOneSocket != nullptr)
                    {
                        pair.second.m_playerOneSocket->send(request.c_str(), request.size());
                    }
                    break;
                }
            }
        }
    }
}

void Server::notifyServerOfNewHost(sf::TcpSocket* m_client)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string hostID;
    for (const auto& pair : m_sessions)
    {
        if (pair.second.m_playerOneSocket == m_client)
        {
            hostID = pair.first;
            break;
        }
    }

    if (!hostID.empty())
    {
        std::cout << "New host with ID: " << hostID << " waiting for a player to join" << std::endl;
    }
    else
    {
        std::cout << "No host found" << std::endl;
    }
}

void Server::sendAvailableSessions(sf::TcpSocket* m_client)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string sessions;
    std::string clientID;

    // Find the clientID based on the m_client socket
    for (const auto& pair : m_sessions)
    {
        if (pair.second.m_playerOneSocket == m_client || pair.second.m_playerTwoSocket == m_client)
        {
            clientID = pair.first;
            break;
        }
    }

    // Create the list of available sessions excluding the clients own session
    for (const auto& session : m_sessions) 
    {
        if (session.first != clientID)
        {
            sessions += session.second.m_name + " (" + session.second.m_address.toString() + ")\n";
        }
    }

    if (sessions.empty())
    {
        sessions = "No available sessions found";
    }

    std::cout << "Sending sessions to client(s):\n" << sessions << std::endl;
    m_client->send(sessions.c_str(), sessions.size());
}

void Server::notifyHostPlayerHasJoined(const std::string& m_hostID, const std::string& clientID)
{
    auto it = m_sessions.find(m_hostID);
    if (it != m_sessions.end())
    {
        auto& session = it->second;
        if ((session.m_playerOneSocket != nullptr) && (session.m_playerTwoSocket != nullptr))
        {
            std::string notification = "playerJoined:" + clientID;
            if (session.m_playerOneSocket->send(notification.c_str(), notification.size()) != sf::Socket::Done)
            {
                std::cout << "Failed to send notification to host." << std::endl;
            }
            std::cout << "Player " << clientID << " has joined host " << m_hostID << std::endl;
        }
        else
        {
            std::cout << "Host or client socket is null for session: " << m_hostID << std::endl;
        }
    }
    else
    {
        std::cout << "Session not found: " << m_hostID << std::endl;
    }
}

void Server::notifyClients(const std::string& m_sessionID, const std::string& m_levelName)
{
    std::string message = "levelSelected:" + m_levelName;
    auto& session = m_sessions[m_sessionID];
    if (session.m_playerTwoSocket)
    {
        session.m_playerTwoSocket->send(message.c_str(), message.size());
    }
}

void Server::displayInfo()
{
    std::cout << "Server Commands:\n";
    std::cout << "1: Display all clients\n";
    std::cout << "2: Display number of multiplayer games and participants\n";
    std::cout << "exit: Stop the server\n";
}

void Server::displayAllClients()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "All clients connected:\n";
    for (const auto& session : m_sessions)
    {
        std::cout << "Client ID: " << session.second.m_name << " IP: " << session.second.m_address.toString() << std::endl;
    }
}

void Server::displayClientSessions()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "Multiplayer games being played currently:\n";
    int multiplayerGames = 0;

    for (const auto& session : m_sessions)
    {
        if (session.second.m_playerTwoSocket != nullptr)
        {
            std::string hostID = session.first;
            std::string clientID;

            // Find the client ID based on the playerTwoSocket
            for (const auto& clientSession : m_sessions)
            {
                if (clientSession.second.m_playerOneSocket == session.second.m_playerTwoSocket)
                {
                    clientID = clientSession.second.m_name;
                    break;
                }
            }

            if (!clientID.empty())
            {
                std::cout << "Player 1: " << session.second.m_name << " (Host) <- Player 2: " << clientID << " (Client)" << std::endl;
                multiplayerGames++;
            }
        }
    }

    std::cout << "There are " << multiplayerGames << " multiplayer games being played currently." << std::endl;
}

std::string Server::generateRandomID()
{
    const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int len = 6;
    std::string id;
    id.reserve(len);

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distrib(0, sizeof(alphanum) - 2);

    for (int i = 0; i < len; ++i)
    {
        id += alphanum[distrib(generator)];
    }

    return id;
}

std::vector<std::string> Server::split(const std::string& m_string, char m_delimiter)
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