#include "NTScene.h"
#include <iostream>
#include <cassert>

#include "NT/Server.h"
#include "NT/Client.h"

void NTScene::Initialize() {

	network_ = std::make_unique<Server>();
	unsigned short port = 8000;
	network_->Initialize();

	

}

void NTScene::Finalize() {
	network_->Finalize();
}

void NTScene::Update() {

}

