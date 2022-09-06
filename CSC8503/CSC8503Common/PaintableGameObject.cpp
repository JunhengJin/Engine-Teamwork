#include "PaintableGameObject.h"

NCL::CSC8503::PaintableGameObject::PaintableGameObject(string name) : GameObject(name)
{
	paintCount.insert(std::pair<Pcolour, int>(Pcolour::RED, 0));
	paintCount.insert(std::pair<Pcolour, int>(Pcolour::BLUE, 0));
}

NCL::CSC8503::PaintableGameObject::~PaintableGameObject()
{
	delete paintVertexes;
}

void NCL::CSC8503::PaintableGameObject::Paint(Vector3 position, float size, Vector3 color)
{
	vector<Vector3> vertexes = renderObject->GetVertexes();
	position = transform.GetMatrix().Inverse() * position;
	for (auto it = vertexes.begin(); it != vertexes.end(); it++)
	{
		if ((position - (*it)).Length() < size)
		{
			Pcolour befre = paintColor((*(paintVertexes->begin() + std::distance(vertexes.begin(), it))));

			Pcolour aftr = paintColor(Vector4(color.x, color.y, color.z, 1));

			(*(paintVertexes->begin() + std::distance(vertexes.begin(), it))) = Vector4(color.x, color.y, color.z,1);//w=1 means painted

			if (befre == Pcolour::RED) {//if wall is red
				if (aftr == Pcolour::BLUE) {//if paint shot is blue
					paintCount[Pcolour::RED] --;
					paintCount[Pcolour::BLUE] ++;
				}
			}
			else if (befre == Pcolour::BLUE) {//wall blue
				if (aftr == Pcolour::RED) {//if paint shot is reda
					paintCount[Pcolour::RED] ++;
					paintCount[Pcolour::BLUE] --;
				}
			}
			else {
				if (aftr == Pcolour::RED) { paintCount[Pcolour::RED] ++;}
				else if (aftr == Pcolour::BLUE) { paintCount[Pcolour::BLUE] ++; }
			}

		}
	}

}

void NCL::CSC8503::PaintableGameObject::Paint(Vector3 position, float size, Pcolour color)
{
	Paint(position, size, paintColorEnum(color));
}

std::vector<Vector4> NCL::CSC8503::PaintableGameObject::getPaintVertexes()
{
	return *paintVertexes;
}

float NCL::CSC8503::PaintableGameObject::getPaintFraction(Pcolour colour)
{
	//return value from map. Add 1 to make it Vector4 and get painted values in map

	if (colour == Pcolour::RED) {
		return (float)paintCount[Pcolour::RED] / (float)renderObject->GetVertexes().size();
	}
	if (colour == Pcolour::BLUE) {
		return (float)paintCount[Pcolour::BLUE] / (float)renderObject->GetVertexes().size();
	}
	return 0.0f;
}

void NCL::CSC8503::PaintableGameObject::SetRenderObject(RenderObject* newObject)
{
	renderObject = newObject;
	paintVertexes = new std::vector<Vector4>(newObject->GetVertexCount(), Vector4(1,1,1,0));// w=0 means unpainted
}

NCL::CSC8503::Pcolour NCL::CSC8503::PaintableGameObject::paintColor(Vector4 colourVec)
{
	if (colourVec == Vector4(1, 0, 0, 1)) {
		return Pcolour::RED;
	}
	else if (colourVec == Vector4(0, 0, 1, 1)) {
		return Pcolour::BLUE;
	}
	return Pcolour::UNCOLOURED;
}

Vector4 NCL::CSC8503::PaintableGameObject::paintColorEnum(NCL::CSC8503::Pcolour colourEnum)
{
	if (colourEnum == Pcolour::RED) {
		return Vector4(1, 0, 0, 1);
	}
	else if (colourEnum == Pcolour::BLUE) {
		return  Vector4(0, 0, 1, 1);
	}
	return Vector4(1, 1, 1, 0);
}


void NCL::CSC8503::PaintableGameObject::countPaint()
{
	paintCount.clear();
	paintCount.insert(std::pair<Pcolour, int>(Pcolour::BLUE, 0));


	for (auto it = paintVertexes->begin(); it != paintVertexes->end(); it++)
	{
		if (paintColor(*it) == Pcolour::RED) { paintCount[Pcolour::RED] ++; }
		else if (paintColor(*it) == Pcolour::BLUE) { paintCount[Pcolour::BLUE] ++; }
	}


	//put each vertexes that w value = 1  into map, tallying it up
}
