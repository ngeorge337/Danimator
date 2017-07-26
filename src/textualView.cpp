#include "libs.h"
#include "DanList.h"
#include "DanStateList.h"
#include "SFMLControl.h"
#include "textualPanel.h"
#include "mainFrame.h"
#include "services.h"
#include "textualView.h"

extern DanFrame *theFrame;

wxBEGIN_EVENT_TABLE(TextualView, wxSFMLCanvas)
EVT_IDLE(TextualView::OnIdle)
EVT_PAINT(TextualView::OnPaint)
EVT_ERASE_BACKGROUND(TextualView::OnEraseBackground)
EVT_SIZE(TextualView::OnResize)
EVT_LEFT_DOWN(TextualView::OnLeftMouseDown)
EVT_LEFT_UP(TextualView::OnLeftMouseUp)
EVT_MOUSEWHEEL(TextualView::OnScrollWheel)
EVT_LEAVE_WINDOW(TextualView::OnLeaveWindow)
wxEND_EVENT_TABLE()

void TextualView::OnUpdate()
{
	setActive(true);

	bool stencil = theFrame->textualPanel->stencilBox->GetValue();


	if(!runOnce2)
	{
		runOnce2 = true;

		textureRect = sf::RectangleShape(sf::Vector2f(64, 64));
		textureRect.setFillColor(sf::Color::Cyan);

		DanStatusBar *bar = static_cast<DanStatusBar *>(theFrame->GetStatusBar());
		if(bar) bar->SetZoomValue(m_zoomlevel);
	}


	if(theFrame->textualPanel->GetActiveTexture() != nullptr)
	{
		sf::Vector2f rectSize = sf::Vector2f(theFrame->textualPanel->GetActiveTexture()->GetDimensions());
		sf::Vector2f rectScales = theFrame->textualPanel->GetActiveTexture()->GetScaleFactor();
		rectSize.x *= rectScales.x;
		rectSize.y *= rectScales.y;
		textureRect.setSize(rectSize);
	}

	Camera::GetCamera().setSize(sf::Vector2f(theFrame->textualPanel->textualView->GetSize().x, theFrame->textualPanel->textualView->GetSize().y));

	if(recreate)
	{
		recreate = false;
		rt->create(Camera::GetCamera().getSize().x, Camera::GetCamera().getSize().y, true);
	}
	rt->setSmooth(false);

	Camera::GetCamera().setCenter(sf::Vector2f(Round(textureRect.getSize().x * 0.5f), Round(textureRect.getSize().y * 0.5f)));
	Camera::GetCamera().setViewport(sf::FloatRect(0, 0, 1, 1));
	Camera::GetCamera().zoom(m_zoom);
	sf::Listener::setPosition(Camera::GetCamera().getCenter().x, 0, Camera::GetCamera().getCenter().y);
	
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
	rt->draw(textureRect);

	//activeLayer = theFrame->textualPanel->LayerList->GetSelection();
	activeLayer = theFrame->textualPanel->GetActualSelection();

	if(theFrame->textualPanel->m_activeTexture != nullptr)
	{
		if(theFrame->textualPanel->m_activeTexture->GetAllLayers().size() > 0 && activeLayer >= 0)
		{
			if(this->GetScreenRect().Contains(wxGetMousePosition()) && !theFrame->inModal)
			{
				sf::FloatRect boundary = theFrame->textualPanel->m_activeTexture->GetLayer(activeLayer).spr.getGlobalBounds();
				if(theFrame->textualPanel->m_activeTexture->GetLayer(activeLayer).rotValue == 1 || theFrame->textualPanel->m_activeTexture->GetLayer(activeLayer).rotValue == 3)
					swapvalues(boundary.width, boundary.height);
				boundary.width *= theFrame->textualPanel->GetActiveTexture()->GetScaleFactor().x;
				boundary.height *= theFrame->textualPanel->GetActiveTexture()->GetScaleFactor().y;
				if(isCapturing || boundary.contains(pos))
				{
					// draw outlining rect
					sf::Vector2f selSize = sf::Vector2f(theFrame->textualPanel->m_activeTexture->GetLayer(activeLayer).spr.getTexture()->getSize());
					if(theFrame->textualPanel->m_activeTexture->GetLayer(activeLayer).rotValue == 1 || theFrame->textualPanel->m_activeTexture->GetLayer(activeLayer).rotValue == 3)
						swapvalues(selSize.x, selSize.y);
					selSize.x *= theFrame->textualPanel->GetActiveTexture()->GetScaleFactor().x;
					selSize.y *= theFrame->textualPanel->GetActiveTexture()->GetScaleFactor().y;
					outlineRect = sf::RectangleShape(selSize);
					outlineRect.setFillColor(sf::Color::Transparent);
					outlineRect.setSize(sf::Vector2f(outlineRect.getSize().x - 2, outlineRect.getSize().y - 2));
					outlineRect.setPosition(sf::Vector2f(theFrame->textualPanel->m_activeTexture->GetLayer(activeLayer).spr.getPosition().x + 1, theFrame->textualPanel->m_activeTexture->GetLayer(activeLayer).spr.getPosition().y + 1));
					outlineRect.setOutlineColor(sf::Color::Red);
					outlineRect.setOutlineThickness(1.f);
					allowCapture = true;
				}
				else
					allowCapture = false;
			}
			else
				allowCapture = false;
		}
	}

	if(allowCapture && capturemode && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !theFrame->playingMode)
	{
		if(theFrame->textualPanel->m_activeTexture != nullptr)
		{
			if(theFrame->textualPanel->m_activeTexture->GetAllLayers().size() > 0 && activeLayer >= 0)
			{
				if(!isCapturing)
					theFrame->textualPanel->m_activeTexture->GetActionHistory().Insert(CMDTYPE_TEXTURES, FormatString("Drag offset for layer '%s'", theFrame->textualPanel->m_activeTexture->GetLayer(activeLayer).layerName));
				isCapturing = true;
				sf::Vector2i diff = sf::Vector2i(pos) - lastPos;
				lastPos = sf::Vector2i(pos);
				theFrame->textualPanel->m_activeTexture->GetLayer(activeLayer).spr.move(sf::Vector2f(diff));
				theFrame->textualPanel->xSpin->SetValue(int(theFrame->textualPanel->m_activeTexture->GetLayer(activeLayer).spr.getPosition().x));
				theFrame->textualPanel->ySpin->SetValue(int(theFrame->textualPanel->m_activeTexture->GetLayer(activeLayer).spr.getPosition().y));
				theFrame->saved = false;
			}
		}
	}

	if(stencil)
	{
		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
	}

	if(theFrame->textualPanel->GetActiveTexture() != nullptr)
	{
		rt->draw(*theFrame->textualPanel->GetActiveTexture());

		if(allowCapture)
		{
			if(stencil) glDisable(GL_STENCIL_TEST);
			rt->draw(outlineRect);
			if(stencil) glEnable(GL_STENCIL_TEST);
		}
	}

	if(stencil) glDisable(GL_STENCIL_TEST);

	rt->display();
	this->setActive();
	renderSprite.setTexture(rt->getTexture(), true);
	renderSprite.setScale(Camera::GetCamera().getSize().x / renderSprite.getLocalBounds().width, Camera::GetCamera().getSize().y / renderSprite.getLocalBounds().height);
	renderSprite.setOrigin(renderSprite.getLocalBounds().width / 2, renderSprite.getLocalBounds().height / 2);
	renderSprite.setPosition(Camera::GetCamera().getCenter());
	this->draw(renderSprite);
}

void TextualView::OnLeftMouseDown(wxMouseEvent& event)
{
	event.Skip();
	if(theFrame->playingMode) return;
	sf::Vector2f pos = GetCanvasMousePosition();

	lastPos = sf::Vector2i(pos);
	capturemode = true;
}

TextualView::TextualView(wxWindow* Parent /*= NULL*/, wxWindowID Id /*= -1*/, const wxPoint& Position /*= wxDefaultPosition*/, const wxSize& Size /*= wxDefaultSize*/, long Style /*= 0*/)
	: wxSFMLCanvas(Parent, Id, Position, Size, Style), activeLayer(-1)
{
	sf::ContextSettings settings;
	settings.depthBits = 32;
	settings.stencilBits = 8;
	sf::RenderWindow::create(GetHandle(), settings);

	runOnce2 = false;

	//textureRect = sf::RectangleShape(sf::Vector2f(64, 64));
	//textureRect.setFillColor(sf::Color::Cyan);

	rt = new sf::RenderTexture();
	rt->create(640, 480, true);
	rt->setSmooth(false);
}

sf::Vector2f TextualView::GetCanvasMousePosition()
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
