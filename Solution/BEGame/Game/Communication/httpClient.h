#pragma once
#include <string>
#include <future>

// CURLのレスポンス受け取り用コールバック関数
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

std::future<std::string> GetAllFacultiesAsync();
std::future<std::string> GetFacultyByIdAsync(int id);
std::future<std::string> PostFacultyAsync(const std::string& name);
std::future<std::string> PatchFacultyAsync(int id, const std::string& newName);
std::future<std::string> DeleteFacultyAsync(int id);

std::future<std::string> RegisterUserAsync(const std::string& name, const std::string& password);
std::future<std::string> LoginAsync(const std::string& name, const std::string& password);

std::future<std::string> PostScoreAsync(int score, const std::string& token);
std::future<std::string> GetAllScoresAsync(const std::string& token);

class TokenManager {
public:
    static std::string& GetToken() {
        static std::string token;
        return token;
    }
};