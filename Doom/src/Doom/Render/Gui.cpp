#include "Gui.h"
#include <functional>
#include "../Core/Timer.h"

using namespace Doom;

void Doom::Gui::Text(std::string str, int m_static, float x, float y, float scale, glm::vec4 color , int charsAfterComma, ...)
{
	scale /= font->size;
	std::vector<Character*> characters1;
	std::vector<unsigned int> newLines1;

	std::vector<Character*> characters;
	std::vector<unsigned int> newLines;
	double widthRatio = Window::GetCamera().GetAspectRation().x  / Window::GetProps()[0];
	double heightRatio = Window::GetCamera().GetAspectRation().y / Window::GetProps()[1];
	text = str;
	m_charsAfterComma = charsAfterComma;
	int counter1 = 0;
	bool border = false;
	va_list argptr;
	va_start(argptr, charsAfterComma);
	size_t strSize = str.size();
	for (size_t i = 0; i < strSize; i++)
	{
		char strChar = str[i];
		if (strChar == '%') {
			if (str[i + 1] == 'd')
				s = std::to_string(va_arg(argptr, int));
			else if (str[i + 1] == 'f') {
				s = std::to_string(va_arg(argptr, double));
			}
			else if (str[i + 1] == 's')
				s = (va_arg(argptr, std::string));
			for (size_t j = 0; j < s.size(); j++)
			{
				pchar = s[j];
				if (pchar == '.') {
					border = true;
				}
				if (border == true)
					counter1++;
				if (charsAfterComma == 0 && pchar == '.')
					break;
				
				size_t size = font->characters.size();
				for (size_t i = 0; i < size; i++)
				{
					if (font->characters[i]->ch == pchar) {
						characters.push_back(font->characters[i]);
						break;
					}
				}
				if (counter1 > charsAfterComma)
					break;
			}
			counter1 = 0;
			border = false;
			i++;
		}
		else if (strChar == '\n') {
			newLines.push_back(characters.size());
			continue;
		}
		else {
			size_t size = font->characters.size();
			for (size_t k = 0; k < size; k++)
			{
				if (font->characters[k]->ch == strChar) {
					characters.push_back(font->characters[k]);
					break;
				}
			}
		}
	}

	unsigned int size = characters.size();

	double _width = 0;
	double _height = 0;

	for (unsigned int i = 0; i < size; i++)
	{
		character = characters[i];
		_width += character->xadvance * character->_scale.x;
		
	}
	
	_height += 80 * character->_scale.y;

	if (xAlign == AlignHorizontally::XCENTER) {
		x -= _width * 0.5;
	}
	if (yAlign == AlignVertically::YCENTER) {
		y += _height * 0.5;
	}

	
	for (unsigned int i = 0; i < size; i++)
	{
		unsigned int newLinesSize = newLines.size();
		for (unsigned int j = 0; j < newLinesSize; j++)
		{
			if (i == newLines[j]) {
				y -= 80 * scale;
				counter = 0;
				newLines.erase(newLines.begin() + j);
				break;
			}
		}
		character = characters[i];
		character->isRelatedToCam = m_static;
		character->_scale.x = scale;
		character->_scale.y = scale;
		character->color = color;

		//Pos is top-left corner
		character->mesh2D[0] = 0;
		character->mesh2D[1] = -(heightRatio * (character->height + character->yoffset));
		character->mesh2D[4] = (widthRatio  * (character->width));
		character->mesh2D[5] = -(heightRatio * (character->height + character->yoffset));
		character->mesh2D[8] = (widthRatio  * (character->width));
		character->mesh2D[9] = -heightRatio * character->yoffset;
		character->mesh2D[12] = 0;
		character->mesh2D[13] = -heightRatio * character->yoffset;

		if (i == 0) {
			counter = 0;
			character->Translate(widthRatio * (x + counter), heightRatio * y);
		}
		else {
			character->Translate(widthRatio * (x + counter), heightRatio * y);
		}
		counter += character->xadvance * character->_scale.x;
		Batch::GetInstance()->Submit(*character);
	}

	va_end(argptr);
}

bool Doom::Gui::Button(std::string str, float x, float y,float scale, float width, float height, glm::vec4 btnColor, glm::vec4 pressedBtnColor, glm::vec4 textColor)
{
	this->btnColor = btnColor;
	bool tempResult = false;
	bool returnResult = false;
	
	glm::vec2 pos = glm::vec2(Window::GetCamera().GetRatio().x * x, Window::GetCamera().GetRatio().y * y);
	glm::dvec2 mousePos = ViewPort::Instance()->GetMousePositionToScreenSpace();

	double tempX = width * Window::GetCamera().GetRatio().x;
	double tempY = -height * Window::GetCamera().GetRatio().y;

	float vertecies[8] = {
		pos.x,(tempY) + pos.y,
		(tempX) + pos.x,(tempY) + pos.y,
		(tempX) + pos.x,pos.y,
		pos.x,pos.y
	};


	if (Input::IsMouseDown(GLFW_MOUSE_BUTTON_1)) {
		tempResult = (mousePos.x > vertecies[0] && mousePos.x < vertecies[2] && mousePos.y > vertecies[1] && mousePos.y < vertecies[7]);
		if (tempResult)
			this->btnColor = pressedBtnColor;
	}

	if (Input::IsMousePressed(GLFW_MOUSE_BUTTON_1)) {
	
		returnResult = tempResult;
	}

	
	Batch::GetInstance()->Submit(vertecies, this->btnColor);

	xAlign = AlignHorizontally::XCENTER;
	yAlign = AlignVertically::YCENTER;
	Text(str, true, (width * 0.5) + x, y - (height * 0.5), scale, textColor);
	yAlign = AlignVertically::BOTTOM;
	xAlign = AlignHorizontally::LEFT;
	return returnResult;
}

void Doom::Gui::Panel(float x, float y, float width, float height, glm::vec4 color,Texture* texture)
{
	glm::vec2 pos = glm::vec2(Window::GetCamera().GetRatio().x * x, Window::GetCamera().GetRatio().y * y);

	double tempX = width * Window::GetCamera().GetRatio().x;
	double tempY = -height * Window::GetCamera().GetRatio().y;

	float vertecies[8] = {
		pos.x,(tempY)+pos.y,
		(tempX)+pos.x,(tempY)+pos.y,
		(tempX)+pos.x,pos.y,
		pos.x,pos.y
	};

	Batch::GetInstance()->Submit(vertecies, color,texture);
}

void Doom::Gui::LoadStandartFonts()
{
	
	Font* font = new Font();
	font->LoadFont("src/Fonts/calibri.txt", "src/Fonts/calibri.png");
	font->LoadCharacters();
	standartFonts.push_back(font);

	font = new Font();
	font->LoadFont("src/Fonts/arial.txt", "src/Fonts/arial.png");
	font->LoadCharacters();
	standartFonts.push_back(font);

	font = new Font();
	font->LoadFont("src/Fonts/harrington.txt", "src/Fonts/harrington.png");
	font->LoadCharacters();
	standartFonts.push_back(font);


	FontBind(standartFonts.front());
}