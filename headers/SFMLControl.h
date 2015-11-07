#pragma once

class wxSFMLCanvas : public wxControl, public sf::RenderWindow
{
public:

	wxSFMLCanvas(wxWindow* Parent = NULL, wxWindowID Id = -1, const wxPoint& Position = wxDefaultPosition,
				 const wxSize& Size = wxDefaultSize, long Style = 0);

	virtual ~wxSFMLCanvas() { /*if(rt) { rt->setActive(false);  delete rt; }*/ };

	int GetZoomLevel() const { return m_zoomlevel; }

	static float m_zoom;
	static int m_zoomlevel;

private:

	DECLARE_EVENT_TABLE()

	sf::RenderTexture *rt;
	sf::Sprite renderSprite;


	virtual void OnUpdate();

	void OnResize(wxSizeEvent& event);
	void OnIdle(wxIdleEvent&);
	void OnPaint(wxPaintEvent&);
	void OnEraseBackground(wxEraseEvent&);
	void OnLeftMouseDown(wxMouseEvent& event);
	void OnLeftMouseUp(wxMouseEvent& event);
	void OnScrollWheel(wxMouseEvent& event);
	void OnLeaveWindow(wxMouseEvent& event);
};