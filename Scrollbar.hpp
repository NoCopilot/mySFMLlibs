#ifndef SCROLLBAR_HPP
#define SCROLLBAR_HPP

#include "SFML/Graphics.hpp"

class Scrollbar
{
	public:
		void setUp(sf::View& v, float x, float y, float w, float h)
		{
			view = &v;
			
			if(w > h) scroll_toward = false;
			else scroll_toward = true;
			
			base.setSize({w, h});
			base.setPosition({x, y});
			base.setFillColor(sf::Color(100, 100, 100));
			
			movable.setSize({w, h});
			movable.setPosition({x, y});
			movable.setFillColor(sf::Color(120, 120, 120));
		}
		void update(float mx, float my)
		{
			max_width = mx; max_height = my;
			movable.setSize()
		}
		void draw(sf::RenderWindow& win)
		{
			
		}
	private:
		sf::View* view;
		sf::RectangleShape base, movable;
		float max_width, max_height;
		bool scroll_toward;
};

#endif
