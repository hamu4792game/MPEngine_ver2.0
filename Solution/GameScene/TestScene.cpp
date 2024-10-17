#include "TestScene.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "MPEngine/Graphics/Texture/Texture.h"
#include "MPEngine/Graphics/Object3d/Object3d.h"
#include "MPEngine/Input/Audio.h"
#include "ImGuiManager/ImGuiManager.h"

void TestScene::Initialize() {
	// 必要なResourceのLoad
	auto rs = ResourceManager::GetInstance();
	std::string directryPath = "Resources/Model/";
	rs->AddTexture("white2x2", "Resources/Texture/white2x2.png");
	rs->AddTexture("UVChecker","Resources/Texture/uvChecker.png");

	rs->AddModel("Box", "Resources/Model/AnimatedCube/AnimatedCube.gltf");
	rs->AddModel("Plane", "Resources/Model/plane/plane.obj");
	rs->AddModel("Sphere", "Resources/Model/sphere/sphere.obj");
	rs->AddModel("Target", "Resources/Model/Test/test.obj");
	
	rs->AddTexture("ground", "Resources/Texture/ground.png");
	rs->AddTexture("ground1", "Resources/Texture/ground1.png");
	rs->AddTexture("ground2", "Resources/Texture/ground2.png");
	rs->AddTexture("ground3", "Resources/Texture/ground3.png");
	rs->AddTexture("Circle", "Resources/Texture/particle.png");
	rs->AddTexture("ABText", "Resources/Texture/hud/AB.png");
	
	rs->AddTexture("MonsterBall", "Resources/Texture/monsterBall.png");
	rs->AddTexture("CheckerBoard", "Resources/Texture/checkerBoard.png");

	rs->AddTexture("TitleText", "Resources/Texture/hud/titleText.png");
	rs->AddTexture("PushText", "Resources/Texture/hud/pushText.png");
	rs->AddTexture("ABCD", "Resources/Texture/uvChecker.png");
	rs->AddTexture("Target", "Resources/Texture/hud/target.png");
	rs->AddTexture("Block", "Resources/Texture/ground.png");
	rs->AddTexture("Airport", "Resources/Texture/output_image.dds");

	rs->AddModel("Human", "Resources/Model/human/wait.gltf");
	rs->AddModel("PlayerHead", "Resources/Model/Player/Head/head.obj");
	rs->AddModel("PlayerBody", "Resources/Model/Player/Body/body.obj");
	rs->AddModel("PlayerLeftArm", "Resources/Model/Player/LArm/LArm.obj");
	rs->AddModel("PlayerRightArm", "Resources/Model/Player/RArm/RArm.obj");
	rs->AddModel("PlayerLegs", "Resources/Model/Player/Legs/limbs.obj");
	rs->AddModel("PlayerWeapon", "Resources/Model/Player/weapon/weapon.obj");
	rs->AddModel("Goal", "Resources/Model/GoalWell/GoalWell.obj");

	rs->AddAnimation("Rotate", "Resources/Model/AnimatedCube/AnimatedCube.gltf");
	rs->AddAnimation("HumanWait", "Resources/Model/human/wait.gltf");
	rs->AddAnimation("HumanWalk", "Resources/Model/human/run.gltf");
	rs->AddAnimation("HumanJump", "Resources/Model/human/jump.gltf");

	//rs->AddAudio("Title", "./Resources/Sound/w006.wav");
	rs->AddAudio("Battle", "Resources/Audio/mLoop1.mp3");

	spring_ = std::make_unique<Spring>();
	spring_->Initialize();

}

void TestScene::Finalize() {
	
}

void TestScene::Update() {
	spring_->Update();
	
}

