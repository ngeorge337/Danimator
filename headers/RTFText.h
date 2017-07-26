#pragma once

enum eAlignment
{
	ALIGN_LEFT,
	ALIGN_CENTER,
	ALIGN_RIGHT
};

class RTFText : public sf::Drawable, public sf::Transformable
{
public:
	RTFText();
	RTFText(const sf::String& source, sf::Font& font, unsigned int characterSize = 30);

	enum Style
	{
		Regular = sf::Text::Regular,
		Bold = sf::Text::Bold,
		Italic = sf::Text::Italic,
		Underlined = sf::Text::Underlined,
	};

	sf::String getString() const;
	sf::String getSource() const;
	void setString(sf::String source);
	void clear();

	unsigned int getCharacterSize() const;
	void setCharacterSize(unsigned int size);

	sf::Font* getFont() const;
	void setFont(sf::Font& font);
	//void changeFont(std::string fontname);

	void setAlignment(eAlignment aligning);
	int getAlignment();

	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;

	//	Set names for color substitutions (for example, ff0000 would be substituted for "red")
	void addColor(const sf::String& name, const sf::Color& color);
	void addColor(const sf::String& name, unsigned int argbHex);

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	void initializeColors();
	sf::Color getColor(const sf::String& source) const;
	sf::Color getColor(unsigned int argbHex) const;

	std::map<sf::String, sf::Color> colors;
	std::vector<sf::Text> texts;

	eAlignment alignment;

	unsigned int characterSize;
	sf::Font *font;
	std::string currentfont;
	float basex, basey;

	sf::String source;
	sf::String string;
	sf::FloatRect bounds;
};