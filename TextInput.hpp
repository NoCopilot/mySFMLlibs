#ifndef TEXTINPUT_HPP
#define TEXTINPUT_HPP

#include "SFML/Graphics.hpp"

inline void alignTextY(sf::Text& text, float y, float height)
{
	text.setPosition({text.getPosition().x, y + height * 0.5f - text.getLocalBounds().height * 0.5f - text.getLocalBounds().top});
}

class TextInput
{
	sf::RectangleShape bar, bg;
	sf::Font font;
	sf::Text textView, placeholder;
	sf::String text;
	bool isSelected = false, alwaysSelected = false;
	int current, begin;
	
	void updateView()
	{
		//check
		if(text.getSize() == 0)
		{
			bar.setPosition({bg.getPosition().x, bar.getPosition().y});
			textView.setString("");
			return;
		}
		
		//set text view
		textView.setString(text.substring(begin, current - begin));
		if((textView.getLocalBounds().width + textView.getLocalBounds().left * 2) > bg.getSize().x)
		{
			//string too long for view -> increment the start character
			do
			{
				begin++;
				textView.setString(text.substring(begin, current - begin));
			}while((textView.getLocalBounds().width + textView.getLocalBounds().left * 2) > bg.getSize().x);
		}
		else
		{
			//string is short for view, so check if more character can be displayed
			int i = 1;
			while((current - begin + i) < (int)text.getSize())
			{
				textView.setString(text.substring(begin, current - begin + i));
				if((textView.getLocalBounds().width + textView.getLocalBounds().left * 2) > bg.getSize().x)
					break;
				i++;
			}
			textView.setString(text.substring(begin, current - begin + i));
		}
		alignTextY(textView, bg.getPosition().y, bg.getSize().y);
		
		//set bar position
		int z = current - begin;
		if(z == (int)textView.getString().getSize())
		{
			bar.setPosition({textView.getLocalBounds().width + textView.getLocalBounds().left*2, bar.getPosition().y});
		}
		else
		{
			bar.setPosition({textView.findCharacterPos(z).x, bar.getPosition().y});
		}
	}
	
	public:
		TextInput()
		{
			text = "";
			
			textView.setFillColor(sf::Color::Black);
			
			placeholder.setFillColor(sf::Color(180, 180, 180));
			
			bg.setFillColor(sf::Color::White);
			bg.setOutlineColor(sf::Color::Black);
			bg.setOutlineThickness(1);
			
			bar.setFillColor(sf::Color::Red);
			
			setPos({0, 0});
			setSize({100, 50});
			
			current = 0;
			begin = 0;
		}
		void draw(sf::RenderWindow& win)
		{
			win.draw(bg);
			
			if(textView.getString() == "") win.draw(placeholder);
			else win.draw(textView);
			
			if(isSelected || alwaysSelected)win.draw(bar);
		}
		void listen(sf::Event& e)
		{
			if(e.type == sf::Event::MouseButtonPressed)
			{
				if(e.mouseButton.button == sf::Mouse::Left && !alwaysSelected)
				{
					isSelected = false;
					if(e.mouseButton.x >= bg.getPosition().x && e.mouseButton.x <= bg.getPosition().x + bg.getSize().x)
						if(e.mouseButton.y >= bg.getPosition().y && e.mouseButton.y <= bg.getPosition().y + bg.getSize().y)
							isSelected = true;
				}
			}
			if(e.type == sf::Event::KeyPressed)
			{
				//left
				if(e.key.code == sf::Keyboard::Left)
				{
					if(current > 0) current--;
					if(current < begin) begin = current;
					updateView();
				}
				//right
				if(e.key.code == sf::Keyboard::Right)
				{
					if(current < (int)text.getSize())
						current++;
					updateView();
				}
				//canc
				if(e.key.code == sf::Keyboard::Delete)
				{
					if(current < (int)text.getSize())
					{
						text.erase(current + 1);
						updateView();
					}
				}
			}
			if(e.type == sf::Event::TextEntered)
			{
				//write
				if(e.key.code >= 32 && e.key.code < 127)
				{
					text.insert(current, (char)e.key.code);
					current++;
				}
				//del
				if(e.key.code == 8)
				{
					if(current >= 1){
						text.erase(--current);
						if(current < begin) begin = current;
					}
				}
				updateView();
			}
		}
		
		void clear()
		{
			text = "";
			current = 0;
			begin = 0;
			updateView();
		}
		inline sf::String getText()
		{
			return text;
		}
		void setText(sf::String str)
		{
			text = str;
			current = text.getSize();
			begin = 0;
			
			updateView();
		}
		
		void setPos(sf::Vector2f new_pos)
		{
			int diff = bar.getPosition().x - bg.getPosition().x;
			
			bg.setPosition(new_pos);
			bar.setPosition({new_pos.x + diff, new_pos.y + bg.getSize().y * 0.1f});
			
			textView.setPosition(new_pos);
			placeholder.setPosition(new_pos);
			
			alignTextY(textView, bg.getPosition().y, bg.getSize().y);
			alignTextY(placeholder, bg.getPosition().y, bg.getSize().y);
		}
		void setSize(sf::Vector2f new_size)
		{
			if(placeholder.getString() != "")
				if(new_size.x < placeholder.getLocalBounds().width + placeholder.getLocalBounds().left * 2)
					new_size.x = placeholder.getLocalBounds().width + placeholder.getLocalBounds().left * 2;
			
			bg.setSize(new_size);
			bar.setSize({4, new_size.y * 0.8f});
			
			alignTextY(textView, bg.getPosition().y, bg.getSize().y);
			alignTextY(placeholder, bg.getPosition().y, bg.getSize().y);
			
			begin = 0;
			updateView();
		}
		
		void setChSize(int chsize)
		{
			textView.setCharacterSize(chsize);
			placeholder.setCharacterSize(chsize);
			
			setSize(bg.getSize());
		}
		
		inline void setAlwaysFocus(bool boolean)
		{
			alwaysSelected = boolean;
		}
		
		void setFont(sf::String font_path)
		{
			if(font.loadFromFile(font_path))
			{
				textView.setFont(font);
				placeholder.setFont(font);
			
				setSize(bg.getSize());
			
				alignTextY(textView, bg.getPosition().y, bg.getSize().y);
				alignTextY(placeholder, bg.getPosition().y, bg.getSize().y);
			}
		}
		
		inline void setTextColor(sf::Color color)
		{
			textView.setFillColor(color);
		}
		inline void setBgColor(sf::Color color)
		{
			bg.setFillColor(color);
		}
};

#endif
