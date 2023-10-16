#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include "SFML/Graphics.hpp"
#include <vector>
#include <iostream>

class TextBox
{
	protected:
		bool multiple_lines;
		std::vector<sf::String> text;
		int px, py;
		sf::Text toDraw;
		int line_height;
	public:
		TextBox()
		{
			multiple_lines = false;
			text.push_back("");
			px = 0;
			py = 0;
			line_height = 30;
		}
		void listen(sf::Event& e)
		{
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
							px = text[py].getSize() - 1;
						}
						break;
					}
					//add character
					default:
						text[py].insert(px++, e.text.unicode);
				}
				std::cout<<text[py].toAnsiString()<<"\n";
			}
		}
		void draw(sf::RenderWindow& win)
		{
			for(int i=0; i<text.size(); i++)
			{
				toDraw.setString(text[i]);
				toDraw.setPosition({0, i * line_height});
				win.draw(toDraw);
			}
		}
};

#endif
