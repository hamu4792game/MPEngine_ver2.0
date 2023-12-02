#pragma once
#include <xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#include <fstream>
#include <wrl.h>
#include <array>

#include "Engine/Manager/AudioManager/AudioManager.h"

class AudioInput
{
public:
	AudioInput() = default;
	~AudioInput() = default;

private:

	//	チャンクヘッダ
	struct ChunkHeader {
		std::array<char, 4> id;		// チャンク毎のID
		int32_t size;	// チャンクサイズ
	};

	//	RIFFヘッダチャンク
	struct RiffHeader {
		ChunkHeader chunk;	// "RIFF"
		std::array<char, 4> type;	// "WAVE"
	};

	//	FMTチャンク
	struct FormatChunk {
		ChunkHeader chunk;	// fmt
		WAVEFORMATEX fmt;	// 波形フォーマット
	};

	struct SoundData {
		WAVEFORMATEX wfex;	// 波形フォーマット
		BYTE* pBuffer;		// バッファの先頭サイズ
		uint32_t bufferSize;// バッファのサイズ
	};

	SoundData soundData{};
	IXAudio2SourceVoice* pSourceVoice = nullptr;

public:
	//	サウンドデータの読み込み関数
	void SoundLoadWave(const std::string fileName);

	//	音声データ解放
	void SoundUnload(SoundData* soundData);

	//	音声再生
	void SoundPlayWave(bool loop = false);

	//	音声のストップ
	void SoundStop();

	//	ポーズ
	void Pose();
	
	//	ポーズで止めていたところから再生
	void ReStart();

	//	音量のセット
	void SetVolume(float volume);

};
