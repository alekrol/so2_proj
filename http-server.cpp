#include "http-server.h"

std::mutex dbMutex;
std::map<int, std::string> database;

std::array<std::string, 3> parseRequest(const std::string &requestLine)
{
    std::regex requestRegex(R"((\w+)\s+([^\s]+)\s+(HTTP/\d\.\d))");
    std::smatch match;
    if (std::regex_search(requestLine, match, requestRegex))
    {
        return {match[1], match[2], match[3]};
    }
    return {"", "", ""};
}

int extractId(const std::string &path)
{
    std::regex idRegex(R"(/resource/(\d+))");
    std::smatch match;
    if (std::regex_match(path, match, idRegex))
    {
        return std::stoi(match[1]);
    }
    return -1;
}

int initServer(int port)
{
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(serverFd, reinterpret_cast<sockaddr *>(&address), sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(serverFd, 10) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    return serverFd;
}

std::string handleGet(int id)
{
    std::lock_guard lock(dbMutex);
    if (database.find(id) != database.end())
    {
        return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n" + database[id];
    }
    else
    {
        return "HTTP/1.1 404 Not Found\r\n\r\n";
    }
}

std::string handlePost(const std::string &body, int requestedId)
{
    std::lock_guard lock(dbMutex);

    int newId = (requestedId > 0) ? requestedId : database.size() + 1;

    if (database.find(newId) != database.end())
    {
        return "HTTP/1.1 409 Conflict\r\n\r\nResource with this ID already exists";
    }

    database[newId] = body;

    std::string responseBody = "ID: " + std::to_string(newId) + " " + body;

    std::string response =
        "HTTP/1.1 201 Created\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " +
        std::to_string(responseBody.length()) + "\r\n\r\n" +
        responseBody;

    return response;
}

std::string handlePut(int id, const std::string &body)
{
    std::lock_guard lock(dbMutex);
    database[id] = body;
    return "HTTP/1.1 200 OK\r\n\r\nUpdated";
}

std::string handleDelete(int id)
{
    std::lock_guard lock(dbMutex);
    if (database.erase(id))
    {
        return "HTTP/1.1 200 OK\r\n\r\nDeleted";
    }
    else
    {
        return "HTTP/1.1 404 Not Found\r\n\r\n";
    }
}

void handleClient(int clientSocket)
{
    char buffer[4096] = {};
    read(clientSocket, buffer, sizeof(buffer));
    std::string request(buffer);

    auto firstLineEnd = request.find("\r\n");
    if (firstLineEnd == std::string::npos)
    {
        close(clientSocket);
        return;
    }

    std::string requestLine = request.substr(0, firstLineEnd);
    auto parts = parseRequest(requestLine);
    std::string method = parts[0];
    std::string path = parts[1];
    std::string version = parts[2];

    std::string body = request.substr(request.find("\r\n\r\n") + 4);
    std::string response;

    if (method == "POST")
    {
        if (path == "/resource")
        {
            response = handlePost(body);
        }
        else
        {
            int id = extractId(path);
            if (id != -1)
            {
                response = handlePost(body, id);
            }
            else
            {
                response = "HTTP/1.1 400 Bad Request\r\n\r\n";
            }
        }
    }
    else
    {
        int id = extractId(path);
        if (id != -1)
        {
            if (method == "GET")
            {
                response = handleGet(id);
            }
            else if (method == "PUT")
            {
                response = handlePut(id, body);
            }
            else if (method == "DELETE")
            {
                response = handleDelete(id);
            }
            else
            {
                response = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
            }
        }
        else
        {
            response = "HTTP/1.1 400 Bad Request\r\n\r\n";
        }
    }

    send(clientSocket, response.c_str(), response.length(), 0);
    close(clientSocket);
}

int main()
{
    int serverFd = initServer(SERVER_PORT);

    while (true)
    {
        sockaddr_in clientAddr{};
        socklen_t addrLen = sizeof(clientAddr);
        int clientSocket = accept(serverFd, reinterpret_cast<sockaddr *>(&clientAddr), &addrLen);
        if (clientSocket >= 0)
        {
            std::thread(handleClient, clientSocket).detach();
        }
    }

    return 0;
}
// g++ -std=c++17 -pthread http-server.cpp -o http-server