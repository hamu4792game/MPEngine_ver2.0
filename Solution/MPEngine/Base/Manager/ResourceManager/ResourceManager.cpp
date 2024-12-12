#include "ResourceManager.h"
#include "MPEngine/Base/Log.h"
#include "MPEngine/Graphics/Texture/Texture.h"
#include "MPEngine/Graphics/Object3d/Object3d.h"
#include "MPEngine/Input/Audio.h"
#include "MPEngine/Graphics/Animation/ModelAnimation.h"
#include "MPEngine/Graphics/Animation/AnimationData.h"
#include <format>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "externals/DirectXTex/d3dx12.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

ResourceManager* ResourceManager::GetInstance() {
	static ResourceManager instance;
	return &instance;
}

void ResourceManager::Initialize() {
	srvDescriptorHeap_ = std::make_unique<DescriptorHeap>();
	srvDescriptorHeap_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kDescriptorHeapSize_, true);

}

void ResourceManager::Finalize() {
	srvDescriptorHeap_->Release();
}

const uint32_t ResourceManager::GetCount() {
	textureCount_++;
	return textureCount_ - 1u;
}

void ResourceManager::AddTexture(const std::string& name, const std::string& fileName) {
	// 既にある場合早期リターン
	if (textureContainer_.find(name) != textureContainer_.end()) {
		return;
	}
	static const std::string directryPath = "Resources/Texture/";
	// textureの追加
	textureContainer_.emplace(std::make_pair(name, std::make_unique<Texture>()));
	textureContainer_.at(name).get()->Load(name, fileName);
}

void ResourceManager::AddModel(const std::string& name, const std::string& fileName) {
	// 既にある場合早期リターン
	if (object3dContainer_.find(name) != object3dContainer_.end()) {
		return;
	}
	static const std::string directryPath = "Resources/Model/";
	// modelDataの追加
	object3dContainer_.emplace(std::make_pair(name, std::make_unique<Object3d>()));
	object3dContainer_.at(name).get()->Load(name, fileName);
}

void ResourceManager::AddAudio(const std::string& name, const std::string& fileName) {
	// 既にある場合早期リターン
	if (audioContainer_.find(name) != audioContainer_.end()) {
		return;
	}
	static const std::string directryPath = "Resources/Audio/";
	// audioDataの追加
	audioContainer_.emplace(std::make_pair(name, std::make_unique<Audio>()));
	audioContainer_.at(name).get()->SoundLoad(fileName);
}

void ResourceManager::AddAnimation(const std::string& name, const std::string& fileName) {
	// 既にある場合早期リターン
	if (animationContainer_.find(name) != animationContainer_.end()) {
		return;
	}
	static const std::string directryPath = "Resources/Model/";
	// animationDataの追加
	animationContainer_.emplace(std::make_pair(name, std::make_unique<AnimationData>()));
	animationContainer_.at(name).get()->Load(name, fileName);
}

DirectX::ScratchImage ResourceManager::LoadTexture(const std::string& filePath) {
	// テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr;
	if (filePathW.ends_with(L".dds")) {
		// .ddsで終わっていたらddsとみなす
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else {
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	}
	assert(SUCCEEDED(hr));

	// ミニマップの作成
	DirectX::ScratchImage mipImages{};
	// 圧縮フォーマットかどうか調べる
	if (DirectX::IsCompressed(image.GetMetadata().format)) {
		mipImages = std::move(image);
	}
	else {
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
		assert(SUCCEEDED(hr));
	}
	// ミニマップ付きのデータを返す
	return mipImages;
}

void ResourceManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {
	// meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	// 全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; mipLevel++)
	{
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,
			img->pixels,
			UINT(img->rowPitch),
			UINT(img->slicePitch)
		);
		assert(SUCCEEDED(hr));
	}
}

//	TextureResourceにデータを転送する
[[nodiscard]]	//戻り値を破棄してはならない
ID3D12Resource* ResourceManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {
	//	中間リソース
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	ID3D12Resource* intermediateResource = CreateBufferResource(device, intermediateSize);
	//	データ転送をコマンドに積む
	UpdateSubresources(commandList, texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());
	//	Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);
	return intermediateResource;
}

ID3D12Resource* ResourceManager::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
	// metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width); // textureの幅
	resourceDesc.Height = UINT(metadata.height); // textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels); // bitmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行き or 配列Textureの配列数
	resourceDesc.Format = metadata.format; // TextureのFormat
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // Textureの次元数。普段使っているのは二次元
	// 利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケース版がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作成する
	//heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; // WriteBackポリシーでCPUアクセス可能
	//heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; // プロセッサの近くに配置
	// Resourceの設定する
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties, // Heapの設定
		D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし
		&resourceDesc, // Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST, // データ転送される設定
		nullptr, // Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource)); // 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}

ID3D12Resource* ResourceManager::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {

	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // uploadHeapを使う
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC ResourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = sizeInBytes; // リソースのサイズ。今回はVector4を3頂点分
	// バッファの場合はこれらは1にする決まり
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;
	// バッファの頂点はこれにする決まり
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際に頂点リソースを作る
	ID3D12Resource* Resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&Resource));
	hr = device->GetDeviceRemovedReason();
	assert(SUCCEEDED(hr));
	return Resource;

}

ID3D12Resource* ResourceManager::CreateRenderTextureResource(ID3D12Device* device, D3D12_RESOURCE_DESC resDesc, DXGI_FORMAT format, const Vector4& color) {
	D3D12_RESOURCE_DESC resourceDesc = resDesc; // 使っているバッファの情報を利用する
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; // RenderTargetとして利用する
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // 当然VRAM状に作る
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = format;
	clearValue.Color[0] = color.x;
	clearValue.Color[1] = color.y;
	clearValue.Color[2] = color.z;
	clearValue.Color[3] = color.w;

	// リソースの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &clearValue, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));
	return resource;
}

std::vector<ModelData> ResourceManager::LoadModelFile(const std::string& filename) {
	//	ファイルを開く
	std::ifstream file(filename);
	assert(file.is_open());

	//	ディレクトリパスの取得
	std::filesystem::path ps = std::filesystem::path(filename);
	std::string directryPath = ps.parent_path().string();

	Assimp::Importer importer;
	// fileからassimpのSceneを構築する
	// aiProcess_FlipWindingOrder =	三角形の並び順を逆にする
	// aiProcess_FlipUVs = UVをフリップする(texcoord.y = 1.0f - texcoord.yの処理)
	const aiScene* scene = importer.ReadFile(filename.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes()); // メッシュがないのは対応しない

	// dataの宣言
	std::vector<ModelData> modelDatas;
	modelDatas.resize(scene->mNumMeshes);
	ModelData modelData;
	// メッシュの解析
	for (uint32_t index = 0; index < scene->mNumMeshes; index++) {
		aiMesh* mesh = scene->mMeshes[index];
		assert(mesh->HasNormals()); // 法線がないメッシュを非対応に
		assert(mesh->HasTextureCoords(0)); // texcoordがないメッシュを非対応に

		// ここからVertexの解析を行う
		modelData.vertices.resize(mesh->mNumVertices);
		for (uint32_t vertexIndex = 0u; vertexIndex < mesh->mNumVertices; vertexIndex++) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
			// 右手→左手座標系へ変換
			modelData.vertices[vertexIndex].position = { -position.x,position.y,position.z,1.0f };
			modelData.vertices[vertexIndex].normal = { -normal.x,normal.y,normal.z };
			modelData.vertices[vertexIndex].texcoord = { texcoord.x,texcoord.y };
		}
		// indexの解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3); // 三角形のみサポート
			// indexの解析
			for (uint32_t element = 0u; element < face.mNumIndices; element++) {
				uint32_t vertexIndex = face.mIndices[element];
				modelData.indices.push_back(vertexIndex);
			}
		}
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) {
			// Jointごとの格納領域を作る
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData.skinClusterData[jointName];

			// inverseBindPoseMatrixの抽出
			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
			// 左手座標系で
			Matrix4x4 bindPoseMatrix = MakeAffineMatrix(Vector3(scale.x, scale.y, scale.z), Quaternion(rotate.x, -rotate.y, -rotate.z, rotate.w), Vector3(-translate.x, translate.y, translate.z));
			jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);
			
			// Weight情報を取り出す
			for (uint32_t weightIndex = 0u; weightIndex < bone->mNumWeights; weightIndex++) {
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight,bone->mWeights[weightIndex].mVertexId });
			}
		}

		// シーン全体の階層構造を作り上げる
		modelData.rootNode = ReadNode(scene->mRootNode);

		// 生成したデータを入れ込む
		modelDatas.at(index) = modelData;
	}

	// materialの解析を行う
	uint32_t index = 0u;
	std::vector<std::string> texturesPath;
	texturesPath.resize(scene->mNumMaterials);
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; materialIndex++) {
		aiMaterial* material = scene->mMaterials[materialIndex];

		// 一般的に模様という用途の aiTextureType_DIFFUSE を使用する
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			// 対応した場所にテクスチャファイルとして入れ込む
			uint32_t handle = 1u;
			if (ps.extension() == ".gltf") {
				// gltfだった場合は特殊なため、例外処理を入れる
				handle = 0u;
			}
			texturesPath.at(static_cast<std::vector<std::string, std::allocator<std::string>>::size_type>(index) - handle) = directryPath + "/" + textureFilePath.C_Str();
		}
		index++;
	}

	// textureを全部読み込んでからメッシュ事にテクスチャを振り分けていく
	uint32_t handle = 1u;
	if (ps.extension() == ".gltf") {
		// gltfだった場合は特殊なため、例外処理を入れる
		handle = 0u;
	}
	for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[i];
		modelDatas.at(static_cast<uint32_t>(i)).material.textureFilePath = texturesPath.at(mesh->mMaterialIndex - handle);
	}

	return modelDatas;
}

AnimationData ResourceManager::LoadAnimationFile(const std::string& filename) {
	AnimationData animation; // 今回作るアニメーション
	Assimp::Importer importer;
	//	ディレクトリパスの取得
	std::filesystem::path ps = std::filesystem::path(filename);
	std::string directryPath = ps.parent_path().string();

	const aiScene* scene = importer.ReadFile(filename.c_str(), 0);
	assert(scene->mNumAnimations != 0); // アニメーションがない
	aiAnimation* animationAssimp = scene->mAnimations[0]; // 最初のアニメーションだけ採用。もちろん複数対応するに越したことはない
	// mTicksPerSecond : 周波数 / mDuration : 周波数の長さ
	// 例 周波数が1000Hzの時、周期(1Tick)は1ms。周波数の長さが2000なら、2000ms = 2sである
	animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond); // 時間の単位を秒に変換

	// NodeAnimationの解析
	// assimpでは個々のNodeのAnimationをchannelと呼んでいるのでchannnelを回してNodeanimationの情報を取ってくる
	for (uint32_t channnelIndex = 0; channnelIndex < animationAssimp->mNumChannels; channnelIndex++) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channnelIndex];
		NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
		// 座標の取得
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; keyIndex++) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			Keyframe<Vector3> keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // ここも秒に変換
			keyframe.value = Vector3(-keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z); // 右手→左手
			nodeAnimation.translate.keyframes.push_back(keyframe);
		}
		// 回転の取得
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; keyIndex++) {
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			Keyframe<Quaternion> keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // ここも秒に変換
			keyframe.value = Quaternion(keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w); // 右手→左手
			nodeAnimation.rotate.keyframes.push_back(keyframe);
		}
		// 拡縮の取得
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; keyIndex++) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			Keyframe<Vector3> keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // ここも秒に変換
			keyframe.value = Vector3(keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z); // 右手→左手
			nodeAnimation.scale.keyframes.push_back(keyframe);
		}
	}
	// アニメーションを返す
	return animation;
}

MaterialData ResourceManager::LoadMaterialTemplateFile(const std::string& filename) {
	MaterialData materialData; // 構築するデータ
	std::string line;	// ファイルから読んだ1行を格納するもの
	std::ifstream file(filename);	//ファイルを開く
	assert(file.is_open());

	// ディレクトリパスの取得
	std::filesystem::path ps = std::filesystem::path(filename);
	std::string directryPath = ps.parent_path().string();

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// 連結してファイルパスにする
			materialData.textureFilePath = directryPath + "/" + textureFilename;
		}
	}

	return materialData;
}

Node ResourceManager::ReadNode(aiNode* node) {
	Node result;
	//aiMatrix4x4 aiLocalMatrix = node->mTransformation; // nodeのlocalMatrixを取得
	//aiLocalMatrix.Transpose(); // 列ベクトル形式を行ベクトル形式に転置
	aiVector3D scale, translate;
	aiQuaternion rotate;
#pragma region 行列代入
	node->mTransformation.Decompose(scale, rotate, translate); // assimpの行列からSRTを抽出
	result.transform.scale_ = Vector3(scale.x, scale.y, scale.z);
	result.transform.rotationQuat_ = Quaternion(rotate.x, rotate.y, -rotate.z, rotate.w); // x軸を反転、さらに回転方向が逆なので軸を反転させる
	result.transform.translation_ = Vector3(-translate.x, translate.y, translate.z); // x軸を反転
	// quaternionを使用して更新
	result.transform.isQuaternion_ = true;
	result.localMatrix = result.transform.UpdateMatrix();
#pragma endregion
	result.name = node->mName.C_Str(); // Node名を取得
	result.children.resize(node->mNumChildren); // 子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; childIndex++) {
		// 再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}
