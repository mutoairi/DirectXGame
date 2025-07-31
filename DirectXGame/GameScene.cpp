#include "GameScene.h"
#include <cassert>
#include <random>
#include<json.hpp>
#include<fstream>

using namespace KamataEngine;
using namespace MathUtility;


std::uniform_real_distribution<float> distrubution(-1.0f, 1.0f);

GameScene::~GameScene() {
	
}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	

	const std::string fullpath = std::string("Resources/levels/") + "Untitled.json";

	//ファイルストリーム
	std::ifstream file;
	//ファイルを開く
	file.open(fullpath);
	//ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	//JSON文字列から解凍したデータ
	nlohmann::json deserialized;

	//解凍
	file >> deserialized;

	//正しいレベルデータファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	//"name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();
	//正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	//レベルデータ格納インスタンスを作成
	levelData=new LevelData();
	for (nlohmann::json& object : deserialized["objects"]) {
		assert(object.contains("type"));

		std::string type = object["type"].get<std::string>();

		if (type.compare("MESH") == 0) {
			levelData->objects.emplace_back(LevelData::ObjectData{});
			LevelData::ObjectData& objectData = levelData->objects.back();
			if (object.contains("file_name")) {
				objectData.fileName = object["file_name"];
			}
			nlohmann::json& transform = object["transform"];
			objectData.translation.x = (float)transform["translation"][0];
			objectData.translation.y = (float)transform["translation"][2];
			objectData.translation.z = (float)transform["translation"][1];

			objectData.rotation.x = (float)transform["rotation"][0];
			objectData.rotation.y = (float)transform["rotation"][2];
			objectData.rotation.z = (float)transform["rotation"][1];

			objectData.scaling.x = (float)transform["scaling"][0];
			objectData.scaling.y = (float)transform["scaling"][2];
			objectData.scaling.z = (float)transform["scaling"][1];
		}

		if (object.contains("children")) {
		}
		
	}
	for (auto& objectData : levelData->objects) {

		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it == models.end()) {
			Model* model = Model::CreateFromOBJ(objectData.fileName);
			models[objectData.fileName] = model;
		}
	}
		for (auto& objectData : levelData->objects) {
		
		WorldTransform* newObject = new WorldTransform();
		newObject->translation_ = objectData.translation;
		newObject->rotation_ = objectData.rotation;
		newObject->scale_ = objectData.scaling;
		newObject->Initialize();
		objects.push_back(newObject);
	}
	camera_.Initialize();
}

void GameScene::Update() {
	for (WorldTransform*object : objects) {
		// スケーリング行列の作成
		Matrix4x4 matScale = MakeScaleMatrix(object->scale_);
		// 回転行列の作成
		Matrix4x4 matRotX = MakeRotateXMatrix(object->rotation_.x);
		Matrix4x4 matRotY = MakeRotateYMatrix(object->rotation_.y);
		Matrix4x4 matRotZ = MakeRotateZMatrix(object->rotation_.z);
		Matrix4x4 matRotZX = matRotZ * matRotX;
		Matrix4x4 matRot = matRotZX * matRotY;

		// 平行移動行列の作成
		Matrix4x4 matTrans = MakeTranslateMatrix(object->translation_);
		object->matWorld_ =matScale * matRot * matTrans;
		object->TransferMatrix();
	}
	}

void GameScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw();

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	///
	size_t count = min(objects.size(), levelData->objects.size());
	for (size_t i = 0; i < count; ++i) {
		const std::string& fileName = levelData->objects[i].fileName;

		auto it = models.find(fileName);
		if (it != models.end() && objects[i]) {
			it->second->Draw(*objects[i], camera_);
		}
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion
	
#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
