#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <iostream>
#include <string>
#include <thread>
#include <regex>
#include <map>
#include <mutex>
#include <netinet/in.h>
#include <unistd.h>
#include <array>

#define SERVER_PORT 8080

extern std::mutex dbMutex;
extern std::map<int, std::string> database;

std::array<std::string, 3> parseRequest(const std::string &requestLine);
int extractId(const std::string &path);
int initServer(int port);
std::string handleGet(int id);
std::string handlePost(const std::string &body, int requestedId = -1);
std::string handlePut(int id, const std::string &body);
std::string handleDelete(int id);
void handleClient(int clientSocket);

#endif