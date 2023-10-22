#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include "SFML/Graphics.hpp"
#include <vector>
#include <iostream>

class TextBox
{
	public:
		void setUp(sf::RenderWindow& window, float top, float left, float width, float height)
		{
			win = &window;
			x = left; y = top; w = width; h = height;
			view.reset(sf::FloatRect(0, 0, w, h));
			view.setViewport(sf::FloatRect(left / win->getSize().x, top / win->getSize().y, 
				(width) / win->getSize().x, (height) / win->getSize().y));
			
			multiple_lines = false;
			px = 0;
			py = 0;
			text.push_back("");
			
			toDraw.setFillColor(sf::Color::Black);
			line_height = toDraw.getCharacterSize() + 10;
			max_width = 0;
			
			bar.setFillColor(sf::Color::Red);
			bar.setSize({3, line_height});
			bar.setPosition({0, 0});
		}
		void listen(sf::Event& e)
		{
			if(e.type == sf::Event::KeyPressed)
			{
				if(e.key.code == sf::Keyboard::Delete)
				{
					adjustView();
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
					adjustView();
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
					adjustView();
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
					adjustView();
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
					adjustView();
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
				adjustView();
			}
		}
		void draw()
		{
			sf::View temp = win->getView();
			win->setView(view);
			
			if(multiple_lines) multiLinesRender();
			else singleLinesRender();
			win->draw(bar);
			
			win->setView(temp);
		}
		void setFont(sf::String path)
		{
			font.loadFromFile(path);
			toDraw.setFont(font);
			
			adjustView();
		}
		void setChSize(unsigned short int size)
		{
			toDraw.setCharacterSize(size);
			line_height = size + 10;
			bar.setSize({3, line_height});
			
			adjustView();
		}
		void write(sf::String toWrite)
		{
			return;
		}
		void setMultiLines(bool b)
		{
			multiple_lines = b;
		}
	private:
		/*----------Variables----------*/
		//text manipulation
		bool multiple_lines;
		std::vector<sf::String> text;
		int px, py;
		int select_begin, select_end;
		
		//text render
		sf::Font font;
		sf::Text toDraw;
		float line_height, max_width;
		
		//scrollbars
		//to implement
		
		//bar to show current position in text
		sf::RectangleShape bar;
		
		//view and window
		sf::RenderWindow* win;
		sf::View view;
		float x, y, w, h;
		/*----------Functions----------*/
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
			for(; i<n && i<(int)text.size(); ++i)
			{
				toDraw.setString(text[i]);
				toDraw.setPosition(0, i*line_height);
				win->draw(toDraw);
			}
		}
		void adjustView()
		{
			//updating max width
			if(multiple_lines)
			{
				max_width = 0;
				for(sf::String str: text)
				{
					toDraw.setString(str);
					if(max_width < toDraw.getLocalBounds().width + toDraw.getLocalBounds().left*2)
						max_width = toDraw.getLocalBounds().width + toDraw.getLocalBounds().left*2;
				}
			}
			else
			{
				toDraw.setString(text[0]);
				max_width = toDraw.getLocalBounds().width + toDraw.getLocalBounds().left*2;
			}
			
			//check width
			toDraw.setString(text[py]);
			float width = toDraw.findCharacterPos(px).x;
			if(max_width > w && max_width < (view.getCenter().x + w*0.5f))	//wrap
			{
				view.setCenter({max_width - w*0.5f + bar.getSize().x, view.getCenter().y});
			}
			if(width > (view.getCenter().x + w*0.5f))						 //scroll right
			{
				view.move({width - view.getCenter().x - w*0.5 + bar.getSize().x, 0});
			}
			if(width < (view.getCenter().x - w*0.5f))						 //scroll left
			{
				view.move({width - (view.getCenter().x - w*0.5f), 0});
			}
			
			//check height
			float height = (py+1) * line_height;
			if(height > h && height < (view.getCenter().y + h*0.5f))	//wrap
			{
				view.setCenter({view.getCenter().x, height - h*0.5f});
			}
			else if(height < h)									 	//check if height less than view height, then adjust 
			{
				view.setCenter({view.getCenter().x, h*0.5f});
			}
			if(height > (view.getCenter().y + h*0.5f))				  //scroll down
			{
				view.move({0, height - (view.getCenter().y + h*0.5f)});
			}
			if((py * line_height) < (view.getCenter().y - h*0.5f))	  //scroll up
			{
				view.move({0, (py * line_height) - (view.getCenter().y - h*0.5f)});
			}
			
			//update bar position
			bar.setPosition({width, py*line_height});
		}
};
#endif
