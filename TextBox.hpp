#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include "SFML/Graphics.hpp"
#include <vector>
#include <iostream>

class TextBox
{
	private:
		bool multiple_lines;
		std::vector<sf::String> text;
		int px, py;
		int select_begin, select_end;
		sf::Font font;
		sf::Text toDraw;
		int line_height;
		
		sf::RenderWindow* win;
		sf::View view;
		float x, y, w, h;
	public:
		void setUp(sf::RenderWindow& window, sf::Vector2f size, float top, float left, float bottom, float right)
		{
			win = &window;
			view = win->getView();
			view.setViewport(sf::FloatRect(left / view.getSize().x, top / view.getSize().y, 
				(view.getSize().x - right) / view.getSize().x - left / view.getSize().x, 
				(view.getSize().y - bottom) / view.getSize().y - top / view.getSize().y));
			view.setSize(size);
			
			multiple_lines = false;
			px = 0;
			py = 0;
			text.push_back("");
			
			toDraw.setFillColor(sf::Color::White);
			line_height = toDraw.getCharacterSize() + 10;
			x = left; y = top; w = right; h = bottom;
		}
		void listen(sf::Event& e)
		{
			if(e.type == sf::Event::KeyPressed)
			{
				switch(e.key.code)
				{
					case sf::Keyboard::Delete:
					{
						break;
					}
					case sf::Keyboard::Up:
					{
						if(py > 0)
						{
							py--;
							if(px > text[py].getSize())
								px = text[py].getSize();
						}
						break;
					}
					case sf::Keyboard::Down:
					{
						if(py < text.size() - 1)
						{
							py++;
							if(px > text[py].getSize())
								px = text[py].getSize();
						}
						break;
					}
					case sf::Keyboard::Left:
					{
						if(px > 0) px--;
						else if(py > 0)
						{
							py--;
							px = text[py].getSize();
						}
						break;
					}
					case sf::Keyboard::Right:
					{
						if(px < text[py].getSize()) px++;
						else if(py < (text.size() - 1))
						{
							px = 0;
							py++;
						}
						break;
					}
					
				}
			}
			if(e.type == sf::Event::TextEntered)
			{
				switch(e.text.unicode)
				{
					//copy
					case 3:
					{
						break;
					}
					//copy + delete selected text
					case 22:
					{
						break;
					}
					//paste
					case 24:
					{
						break;
					}
					//new line
					case 13:
					{
						text.insert(text.begin() + py + 1, text[py].substring(px));
						text[py] = text[py].substring(0, px);
						py++;
						px = 0;
						break;
					}
					//delete
					case 8:
					case 127:
					{
						if(px > 0) text[py].erase(--px);
						else if(py > 0)
						{
							text.erase(text.begin() + py--);
							px = text[py].getSize();
						}
						break;
					}
					//add character
					default:
						text[py].insert(px++, e.text.unicode);
				}
			}
		}
		void draw()
		{
			sf::View temp = win->getView();
			win->setView(view);
			
			for(int i=0; i < (int)text.size(); i++)
			{
				toDraw.setString(text[i]);
				toDraw.setPosition({0, i * line_height});
				win->draw(toDraw);
			}
			
			win->setView(temp);
		}
		void setFont(sf::String path)
		{
			font.loadFromFile(path);
			toDraw.setFont(font);
		}
		void setChSize(unsigned short int size)
		{
			toDraw.setCharacterSize(size);
			line_height = size + 10;
		}
		void write(sf::String toWrite)
		{
			
		}
};

#endif
