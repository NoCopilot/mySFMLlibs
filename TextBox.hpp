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
		void setUp(sf::RenderWindow& window, float top, float left, float width, float height)
		{
			win = &window;
			x = left; y = top; w = width; h = height;
			view.reset(sf::FloatRect(0, 0, width, height));
			view.setViewport(sf::FloatRect(left / win->getSize().x, top / win->getSize().y, 
				(width) / win->getSize().x, (height) / win->getSize().y));
			
			multiple_lines = false;
			px = 0;
			py = 0;
			text.push_back("");
			
			toDraw.setFillColor(sf::Color::Black);
			line_height = toDraw.getCharacterSize() + 10;
		}
		void listen(sf::Event& e)
		{
			if(e.type == sf::Event::KeyPressed)
			{
				if(e.key.code == sf::Keyboard::Delete)
				{
					return;
				}
				if(e.key.code == sf::Keyboard::Up)
				{
					if(py > 0)
					{
						py--;
						if(px > (int)text[py].getSize())
							px = text[py].getSize();
					}
					return;
				}
				if(e.key.code == sf::Keyboard::Down)
				{
					if(py < text.size() - 1)
					{
						py++;
						if(px > (int)text[py].getSize())
							px = text[py].getSize();
					}
					return;
				}
				if(e.key.code == sf::Keyboard::Left)
				{
					if(px > 0) px--;
					else if(py > 0)
					{
						py--;
						px = text[py].getSize();
					}
					
					return;
				}
				if(e.key.code == sf::Keyboard::Right)
				{
					if(px < (int)text[py].getSize()) px++;
					else if(py < (int)(text.size() - 1))
					{
						px = 0;
						py++;
					}
					
					return;
				}
			}
			if(e.type == sf::Event::TextEntered)
			{
				switch(e.text.unicode)
				{
					case 3:
					{
						//copy
						break;
					}
					case 22:
					{
						//copy + delete selected text
						break;
					}
					case 24:
					{
						//paste
						break;
					}
					case 13:
					{
						//new line
						if(!multiple_lines) break;
						text.insert(text.begin() + py + 1, text[py].substring(px));
						text[py] = text[py].substring(0, px);
						py++;
						px = 0;
						break;
					}
					case 8:
					case 127:
					{
						//delete
						if(px > 0) text[py].erase(--px);
						else if(py > 0)
						{
							text.erase(text.begin() + py--);
							px = text[py].getSize();
						}
						break;
					}
					default:
						//add character
						text[py].insert(px++, e.text.unicode);
				}
			}
		}
		void draw()
		{
			sf::View temp = win->getView();
			win->setView(view);
			
			if(multiple_lines) multiLinesRender();
			else singleLinesRender();
			
			win->setView(temp);
		}
		void singleLinesRender()
		{
			toDraw.setString(text[0]);
			toDraw.setPosition(0, 0);
			win->draw(toDraw);
		}
		void multiLinesRender()
		{
			int n = (view.getCenter().y - view.getSize().y/2)/line_height;
			int i = n;
			n = (view.getCenter().y + view.getSize().y/2)/line_height;
			for(; i<n && i<text.size(); ++i)
			{
				toDraw.setString(text[i]);
				toDraw.setPosition(0, i*line_height);
				win->draw(toDraw);
			}
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
			return;
		}
		void setMultiLines(bool b)
		{
			multiple_lines = b;
		}
};

#endif
