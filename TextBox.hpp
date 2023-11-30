#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include "SFML/Graphics.hpp"
#include "Scrollbar.hpp"
#include <vector>

//todo: add line numbers (to try: add another view for numbers)

class TextBox
{
public:
	void setUp(sf::RenderWindow& window, float top, float left, float width, float height)
	{
		win = &window;

		x = left; y = top; w = width - scrollBarSize; h = height - scrollBarSize;
		view.reset(sf::FloatRect(0, 0, w, h));
		view.setViewport(sf::FloatRect(x / win->getSize().x, y / win->getSize().y, w / win->getSize().x, h / win->getSize().y));

		scrollX.setUp(view, x, y + h, w, scrollBarSize);
		scrollY.setUp(view, x + w, y, scrollBarSize, h);
		scrollX.setColor(sf::Color(200, 200, 200), sf::Color(100, 100, 100));
		scrollY.setColor(sf::Color(200, 200, 200), sf::Color(100, 100, 100));
		focus = false;
		background.setSize({ width, height });
		background.setPosition({ 0.f, 0.f });
		background.setOutlineThickness(-1);

		multiple_lines = false;
		px = 0;
		py = 0;
		text.push_back("");

		select_begin = { -1, -1 };
		select_end = { -1, -1 };
		selecting = false;
		select_highlight.setFillColor(sf::Color(0, 100, 230));
		toDraw.setFillColor(sf::Color::Black);
		line_height = toDraw.getCharacterSize() + 10.f;
		max_width = 0;

		bar.setFillColor(sf::Color::Black);
		bar.setSize({ 1.5f, line_height });
		bar.setPosition({ 0, 0 });
	}

	void listen(sf::Event& e)
	{
		scrollX.listen(e);
		if(multiple_lines)scrollY.listen(e);
		if (scrollX.isMoving() || scrollY.isMoving())
		{
			background.setPosition({ view.getCenter().x - view.getSize().x * 0.5f, view.getCenter().y - view.getSize().y * 0.5f });
			return;
		}
		if (e.type == sf::Event::Resized)
		{
			view.setViewport(sf::FloatRect(x / win->getSize().x, y / win->getSize().y,
				w / win->getSize().x, h / win->getSize().y));
			adjustView();
		}

		if (lock) return;

		if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
		{
			lostfocus = focus;
			focus = false;
			if (e.mouseButton.x > x && e.mouseButton.x < (x + w) && e.mouseButton.y > y && e.mouseButton.y < (y + h))
				focus = true;
			if (focus != lostfocus && lostfocus)
				lostfocus = true;
			
			if (!focus) return;

			sf::Vector2i pos = getTextPos(getMousePos());
			px = pos.x;
			py = pos.y;

			//update bar pos
			bar.setPosition({ getTextWidth(0, py, px), py * line_height });

			selecting = true;
			select_begin.x = px;
			select_begin.y = py;
			select_end = select_begin;
			clock.restart();
		}
		if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left)
		{
			selecting = false;
		}
		/*mouse moved while selecting event is implemented in draw() function to be better looking*/
		if (!focus) return;
		if (multiple_lines && e.type == sf::Event::MouseWheelScrolled)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
			{
				int n = toDraw.getCharacterSize();
				n += (int)e.mouseWheelScroll.delta;
				if (n < 10 || n > 80) n -= (int)e.mouseWheelScroll.delta;
				toDraw.setCharacterSize(n);
				line_height = n + 10.f;
				bar.setSize({ bar.getSize().x, line_height });

				if(max_width < view.getSize().x && view.getCenter().x > view.getSize().x * 0.5f)
					view.setCenter({view.getSize().x * 0.5f, view.getCenter().y});
				
				adjustView();
				return;
			}
			if (!multiple_lines || (line_height * text.size()) < view.getSize().y) return;
			if (
				e.mouseWheelScroll.delta > 0)
			{
				view.move(0, -line_height);
				if ((view.getCenter().y - view.getSize().y * 0.5f) < 0)
					view.setCenter({ view.getCenter().x, view.getSize().y * 0.5f });
			}
			else
			{
				view.move(0, line_height);
				if ((view.getCenter().y + view.getSize().y * 0.5f) > (line_height * text.size()))
					view.setCenter({ view.getCenter().x, line_height * text.size() - view.getSize().y * 0.5f });
			}
			scrollX.update(max_width);
			scrollY.update(line_height * text.size());
			//update background pos
			background.setPosition({ view.getCenter().x - w * 0.5f, view.getCenter().y - h * 0.5f });
		}
		if (editable && e.type == sf::Event::KeyPressed)
		{
			if (e.key.code == sf::Keyboard::Delete)
			{
				if (deleteSelected())
				{
					adjustView();
					return;
				}
				if (px < (int)text[py].getSize())
				{
					text[py].erase(px);
				}
				else if (py < (int)(text.size() - 1))
				{
					text[py] += text[py + 1];
					text.erase(text.begin() + py + 1);
				}
				adjustView();
				return;
			}
			if (e.key.code == sf::Keyboard::Up)
			{
				if (py > 0)
				{
					py--;
					if (px > (int)text[py].getSize())
						px = (int)text[py].getSize();
				}
				adjustView();
				unselect();
				return;
			}
			if (e.key.code == sf::Keyboard::Down)
			{
				if (py < (int)(text.size() - 1))
				{
					py++;
					if (px > (int)text[py].getSize())
						px = (int)text[py].getSize();

				}
				adjustView();
				unselect();
				return;
			}
			if (e.key.code == sf::Keyboard::Left)
			{
				if (px > 0) px--;
				else if (py > 0)
				{
					py--;
					px = (int)text[py].getSize();
				}
				adjustView();
				unselect();
				return;
			}
			if (e.key.code == sf::Keyboard::Right)
			{
				if (px < (int)text[py].getSize()) px++;
				else if (py < (int)(text.size() - 1))
				{
					px = 0;
					py++;

				}
				adjustView();
				unselect();
				return;
			}
		}
		if (e.type == sf::Event::TextEntered)
		{
			if (!editable)
			{
				if (e.text.unicode == 3) goto copy;
				return;
			}
			switch (e.text.unicode)
			{
				case 3:
				case 24:
				{
					//copy
				copy: {}
					std::vector<sf::Vector3i> v = getSelectedText();
					if (v.size() == 0) break;

					sf::String toCopy = "";
					//x is y, y is begin and z is n character
					for (int i = 0; i < v.size() - 1; i++)
						toCopy += text[v[i].x].substring(v[i].y, v[i].z) + "\n";
					toCopy += text[v[v.size() - 1].x].substring(v[v.size() - 1].y, v[v.size() - 1].z);
					sf::Clipboard::setString(toCopy);

					if (!editable) return;

					//if ctrl + x
					if (e.text.unicode == 24) deleteSelected();
					break;
				}
				case 22:
				{
					//paste
					deleteSelected();
					write(sf::Clipboard::getString(), px, py);
					break;
				}
				case 13:
				{
					//new line
					deleteSelected();
					if (!multiple_lines) break;
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
					if (deleteSelected()) break;
					if (px > 0) text[py].erase(--px);
					else if (py > 0)
					{
						px = (int)text[py - 1].getSize();
						text[py - 1] += text[py];
						text.erase(text.begin() + py--);
					}
					break;
				}
				default:
				{
					//add character
					if (e.text.unicode >= 32)
					{
						deleteSelected();
						text[py].insert(px++, e.text.unicode);
					}
					else switch(e.text.unicode)
					{
					case 9: //tab
						deleteSelected();
						text[py].insert(px++, e.text.unicode);
						break;
					case 4: //ctrl + d
						if (select_begin.x == -1) text.insert(text.begin() + py + 1, text[py]);
						else
						{
							std::vector<sf::Vector3i> v = getSelectedText();
							for (int i = 0; i < v.size(); i++)
								text.insert(text.begin() + v[i].x + v.size(), text[v[i].x].substring(v[i].y, v[i].z).toAnsiString());
						}
						adjustView();
						break;
					}
				}
			}
			adjustView();
		}
	}

	void draw()
	{
		if (selecting)
		{
			//updating select position
			select_end = getTextPos(getMousePos());
			//moving view while selecting if cursor > or < window 
			if (clock.getElapsedTime().asSeconds() > 0.03f)
			{
				if (sf::Mouse::getPosition(*win).y < y)
				{
					view.move(0, -line_height);
					if (view.getCenter().y < (view.getSize().y * 0.5f))
						view.setCenter({ view.getCenter().x, view.getSize().y * 0.5f });
				}
				else if (sf::Mouse::getPosition(*win).y > (y + h))
				{
					view.move(0, line_height);
					if ((view.getCenter().y + view.getSize().y * 0.5f) > (line_height * text.size()))
						view.setCenter({ view.getCenter().x, line_height * text.size() - view.getSize().y * 0.5f });
				}
				if (sf::Mouse::getPosition(*win).x < x)
				{
					view.move(-0.01f * max_width, 0);
					if (view.getCenter().x < (view.getSize().x * 0.5f))
						view.setCenter({ view.getSize().x * 0.5f, view.getCenter().y });
				}
				else if (sf::Mouse::getPosition(*win).x > (x + w))
				{
					view.move(0.01f * max_width, 0);
					if ((view.getCenter().x + view.getSize().x * 0.5f) > (max_width))
						view.setCenter({ max_width - view.getSize().x * 0.5f, view.getCenter().y });
				}
				background.setPosition(view.getCenter().x - view.getSize().x * 0.5f, view.getCenter().y - view.getSize().y * 0.5f);
				clock.restart();
			}
		}
		
		sf::View temp = win->getView();
		win->setView(view);

		//draw background
		win->draw(background);

		//draw selected text
		if (select_begin.x != -1) drawSelected();

		//draw text
		if (multiple_lines) multiLinesRender();
		else singleLinesRender();
		if(focus)win->draw(bar);

		win->setView(temp);

		//draw scrollbars
		scrollX.draw(win);
		if(multiple_lines)scrollY.draw(win);
	}

	void loadSpecialKeys(std::vector<sf::String> v, sf::Color c)
	{
		special.push_back({ v, c });
	}
	void loadDelimKeys(sf::Uint8 begin, sf::Uint8 end, sf::Color color)
	{
		delimitator.push_back({begin, end, color});
	}
	inline void setFocus()
	{
		focus = true;
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
		line_height = size + 10.f;
		bar.setSize({ 3, line_height });
		adjustView();
	}
	void setSize(sf::Vector2f size)
	{
		w = size.x - scrollBarSize;
		h = size.y - scrollBarSize;

		view.reset(sf::FloatRect(0, 0, w, h));
		view.setViewport(sf::FloatRect(x / win->getSize().x, y / win->getSize().y, w / win->getSize().x, h / win->getSize().y));

		scrollX.setPos({ x, y + h });
		scrollX.setSize({ w, scrollBarSize });

		scrollY.setPos({ x + w, y });
		scrollY.setSize({ scrollBarSize, h });

		background.setSize({ w, h });
		adjustView();
	}
	sf::Vector2f getSize()
	{
		sf::Vector2f v = { w, h + scrollBarSize };
		if (multiple_lines) w += scrollBarSize;
		return v;
	}
	inline void setBgColor(sf::Color color)
	{
		background.setFillColor(color);
	}
	inline void setOutlineColor(sf::Color color)
	{
		background.setOutlineColor(color);
	}
	inline void setTextColor(sf::Color color)
	{
		toDraw.setFillColor(color);
	}
	inline void setBarColor(sf::Color color)
	{
		bar.setFillColor(color);
	}
	inline sf::Color getBarColor()
	{
		return bar.getFillColor();
	}
	void setScrollBarSize(float n)
	{
		w = w + scrollBarSize - n;
		h = h + scrollBarSize - n;
		scrollBarSize = n;

		view.reset({0, 0, w, h});
		view.setViewport(sf::FloatRect(x / win->getSize().x, y / win->getSize().y,
			w / win->getSize().x, h / win->getSize().y));

		scrollX.setSize({ w, scrollBarSize });
		scrollY.setSize({ scrollBarSize, h });

		scrollX.setPos({ x, y + h });
		scrollY.setPos({ x + w, y });
	}
	void setPos(sf::Vector2f newpos)
	{
		x = newpos.x; y = newpos.y;
		view.setViewport(sf::FloatRect(x / win->getSize().x, y / win->getSize().y,
			w / win->getSize().x, h / win->getSize().y));

		scrollX.setPos({ x, y + h });
		scrollY.setPos({ x + w, y });
	}
	inline sf::Vector2f getPos()
	{
		return { x, y };
	}
	void write(sf::String str, int tx = -1, int ty = -1)
	{
		if (ty == -1)
		{
			ty = (int)text.size() - 1;
			tx = (int)text[ty].getSize();
		}

		if (ty < 0 || ty >= text.size()) return;
		if (tx < 0 || tx > text[ty].getSize()) return;

		sf::String s = text[ty].substring(tx);
		std::vector<sf::String> t = split(str, '\n');
		if (!multiple_lines)
		{
			text[0] = text[0].substring(0, tx) + t[0] + text[0].substring(tx);
			return;
		}
		text[ty] = text[ty].substring(0, tx) + t[0];
		for (int i = 1; i < t.size(); i++)
			text.insert(text.begin() + ty + i, t[i]);
		text[ty + t.size() - 1] += s;

		return;
	}

	inline std::vector<sf::String> getText()
	{
		return text;
	}
	void clear()
	{
		text.clear();
		text.push_back("");
		px = 0;
		py = 0;
		unselect();
		adjustView();
	}
	void setMultiLines(bool b)
	{
		multiple_lines = b;
		if (!b)setSize({w + scrollBarSize, line_height + scrollBarSize});
	}
	bool lostFocus()
	{
		if (lostfocus) 
		{
			lostfocus = false;
			return true;
		}
		return false;
	}
	void showNumbers(bool b)
	{
		
	}
	inline void Lock(bool b)
	{
		lock = b;
	}
	inline void setEditable(bool b)
	{
		editable = b;
	}
private:

	/*----------Variables----------*/
	//text manipulation
	bool multiple_lines = false;
	std::vector<sf::String> text;
	int px = 0, py = 0;
	sf::Vector2i select_begin, select_end;
	bool selecting;
	struct keys
	{
		std::vector<sf::String> specialKeys;
		sf::Color color;
	};
	struct delim
	{
		sf::Uint8 begin, end;
		sf::Color color;
	};
	std::vector<keys> special;
	std::vector<delim> delimitator;
	sf::Clock clock;

	//text render
	sf::Font font;
	sf::Text toDraw;
	float line_height = 0, max_width = 0;
	sf::RectangleShape select_highlight;

	//scrollbars
	Scrollbar scrollX, scrollY;
	float scrollBarSize = 7;

	//bar to show current position in text
	sf::RectangleShape bar;

	//view and window
	sf::RenderWindow* win;
	sf::View view;
	float x = 0, y = 0, w = 0, h = 0;
	bool focus = false, lostfocus = false, lock = false, editable = true;
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
		int n = (view.getCenter().y - view.getSize().y / 2) / line_height;
		int i = n;
		n = (view.getCenter().y + view.getSize().y / 2) / line_height;

		for (; i < n && i < (int)text.size(); ++i)
		{
			std::vector<sf::String> temp = divide(text[i]);
			float xpos = 0;
			for (sf::String str : temp)
			{
				bool delimfound = false;
				for (int j = 0; j < delimitator.size(); ++j)
				{
					if (str[0] == delimitator[j].begin)
					{
						delimfound = true;
						toDraw.setFillColor(delimitator[j].color);
						break;
					}
				}
				if(!delimfound)for (int j = 0; j < special.size(); j++)
				{
					bool b = false;
					for (sf::String s : special[j].specialKeys)
					{
						if (s == str)
						{
							toDraw.setFillColor(special[j].color);
							b = true;
							break;
						}
					}
					if (b) break;
				}
				toDraw.setString(str);
				toDraw.setPosition(xpos, i * line_height);
				win->draw(toDraw);

				xpos += toDraw.getLocalBounds().width + toDraw.getLocalBounds().left;
				toDraw.setFillColor(sf::Color::Black);
			}
		}
	}
	void drawSelected()
	{
		if (select_begin == select_end) return;

		sf::Vector2i begin, end;

		toDraw.setPosition({ 0.f, 0.f });
		//get real begin and end x pos if on same y
		if (select_begin.y == select_end.y)
		{
			if (select_begin.x > select_end.x)
			{
				begin = select_end;
				end = select_begin;
			}
			else
			{
				begin = select_begin;
				end = select_end;
			}

			select_highlight.setSize({ getTextWidth(begin.x, begin.y, end.x - begin.x), line_height});
			select_highlight.setPosition({ getTextWidth(0, begin.y, begin.x), begin.y * line_height });
			win->draw(select_highlight);

			return;
		}

		//else get real begin and end y pos
		if (select_begin.y > select_end.y)
		{
			begin = select_end;
			end = select_begin;
		}
		else
		{
			begin = select_begin;
			end = select_end;
		}

		//draw first
		select_highlight.setSize({ getTextWidth(begin.x, begin.y, text[begin.y].getSize() - begin.x), line_height});
		select_highlight.setPosition({ getTextWidth(0, begin.y, begin.x), begin.y * line_height });
		win->draw(select_highlight);

		//draw between
		for (int i = begin.y + 1; i < end.y; i++)
		{
			select_highlight.setPosition({ 0.f, i * line_height });
			select_highlight.setSize({ getTextWidth(0, i, text[i].getSize()), line_height});
			win->draw(select_highlight);
		}

		//draw last
		select_highlight.setPosition({ 0.f, end.y * line_height });
		select_highlight.setSize({ getTextWidth(0, end.y, end.x), line_height });
		win->draw(select_highlight);

	}
	std::vector<sf::Vector3i> getSelectedText()
	{
		std::vector<sf::Vector3i> v;

		if (select_begin == select_end) return v;

		sf::Vector2i begin, end;

		//get real begin and end x pos if on same y
		if (select_begin.y == select_end.y)
		{
			if (select_begin.x > select_end.x)
			{
				begin = select_end;
				end = select_begin;
			}
			else
			{
				begin = select_begin;
				end = select_end;
			}

			v.push_back({ begin.y, begin.x, end.x - begin.x });
			return v;
		}

		//else get real begin and end y pos
		if (select_begin.y > select_end.y)
		{
			begin = select_end;
			end = select_begin;
		}
		else
		{
			begin = select_begin;
			end = select_end;
		}

		v.push_back({ begin.y, begin.x, (int)text[begin.y].getSize() - begin.x });
		for (int i = begin.y + 1; i < end.y; i++)
			v.push_back({ i, 0, (int)text[i].getSize() });
		v.push_back({ end.y, 0, end.x });

		return v;
	}
	bool deleteSelected()
	{
		auto v = getSelectedText();
		switch (v.size())
		{
		case 0:
		{
			return false;
		}
		case 1:
		{
			text[v[0].x] = text[v[0].x].substring(0, v[0].y) + text[v[0].x].substring(v[0].y + v[0].z);
			break;
		}
		case 2:
		{
			text[v[0].x] = text[v[0].x].substring(0, v[0].y) + text[v[1].x].substring(v[1].y + v[1].z);
			text.erase(text.begin() + v[1].x);
			break;
		}
		default:
			text[v[0].x] = text[v[0].x].substring(0, v[0].y) + text[v[v.size() - 1].x].substring(v[v.size() - 1].y + v[v.size() - 1].z);
			text.erase(text.begin() + v[1].x, text.begin() + v[v.size() - 1].x + 1);
			break;
		}
		py = v[0].x;
		px = v[0].y;
		unselect();
		return true;
	}
	void unselect()
	{
		select_begin = sf::Vector2i(-1, -1);
		select_end = sf::Vector2i(-1, -1);
		selecting = false;
	}
	sf::Vector2i getMousePos()
	{
		sf::Vector2i vv = sf::Mouse::getPosition(*win);

		sf::Vector2f v2f(view.getCenter().x - view.getSize().x / 2, view.getCenter().y - view.getSize().y / 2);

		vv.x = (int)((vv.x - x) * (view.getSize().x / w) + v2f.x);
		vv.y = (int)((vv.y - y) * (view.getSize().y / h) + v2f.y);
		return vv;
	}
	sf::Vector2i getTextPos(sf::Vector2i mouse)
	{
		//pos to return
		sf::Vector2i pos;

		//getting line y pos
		if ((mouse.y / line_height) >= text.size())
			pos.y = text.size() - 1;
		else
			pos.y = mouse.y / (int)line_height;
		if (pos.y < 0) pos.y = 0;

		//getting line x pos
		toDraw.setString(text[pos.y]);
		mouse.x;
		for (int i = text[pos.y].getSize(); i >= 0; i--)
		{
			if (mouse.x >= getTextWidth(0, pos.y, i))
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
		if (multiple_lines)
		{
			max_width = 0;
			for (int i = 0; i < text.size(); i++)
			{
				float temp = getTextWidth(0, i, text[i].getSize());
				if (max_width < temp) max_width = temp;
			}
		} else max_width = getTextWidth(0, 0, text[0].getSize());
		//check width
		float width = getTextWidth(0, py, px) + bar.getSize().x;
		if (width <= view.getSize().x) view.setCenter(view.getSize().x * 0.5f, view.getCenter().y);
		else
		{
			if (max_width > view.getSize().x && max_width < (view.getCenter().x + view.getSize().x * 0.5f))
				view.setCenter({ max_width - view.getSize().x * 0.5f, view.getCenter().y });	//wrap
			if (width > (view.getCenter().x + view.getSize().x * 0.5f))
				view.move({ width - view.getCenter().x - view.getSize().x * 0.5f, 0.f });		//scroll right
			if (width < (view.getCenter().x - view.getSize().x * 0.5f))
				view.move({ width - (view.getCenter().x - view.getSize().x * 0.5f), 0 });		//scroll left
		}
		//check height
		float height = (py + 1) * line_height;
		if (height > (view.getCenter().y + h * 0.5f))				  
			view.move({ 0, height - (view.getCenter().y + h * 0.5f) });							//scroll down
		if ((py * line_height) < (view.getCenter().y - h * 0.5f))	  
			view.move({ 0, (py * line_height) - (view.getCenter().y - h * 0.5f) });				//scroll up
		//update bar position
		bar.setPosition({ width - bar.getSize().x, py * line_height});
		//update background pos
		background.setPosition({ view.getCenter().x - view.getSize().x * 0.5f, view.getCenter().y - view.getSize().y * 0.5f });
		//update scrollbar size and pos
		scrollX.update(max_width);
		scrollY.update(line_height * text.size());
	}

	std::vector<sf::String> split(sf::String str, char del)
	{
		std::vector<sf::String> res;
		sf::String s = "";
		for (int i = 0; i < str.getSize(); i++)
		{
			if ((char)str[i] == del) {
				res.push_back(s);
				s = "";
				continue;
			}
			s += str[i];
		}
		res.push_back(s);
		return res;
	}
	
	std::vector<sf::String> divide(sf::String s)
	{
		std::vector<sf::String> v;
		sf::String str = "";
		for (int i = 0; i < (int)s.getSize(); i++)
		{
			bool checkdelimfound = false;
			for (int j = 0; j < delimitator.size(); j++)
			{
				if (s[i] == delimitator[j].begin)
				{
					wchar_t ch = delimitator[j].end;
					v.push_back(str);

					for (j = i + 1; j < (int)s.getSize(); j++)
					{
						if (s[j] == ch) {
							j++;
							break;
						}
					}
					str = s.substring(i, j - i);
					v.push_back(str);
					str = "";
					i = j - 1;
					checkdelimfound = true;
					break;
				}
			}
			
			if (checkdelimfound) continue;
			
			if ((char)s[i] == '\t' || (char)s[i] == ' ')
			{
				if(str != "")v.push_back(str);
				v.push_back(s[i]);
				str = "";
				continue;
			}
			str += s[i];
		}
		if (str != "")v.push_back(str);

		return v;
	}

	float getTextWidth(int sx, int sy, int n)
	{
		std::vector<sf::String> temp = divide(text[sy].substring(sx, n));
		float width = 0.f;
		for (sf::String s : temp)
		{
			toDraw.setString(s);
			width += toDraw.getLocalBounds().width + toDraw.getLocalBounds().left;
		}
		return width;
	}
};
#endif