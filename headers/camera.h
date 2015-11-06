#pragma once

class Camera
{
public:
	Camera();
	Camera(sf::FloatRect camsize);
	Camera(sf::FloatRect camsize, sf::Vector2f pos);

	static void Init(sf::FloatRect camsize);
	static void MoveCamera(sf::Vector2f newpos);
	static void AdjustCamera(float x, float y);
	static void SetShake(float xs, float ys, int framecount);
	static void Update();

	static sf::View &GetCamera();

	static void SetBoundaries(sf::Vector2f sz);

private:
	static void ResetParams();
	static int shakeFrames;
	static float xShake;
	static float yShake;
	static float offsetx, offsety;
	static sf::View m_Camera;
	
	static sf::FloatRect m_boundaries;
	static sf::Vector2f m_boundariesvec;
	static bool m_bound;
};