#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include "SFML/Graphics.hpp"
#include "TextBox.hpp"
#include "Button.hpp"
#include "Label.hpp"
#include <string>
#include <filesystem>

class Explorer
{
public:
	Explorer(std::string dir)
	{
		currentDirectory = dir;
	}

	std::string selectFile(std::string extension)
	{
		sf::RenderWindow win(sf::VideoMode(600, 500), "Select a file");
		sf::Event e;

		Label label;
		TextBox filename;
		Button ok, cancel;

		while (win.isOpen())
		{
			while (win.pollEvent(e))
			{
				if (e.type == sf::Event::Closed)
				{
					win.close();
					return "";
				}
			}
			win.clear();

			win.display();
		}
	}

	std::string createFile()
	{
		sf::RenderWindow win(sf::VideoMode(600, 500), "Create a file");
		sf::Event e;

		Label label;
		TextBox filename;
		Button ok, cancel;

		while (win.isOpen())
		{
			while (win.pollEvent(e))
			{
				if (e.type == sf::Event::Closed)
				{
					win.close();
					return "";
				}
			}
			win.clear();

			win.display();
		}
	}
private:
	std::string currentDirectory;
	std::vector<std::string> directories, files;

	void update(sf::String extension = "*")
	{
		std::string line;
		for (const auto& entry : std::filesystem::directory_iterator(currentDirectory)) {
			if (std::filesystem::is_directory(entry)) {
				line = entry.path().generic_string();
				directories.push_back(line.substr(line.rfind('/') + 1));
			}
			else if (std::filesystem::is_regular_file(entry)) {
				line = entry.path().generic_string();

				if (line.substr(line.rfind('.') + 1) == extension || extension == "*")
				{
					line = line.substr(line.rfind('/') + 1);
					files.push_back(line);
				}
			}
		}
	}

	void goTo(sf::String dir)
	{
		currentDirectory += "/" + dir;
	}

	void goBack()
	{
		currentDirectory = currentDirectory.substr(0, currentDirectory.rfind('/'));
	}
};
#endif