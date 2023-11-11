#ifndef LABEL_HPP
#define LABEL_HPP

#include "SFML/Graphics.hpp"

class Label
{
public:
	Label(){}
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
	void draw(sf::RenderWindow& win)
	{
		win.draw(text);
	}
	void setPos(sf::Vector2f pos)
	{
		text.setPosition(pos);
	}
	sf::Vector2f getSize()
	{
		return sf::Vector2f(text.getGlobalBounds().width, text.getGlobalBounds().height);
	}
	sf::Vector2f getPos()
	{
		return text.getPosition();
	}
	void setText(sf::String str)
	{
		text.setString(str);
	}
	void setFont(sf::String ffont)
	{
		font.loadFromFile(ffont);
	}
private:
	sf::Text text;
	sf::Font font;
};

#endif

