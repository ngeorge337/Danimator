#pragma once

class TextualView : public wxSFMLCanvas
{
public:
	TextualView(wxWindow* Parent = NULL, wxWindowID Id = -1, const wxPoint& Position = wxDefaultPosition,
				const wxSize& Size = wxDefaultSize, long Style = 0);

	int GetZoomLevel() const { return wxSFMLCanvas::m_zoomlevel; }

	virtual sf::Vector2f GetCanvasMousePosition();

protected:

	int activeLayer;

	DECLARE_EVENT_TABLE()

	sf::RenderTexture *rt;
	sf::Sprite renderSprite;
	
	bool runOnce2;

	sf::RectangleShape textureRect;

	virtual void OnUpdate();

	//virtual void OnResize(wxSizeEvent& event);
	//virtual void OnIdle(wxIdleEvent&);
	//virtual void OnPaint(wxPaintEvent&);
	//virtual void OnEraseBackground(wxEraseEvent&);
	virtual void OnLeftMouseDown(wxMouseEvent& event);
	//virtual void OnLeftMouseUp(wxMouseEvent& event);
	//virtual void OnScrollWheel(wxMouseEvent& event);
	//virtual void OnLeaveWindow(wxMouseEvent& event);
};