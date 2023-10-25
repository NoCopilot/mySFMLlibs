#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include "SFML/Graphics.hpp"
#include <vector>
#include <iostream>

/*---------------todo---------------*\
*		 	scrollbars	   	  *
* 		  text selection 		  *
*   change color to specified text   *
\*----------------------------------*/

class TextBox
{
	public:
		void setUp(sf::RenderWindow& window, float top, float left, float width, float height)
		{
			win = &window;
			x = left; y = top; w = width; h = height;
			view.reset(sf::FloatRect(0, 0, w, h));
			view.setViewport(sf::FloatRect(left / win->getSize().x, top / win->getSize().y, 
										   width / win->getSize().x, height / win->getSize().y));
			focus = false;
			background.setSize({width, height});
			background.setPosition({0.f, 0.f});
			background.setFillColor(sf::Color::Blue);
			
			multiple_lines = false;
			px = 0;
			py = 0;
			text.push_back("");
			select_begin = {-1, -1};
			select_end = {-1, -1};
			selecting = false;
			
			toDraw.setFillColor(sf::Color::Black);
			line_height = toDraw.getCharacterSize() + 10;
			max_width = 0;
			
			bar.setFillColor(sf::Color::Red);
			bar.setSize({3, line_height});
			bar.setPosition({0, 0});
		}
		void listen(sf::Event& e)
		{
			if(e.type == sf::Event::Resized)
			{
				view.setViewport(sf::FloatRect(x / win->getSize().x, y / win->getSize().y, 
					w / win->getSize().x, h / win->getSize().y));
				adjustView();
			}
			if(e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
			{
				focus = false;
				if(e.mouseButton.x > x && e.mouseButton.x < (x+w))
					focus = true;
				if(!focus) return;
				
				sf::Vector2i pos = getTextPos(getMousePos());
				px = pos.x;
				py = pos.y;
				
				//update bar pos
				bar.setPosition({toDraw.findCharacterPos(px).x, py*line_height});
				
				selecting = true;
				select_begin.x = px;
				select_begin.y = py;
				select_end.x = -1;
				select_end.y = -1;
			}
			if(e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left)
			{
				selecting = false;
			}
			if(selecting && e.type == sf::Event::MouseMoved)
			{
				select_end = getTextPos(getMousePos());
				std::cout << select_end.x << " " << select_end.y << "\n";
			}
			if(e.type == sf::Event::KeyPressed)
			{
				if(e.key.code == sf::Keyboard::Delete)
				{
					if(px < (int)text[py].getSize())
					{
						text[py].erase(px);
					}
					else if(py < (int)(text.size() - 1))
					{
						text[py] += text[py+1];
						text.erase(text.begin() + py + 1);
					}
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
					if(py < (int)(text.size() - 1))
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
			
			//draw background
			win->draw(background);
			
			//draw selected text
			if(begin.x != -1) drawSelected();
			
			//draw text
			if(multiple_lines) multiLinesRender();
			else singleLinesRender();
			win->draw(bar);
			
			//draw scrollbars
			
			
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
		void setSize(sf::Vector2f size)
		{
			w = size.x;
			h = size.y;
			background.setSize({w, h});
			
			view.reset({0, 0, w, h});
			view.setViewport(sf::FloatRect(x / win->getSize().x, y / win->getSize().y, 
			   w / win->getSize().x, h / win->getSize().y));

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
		sf::Vector2i select_begin, select_end;
		bool selecting;
		
		//text render
		sf::Font font;
		sf::Text toDraw;
		float line_height, max_width;
		sf::RectangleShape select_highlight;
		
		//scrollbars
		//to implement
		
		//bar to show current position in text
		sf::RectangleShape bar;
		
		//view and window
		sf::RenderWindow* win;
		sf::View view;
		float x, y, w, h;
		bool focus;
		sf::RectangleShape background;
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
		void drawSelected()
		{
			
		}
		sf::Vector2i getMousePos()
		{
			sf::Vector2i vv = sf::Mouse::getPosition(*win);

			sf::Vector2f v2f(view.getCenter().x - view.getSize().x / 2, view.getCenter().y - view.getSize().y / 2);

			vv.x = (vv.x - x) * (view.getSize().x / w) + v2f.x;
			vv.y = (vv.y - y) * (view.getSize().y / h) + v2f.y;

			return vv;
		}
		sf::Vector2i getTextPos(sf::Vector2i mouse)
		{
			//pos to return
			sf::Vector2i pos;
			
			//getting line y pos
			if((mouse.y / line_height) >= text.size())
				pos.y = text.size() - 1;
			else
				pos.y = mouse.y / line_height;
			
			//getting line x pos
			toDraw.setString(text[pos.y]);
			for(int i = text[pos.y].getSize(); i >= 0; i--)
			{
				if(mouse.x >= toDraw.findCharacterPos(i).x)
				{
					pos.x = i;
					break;
				}
			}
			
			return pos;
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
				view.move({width - view.getCenter().x - w*0.5f + bar.getSize().x, 0.f});
			}
			if(width < (view.getCenter().x - w*0.5f))						 //scroll left
			{
				view.move({width - (view.getCenter().x - w*0.5f), 0});
			}
			
			//check height
			float height = (py+1) * line_height;
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
			
			//update background pos
			background.setPosition({view.getCenter().x - w*0.5f, view.getCenter().y - h*0.5f});
		}
};
#endif
