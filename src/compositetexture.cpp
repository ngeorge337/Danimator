#include "libs.h"
#include "resrcman.h"
#include "texman.h"
#include "animator.h"
#include "SFMLControl.h"
#include "actionmanager.h"
#include "compositetexture.h"
#include "textualView.h"
#include "textualPanel.h"
#include "mainFrame.h"

//========================================================
// Composite Texture implementation
//========================================================

std::shared_ptr<sf::Texture> CompositeTexture::operator=(std::shared_ptr<sf::Texture> t)
{
	return m_composite;
}

void CompositeTexture::DeleteLayer(int pos)
{
	if(pos >= m_layers.size())
		return;
	auto it = m_layers.begin();
	std::advance(it, pos);
	m_layers.erase(it);
}

int CompositeTexture::MoveLayerTowardsBegin(int id)
{
	// Cannot move a layer 'Down' when it's the bottom-most layer
	if(id <= 0)
		return id;
	// Moving "down" moves the layer further out of the vector/towards the front
	// because we render the layers starting from first item to last
	// so the last item is drawn over all the others for instance
	auto it1 = m_layers.begin();
	auto it2 = m_layers.begin();
	std::advance(it1, id);
	std::advance(it2, id - 1);
	std::iter_swap(it1, it2);
	return id - 1;
}

int CompositeTexture::MoveLayerTowardsEnd(int id)
{
	// Cannot move a layer 'Up' when it's the top-most layer
	if(id >= m_layers.size() - 1)
		return id;
	// Moving "up" moves the layer further into the vector/towards the back
	// because we render the layers starting from first item to last
	// so the last item is drawn over all the others for instance
	auto it1 = m_layers.begin();
	auto it2 = m_layers.begin();
	std::advance(it1, id);
	std::advance(it2, id + 1);
	std::iter_swap(it1, it2);
	return id + 1;
}

CompositeLayer_t & CompositeTexture::GetLayer(int id)
{
	return m_layers[id];
}

void CompositeTexture::CreateComposite(bool recompile)
{
	if(m_composite) m_composite.reset();

	ClearDependencies();

	sf::RenderTexture outputTexture;
	outputTexture.create(int(Round(float(m_dims.x) * m_scaleFactor.x)), int(Round(float(m_dims.y) * m_scaleFactor.y)));
	outputTexture.setActive();
	outputTexture.clear(sf::Color::Transparent);

	sf::RenderStates states;
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0);

	for(int i = 0; i < m_layers.size(); i++)
	{
		if(m_layers[i].isVisible)
		{
			//doOverlay = false;

			// Update composite dependencies here
			if(Locator::GetTextureManager()->IsComposite(m_layers[i].spriteName))
			{
				AddDependency(m_layers[i].spriteName);
				if(recompile) Locator::GetTextureManager()->GetCompositeTexture(m_layers[i].spriteName)->CreateComposite();	// compile the dependencies too
			}

			sf::Sprite drawSprite = sf::Sprite(m_layers[i].spr);
			drawSprite.setTexture(*Locator::GetTextureManager()->GetTexture(m_layers[i].spriteName), true);


			sf::Color cr = drawSprite.getColor();
			switch(m_layers[i].renderStyle)
			{
			case STYLE_COPY:
				states.blendMode = sf::BlendNone;
				break;
			case STYLE_COPYALPHA:
				states.blendMode = sf::BlendAlpha;
				cr.a = 255;
				drawSprite.setColor(cr);
				break;
			case STYLE_ADD:
				states.blendMode = sf::BlendAdd;
				break;
			case STYLE_TRANSLUCENT:
				states.blendMode = sf::BlendAlpha;
				break;
			default:
				states.blendMode = sf::BlendNone;
				break;
			}
			if(m_layers[i].flipX || m_layers[i].flipY || m_layers[i].rotValue > 0)
			{
				sf::Vector2f newScale = drawSprite.getScale();
				sf::Vector2f offs = sf::Vector2f(0, 0);
				sf::FloatRect bounds = drawSprite.getLocalBounds();
				offs.x += Round(bounds.width * 0.5f);
				offs.y += Round(bounds.height * 0.5f);
				sf::Vector2f origpos = drawSprite.getPosition();
				sf::Vector2f pos = drawSprite.getPosition();
				pos.x += offs.x;
				pos.y += offs.y;
				drawSprite.setOrigin(bounds.width * 0.5f, bounds.height * 0.5f);
				if(m_layers[i].rotValue > 0)
				{
					float finalRotation = float(m_layers[i].rotValue) * 90.f;
					drawSprite.setRotation(finalRotation);
					switch(m_layers[i].rotValue)
					{
					case 1: // 90
					case 3: // 270
						pos = origpos;
						pos.x += offs.y;
						pos.y += offs.x;
						break;
					}
				}
				if(m_layers[i].flipX)
				{
					newScale.x *= -1.f;
				}
				if(m_layers[i].flipY)
				{
					newScale.y *= -1.f;
				}
				drawSprite.setScale(newScale);
				drawSprite.setPosition(pos);
			}
			outputTexture.draw(drawSprite, states);
		}
	}

	glDisable(GL_ALPHA_TEST);
	outputTexture.display();

	sf::Image img = outputTexture.getTexture().copyToImage();
	m_composite = std::shared_ptr<sf::Texture>(new sf::Texture());
	if(!m_composite->loadFromImage(img))
		Console->LogMessage("Failed to create composite texture!");
	m_composite->setSmooth(r_bilinear);
}

std::shared_ptr<sf::Texture> CompositeTexture::GetFinalTexture()
{
	return m_composite;
}

void CompositeTexture::SetDimensions(int w, int h)
{
	m_dims = sf::Vector2i(w, h);
}

sf::Vector2i CompositeTexture::GetDimensions()
{
	return m_dims;
}

CompositeTexture::CompositeTexture() : m_composite(new sf::Texture()), m_dims(sf::Vector2i(64, 64)), m_scaleFactor(sf::Vector2f(1.f, 1.f)), m_realScale(sf::Vector2f(1.f, 1.f)), m_namespace("Sprite")
{

}

std::vector<CompositeLayer_t> & CompositeTexture::GetAllLayers()
{
	return m_layers;
}

void CompositeTexture::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	//bool doOverlay = false;
	for(int i = 0; i < m_layers.size(); i++)
	{
		if(m_layers[i].isVisible)
		{
			//doOverlay = false;
			sf::Sprite drawSprite = sf::Sprite(m_layers[i].spr);
			drawSprite.setTexture(*Locator::GetTextureManager()->GetTexture(m_layers[i].spriteName), true);
			sf::Color cr = drawSprite.getColor();
			switch(m_layers[i].renderStyle)
			{
			case STYLE_COPY:
				states.blendMode = sf::BlendNone;
				break;
			case STYLE_COPYALPHA:
				states.blendMode = sf::BlendAlpha;
				cr.a = 255;
				drawSprite.setColor(cr);
				break;
			case STYLE_ADD:
				states.blendMode = sf::BlendAdd;
				break;
			case STYLE_TRANSLUCENT:
				states.blendMode = sf::BlendAlpha;
				break;
			default:
				states.blendMode = sf::BlendNone;
				break;
			}
			if(m_layers[i].flipX || m_layers[i].flipY || m_layers[i].rotValue > 0)
			{
				sf::Vector2f newScale = drawSprite.getScale();
				sf::Vector2f offs = sf::Vector2f(0, 0);
				sf::FloatRect bounds = drawSprite.getLocalBounds();
				offs.x += Round(bounds.width * 0.5f);
				offs.y += Round(bounds.height * 0.5f);
				sf::Vector2f origpos = drawSprite.getPosition();
				sf::Vector2f pos = drawSprite.getPosition();
				pos.x += offs.x;
				pos.y += offs.y;
				drawSprite.setOrigin(bounds.width * 0.5f, bounds.height * 0.5f);
				if(m_layers[i].rotValue > 0)
				{
					float finalRotation = float(m_layers[i].rotValue) * 90.f;
					drawSprite.setRotation(finalRotation);
					switch(m_layers[i].rotValue)
					{
					case 1: // 90
					case 3: // 270
						pos = origpos;
						pos.x += offs.y;
						pos.y += offs.x;
						break;
					}
				}
				if(m_layers[i].flipX)
				{
					newScale.x *= -1.f;
				}
				if(m_layers[i].flipY)
				{
					newScale.y *= -1.f;
				}
				drawSprite.setScale(newScale);
				drawSprite.setPosition(pos);
			}
			target.draw(drawSprite, states);
		}
	}
}

TextureActionManager & CompositeTexture::GetActionHistory()
{
	return m_Actions;
}

sf::Vector2f CompositeTexture::GetRealScale()
{
	return m_realScale;
}

void CompositeTexture::SetScaleFactor(float w, float h)
{
	// w and h will be passed in as percentages, so convert them in this function
	m_scaleFactor = sf::Vector2f(w / 100.f, h / 100.f);
	float realX = RoundToPlace(1.f / m_scaleFactor.x, 2);
	float realY = RoundToPlace(1.f / m_scaleFactor.y, 2);
	m_realScale = sf::Vector2f(realX, realY);
	// Combine the matrix with the final one to create the scaled visual
	for(int i = 0; i < m_layers.size(); i++)
	{
		m_layers[i].spr.setScale(m_scaleFactor);
	}
}

sf::Vector2f CompositeTexture::GetScaleFactor()
{
	return m_scaleFactor;
}

void CompositeTexture::SetNamespace(const std::string &ns)
{
	m_namespace = ns;
}

std::string CompositeTexture::GetNamespace()
{
	return m_namespace;
}

void CompositeTexture::AddLayer(const std::string &name)
{
	//m_layers.insert(m_layers.begin(), 1, CompositeLayer_t(name));
	m_layers.push_back(CompositeLayer_t(name));	// REMEMBER: end of vector -> top of layers
}

wxString CompositeTexture::GetTextureCode()
{
	/*

		<namespace> <name>, <width>, <height>
		{
			xscale <value>
			yscale <value>
			patch <name>, <x origin>, <y origin>
			{
				<flipx>
				<flipy>
				rotate <value>
				blend <r>, <g>, <b>, 255
				alpha <value as normalized>
				style <style type>
			}
		}

	*/

	std::string output = "";

	output += m_namespace + " ";
	output += m_texname + ", ";
	output += std::to_string(m_dims.x) + ", " + std::to_string(m_dims.y);
	output += "\n{\n";
	if(m_realScale.x != 1.f)
	{
		output += "\t";
		output += "XScale " + std::to_string(m_realScale.x);
		output += "\n";
	}
	if(m_realScale.y != 1.f)
	{
		output += "\t";
		output += "YScale " + std::to_string(m_realScale.y);
		output += "\n";
	}
	if(!m_layers.empty())
	{
		for(int i = 0; i < m_layers.size(); ++i)
		{
			output += "\t";
			output += "Patch " + m_layers[i].spriteName + ", " + std::to_string(int(m_layers[i].spr.getPosition().x)) + ", " + std::to_string(int(m_layers[i].spr.getPosition().y));
			output += "\n\t{\n";
			if(m_layers[i].flipX)
				output += "\t\tFlipX\n";
			if(m_layers[i].flipY)
				output += "\t\tFlipY\n";
			if(m_layers[i].rotValue)
				output += "\t\tRotate " + std::to_string(int(m_layers[i].rotValue * 90)) + "\n";
			if(m_layers[i].spr.getColor() != sf::Color::White)
			{
				sf::Color outColor = m_layers[i].spr.getColor();
				std::string redValue = std::to_string(outColor.r);
				std::string greenValue = std::to_string(outColor.g);
				std::string blueValue = std::to_string(outColor.b);
				output += "\t\t";
				output += "Blend " + redValue + ", " + greenValue + ", " + blueValue + ", 255";
				output += "\n";
			}
			if(m_layers[i].opacityValue != 255)
				output += "\t\tAlpha " + std::to_string(ByteToFloat(m_layers[i].opacityValue));
			switch(m_layers[i].renderStyle)
			{
			case STYLE_COPY:
				output += "\t\tStyle Copy\n";
				break;
			case STYLE_COPYALPHA:
				output += "\t\tStyle CopyAlpha\n";
				break;
			case STYLE_ADD:
				output += "\t\tStyle Add\n";
				break;
			case STYLE_TRANSLUCENT:
				output += "\t\tStyle Translucent\n";
				break;
			}
			output += "\t}\n";
		}
	}
	output += "}\n";

	return wxString(output);
}

void CompositeTexture::ClearDependencies()
{
	m_texDepends.clear();
}

void CompositeTexture::AddDependency(const std::string &depName)
{
	if(m_texDepends.find(depName) == m_texDepends.end())
		m_texDepends.insert(depName);
}

bool CompositeTexture::HasDependency(const std::string &depName)
{
	return m_texDepends.find(depName) != m_texDepends.end();
}

bool CompositeTexture::HasAnyDependencies()
{
	return !m_texDepends.empty();
}

const std::set<std::string> & CompositeTexture::GetDependencies()
{
	return m_texDepends;
}

void CompositeTexture::Deserialize(File &file)
{
	file >> m_dims
		>> m_scaleFactor
		>> m_realScale
		>> m_namespace
		>> m_texname
		>> m_hash;

	int layerCount;
	file >> layerCount;
	for(int i = 0; i < layerCount; i++)
	{
		CompositeLayer_t layer;
		file >> layer;
		m_layers.push_back(layer);
	}
	int depCount;
	file >> depCount;
	for(int x = 0; x < depCount; x++)
	{
		std::string dep;
		file >> dep;
		m_texDepends.insert(dep);
	}
}

void CompositeTexture::Serialize(File &file)
{
	file << m_dims
		<< m_scaleFactor
		<< m_realScale
		<< m_namespace
		<< m_texname
		<< m_hash;

	file << int(m_layers.size());
	for(int i = 0; i < m_layers.size(); i++)
	{
		file << m_layers[i];
	}
	file << int(m_texDepends.size());
	for(auto it = m_texDepends.begin(); it != m_texDepends.end(); ++it)
	{
		file << *it;
	}
}
