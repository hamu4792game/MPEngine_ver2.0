#include "Audio.h"
#include <cassert>
#include "Base/Log.h"
#include <vector>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

MasterAudio::~MasterAudio() {
	if (xAudio2_) {
		xAudio2_.Reset();
	}
	MFShutdown();
}

MasterAudio* MasterAudio::GetInstance() {
	static MasterAudio instance;
	return &instance;
}

void MasterAudio::Intialize() {
	HRESULT result = 0;
	// XAudioエンジンのインスタンス生成
	result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	// マスターボイスの生成
	result = xAudio2_->CreateMasteringVoice(&masterVoice_);
	// MediaFoundationの初期化
	result = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
}

Audio::~Audio() {
	SoundUnload(&soundData);
}

void Audio::SoundLoadWave(const std::string fileName) {
	// ファイルオープン
	// ファイル入力ストリームのインスタンス
	std::ifstream file;
	// .wavファイルをバイナリモードで開く
	file.open(fileName, std::ios_base::binary);
	// 失敗の検出
	assert(file);

	// wavデータの読み込み
	// RIFFヘッダーの読み込み
	RiffHeader riff{};
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id.data(), "RIFF", 4) != 0) {
		assert(0);
	}
	// タイプがWAVEかチェック
	if (strncmp(riff.type.data(), "WAVE", 4) != 0) {
		assert(0);
	}
	// フォーマットチャンクの読み込み
	FormatChunk format{};
	// チャンクヘッダーの確認
	uint32_t nowRead = 0u;
	file.read((char*)&format, sizeof(ChunkHeader));
	while (strncmp(format.chunk.id.data(), "fmt ", 4) != 0) {
		// 現在位置からfmtチャンクまで移動
		file.seekg(nowRead, std::ios_base::beg);
		if (file.eof()) {
			assert(0);
		}
		nowRead++;
		// 再読み込み
		file.read((char*)&format, sizeof(ChunkHeader));
	}
	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
	// データチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	
	// dataの読み込み
	while (strncmp(data.id.data(), "data", 4) != 0) {
		// 読み取り位置をdataまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 読み込み
		file.read((char*)&data, sizeof(data));
		// 読み込めなかったら
		if (file.eof()) {
			assert(0);
		}
	}
	// Dataチャンクのデータ部（波形データ）の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);
	// Waveファイルを閉じる
	file.close();
	// 読み込んだ音声データをreturn
	// returnする為の音声データ
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
}

void Audio::SoundLoad(const std::string fileName) {
	// ファイルオープン
	// ファイル入力ストリームのインスタンス
	std::ifstream file;
	// .wavファイルをバイナリモードで開く
	file.open(fileName, std::ios_base::binary);
	// 失敗の検出
	assert(file);

	// ソースリーダーの作成
	IMFSourceReader* pMFSourceReader{ nullptr };
	MFCreateSourceReaderFromURL(ConvertString(fileName).c_str(), nullptr, &pMFSourceReader);
	
	// メディアタイプの取得
	IMFMediaType* pMFMediaType{ nullptr };
	MFCreateMediaType(&pMFMediaType);
	pMFMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pMFMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	pMFSourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pMFMediaType);

	pMFMediaType->Release();
	pMFMediaType = nullptr;
	pMFSourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pMFMediaType);

	// オーディオデータ形式の作成
	WAVEFORMATEX* waveFormat{ nullptr };
	MFCreateWaveFormatExFromMFMediaType(pMFMediaType, &waveFormat, nullptr);

	// データの読み込み
	std::vector<BYTE> mediaData;
	while (true) {
		IMFSample* pMFSample{ nullptr };
		DWORD dwStreamFlags{ 0 };
		pMFSourceReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);

		if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
			break;
		}

		IMFMediaBuffer* pMFMediaBuffer{ nullptr };
		pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

		BYTE* pBuffer{ nullptr };
		DWORD cbCurrentLength{ 0 };
		pMFMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength);

		mediaData.resize(mediaData.size() + cbCurrentLength);
		memcpy(mediaData.data() + mediaData.size() - cbCurrentLength, pBuffer, cbCurrentLength);

		pMFMediaBuffer->Unlock();

		pMFMediaBuffer->Release();
		pMFSample->Release();
	}
	
	BYTE* pMemory = new BYTE[mediaData.size()];
	memcpy(pMemory, mediaData.data(), mediaData.size());

	// soundDataに代入
	soundData.pBuffer = pMemory;
	soundData.bufferSize = sizeof(BYTE) * static_cast<UINT32>(mediaData.size());
	soundData.wfex = *waveFormat;

	file.close();

	// ローカルポインタ解放処理
	CoTaskMemFree(waveFormat);
	if (pMFMediaType) {
		pMFMediaType->Release();
	}
	if (pMFSourceReader) {
		pMFSourceReader->Release();
	}
}

void Audio::SoundUnload(SoundData* soundData) {
	// バッファのメモリを解放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void Audio::SoundPlayWave(bool loop) {
	HRESULT result = 0;

	//	波形フォーマットを元にSourceVoiceの生成
	//	既にあればスルー
	if (!pSourceVoice) {
		result = MasterAudio::GetInstance()->GetAudio()->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
		assert(SUCCEEDED(result));
	}

	//	再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	//	ループしたい時
	if (loop) { buf.LoopCount = XAUDIO2_LOOP_INFINITE; };
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	//	波形データの再生
	result = pSourceVoice->Start();
}

void Audio::SoundStop() {
	if (pSourceVoice) {
		pSourceVoice->Stop();
		pSourceVoice = nullptr;
	}
}

void Audio::Pose() {
	pSourceVoice->Stop();
}

void Audio::ReStart() {
	pSourceVoice->Start();
}

void Audio::SetVolume(float volume) {
	// ボリュームのセット
	pSourceVoice->SetVolume(volume);
}
