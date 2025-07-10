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

std::future<std::string> PostScoreAsync(int score);
std::future<std::string> GetAllScoresAsync();