#pragma once
#include <list>


// モデル
class Model {
	friend class ModelRender;
public:
	Model();
	~Model();

private:
	static std::list<Model*> modelLists_;

};
