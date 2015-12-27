#pragma once

class wxSFMLCanvas : public wxControl, public sf::RenderWindow
{
public:

	wxSFMLCanvas(wxWindow* Parent = NULL, wxWindowID Id = -1, const wxPoint& Position = wxDefaultPosition,
				 const wxSize& Size = wxDefaultSize, long Style = 0);

	virtual ~wxSFMLCanvas() { /*if(rt) { rt->setActive(false);  delete rt; }*/ };

	int GetZoomLevel() const { return m_zoomlevel; }

	virtual sf::Vector2f GetCanvasMousePosition();

	static float m_zoom;
	static int m_zoomlevel;

protected:

	DECLARE_EVENT_TABLE()

	sf::RenderTexture *rt;
	sf::Sprite renderSprite;

	sf::Clock engineClock;

	double t;
	const double dt = 0.01;
	double frameTime;
	bool runOnce;

	double currentTime;
	int tickTime;
	double accumulator;
	float interpolation;

	bool recreate = true;

	bool capturemode;
	bool allowCapture;
	bool isCapturing;
	sf::Vector2i lastPos;
	sf::RectangleShape outlineRect;


	virtual void OnUpdate();

	virtual void OnResize(wxSizeEvent& event);
	virtual void OnIdle(wxIdleEvent&);
	virtual void OnPaint(wxPaintEvent&);
	virtual void OnEraseBackground(wxEraseEvent&);
	virtual void OnLeftMouseDown(wxMouseEvent& event);
	virtual void OnLeftMouseUp(wxMouseEvent& event);
	virtual void OnScrollWheel(wxMouseEvent& event);
	virtual void OnLeaveWindow(wxMouseEvent& event);

private:
	sf::RectangleShape hudRect;
	sf::RectangleShape crosshairRects[4];
};