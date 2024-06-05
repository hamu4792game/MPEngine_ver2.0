#pragma once
#include <list>
#include "MPEngine/Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"
#include "Utils/WorldTransform/WorldTransform.h"

// モデル
class Line {
	friend class LineRender;
public:
	Line();
	~Line();
public:
	void SetColor(const Vector4& color) { color_ = color; }
	void SetColor(const uint32_t& color) { color_ = ChangeColor(color); }
	void SetLine(const Vector3& start, const Vector3& end);
private:

public:
	bool isActive_ = true;

private:
	static std::list<Line*> lineLists_;
	Vector4 color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	struct LineVertex {
		Vector3 start; // 始点
		Vector3 end; // 終点
	};
	LineVertex pos_;
	
};
