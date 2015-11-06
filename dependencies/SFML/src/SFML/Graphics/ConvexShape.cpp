////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2014 Laurent Gomila (laurent.gom@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/ConvexShape.hpp>


namespace sf
{
////////////////////////////////////////////////////////////
ConvexShape::ConvexShape(unsigned int pointCount)
{
	setPointCount(pointCount);
}


////////////////////////////////////////////////////////////
void ConvexShape::setPointCount(unsigned int count)
{
	m_points.resize(count);
	update();
}


////////////////////////////////////////////////////////////
unsigned int ConvexShape::getPointCount() const
{
	return static_cast<unsigned int>(m_points.size());
}


////////////////////////////////////////////////////////////
void ConvexShape::setPoint(unsigned int index, const Vector2f& point)
{
	m_points[index] = point;
	update();
}


////////////////////////////////////////////////////////////
Vector2f ConvexShape::getPoint(unsigned int index) const
{
	return m_points[index];
}


////////////////////////////////////////////////////////////
void ConvexShape::updateCentroid()
{
	if(m_points.size() < 3)
	{
		//we don't have a triangle so use bounding box
		//m_centroid = sf::Vector2f(getPosition().x + (getLocalBounds().width * 0.5f), getPosition().y + (getLocalBounds().height * 0.5f));
		m_centroid = sf::Vector2f(getLocalBounds().width * 0.5f, getLocalBounds().height * 0.5f);
		return;
	}
	//calc centroid of poly shape
	sf::Vector2f centroid;
	float signedArea = 0.f;
	float x0 = 0.f; // Current vertex X
	float y0 = 0.f; // Current vertex Y
	float x1 = 0.f; // Next vertex X
	float y1 = 0.f; // Next vertex Y
	float a = 0.f;  // Partial signed area

	// For all vertices except last
	unsigned i;
	for(i = 0; i < m_points.size() - 1; ++i)
	{
		x0 = m_points[i].x;
		y0 = m_points[i].y;
		x1 = m_points[i + 1].x;
		y1 = m_points[i + 1].y;
		a = x0 * y1 - x1 * y0;
		signedArea += a;
		centroid.x += (x0 + x1) * a;
		centroid.y += (y0 + y1) * a;
	}

	// Do last vertex
	x0 = m_points[i].x;
	y0 = m_points[i].y;
	x1 = m_points[0].x;
	y1 = m_points[0].y;
	a = x0 * y1 - x1 * y0;
	signedArea += a;
	centroid.x += (x0 + x1) * a;
	centroid.y += (y0 + y1) * a;

	signedArea *= 0.5;
	centroid.x /= (6 * signedArea);
	centroid.y /= (6 * signedArea);

	//convert to world space
	//centroid += getPosition();
	//return centroid;

	m_centroid = centroid;
}



} // namespace sf
