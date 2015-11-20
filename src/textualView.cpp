#include "libs.h"
#include "animator.h"
#include "DanList.h"
#include "DanStateList.h"
#include "SFMLControl.h"
#include "textualPanel.h"
#include "mainFrame.h"
#include "actionmanager.h"
#include "resrcman.h"
#include "texman.h"
#include "soundman.h"
#include "locator.h"
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
	bool stencil = theFrame->textualPanel->stencilBox->GetValue();

	if(!runOnce)
	{
		runOnce = true;

		DanStatusBar *bar = static_cast<DanStatusBar *>(theFrame->GetStatusBar());
		if(bar) bar->SetZoomValue(m_zoomlevel);
	}

	Camera::GetCamera().setSize(sf::Vector2f(theFrame->textualPanel->textualView->GetSize().x, theFrame->textualPanel->textualView->GetSize().y));

	if(recreate)
	{
		recreate = false;
		rt->create(Camera::GetCamera().getSize().x, Camera::GetCamera().getSize().y, true);
	}
	rt->setSmooth(false);

	Camera::GetCamera().setCenter(sf::Vector2f(Round(textureRect.getScale().x), Round(textureRect.getScale().y)));
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

	activeLayer = theFrame->textualPanel->LayerList->GetSelection();

	if(theFrame->textualPanel->m_activeTexture != nullptr)
	{
		if(theFrame->textualPanel->m_activeTexture->GetAllLayers().size() > 0 && activeLayer >= 0)
		{
			if(this->GetScreenRect().Contains(wxGetMousePosition()))
			{
				if(isCapturing || theFrame->textualPanel->m_activeTexture->GetLayer(activeLayer).spr.getGlobalBounds().contains(pos))
				{
					// draw outlining rect
					if(stencil) glDisable(GL_STENCIL_TEST);
					outlineRect = sf::RectangleShape(sf::Vector2f(theFrame->textualPanel->m_activeTexture->GetLayer(activeLayer).spr.getTexture()->getSize()));
					outlineRect.setFillColor(sf::Color::Transparent);
					outlineRect.setSize(sf::Vector2f(outlineRect.getSize().x - 2, outlineRect.getSize().y - 2));
					outlineRect.setPosition(sf::Vector2f(theFrame->textualPanel->m_activeTexture->GetLayer(activeLayer).spr.getPosition().x + 1, theFrame->textualPanel->m_activeTexture->GetLayer(activeLayer).spr.getPosition().y + 1));
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
	}

	if(allowCapture && capturemode && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !theFrame->playingMode)
	{
		if(theFrame->GetAnimator().GetCurrentState() != nullptr)
		{
			//if(!isCapturing)
			//	Locator::GetActionManager()->Insert(CMDTYPE_FRAMES, FormatString("Drag offset for %s, frame %d", theFrame->GetAnimator().GetCurrentState()->name, theFrame->GetAnimator().GetCurrentState()->GetOffset() + 1));
			isCapturing = true;
			sf::Vector2i diff = sf::Vector2i(pos) - lastPos;
			lastPos = sf::Vector2i(pos);
			theFrame->textualPanel->m_activeTexture->GetLayer(activeLayer).spr.move(sf::Vector2f(diff));
			theFrame->xSpin->SetValue(int(theFrame->textualPanel->m_activeTexture->GetLayer(activeLayer).spr.getPosition().x));
			theFrame->ySpin->SetValue(int(theFrame->textualPanel->m_activeTexture->GetLayer(activeLayer).spr.getPosition().y));
			theFrame->saved = false;
		}
	}

	if(stencil)
	{
		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
	}

	if(theFrame->textualPanel->m_activeTexture != nullptr)
	{
		for(int i = 0; i < theFrame->textualPanel->m_activeTexture->GetAllLayers().size(); i++)
		{
			rt->draw(theFrame->textualPanel->m_activeTexture->GetLayer(i).spr);
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

	rt = new sf::RenderTexture();
	rt->create(640, 480, true);
	rt->setSmooth(false);
}
