#ifndef SCROLLBAR_HPP

#define SCROLLBAR_HPP

#include "SFML/Graphics.hpp"
#include <iostream>
class Scrollbar
{
	public:
		void setUp(sf::View& v, float x, float y, float w, float h)
		{
			view = &v;
			
			if(w < h){
				horizontal = false;
				max = view->getSize().x;
			}
			else{
				horizontal = true;
				max = view->getSize().y;
			}
			
			base.setSize({w, h});
			base.setPosition({x, y});

			movable = base;
		}
		void update(float f)
		{
			max = f;
			
			if(horizontal)
			{
				movable.setSize({base.getSize().x * (view->getSize().x / max), base.getSize().y});
				if(max < view->getSize().x) max = view->getSize().x;
				movable.setPosition({(view->getCenter().x - view->getSize().x*0.5f) / view->getSize().x * movable.getSize().x + base.getPosition().x, base.getPosition().y});
			}
			else
			{
				movable.setSize({base.getSize().x, base.getSize().y * (view->getSize().y / max)});
				if(max < view->getSize().y) max = view->getSize().y;
				movable.setPosition({base.getPosition().x, (view->getCenter().y - view->getSize().y*0.5f) / view->getSize().y * movable.getSize().y + base.getPosition().y});
			}
			
			checkSize();
			checkPos();
		}
		void draw(sf::RenderWindow* win)
		{
			win->draw(base);
			win->draw(movable);
		}
		void listen(sf::Event& e)
		{
			if(e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
			{
				mouse = {e.mouseButton.x, e.mouseButton.y};
				mousePressing = false;
				if(mouse.x >= movable.getPosition().x && mouse.x <= (movable.getPosition().x + movable.getSize().x) &&
				   mouse.y >= movable.getPosition().y && mouse.y <= (movable.getPosition().y + movable.getSize().y))
				{
					mousePressing = true;
				}
			}
			if(mousePressing && e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left)
			{
				mousePressing = false;
			}
			if(mousePressing && e.type == sf::Event::MouseMoved)
			{
				if(horizontal)
				{
					movable.move({(float)e.mouseMove.x - mouse.x, 0});
					mouse.x = e.mouseMove.x;
				}
				else
				{
					movable.move({0, (float)e.mouseMove.y - mouse.y});
					mouse.y = e.mouseMove.y;
				}
				checkPos();
				if(horizontal) view->setCenter({getViewDiff() * view->getSize().x + view->getSize().x*0.5f, view->getCenter().y});
				else view->setCenter({view->getCenter().x, getViewDiff() * view->getSize().y + view->getSize().y*0.5f});
			}
		}
		bool isMoving()
		{
			return mousePressing;
		}
		float getDiff()
		{
			if(horizontal)
				return movable.getPosition().x - base.getPosition().x;
			return movable.getPosition().y - base.getPosition().y;
		}
		float getViewDiff()
		{
			if(horizontal)
				return (movable.getPosition().x - base.getPosition().x) / movable.getSize().x;
			return (movable.getPosition().y - base.getPosition().y) / movable.getSize().y;
		}
		sf::Vector2f getSize()
		{
			return base.getSize();
		}
		sf::Vector2f getMovableSize()
		{
			return movable.getSize();
		}
		void setSize(sf::Vector2f newsize)
		{
			base.setSize(newsize);
			update(max);
		}
		void setPos(sf::Vector2f newpos)
		{
			float diff = getDiff();
			base.setPosition(newpos);
			movable.setPosition(newpos);
			if(horizontal) movable.move({diff, 0});
			else movable.move({0, diff});
		}
		void setColor(sf::Color b, sf::Color m)
		{
			base.setFillColor(b);
			movable.setFillColor(m);
		}
	private:
		sf::View* view;
		sf::RectangleShape base, movable;
		float max;
		bool horizontal, mousePressing = false;
		sf::Vector2i mouse;
		
		void checkPos()
		{
			if(horizontal)
			{
				if(base.getPosition().x > movable.getPosition().x)
					movable.setPosition(base.getPosition());
				if((movable.getPosition().x + movable.getSize().x) > (base.getPosition().x + base.getSize().x))
					movable.setPosition({base.getPosition().x + base.getSize().x - movable.getSize().x, movable.getPosition().y});
				
				return;
			}
			if(base.getPosition().y > movable.getPosition().y)
				movable.setPosition(base.getPosition());
			if((movable.getPosition().y + movable.getSize().y) > (base.getPosition().y + base.getSize().y))
				movable.setPosition({movable.getPosition().x, base.getPosition().y + base.getSize().y - movable.getSize().y});
		}
		
		void checkSize()
		{
			if(horizontal)
			{
				if(base.getSize().x < movable.getSize().x)
					movable.setSize(base.getSize());
				return;
			}
			if(base.getSize().y < movable.getSize().y)
					movable.setSize(base.getSize());
		}
};
#endif
