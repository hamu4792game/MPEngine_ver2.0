#include "Engine/Input/AudioInput/AudioInput.h"
#include <cassert>

void AudioInput::SoundLoadWave(const std::string fileName)
{
//	ファイルオープン
	//	ファイル入力ストリームのインスタンス
	std::ifstream file;
	//	.wavファイルをバイナリモードで開く
	file.open(fileName, std::ios_base::binary);
	//	失敗の検出
	assert(file);

//	wavデータの読み込み
	//	RIFFヘッダーの読み込み
	RiffHeader riff{};
	file.read((char*)&riff, sizeof(riff));
	//	ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id.data(), "RIFF", 4) != 0) {
		assert(0);
	}
	//	タイプがWAVEかチェック
	if (strncmp(riff.type.data(), "WAVE", 4) != 0) {
		assert(0);
	}
	//	フォーマットチャンクの読み込み
	FormatChunk format{};
	//	チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id.data(), "fmt ", 4) != 0) {
		assert(0);
	}
	//	チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
	//	データチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//	JUNKチャンクの読み込み
	if (strncmp(data.id.data(), "JUNK", 4) == 0) {
		//	読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		//	再読み込み
		file.read((char*)&data, sizeof(data));
	}
	//	dataの読み込み
	while (strncmp(data.id.data(), "data", 4) != 0)	{
		//	読み取り位置をdataまで進める
		file.seekg(data.size, std::ios_base::cur);
		//	読み込み
		file.read((char*)&data, sizeof(data));
		//	読み込めなかったら
		if (file.eof())	{
			assert(0);
		}
	}
	//	Dataチャンクのデータ部（波形データ）の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);
	//	Waveファイルを閉じる
	file.close();
//	読み込んだ音声データをreturn
	//	returnする為の音声データ
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
}

void AudioInput::SoundUnload(SoundData* soundData)
{
	//	バッファのメモリを解放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void AudioInput::SoundPlayWave(bool loop)
{
	HRESULT result = 0;
	
	//	波形フォーマットを元にSourceVoiceの生成
	//	既にあればスルー
	if (!pSourceVoice) {
		result = AudioManager::GetInstance()->GetAudio2()->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
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

void AudioInput::SoundStop() {
	if (pSourceVoice) {
		pSourceVoice->Stop();
		pSourceVoice = nullptr;
	}
}

void AudioInput::Pose()
{
	pSourceVoice->Stop();
}

void AudioInput::ReStart()
{
	pSourceVoice->Start();
}

void AudioInput::SetVolume(float volume)
{
	//	ボリュームのセット
	pSourceVoice->SetVolume(volume);
}
