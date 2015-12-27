#pragma once

class TextureActionManager : public ActionManager
{
protected:
	virtual void Finish() { }
};

class CompositeTexture : public sf::Drawable, public Serializable
{
	friend class ActionManager;
	friend class TextureActionManager;
	friend class DanFrame;
public:
	virtual void Serialize(File &file);
	virtual void Deserialize(File &file);

	CompositeTexture();

	wxString GetTextureCode();

	sf::Vector2i GetDimensions();
	void SetDimensions(int w, int h);

	sf::Vector2f GetScaleFactor();
	void SetScaleFactor(float w, float h);
	sf::Vector2f GetRealScale();

	void SetNamespace(const std::string &ns);
	std::string GetNamespace();

	std::shared_ptr<sf::Texture> GetFinalTexture();

	void CreateComposite(bool recompile = false);

	CompositeLayer_t &GetLayer(int id);
	std::vector<CompositeLayer_t> &GetAllLayers();
	int MoveLayerTowardsEnd(int id);
	int MoveLayerTowardsBegin(int id);
	void AddLayer(const std::string &name = "");
	void DeleteLayer(int pos);

	std::shared_ptr<sf::Texture> operator=(std::shared_ptr<sf::Texture> t);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	TextureActionManager &GetActionHistory();

	const std::set<std::string> &GetDependencies();
	bool HasDependency(const std::string &depName);
	bool HasAnyDependencies();
	void AddDependency(const std::string &depName);
	void ClearDependencies();

	int m_hash;	// dumb hash value for sprite list
	std::string m_texname;
private:
	// Store a full copy of its own actions
	TextureActionManager m_Actions;

	sf::Vector2i m_dims;
	sf::Vector2f m_scaleFactor;
	sf::Vector2f m_realScale;	// actual scale value for TEXTURES entry (inverted scale)
	std::string m_namespace;
	std::shared_ptr<sf::Texture> m_composite;
	std::vector<CompositeLayer_t> m_layers;

	std::set<std::string> m_texDepends;	// dependencies of other composite textures
};