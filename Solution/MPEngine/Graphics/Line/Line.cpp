#include "Line.h"
decltype(Line::lineLists_) Line::lineLists_;

Line::Line() {
	lineLists_.emplace_back(this);
}

Line::~Line() {
	lineLists_.remove(this);
}

void Line::SetLine(const Vector3& start, const Vector3& end) {
	pos_.start = start;
	pos_.end = end;
}
