#include "libs.h"
#include "util.h"
#include "static.h"
#include "file.h"
#include "configfile.h"
#include "console.h"
#include "animator.h"
#include "camera.h"

sf::View Camera::m_Camera = sf::View();
int Camera::shakeFrames = 0;
float Camera::xShake = 0.f;
float Camera::yShake = 0.f;
float Camera::offsetx = 0.f;
float Camera::offsety = 0.f;
sf::FloatRect Camera::m_boundaries = sf::FloatRect();
sf::Vector2f Camera::m_boundariesvec = sf::Vector2f();
bool Camera::m_bound = false;



void Camera::ResetParams()
{
	offsetx = 0.f;
	offsety = 0.f;
}

sf::View & Camera::GetCamera()
{
	return m_Camera;
}

void Camera::Update()
{
	if(shakeFrames > 0)
	{
		m_Camera.setCenter(random.Deviation(m_Camera.getCenter().x, xShake), random.Deviation(m_Camera.getCenter().y, yShake));
		shakeFrames--;
	}
}

void Camera::MoveCamera(sf::Vector2f newpos)
{
	m_Camera.setCenter(newpos);
}

Camera::Camera( sf::FloatRect camsize, sf::Vector2f pos )
{
	ResetParams();
	Camera::m_Camera.reset(camsize);
	MoveCamera(pos);
}

Camera::Camera( sf::FloatRect camsize )
{
	ResetParams();
	Camera::m_Camera.reset(camsize);
}

Camera::Camera()
{
	ResetParams();
}

void Camera::Init( sf::FloatRect camsize )
{
	m_Camera.reset(camsize);
	m_bound = false;
}

void Camera::SetBoundaries( sf::Vector2f sz )
{
	if(sz.x == 0 || sz.y == 0)
	{
		m_boundaries = sf::FloatRect(0, 0, 1, 1);
		m_boundariesvec = sz;
		m_bound = false;
	}
	else
	{
		m_boundaries = sf::FloatRect(0, 0, sz.x, sz.y);
		m_boundariesvec = sz;
		m_bound = true;
	}
}

void Camera::AdjustCamera( float x, float y )
{
	m_Camera.move(x, y);
}

void Camera::SetShake( float xs, float ys, int framecount )
{
	framecount = std::abs(framecount);
	if(framecount == 0)
		return;

	shakeFrames = framecount;
	xShake = std::abs(xs);
	yShake = std::abs(ys);
}