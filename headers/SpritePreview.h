#pragma once

class wxImagePanel : public wxPanel
{
	friend class DanFrame;
public:
	wxImagePanel(wxWindow *parent, const wxPoint& Position = wxDefaultPosition,
				 const wxSize& Size = wxDefaultSize);

	void SetImageFromTexture(const std::string &texName);
	void ClearImage();

	void paintEvent(wxPaintEvent & evt);
	void paintNow();
	void OnSize(wxSizeEvent& event);
	void OnIdle(wxIdleEvent& WXUNUSED(event));
	void render(wxDC& dc);

	// some useful events
	/*
	void mouseMoved(wxMouseEvent& event);
	void mouseDown(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void mouseReleased(wxMouseEvent& event);
	void rightClick(wxMouseEvent& event);
	void mouseLeftWindow(wxMouseEvent& event);
	void keyPressed(wxKeyEvent& event);
	void keyReleased(wxKeyEvent& event);
	*/

	DECLARE_EVENT_TABLE()

private:
	unsigned char *colorpixels;
	unsigned char *alphapixels;
	wxImage image;
	wxBitmap resized;
	int w, h;
	bool needUpdate;
	bool loaded;
};


