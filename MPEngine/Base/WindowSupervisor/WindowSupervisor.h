#pragma once
#include <Windows.h>
#include <cstdint>

// windowの管理・監視クラス
class WindowSupervisor {
public:
	WindowSupervisor() = default;
	~WindowSupervisor() = default;

public: // 静的メンバ関数
	static WindowSupervisor* GetInstance();

	/// <summary>
	/// ウィンドウプロシージャ
	/// </summary>
	/// <param name="hwnd">ウィンドウハンドル</param>
	/// <param name="msg">メッセージ番号</param>
	/// <param name="wparam">メッセージ情報1</param>
	/// <param name="lparam">メッセージ情報2</param>
	/// <returns>可否</returns>
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	/// <summary>
	/// ウィンドウメッセージの処理
	/// </summary>
	/// <returns>true:終了 false:継続</returns>
	bool ProcessMessage();
public: // メンバ関数

	/// <summary>
	/// ゲームウィンドウの作成
	/// </summary>
	/// <param name="title">ウィンドウタイトル</param>
	/// <param name="clientWidth">横幅</param>
	/// <param name="clientHeight">縦幅</param>
	void CreateGameWindow(const wchar_t* title, int32_t clientWidth, int32_t clientHeight);

	/// <summary>
	/// ウィンドウの破棄
	/// </summary>
	void DeleteGameWindow();


private:
	// Window関連
	//	ウィンドウクラス
	WNDCLASS wc{};
	// ウィンドウハンドル
	HWND hwnd = nullptr;

public:	// ゲッター

	const HWND GetHwnd() const { return hwnd; }
	const WNDCLASS GetWc() const { return wc; }

};
