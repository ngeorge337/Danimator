#pragma once

struct CompositeLayer_t
{
	sf::Sprite spr;
	std::string spriteName;
	bool isVisible;
};

class CompositeTexture
{
public:

	CompositeTexture();

	sf::Vector2i GetDimensions();
	void SetDimensions(int w, int h);
	std::shared_ptr<sf::Texture> GetFinalTexture();

	void CreateComposite();

	CompositeLayer_t &GetLayer(int id);
	std::vector<CompositeLayer_t> &GetAllLayers();
	void MoveLayerUp(int id);
	void MoveLayerDown(int id);
	void DeleteLayer(int pos);

	std::shared_ptr<sf::Texture> operator=(std::shared_ptr<sf::Texture> t);
private:
	sf::Vector2i m_dims;
	sf::Vector2f m_scaleFactor;
	std::string m_namespace;
	std::shared_ptr<sf::Texture> m_composite;
	std::vector<CompositeLayer_t> m_layers;
};

class TextureManager : public ResourceManager
{
	friend class DanFrame;
public:
	TextureManager(std::string directory) : ResourceManager(directory)
	{
		CreateEmptyTexture("TNT1A0");
	}
	virtual bool Precache(std::string fileName);
	virtual void Reload(std::string fileName);
	virtual void Unload(std::string fileName);
	virtual void ReloadAll();
	virtual void UnloadAll();
	void SetFiltering(bool enable);
	void CreateEmptyTexture(const std::string &texName);
	void Acquire(sf::Texture &tex, std::string fileName);
	void Remap(const std::string &name, const std::string &newname);

	// Composite Texture stuff
	std::string GetTexturesCode();
	std::shared_ptr<CompositeTexture> GetCompositeTexture(const std::string &name);

	std::shared_ptr<sf::Texture> GetTexture(std::string name);
private:
	std::map<std::string, Resource_t<sf::Texture>> texmap;

	std::map<std::string, std::shared_ptr<CompositeTexture>> compmap;
};