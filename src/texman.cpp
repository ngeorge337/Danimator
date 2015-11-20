#include "libs.h"
#include "util.h"
#include "static.h"
#include "file.h"
#include "configfile.h"
#include "ccmd.h"
#include "cvar.h"
#include "console.h"
#include "token.h"
#include "strmanip.h"
#include "resrcman.h"
#include "texman.h"


void TextureManager::SetFiltering(bool enable)
{
	for(auto it = texmap.begin(); it != texmap.end(); ++it)
	{
		it->second.ptr->setSmooth(enable);
	}

	for(auto itx = compmap.begin(); itx != compmap.end(); ++itx)
	{
		itx->second->GetFinalTexture()->setSmooth(enable);
	}
}

void TextureManager::UnloadAll()
{
	texmap.clear();
	compmap.clear();
}

void TextureManager::ReloadAll()
{
	for(auto it = texmap.begin(); it != texmap.end(); ++it)
	{
		Reload(it->first);
	}
}

void TextureManager::Unload( std::string fileName )
{
	if(texmap.find(fileName) != texmap.end())
		texmap.erase(fileName);
	else if(compmap.find(fileName) != compmap.end())
		compmap.erase(fileName);
}

void TextureManager::Reload( std::string fileName )
{
	Unload(fileName);
	Precache(fileName);
}

bool TextureManager::Precache( std::string fileName )
{
	bool success = false;
	std::string origname = fileName;
	std::shared_ptr<sf::Texture> t(new sf::Texture);

	if(texmap.find(fileName) != texmap.end())
		return true;

	for(int i = 0; i < m_searchDirs.size(); i++)
	{
		if(!HasDirectory(origname))
			fileName = m_searchDirs[i] + origname;

		if(t->loadFromFile(fileName))
		{
			success = true;
			break;
		}
	}

	if(!success)
	{
		Console->LogMessage("TextureManager::Precache() -- Could not load texture '%s'", fileName);
	}

	Resource_t<sf::Texture> resrc;
	resrc.ptr = t;
	resrc.file_name = GetStringFilename(fileName);
	resrc.file_dir = GetStringDirectory(fileName);

	this->texmap.insert(std::pair<std::string, Resource_t<sf::Texture>>(origname, resrc));

	return true;
}

std::shared_ptr<sf::Texture> TextureManager::GetTexture(std::string name)
{
	// Search composite textures first, since we don't precache those as they are user-generated
	if(compmap.find(name) != compmap.end())
		return compmap[name]->GetFinalTexture();

	else if(texmap.find(name) == texmap.end())
		Precache(name);
		//if(!Precache(name))
		//	return nullptr;

	return texmap[name].ptr;
}

void TextureManager::Acquire( sf::Texture &tex, std::string fileName )
{
	Resource_t<sf::Texture> resrc;
	resrc.ptr = std::shared_ptr<sf::Texture>(new sf::Texture(tex));
	resrc.file_name = GetStringFilename(fileName);
	resrc.file_dir = GetStringDirectory(fileName);
	this->texmap.insert(std::pair<std::string, Resource_t<sf::Texture>>(resrc.file_name, resrc));
}

void TextureManager::Remap(const std::string &name, const std::string &newname)
{
	if(name == newname)
		return;
	texmap[newname] = texmap[name];
	texmap.erase(name);
}

void TextureManager::CreateEmptyTexture(const std::string &texName)
{
	Resource_t<sf::Texture> resrc;
	resrc.ptr = std::shared_ptr<sf::Texture>(new sf::Texture());
	resrc.file_name = texName;
	resrc.file_dir = "";
	this->texmap.insert(std::pair<std::string, Resource_t<sf::Texture>>(texName, resrc));
}


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

void CompositeTexture::MoveLayerDown(int id)
{
	// Cannot move a layer 'Down' when it's the bottom-most layer
	if(id <= 0)
		return;
	// Moving "down" moves the layer further out of the vector/towards the front
	// because we render the layers starting from first item to last
	// so the last item is drawn over all the others for instance
	auto it1 = m_layers.begin();
	auto it2 = m_layers.begin();
	std::advance(it1, id);
	std::advance(it2, id - 1);
	std::iter_swap(it1, it2);
}

void CompositeTexture::MoveLayerUp(int id)
{
	// Cannot move a layer 'Up' when it's the top-most layer
	if(id >= m_layers.size() - 1)
		return;
	// Moving "up" moves the layer further into the vector/towards the back
	// because we render the layers starting from first item to last
	// so the last item is drawn over all the others for instance
	auto it1 = m_layers.begin();
	auto it2 = m_layers.begin();
	std::advance(it1, id);
	std::advance(it2, id + 1);
	std::iter_swap(it1, it2);
}

CompositeLayer_t & CompositeTexture::GetLayer(int id)
{
	return m_layers[id];
}

void CompositeTexture::CreateComposite()
{
	if(m_composite) m_composite.reset();

	sf::RenderTexture outputTexture;
	outputTexture.create(m_dims.x, m_dims.y);
	outputTexture.setActive();
	outputTexture.clear();

	for(int i = 0; i < m_layers.size(); i++)
	{
		if(m_layers[i].isVisible) outputTexture.draw(m_layers[i].spr);
	}

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

CompositeTexture::CompositeTexture() : m_composite(new sf::Texture()), m_dims(sf::Vector2i())
{

}

std::vector<CompositeLayer_t> & CompositeTexture::GetAllLayers()
{
	return m_layers;
}
