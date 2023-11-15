#ifndef LABEL_HPP
#define LABEL_HPP

#include "SFML/Graphics.hpp"

class Label
{
public:
	inline Label(){}
	Label(sf::String str, int size, sf::Color color, sf::String ffont)
	{
		font.loadFromFile(ffont);

		text.setString(str);
		text.setCharacterSize(size);
		text.setFont(font);
		text.setFillColor(color);
	}
	Label(sf::Vector2f pos, sf::String str, int size, sf::Color color, sf::String ffont)
	{
		font.loadFromFile(ffont);

		text.setString(str);
		text.setCharacterSize(size);
		text.setPosition(pos);
		text.setFont(font);
		text.setFillColor(color);
	}
	inline void draw(sf::RenderWindow* win)
	{
		win->draw(text);
	}
	inline void setPos(sf::Vector2f pos)
	{
		text.setPosition(pos);
	}
	inline sf::Vector2f getPos()
	{
		return text.getPosition();
	}
	inline void setText(sf::String str)
	{
		text.setString(str);
	}
	inline void setFont(sf::String ffont)
	{
		font.loadFromFile(ffont);
	}
	inline void setChSize(int n)
	{
		text.setCharacterSize(n);
	}
	inline void setColor(sf::Color color)
	{
		text.setFillColor(color);
	}
	inline sf::Vector2f getSize()
	{
		return sf::Vector2f(text.getGlobalBounds().width, text.getGlobalBounds().height);
	}
private:
	sf::Text text;
	sf::Font font;
};

#endif

