#include "libs.h"
#include "SpritePreview.h"

BEGIN_EVENT_TABLE(wxImagePanel, wxPanel)
// some useful events
/*
EVT_MOTION(wxImagePanel::mouseMoved)
EVT_LEFT_DOWN(wxImagePanel::mouseDown)
EVT_LEFT_UP(wxImagePanel::mouseReleased)
EVT_RIGHT_DOWN(wxImagePanel::rightClick)
EVT_LEAVE_WINDOW(wxImagePanel::mouseLeftWindow)
EVT_KEY_DOWN(wxImagePanel::keyPressed)
EVT_KEY_UP(wxImagePanel::keyReleased)
EVT_MOUSEWHEEL(wxImagePanel::mouseWheelMoved)
*/

// catch paint events
EVT_PAINT(wxImagePanel::paintEvent)
//Size event
EVT_SIZE(wxImagePanel::OnSize)
EVT_IDLE(wxImagePanel::OnIdle)
END_EVENT_TABLE()


// some useful events
/*
void wxImagePanel::mouseMoved(wxMouseEvent& event) {}
void wxImagePanel::mouseDown(wxMouseEvent& event) {}
void wxImagePanel::mouseWheelMoved(wxMouseEvent& event) {}
void wxImagePanel::mouseReleased(wxMouseEvent& event) {}
void wxImagePanel::rightClick(wxMouseEvent& event) {}
void wxImagePanel::mouseLeftWindow(wxMouseEvent& event) {}
void wxImagePanel::keyPressed(wxKeyEvent& event) {}
void wxImagePanel::keyReleased(wxKeyEvent& event) {}
*/

wxImagePanel::wxImagePanel(wxWindow* parent, const wxPoint& Position, const wxSize& Size) :
wxPanel(parent, wxID_ANY, Position, Size, wxBORDER_SIMPLE, "Preview"), colorpixels(nullptr), alphapixels(nullptr), needUpdate(true), loaded(false)
{
	// load the file... ideally add a check to see if loading was successful
	//image.LoadFile(file, format);
	w = -1;
	h = -1;
}

/*
* Called by the system of by wxWidgets when the panel needs
* to be redrawn. You can also trigger this call by
* calling Refresh()/Update().
*/

void wxImagePanel::paintEvent(wxPaintEvent & evt)
{
	// depending on your system you may need to look at double-buffered dcs
	wxPaintDC dc(this);
	render(dc);
}

/*
* Alternatively, you can use a clientDC to paint on the panel
* at any time. Using this generally does not free you from
* catching paint events, since it is possible that e.g. the window
* manager throws away your drawing when the window comes to the
* background, and expects you will redraw it when the window comes
* back (by sending a paint event).
*/
void wxImagePanel::paintNow()
{
	// depending on your system you may need to look at double-buffered dcs
	wxClientDC dc(this);
	render(dc);
}

/*
* Here we do the actual rendering. I put it in a separate
* method so that it can work no matter what type of DC
* (e.g. wxPaintDC or wxClientDC) is used.
*/
void wxImagePanel::render(wxDC&  dc)
{
	int neww, newh;
	dc.GetSize(&neww, &newh);

	float scaleRatio = 1.f;

	if(loaded)
	{
		if(needUpdate || (neww != w || newh != h))
		{
			needUpdate = false;

			w = neww;
			h = newh;

			wxSize iSize = image.GetSize();

			if(iSize.x > iSize.y)
			{
				scaleRatio = float(w) / float(iSize.x);
			}

			else if(iSize.x < iSize.y)
			{
				scaleRatio = float(h) / float(iSize.y);
			}
			int scaledx = int(float(iSize.x) * scaleRatio);
			int scaledy = int(float(iSize.y) * scaleRatio);

			if(scaledx > neww || scaledy > newh)
			{
				int diff = 0;
				if(scaledx > neww)
				{
					diff = scaledx - neww;
				}
				else if(scaledy > newh)
				{
					diff = scaledy - newh;
				}
				scaledx -= diff;
				scaledy -= diff;
			}

			resized = wxBitmap(image.Scale(scaledx, scaledy /*, wxIMAGE_QUALITY_HIGH*/));
			dc.DrawBitmap(resized, 0, 0, false);
		}
		else
		{
			dc.DrawBitmap(resized, 0, 0, false);
		}
	}
}

/*
* Here we call refresh to tell the panel to draw itself again.
* So when the user resizes the image panel the image should be resized too.
*/
void wxImagePanel::OnSize(wxSizeEvent& event){
	Refresh();
	//skip the event.
	event.Skip();
}

void wxImagePanel::SetImageFromTexture(const std::string &texName)
{
	std::shared_ptr<sf::Texture> tex = Locator::GetTextureManager()->GetTexture(texName);
	sf::Image img = tex->copyToImage();
	const sf::Uint8 *texpixels = img.getPixelsPtr();

	if(texpixels == nullptr)
		return;

	const sf::Uint8 *p = texpixels;

	int texWidth = tex->getSize().x;
	int texHeight = tex->getSize().y;

	int len = (texWidth * texHeight) * 4;
	int pos = 0;

	if(len == 0)
		return;

	if(colorpixels != nullptr)
	{
		delete[] colorpixels;
		colorpixels = nullptr;
	}
	if(alphapixels != nullptr)
	{
		delete[] alphapixels;
		alphapixels = nullptr;
	}

	colorpixels = new unsigned char[(texWidth * texHeight) * 3]();
	alphapixels = new unsigned char[(texWidth * texHeight)]();

	unsigned char *cpx = colorpixels;
	unsigned char *apx = alphapixels;

	while(pos < len)
	{
		cpx[0] = *p;
		++cpx;
		++pos;
		++p;
		if(pos >= len) break;

		cpx[0] = *p;
		++cpx;
		++pos;
		++p;
		if(pos >= len) break;

		cpx[0] = *p;
		++cpx;
		++pos;
		++p;
		if(pos >= len) break;

		apx[0] = *p;
		++apx;
		++pos;
		++p;
		if(pos >= len) break;
	}

	image = wxImage(texWidth, texHeight, colorpixels, alphapixels, true);
	loaded = true;
	needUpdate = true;
}

void wxImagePanel::ClearImage()
{
	if(colorpixels != nullptr)
	{
		delete[] colorpixels;
		colorpixels = nullptr;
	}
	if(alphapixels != nullptr)
	{
		delete[] alphapixels;
		alphapixels = nullptr;
	}
	loaded = false;
}

void wxImagePanel::OnIdle(wxIdleEvent& WXUNUSED(event))
{
	//Refresh();
}
