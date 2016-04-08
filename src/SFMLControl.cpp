#include "libs.h"
#include "animator.h"
#include "DanList.h"
#include "DanStateList.h"
#include "textualPanel.h"
#include "SFMLControl.h"
#include "mainFrame.h"
#include "actionmanager.h"
#include "resrcman.h"
#include "texman.h"
#include "soundman.h"
#include "locator.h"

CVAR(Bool, r_bilinear, true, false, true, "Use bilinear filtering mode in the render view", CVAR_ARCHIVE);

extern DanFrame *theFrame;

float wxSFMLCanvas::m_zoom = 1.f;
int wxSFMLCanvas::m_zoomlevel = 100;


BEGIN_EVENT_TABLE(wxSFMLCanvas, wxControl)
EVT_IDLE(wxSFMLCanvas::OnIdle)
EVT_PAINT(wxSFMLCanvas::OnPaint)
EVT_ERASE_BACKGROUND(wxSFMLCanvas::OnEraseBackground)
EVT_SIZE(wxSFMLCanvas::OnResize)
EVT_LEFT_DOWN(wxSFMLCanvas::OnLeftMouseDown)
EVT_LEFT_UP(wxSFMLCanvas::OnLeftMouseUp)
EVT_MOUSEWHEEL(wxSFMLCanvas::OnScrollWheel)
EVT_LEAVE_WINDOW(wxSFMLCanvas::OnLeaveWindow)
END_EVENT_TABLE()

void wxSFMLCanvas::OnIdle(wxIdleEvent&)
{
	// Send a paint message when the control is idle, to ensure maximum framerate
	/*
	sf::Event ev;
	while(this->pollEvent(ev))
	{
	}
	*/
	Refresh();
}

void wxSFMLCanvas::OnEraseBackground(wxEraseEvent&)
{
}

void wxSFMLCanvas::OnPaint(wxPaintEvent&)
{
	// Prepare the control to be repainted
	wxPaintDC Dc(this);

	setActive(true);

	// Let the derived class do its specific stuff
	OnUpdate();

	// Display on screen
	display();
}

wxSFMLCanvas::wxSFMLCanvas(wxWindow* Parent, wxWindowID Id, const wxPoint& Position, const wxSize& Size, long Style) :
wxControl(Parent, Id, Position, Size, Style)
{
#ifdef __WXGTK__

	// GTK implementation requires to go deeper to find the
	// low-level X11 identifier of the widget
	gtk_widget_realize(m_wxwindow);
	gtk_widget_set_double_buffered(m_wxwindow, false);
	GdkWindow* Win = gtk_widget_get_window((GtkWidget *)GetHandle());
	XFlush(GDK_WINDOW_XDISPLAY(Win));
	sf::RenderWindow::create(GDK_WINDOW_XWINDOW(Win));

#else
	
	// Tested under Windows XP only (should work with X11
	// and other Windows versions - no idea about MacOS)
	sf::ContextSettings settings;
	settings.depthBits = 32;
	settings.stencilBits = 8;
	sf::RenderWindow::create(GetHandle(), settings);

	rt = new sf::RenderTexture();
	rt->create(640, 480, true);
	rt->setSmooth(false);

#endif

	engineClock.restart();

	t = 0.0;
	frameTime = 0.0;
	runOnce = false;
	accumulator = 0.0;
	interpolation = 0.f;

	recreate = true;
	capturemode = false;
	allowCapture = false;
	isCapturing = false;

	currentTime = engineClock.getElapsedTime().asSeconds();
	tickTime = currentTime;

	hudRect = sf::RectangleShape(sf::Vector2f(320, 200));
	hudRect.setFillColor(sf::Color::Cyan);

	for(int i = 0; i < 4; i++)
	{
		crosshairRects[i].setFillColor(sf::Color::Red);
	}

	const float GAP = 14.f;

	// Top
	crosshairRects[0].setSize(sf::Vector2f(4, 12));
	crosshairRects[0].setOrigin(crosshairRects[0].getSize().x * 0.5f, crosshairRects[0].getSize().y * 0.5f);
	crosshairRects[0].setPosition(sf::Vector2f(Round(160.f), Round(100.f - GAP)));

	// Right
	crosshairRects[1].setSize(sf::Vector2f(12, 4));
	crosshairRects[1].setOrigin(crosshairRects[1].getSize().x * 0.5f, crosshairRects[1].getSize().y * 0.5f);
	crosshairRects[1].setPosition(sf::Vector2f(Round(160.f + GAP), Round(100.f)));

	// Left
	crosshairRects[2].setSize(sf::Vector2f(12, 4));
	crosshairRects[2].setOrigin(crosshairRects[2].getSize().x * 0.5f, crosshairRects[2].getSize().y * 0.5f);
	crosshairRects[2].setPosition(sf::Vector2f(Round(160.f - GAP), Round(100.f)));

	// Bottom
	crosshairRects[3].setSize(sf::Vector2f(4, 12));
	crosshairRects[3].setOrigin(crosshairRects[3].getSize().x * 0.5f, crosshairRects[3].getSize().y * 0.5f);
	crosshairRects[3].setPosition(sf::Vector2f(Round(160.f), Round(100.f + GAP)));
}

void wxSFMLCanvas::OnUpdate()
{
	setActive(true);
	bool stencil = theFrame->stencilCheckBox->GetValue();

	if(!runOnce)
	{
		runOnce = true;

		DanStatusBar *bar = static_cast<DanStatusBar *>(theFrame->GetStatusBar());
		if(bar) bar->SetZoomValue(m_zoomlevel);
	}

	Camera::GetCamera().setSize(sf::Vector2f(theFrame->glCanvas->GetSize().x, theFrame->glCanvas->GetSize().y));

	if(recreate)
	{
		recreate = false;
		rt->create(Camera::GetCamera().getSize().x, Camera::GetCamera().getSize().y, true);
	}
	rt->setSmooth(false);

	Camera::GetCamera().setCenter(sf::Vector2f(160, 100));
	Camera::GetCamera().setViewport(sf::FloatRect(0, 0, 1, 1));
	Camera::GetCamera().zoom(m_zoom);
	sf::Listener::setPosition(Camera::GetCamera().getCenter().x, 0, Camera::GetCamera().getCenter().y);

	double newTime = engineClock.getElapsedTime().asSeconds();
	frameTime = newTime - currentTime;
	if(frameTime > 0.25)
		frameTime = 0.25;
	currentTime = newTime;
	accumulator += frameTime;

	while(accumulator >= dt)
	{
		if(theFrame->playingMode) theFrame->GetAnimator().Tick(dt);
		accumulator -= dt;
		t += dt;
	}

	if(theFrame->playingMode && theFrame->GetAnimator().GetCurrentState() != nullptr) theFrame->timelineSlider->SetValue(theFrame->GetAnimator().GetCurrentState()->frameOffset + 1);

	this->setView(Camera::GetCamera());
	clear(sf::Color::Black);

	rt->setActive();
	rt->setView(Camera::GetCamera());
	rt->clear(sf::Color::Black);
		
	sf::Vector2f pos = GetCanvasMousePosition();

	// stencil test
	if(stencil)
	{
		glEnable(GL_STENCIL_TEST);
		glClearStencil(0);
		glClear(GL_STENCIL_BUFFER_BIT);
		glStencilMask(0xFF);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	}
	rt->draw(hudRect);

	if(theFrame->GetAnimator().GetCurrentState() != nullptr && theFrame->GetAnimator().GetCurrentState()->m_frames.size() > 0 && !theFrame->playingMode)
	{
		if(this->GetScreenRect().Contains(wxGetMousePosition()) && !theFrame->inModal)
		{
			if(isCapturing || theFrame->GetAnimator().GetCurrentState()->GetCurrentFrame().sprite.getGlobalBounds().contains(pos))
			{
				// draw outlining rect
				if(stencil) glDisable(GL_STENCIL_TEST);
				outlineRect = sf::RectangleShape(sf::Vector2f(theFrame->GetAnimator().GetCurrentState()->GetCurrentFrame().sprite.getTexture()->getSize()));
				outlineRect.setFillColor(sf::Color::Transparent);
				outlineRect.setSize(sf::Vector2f(outlineRect.getSize().x - 2, outlineRect.getSize().y - 2));
				outlineRect.setPosition(sf::Vector2f(theFrame->GetAnimator().GetCurrentState()->GetCurrentFrame().sprite.getPosition().x + 1, theFrame->GetAnimator().GetCurrentState()->GetCurrentFrame().sprite.getPosition().y + 1));
				outlineRect.setOutlineColor(sf::Color::Red);
				outlineRect.setOutlineThickness(1.f);
				allowCapture = true;

				rt->draw(outlineRect);

				if(stencil) glEnable(GL_STENCIL_TEST);
			}
			else
				allowCapture = false;
		}
		else
			allowCapture = false;
	}

	if(allowCapture && capturemode && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !theFrame->playingMode)
	{
		if(theFrame->GetAnimator().GetCurrentState() != nullptr)
		{
			if(!isCapturing)
				Locator::GetActionManager()->Insert(CMDTYPE_FRAMES, FormatString("Drag offset for %s, frame %d", theFrame->GetAnimator().GetCurrentState()->name, theFrame->GetAnimator().GetCurrentState()->GetOffset() + 1));
			isCapturing = true;
			sf::Vector2i diff = sf::Vector2i(pos) - lastPos;
			lastPos = sf::Vector2i(pos);
			theFrame->GetAnimator().GetCurrentState()->GetCurrentFrame().sprite.move(sf::Vector2f(diff));
			theFrame->xSpin->SetValue(int(theFrame->GetAnimator().GetCurrentState()->GetCurrentFrame().sprite.getPosition().x));
			theFrame->ySpin->SetValue(int(theFrame->GetAnimator().GetCurrentState()->GetCurrentFrame().sprite.getPosition().y));
			theFrame->saved = false;
		}
	}

	if(theFrame->crosshairCheckBox->GetValue())
	{
		if(stencil) glDisable(GL_STENCIL_TEST);
		for(int i = 0; i < 4; i++)
		{
			rt->draw(crosshairRects[i]);
		}
		if(stencil) glEnable(GL_STENCIL_TEST);
	}

	if(stencil)
	{
		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
	}
	rt->draw(theFrame->GetAnimator());

	if(stencil) glDisable(GL_STENCIL_TEST);

	rt->display();
	this->setActive();
	renderSprite.setTexture(rt->getTexture(), true);
	renderSprite.setScale(Camera::GetCamera().getSize().x / renderSprite.getLocalBounds().width, Camera::GetCamera().getSize().y / renderSprite.getLocalBounds().height);
	renderSprite.setOrigin(renderSprite.getLocalBounds().width / 2, renderSprite.getLocalBounds().height / 2);
	renderSprite.setPosition(Camera::GetCamera().getCenter());
	this->draw(renderSprite);
}

void wxSFMLCanvas::OnResize(wxSizeEvent& event)
{
#ifdef __WXGTK__
	// If I recreate here GTK gives an assertion failure.
	recreate_flag = true;
#endif

	recreate = true;
	sf::Event ev;
	while(this->pollEvent(ev))
	{
	}
}

void wxSFMLCanvas::OnLeftMouseDown(wxMouseEvent& event)
{
	event.Skip();
	if(theFrame->playingMode) return;
	sf::Vector2f pos = GetCanvasMousePosition();

	lastPos = sf::Vector2i(pos);
	capturemode = true;
}

void wxSFMLCanvas::OnLeftMouseUp(wxMouseEvent& event)
{
	event.Skip();
	capturemode = false;
	isCapturing = false;
}

void wxSFMLCanvas::OnScrollWheel(wxMouseEvent& event)
{
	int delta = event.GetWheelRotation();
	// max zoom out = 10% (1.9f)
	// max zoom in = 250% (0.25f)
	if(delta < 0 && m_zoomlevel > 10)	// zoom out (increase zoom value == increasing view size)
	{
		m_zoom += 0.05f;
		m_zoomlevel -= 5;
	}
	else if(delta > 0 && m_zoomlevel < 195) // zoom in (decrease zoom value == decrease view size)
	{
		m_zoom -= 0.05f;
		m_zoomlevel += 5;
	}
	//ClampTo(m_zoom, 0.1f, 1.85f);
	//ClampTo(m_zoomlevel, 10, 250);

	DanStatusBar *bar = static_cast<DanStatusBar *>(theFrame->GetStatusBar());
	bar->SetZoomValue(m_zoomlevel);
}

void wxSFMLCanvas::OnLeaveWindow(wxMouseEvent& event)
{
	event.Skip();
	isCapturing = false;
	capturemode = false;
}

sf::Vector2f wxSFMLCanvas::GetCanvasMousePosition()
{
	sf::Vector2f pos;
	wxPoint p = ScreenToClient(wxPoint(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y));
	pos.x = p.x;
	pos.y = p.y;
	pos.x = Round(pos.x);
	pos.y = Round(pos.y);
	pos = rt->mapPixelToCoords(sf::Vector2i(pos));
	return pos;
}
