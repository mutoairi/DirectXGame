#pragma once

#include <KamataEngine.h>
#include<map>

class GameScene {
	struct LevelData {
		struct ObjectData {
			std::string fileName;
			KamataEngine::Vector3 translation;
			KamataEngine::Vector3 rotation;
			KamataEngine::Vector3 scaling;
		};
		std::vector<ObjectData> objects;
	};

public:
	~GameScene();
	void Initialize();
	void Update();
	void Draw();

private:
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;
	KamataEngine::Camera camera_;
	
	LevelData* levelData = nullptr;
	std::map<std::string, KamataEngine::Model*> models;
	
	std::vector<KamataEngine::WorldTransform*> objects;
	
};
