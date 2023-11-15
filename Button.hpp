#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "SFML/Graphics.hpp"
#include <functional>

class Button
{
	sf::RectangleShape btn;
	sf::Text txt;
	sf::Font font;
	sf::Texture texture;
	sf::Sprite img;
	std::function<void(void)> fun;
	sf::Vector2f padding;

	public:
		Button()
		{
			padding = {0, 0};
			btn.setPosition({0, 0});
			btn.setSize({100, 30});
			btn.setFillColor(sf::Color::Black);
			
			txt.setString("Button");
			txt.setFillColor(sf::Color::White);
		}
		//text contructor
		Button(sf::Vector2f pos, sf::String text, sf::String fontpath, int csize, sf::Color bg, sf::Color txtcolor, const sf::Vector2f padding = sf::Vector2f(0, 0))
		{
			this->padding = padding;
	
			btn.setPosition(pos);
			btn.setFillColor(bg);

			font.loadFromFile(fontpath);
			txt.setFont(font);
			txt.setString(text);
			txt.setFillColor(txtcolor);
			txt.setCharacterSize(csize);
			txt.setPosition(pos.x + padding.x, pos.y + padding.y);

			btn.setSize(sf::Vector2f(txt.getLocalBounds().width + txt.getLocalBounds().left * 2 + padding.x * 2, 
									 txt.getLocalBounds().height + txt.getLocalBounds().top * 2 + padding.y * 2));
		}
		//image constructor		
		Button(sf::Vector2f pos, sf::Vector2f size, sf::String path, int csize, sf::Color bg, const sf::Vector2f padding = sf::Vector2f(0, 0))
		{
			this->padding = padding;
	
			btn.setPosition(pos);
			btn.setFillColor(bg);
			btn.setSize(size);
			
			texture.loadFromFile(path);
			img.setTexture(texture);
			adjustImg();
			img.setPosition(btn.getPosition().x + padding.x, btn.getPosition().y + padding.y);

			btn.setSize(sf::Vector2f(img.getGlobalBounds().width + padding.x * 2, img.getGlobalBounds().height + padding.y * 2));
		}
		void draw(sf::RenderWindow* win)
		{
			win->draw(btn);
			win->draw(img);
			win->draw(txt);
		}
		void listen(sf::Event& e)
		{
			if (e.type == sf::Event::MouseButtonPressed && sf::Mouse::Left == e.mouseButton.button)
			{
				sf::Vector2i pos;
				pos.x = e.mouseButton.x;
				pos.y = e.mouseButton.y;
				if (pos.x > btn.getPosition().x && pos.x < btn.getPosition().x + btn.getSize().x)
				{
					if (pos.y > btn.getPosition().y && pos.y < btn.getPosition().y + btn.getSize().y)
					{
						fun();
					}
				}
			}
		}
		
		void setPos(int x, int y)
		{
			btn.setPosition(x, y);
			txt.setPosition(x + padding.x, y + padding.y);
			img.setPosition(btn.getPosition().x + padding.x, btn.getPosition().y + padding.y);
		}
		inline sf::Vector2f getPos()
		{
			return btn.getPosition();
		}
		inline sf::Vector2f getSize()
		{
			return btn.getSize();
		}
		inline void setFunction(std::function<void(void)> f)
		{
			fun = f;
		}
		
		void setText(sf::String str)
		{
			txt.setString(str);
			adjustButton();
			adjustText();
			adjustImg();
		}
		void setText(sf::Text t)
		{
			txt = t;
			adjustButton();
			adjustText();
			adjustImg();
		}
		void setText(sf::String text, sf::String path, sf::Color color, int csize)
		{
			font.loadFromFile(path);
			txt.setString(text);
			txt.setFont(font);
			txt.setFillColor(color);
			txt.setCharacterSize(csize);
			adjustButton();
			adjustText();
			adjustImg();
		}
		void setChSize(int csize)
		{
			txt.setCharacterSize(csize);
			adjustText();
		}
		inline void setTextColor(sf::Color color)
		{
			txt.setFillColor(color);
		}
		void setFont(sf::String path)
		{
			font.loadFromFile(path);
			txt.setFont(font);
			adjustButton();
			adjustText();
			adjustImg();
		}
		
		void adjustText()
		{
			txt.setPosition(btn.getPosition().x + btn.getSize().x / 2 - txt.getGlobalBounds().width / 2 + padding.x,
				btn.getPosition().y - txt.getLocalBounds().top + btn.getSize().y / 2 - txt.getGlobalBounds().height / 2 + padding.y);
		}
		
		void adjustImg()
		{
			img.setScale((btn.getSize().x - padding.x * 2) / img.getLocalBounds().width,
						 (btn.getSize().y - padding.y * 2) / img.getLocalBounds().height);
		}
		
		void adjustButton()
		{
			btn.setSize(sf::Vector2f(txt.getLocalBounds().width + txt.getLocalBounds().left * 2 + padding.x * 2, 
									 txt.getLocalBounds().height + txt.getLocalBounds().top * 2 + padding.y * 2));
		}
		
		void setSize(sf::Vector2f new_size)
		{
			btn.setSize(new_size);
			adjustText();
			adjustImg();
		}
		
		void setImage(std::string path)
		{
			texture.loadFromFile(path);
			img.setTexture(texture);
			img.setPosition(btn.getPosition().x + padding.x, btn.getPosition().y + padding.y);
			adjustImg();
		}
		
		void setBorder(int thickness, sf::Color color)
		{
			btn.setOutlineThickness(thickness);
			btn.setOutlineColor(color);
		}
		inline void setColor(sf::Color color)
		{
			btn.setFillColor(color);
		}
};
#endif
