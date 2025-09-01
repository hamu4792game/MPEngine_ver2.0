#include "httpClient.h"
#include "externals/curl/include/curl/curl.h"
#include <sstream>
#include <iostream>
#include "externals/nlohmann/json.hpp"

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

std::future<std::string> GetAllFacultiesAsync() {
	return std::async(std::launch::async, []() -> std::string {
		CURL* curl = curl_easy_init();
		if (!curl) return "CURL初期化エラー";

		std::string response;
		curl_easy_setopt(curl, CURLOPT_URL, "https://swgame-83u3aeo1i-hamus-projects-531e648e.vercel.app");
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			return std::string("取得エラー: ") + curl_easy_strerror(res);
		}
		return response;
		});
}

std::future<std::string> GetFacultyByIdAsync(int id) {
	return std::async(std::launch::async, [id]() -> std::string {
		CURL* curl = curl_easy_init();
		if (!curl) return "CURL初期化エラー";

		std::string response;
		std::stringstream url;
		url << "https://swgame-83u3aeo1i-hamus-projects-531e648e.vercel.app/" << id;

		curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			return std::string("取得エラー: ") + curl_easy_strerror(res);
		}
		return response;
		});
}

std::future<std::string> PostFacultyAsync(const std::string& name) {
	return std::async(std::launch::async, [name]() -> std::string {
		CURL* curl = curl_easy_init();
		if (!curl) return "CURL初期化エラー";
		
		// 送信するJSONボディの作成
		json body = {
			{"name",name}
		};
		std::string bodyStr = body.dump();

		// ヘッダ設定
		struct curl_slist* headers = nullptr;
		headers = curl_slist_append(headers, "Content-Type: application/json");

		std::string response;
		curl_easy_setopt(curl, CURLOPT_URL, "https://swgame-83u3aeo1i-hamus-projects-531e648e.vercel.app");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			return std::string("取得エラー: ") + curl_easy_strerror(res);
		}
		return response;

		});
}

std::future<std::string> PatchFacultyAsync(int id, const std::string& newName) {
	return std::async(std::launch::async, [id, newName]() -> std::string {
		CURL* curl = curl_easy_init();
		if (!curl) return "CURL初期化エラー";
		
		std::string response;
		std::stringstream url;
		url << "https://swgame-83u3aeo1i-hamus-projects-531e648e.vercel.app/" << id;
		
		// JSONデータ構築
		nlohmann::json requestBody = {
			{"name", newName}
		};
		std::string requestStr = requestBody.dump();

		// ヘッダ設定
		struct curl_slist* headers = nullptr;
		headers = curl_slist_append(headers, "Content-Type: application/json");

		curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestStr.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			return std::string("取得エラー: ") + curl_easy_strerror(res);
		}
		return response;

		});
}

std::future<std::string> DeleteFacultyAsync(int id) {
	return std::async(std::launch::async, [id]() -> std::string {
		CURL* curl = curl_easy_init();
		if (!curl) return "CURL初期化エラー";

		std::string response;
		std::stringstream url;
		url << "https://swgame-83u3aeo1i-hamus-projects-531e648e.vercel.app/" << id;

		curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			return std::string("取得エラー: ") + curl_easy_strerror(res);
		}
		return response;
		});
}


// 共通: ヘッダにトークンをつけるためのヘルパー
struct curl_slist* MakeAuthHeaders(const std::string& token, bool isJson = true) {
	struct curl_slist* headers = nullptr;
	if (isJson) {
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "Accept: application/json");
	}
	if (!token.empty()) {
		std::string authHeader = "Authorization: Bearer " + token;
		headers = curl_slist_append(headers, authHeader.c_str());
	}
	return headers;
}

std::future<std::string> RegisterUserAsync(const std::string& name, const std::string& password) {
	return std::async(std::launch::async, [name, password]() -> std::string {
		CURL* curl = curl_easy_init();
		if (!curl) return "CURL初期化エラー";

		nlohmann::json body = { {"name", name}, {"password", password} };
		std::string bodyStr = body.dump();

		std::string response;
		struct curl_slist* headers = MakeAuthHeaders("", true);

		curl_easy_setopt(curl, CURLOPT_URL, "https://swgame-83u3aeo1i-hamus-projects-531e648e.vercel.app/users/new");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			return std::string("登録エラー: ") + curl_easy_strerror(res);
		}
		return response;
		});
}

std::future<std::string> LoginAsync(const std::string& name, const std::string& password) {
	return std::async(std::launch::async, [name, password]() -> std::string {
		CURL* curl = curl_easy_init();
		if (!curl) return "CURL初期化エラー";

		nlohmann::json body = { {"name", name}, {"password", password} };
		std::string bodyStr = body.dump();

		std::string response;
		struct curl_slist* headers = MakeAuthHeaders("", true);

		curl_easy_setopt(curl, CURLOPT_URL, "https://swgame-83u3aeo1i-hamus-projects-531e648e.vercel.app/users/login");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			return std::string("ログインエラー: ") + curl_easy_strerror(res);
		}
		return response;
		});
}

std::future<std::string> PostScoreAsync(int score, const std::string& token) {
	return std::async(std::launch::async, [score, token]() -> std::string {
		CURL* curl = curl_easy_init();
		if (!curl) return "CURL初期化エラー";

#pragma warning(push)
#pragma warning(disable : 26495) // VisualStudioの末初期化警告を抑制
		json body = json::object();
		body["score"] = score;
#pragma warning(pop)
		std::string bodyStr = body.dump();

		std::string authHeader = "Authorization: Bearer " + token;
		struct curl_slist* headers = nullptr;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, authHeader.c_str());
		
		std::string response;
		curl_easy_setopt(curl, CURLOPT_URL, "https://swgame-83u3aeo1i-hamus-projects-531e648e.vercel.app/scores");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);

		// HTTPステータスコードを取得
		long httpCode = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			return std::string("送信エラー: ") + curl_easy_strerror(res);
		}
		// ステータスコード付きでレスポンスを返す
		std::stringstream ss;
		ss << "HTTP" << httpCode << ": " << response;
		return ss.str();
		});
}

std::future<std::string> GetAllScoresAsync(const std::string& token) {
	return std::async(std::launch::async, [token]() -> std::string {
		CURL* curl = curl_easy_init();
		if (!curl) return "初期化エラー";

		// ヘッダ作成
		struct curl_slist* headers = nullptr;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "Accept: application/json");
		if (!token.empty()) {
			std::string authHeader = "Authorization: Bearer " + token;
			headers = curl_slist_append(headers, authHeader.c_str());
		}

		std::string response;
		curl_easy_setopt(curl, CURLOPT_URL, "https://swgame-83u3aeo1i-hamus-projects-531e648e.vercel.app/scores");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			return std::string("取得エラー: ") + curl_easy_strerror(res);
		}
		return response;
		});
}
