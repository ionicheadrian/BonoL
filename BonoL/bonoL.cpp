#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <fstream>

using namespace std;
using namespace sf;

const int tablaSize = 4;
const sf::Vector2f cellSize(125, 125);
const int windowWidth = 900;
const int windowHeight = 900;
const Color Gray(192, 192, 192);
SoundBuffer click_buffer;
Sound click_sound;

struct GameContent {

	sf::RectangleShape tabla[tablaSize][tablaSize];
	sf::CircleShape circle1{ cellSize.x / 4 }; // Piesa stanga sus
	sf::CircleShape circle2{ cellSize.y / 4 }; // Piesa dreapta jos
	bool isCircle1Selected;
	Color CuloareFundal{ 144, 12, 63 };
	struct {
		bool IsTurn;
		string username;
		int miscari;
	}Player;

	int isgameover;

	struct {
		int blue;
		int red;
	}AmColorat;
	sf::RectangleShape undo[tablaSize][tablaSize];
};

enum class GameState
{
	MENU,
	PLAYERVPLAYER,
	PLAYERVCPU,
	HELP,
	CLASAMENT,
	EXIT
};

struct Button {
	RectangleShape shape;
	Text text;
	bool hover;
};
	


void Copie(const sf::RectangleShape decopiat[tablaSize][tablaSize], sf::RectangleShape copie[tablaSize][tablaSize]) {
	for (int i = 0; i < tablaSize; ++i)
		for (int j = 0; j < tablaSize; ++j)
			copie[i][j].setFillColor(decopiat[i][j].getFillColor());
}

void UndoMove(sf::RectangleShape tabla[tablaSize][tablaSize], const sf::RectangleShape undo[tablaSize][tablaSize]) {
	for (int i = 0; i < tablaSize; ++i)
		for (int j = 0; j < tablaSize; ++j)
			tabla[i][j].setFillColor(undo[i][j].getFillColor());

}

void golire(sf::RectangleShape tabla[tablaSize][tablaSize], sf::Color culoare) {
	for (int i = 0; i < tablaSize; ++i)
		for (int j = 0; j < tablaSize; ++j)
			if (tabla[i][j].getFillColor() == culoare)
				tabla[i][j].setFillColor(sf::Color::White);
}

bool CelulaOcupata(const sf::RectangleShape& cell, const sf::CircleShape& circle1, const sf::CircleShape& circle2) {
	sf::FloatRect cellBounds = cell.getGlobalBounds();
	sf::FloatRect circle1Bounds = circle1.getGlobalBounds();
	sf::FloatRect circle2Bounds = circle2.getGlobalBounds();
	if (cellBounds.intersects(circle1Bounds) == true)
		return true;
	if (cellBounds.intersects(circle2Bounds) == true)
		return true;
	return false;
}

bool CelulaGoala(const sf::RectangleShape& cell, sf::Color targetColor) {
	return cell.getFillColor() == targetColor;
}

void TablaInit(GameContent& joc, const sf::Vector2f& cellSize, int windowWidth, int windowHeight) {

	joc.Player.IsTurn = true;
	joc.isCircle1Selected = true;
	joc.AmColorat.red = joc.AmColorat.blue = 0;
	int xOffset = (windowWidth - (tablaSize * cellSize.x)) / 2;
	int yOffset = (windowHeight - (tablaSize * cellSize.y)) / 2;
	for (int i = 0; i < tablaSize; ++i) {
		for (int j = 0; j < tablaSize; ++j) {
			joc.tabla[i][j].setSize(cellSize);
			joc.tabla[i][j].setFillColor(sf::Color::White);
			joc.tabla[i][j].setOutlineColor(sf::Color::Black);
			joc.tabla[i][j].setOutlineThickness(2);
			joc.tabla[i][j].setPosition(j * cellSize.x + xOffset, i * cellSize.y + yOffset);
		}
	}

	//p2
	joc.tabla[0][2].setFillColor(sf::Color::Blue);
	joc.tabla[1][2].setFillColor(sf::Color::Blue);
	joc.tabla[2][2].setFillColor(sf::Color::Blue);
	joc.tabla[0][1].setFillColor(sf::Color::Blue);
	//p1
	joc.tabla[1][1].setFillColor(sf::Color::Red);
	joc.tabla[2][1].setFillColor(sf::Color::Red);
	joc.tabla[3][1].setFillColor(sf::Color::Red);
	joc.tabla[3][2].setFillColor(sf::Color::Red);

	//neutru
	joc.circle1.setPosition(joc.tabla[0][0].getPosition().x + cellSize.x / 2 - joc.circle1.getRadius(),
		joc.tabla[0][0].getPosition().y + cellSize.y / 2 - joc.circle1.getRadius());
	joc.circle2.setPosition(joc.tabla[tablaSize - 1][tablaSize - 1].getPosition().x + cellSize.x / 2 - joc.circle2.getRadius(),
		joc.tabla[tablaSize - 1][tablaSize - 1].getPosition().y + cellSize.y / 2 - joc.circle2.getRadius());
	joc.circle1.setFillColor(Color::Black);
	joc.circle2.setFillColor(Color::Black);


	Copie(joc.tabla, joc.undo);
}

bool isMouseOverButton(const Button& button, const Vector2f& mousePosition) {
	return button.shape.getGlobalBounds().contains(mousePosition);
}

void displayTura(bool isPlayer1Turn, sf::RenderWindow& window) {
	sf::Font font;
	if (font.loadFromFile("Candy Beans.otf") == 0) {
		cout << "Error loading font\n";
		return;
	}

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(50);
	text.setFillColor(sf::Color::White);

	if (isPlayer1Turn) {
		text.setString("TURA JUCATORULUI 1");
	}
	else {
		text.setString("TURA JUCATORULUI 2");
	}

	text.setPosition(windowWidth / 2 - 180, 30); // Adjust the position as needed
	window.draw(text);
}

void displayInvalidMove(sf::RenderWindow& window) {
	sf::Font font;
	if (font.loadFromFile("Candy Beans.otf") == 0) {
		cout << "Error loading font\n";
		return;
	}

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(40);
	text.setFillColor(sf::Color::Red);
	text.setString("INVALID");

	text.setPosition(windowWidth / 2 - text.getLocalBounds().width / 2, 90);

	window.draw(text);
	window.display();
	sf::sleep(sf::seconds(1));
	/*window.clear();*/
}

bool egale(RectangleShape a[tablaSize][tablaSize], RectangleShape b[tablaSize][tablaSize], Color culoare)
{
	unsigned int k = 0;
	for (int i = 0; i < tablaSize; i++)
		for (int j = 0; j < tablaSize; j++)
			if (a[i][j].getFillColor() == b[i][j].getFillColor())
				if (a[i][j].getFillColor() == culoare)
					k++;
	if (k == 4)
		return true;
	else return false;
}

bool ValidMove(sf::RectangleShape tabla[tablaSize][tablaSize], Color Culoare, sf::RectangleShape undo[tablaSize][tablaSize]) {

	if (egale(tabla, undo, Culoare) == true)
		return false;

	int k = 0;
	for (int i = 0; i < tablaSize; i++)
		for (int j = 0; j < tablaSize; j++)
			if (tabla[i][j].getFillColor() == Culoare)
				k++;
	if (k != 4)
		return false;
	for (int i = 0; i < tablaSize; i++)
		for (int j = 0; j < tablaSize; j++) {
			if (i < tablaSize - 2 && j < tablaSize - 1 &&
				tabla[i][j].getFillColor() == Culoare &&
				tabla[i + 1][j].getFillColor() == Culoare &&
				tabla[i + 2][j].getFillColor() == Culoare &&
				tabla[i + 2][j + 1].getFillColor() == Culoare)
				return true;
			if (i < tablaSize - 2 && j > 0 &&
				tabla[i][j].getFillColor() == Culoare &&
				tabla[i + 1][j].getFillColor() == Culoare &&
				tabla[i + 2][j].getFillColor() == Culoare &&
				tabla[i + 2][j - 1].getFillColor() == Culoare)
				return true;
			if (i > 1 && j > 0 && tabla[i][j].getFillColor() == Culoare &&
				tabla[i - 1][j].getFillColor() == Culoare &&
				tabla[i - 2][j].getFillColor() == Culoare &&
				tabla[i - 2][j - 1].getFillColor() == Culoare)
				return true;
			if (i > 1 && j < tablaSize - 1 &&
				tabla[i][j].getFillColor() == Culoare &&
				tabla[i - 1][j].getFillColor() == Culoare &&
				tabla[i - 2][j].getFillColor() == Culoare &&
				tabla[i - 2][j + 1].getFillColor() == Culoare)
				return true;
			if (i < tablaSize - 2 && j < tablaSize - 1 &&
				tabla[i][j].getFillColor() == Culoare &&
				tabla[i][j + 1].getFillColor() == Culoare &&
				tabla[i + 1][j + 1].getFillColor() == Culoare &&
				tabla[i + 2][j + 1].getFillColor() == Culoare)
				return true;
			if (i < tablaSize - 2 && j > 0 &&
				tabla[i][j].getFillColor() == Culoare &&
				tabla[i][j - 1].getFillColor() == Culoare &&
				tabla[i + 1][j - 1].getFillColor() == Culoare &&
				tabla[i + 2][j - 1].getFillColor() == Culoare)
				return true;
			if (i > 1 && j > 0 &&
				tabla[i][j].getFillColor() == Culoare &&
				tabla[i][j - 1].getFillColor() == Culoare &&
				tabla[i - 1][j - 1].getFillColor() == Culoare &&
				tabla[i - 2][j - 1].getFillColor() == Culoare)
				return true;
			if (i > 1 && j < tablaSize - 1 &&
				tabla[i][j].getFillColor() == Culoare &&
				tabla[i][j + 1].getFillColor() == Culoare &&
				tabla[i - 1][j + 1].getFillColor() == Culoare &&
				tabla[i - 2][j + 1].getFillColor() == Culoare)
				return true;
			// Check for L-shape lying on its back in all four directions
			if (i < tablaSize - 1 && j < tablaSize - 2 &&
				tabla[i][j].getFillColor() == Culoare &&
				tabla[i][j + 1].getFillColor() == Culoare &&
				tabla[i][j + 2].getFillColor() == Culoare &&
				tabla[i + 1][j + 2].getFillColor() == Culoare)
				return true;
			if (i > 0 && j < tablaSize - 2 &&
				tabla[i][j].getFillColor() == Culoare &&
				tabla[i][j + 1].getFillColor() == Culoare &&
				tabla[i][j + 2].getFillColor() == Culoare &&
				tabla[i - 1][j + 2].getFillColor() == Culoare)
				return true;
			if (i < tablaSize - 1 && j > 1 &&
				tabla[i][j].getFillColor() == Culoare &&
				tabla[i][j - 1].getFillColor() == Culoare &&
				tabla[i][j - 2].getFillColor() == Culoare &&
				tabla[i + 1][j - 2].getFillColor() == Culoare)
				return true;
			if (i > 0 && j > 1 &&
				tabla[i][j].getFillColor() == Culoare &&
				tabla[i][j - 1].getFillColor() == Culoare &&
				tabla[i][j - 2].getFillColor() == Culoare &&
				tabla[i - 1][j - 2].getFillColor() == Culoare)
				return true;
		}
	return false;

}

void displayPause(sf::RenderWindow& window, GameContent& joc, GameState& gameState) {

	sf::RectangleShape overlay(sf::Vector2f(windowWidth, windowHeight));
	overlay.setFillColor(sf::Color(0, 0, 0, 200));
	
	if (!click_buffer.loadFromFile("pop.ogg"))
		cout << "error";
	click_sound.setBuffer(click_buffer);
	sf::Font font;
	if (!font.loadFromFile("Candy Beans.otf")) {
		std::cerr << "Error loading font\n";
		return;
	}

	sf::Text titlu;
	titlu.setFont(font);
	titlu.setCharacterSize(60);
	titlu.setString("Pause");
	titlu.setPosition(windowWidth / 2 - titlu.getLocalBounds().width / 2, windowHeight / 2 - titlu.getLocalBounds().height / 2 - 350);
	titlu.setFillColor(sf::Color::White);
	titlu.setStyle(sf::Text::Bold);

	//Back

	sf::RectangleShape buttonInapoi(sf::Vector2f(220, 50));
	buttonInapoi.setFillColor(sf::Color(128, 128, 128)); // Grey
	buttonInapoi.setPosition(windowWidth / 2 - 250, windowHeight / 2 - 100);

	sf::Text textInapoi;
	textInapoi.setFont(font);
	textInapoi.setCharacterSize(20);
	textInapoi.setString("Meniul principal");
	textInapoi.setFillColor(sf::Color::White);
	textInapoi.setStyle(sf::Text::Bold);
	textInapoi.setPosition(buttonInapoi.getPosition().x + 30, buttonInapoi.getPosition().y + 10);


	//Continue

	sf::RectangleShape buttonContinue(sf::Vector2f(220, 50));
	buttonContinue.setFillColor(sf::Color(128, 128, 128)); // Grey
	buttonContinue.setPosition(windowWidth / 2 + 50, windowHeight / 2 - 100);

	sf::Text textContinue;
	textContinue.setFont(font);
	textContinue.setCharacterSize(20);
	textContinue.setString("Continue");
	textContinue.setFillColor(sf::Color::White);
	textContinue.setStyle(sf::Text::Bold);
	textContinue.setPosition(buttonContinue.getPosition().x + 60, buttonContinue.getPosition().y + 10);


	//EXIT//

	sf::RectangleShape buttonExit(sf::Vector2f(220, 50));
	buttonExit.setFillColor(sf::Color(196, 30, 58));//red
	buttonExit.setPosition(windowWidth / 2 - 100, windowHeight / 2);

	sf::Text textExit;
	textExit.setFont(font);
	textExit.setCharacterSize(20);
	textExit.setString("Exit game");
	textExit.setFillColor(sf::Color::White);
	textExit.setStyle(sf::Text::Bold);
	textExit.setPosition(buttonExit.getPosition().x + 60, buttonExit.getPosition().y + 10);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			Vector2i mouseHover = Mouse::getPosition(window);
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
				return; // Exit the dialog
			}

			if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					sf::Vector2i mousePos = sf::Mouse::getPosition(window);
					if (buttonExit.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
						window.close();
						return;
					}
					else if (buttonContinue.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
						click_sound.play();
						return;
					}
					else if (buttonInapoi.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
						click_sound.play();
						gameState = GameState::MENU;
						TablaInit(joc, cellSize, windowWidth, windowHeight);
						joc.Player.IsTurn = true;
						return;
					}
				}
			}
		}
		window.clear(joc.CuloareFundal);
		// Draw the current state of the game as the background
		for (int i = 0; i < tablaSize; ++i) {
			for (int j = 0; j < tablaSize; ++j) {
				window.draw(joc.tabla[i][j]);
			}
		}
		window.draw(joc.circle1);
		window.draw(joc.circle2);
		displayTura(joc.Player.IsTurn, window);
		window.draw(overlay);
		window.draw(titlu);
		window.draw(buttonInapoi);
		window.draw(textInapoi);
		window.draw(buttonContinue);
		window.draw(textContinue);
		window.draw(buttonExit);
		window.draw(textExit);
		window.display();
	}
}

void displayHelp(sf::RenderWindow& window, GameState& gameState) {
	Font font;
	if (font.loadFromFile("Candy Beans.otf") == 0) {
		cout << "Error loading font\n";
		return;
	}

	Text helpText;
	helpText.setFont(font);
	helpText.setCharacterSize(20);
	helpText.setString("In BonoL ai o tabla 4x4 cu 2 piese de forma L si doua piese neutre\n"
		"Pentru a castiga jocul, jucatorul trebuie sa mute piesele de forma L si cele neutre\n"
		"in asa natura ca jucatorul oponent sa nu aiba mutari posibile\n\n\n"
		"CONTROALE:\n"
		"Left Click: Desenare piesa L a Jucatorului 1\n"
		"Right Click: Desenare piesa L a Jucatorului 2\n"
		"M: Mutarea piesei neutre\n"
		"C: Selectare dintre cele doua piese neutre\n\n\n\n\n\n\n\n"
		"Joc creat de catre Ioniche Adrian si Bazon Bogdan");
	helpText.setFillColor(Color::White);
	helpText.setPosition(100, 150);


	Button helpButton;
	helpButton.hover = false;
	helpButton.shape.setSize(Vector2f(200, 50));
	helpButton.shape.setFillColor(Color::White);
	helpButton.shape.setPosition(windowWidth / 2 - 100, 650);

	helpButton.text.setFont(font);
	helpButton.text.setCharacterSize(20);
	helpButton.text.setString("Inapoi");
	helpButton.text.setFillColor(Color::Black);
	helpButton.text.setPosition(helpButton.shape.getPosition().x + (helpButton.shape.getSize().x - helpButton.text.getLocalBounds().width) / 2,
		helpButton.shape.getPosition().y + (helpButton.shape.getSize().y - helpButton.text.getLocalBounds().height) / 2
		- 5);

	while (window.isOpen() && gameState == GameState::HELP) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
				gameState = GameState::EXIT;
			}
			else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
				gameState = GameState::MENU;
			}

			Vector2f mousePosition = Vector2f(Mouse::getPosition(window));

			if (isMouseOverButton(helpButton, mousePosition)) {
				// Mouse is over the button
				helpButton.hover = true;
				helpButton.shape.setFillColor(Gray); // Change color or any other visual indication
			}
			else {
				// Mouse is not over the button
				helpButton.hover = false;
				helpButton.shape.setFillColor(Color::White); // Reset to default color
			}
			if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
			{
				Vector2i mousePos = Mouse::getPosition(window);
				if (helpButton.shape.getGlobalBounds().contains(mousePos.x, mousePos.y))
					gameState = GameState::MENU;
			}
		}

		window.clear(Color::Black);
		window.draw(helpText);
		window.draw(helpButton.shape);
		window.draw(helpButton.text);
		window.display();
	}
}

void handleMenuEvents(sf::RenderWindow& window, GameState& gameState, vector<Button>& buttons) {
	
	Event event;


	while (window.pollEvent(event)) {
		if (event.type == Event::Closed) {
			window.close();
			gameState = GameState::EXIT;
		}
		else if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
			Vector2i mousePos = Mouse::getPosition(window);
			for (Button& button : buttons) {
				if (button.shape.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
					
					if (button.text.getString() == "1v1") {
						gameState = GameState::PLAYERVPLAYER;
					}
					else if (button.text.getString() == "1vCPU") {
						gameState = GameState::PLAYERVCPU;
					}
					else if (button.text.getString() == "Help") {
						gameState = GameState::HELP;
					}
					else if (button.text.getString() == "Exit") {
						window.close();
						gameState = GameState::EXIT;
					}
				}
			}
		}
	}
}

void displayMenu(sf::RenderWindow& window, GameState& gameState, vector<Button>& buttons) {
	Font font;
	if (font.loadFromFile("Candy Beans.otf") == 0) {
		cout << "Error loading font\n";
		return;
	}
	Font fonttitlu;
	if (fonttitlu.loadFromFile("titilefont.TTF") == 0) {
		cout << "Error loading Title font\n";
		return;
	}
	Text title;
	title.setFont(fonttitlu);
	title.setCharacterSize(125);
	title.setString("BONOL");
	title.setFillColor(Color::White);
	title.setPosition(windowWidth / 2 - title.getLocalBounds().width / 2 + 10, 50);

	// Create "1V1" button
	Button start1Button;
	start1Button.hover = false;
	start1Button.shape.setSize(Vector2f(200, 50));
	start1Button.shape.setFillColor(Color::White);
	start1Button.shape.setPosition(windowWidth / 2 - 100, 250);

	start1Button.text.setFont(font);
	start1Button.text.setCharacterSize(20);
	start1Button.text.setString("1v1");
	start1Button.text.setFillColor(Color::Black);
	start1Button.text.setPosition(
		start1Button.shape.getPosition().x + (start1Button.shape.getSize().x - start1Button.text.getLocalBounds().width) / 2,
		start1Button.shape.getPosition().y + (start1Button.shape.getSize().y - start1Button.text.getLocalBounds().height) / 2
		- 5);

	// Create "1VCPU" button
	Button start2Button;
	start2Button.hover = false;
	start2Button.shape.setSize(Vector2f(200, 50));
	start2Button.shape.setFillColor(Color::White);
	start2Button.shape.setPosition(windowWidth / 2 - 100, 350);

	start2Button.text.setFont(font);
	start2Button.text.setCharacterSize(20);
	start2Button.text.setString("1vCPU");
	start2Button.text.setFillColor(Color::Black);
	start2Button.text.setPosition(start2Button.shape.getPosition().x + (start2Button.shape.getSize().x - start2Button.text.getLocalBounds().width) / 2,
		start2Button.shape.getPosition().y + (start2Button.shape.getSize().y - start2Button.text.getLocalBounds().height) / 2
		- 5);

	// Create "Help" button
	Button helpButton;
	helpButton.hover = false;
	helpButton.shape.setSize(Vector2f(200, 50));
	helpButton.shape.setFillColor(Color::White);
	helpButton.shape.setPosition(windowWidth / 2 - 100, 450);

	helpButton.text.setFont(font);
	helpButton.text.setCharacterSize(20);
	helpButton.text.setString("Help");
	helpButton.text.setFillColor(Color::Black);
	helpButton.text.setPosition(helpButton.shape.getPosition().x + (helpButton.shape.getSize().x - helpButton.text.getLocalBounds().width) / 2,
		helpButton.shape.getPosition().y + (helpButton.shape.getSize().y - helpButton.text.getLocalBounds().height) / 2
		- 5);

	// Create "Exit" button
	Button exitButton;
	exitButton.hover = false;
	exitButton.shape.setSize(Vector2f(200, 50));
	exitButton.shape.setFillColor(Color::White);
	exitButton.shape.setPosition(windowWidth / 2 - 100, 550);

	exitButton.text.setFont(font);
	exitButton.text.setCharacterSize(20);
	exitButton.text.setString("Exit");
	exitButton.text.setFillColor(Color::Black);
	exitButton.text.setPosition(exitButton.shape.getPosition().x + (exitButton.shape.getSize().x - exitButton.text.getLocalBounds().width) / 2,
		exitButton.shape.getPosition().y + (exitButton.shape.getSize().y - exitButton.text.getLocalBounds().height) / 2
		- 5);

	buttons = { start1Button, start2Button, helpButton, exitButton };

	
	while (window.isOpen() && gameState == GameState::MENU) {


		Vector2f mousePosition = Vector2f(Mouse::getPosition(window));

		for (Button& button : buttons) {
			if (isMouseOverButton(button, mousePosition)) {
				// Mouse is over the button
				button.hover = true;
				button.shape.setFillColor(Gray); // Change color or any other visual indication
			}
			else {
				// Mouse is not over the button
				button.hover = false;
				button.shape.setFillColor(Color::White); // Reset to default color
			}
		}


		handleMenuEvents(window, gameState, buttons);

		window.clear(Color::Black);
		window.draw(title);
		for (const Button& button : buttons) {
			window.draw(button.shape);
			window.draw(button.text);
		}
		window.display();
	}
}

int CountLShapes(GameContent joc, Color playercolor) {

	int count = 0;
	sf::RectangleShape ctabla[tablaSize][tablaSize];
	Copie(joc.tabla, ctabla);
	golire(ctabla, playercolor);

	for (int i = 0; i < tablaSize; i++)
		for (int j = 0; j < tablaSize; j++)
		{
			//L normal
			if (j + 1 < tablaSize && i + 2 < tablaSize)
				if ((ctabla[i][j].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i][j], joc.circle1, joc.circle2) == false) &&
					(ctabla[i + 1][j].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i + 1][j], joc.circle1, joc.circle2) == false) &&
					(ctabla[i + 2][j].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i + 2][j], joc.circle1, joc.circle2) == false) &&
					(ctabla[i + 2][j + 1].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i + 2][j + 1], joc.circle1, joc.circle2) == false))
					count++;
			//L reversed
			if (j - 1 >= 0 && i + 2 < tablaSize)
				if ((ctabla[i][j].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i][j], joc.circle1, joc.circle2) == false) &&
					(ctabla[i + 1][j].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i + 1][j], joc.circle1, joc.circle2) == false) &&
					(ctabla[i + 2][j].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i + 2][j], joc.circle1, joc.circle2) == false) &&
					(ctabla[i + 2][j - 1].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i + 2][j - 1], joc.circle1, joc.circle2) == false))
					count++;

			//Upside down L

			if (j + 1 < tablaSize && i + 2 < tablaSize)
				if ((ctabla[i][j].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i][j], joc.circle1, joc.circle2) == false) &&
					(ctabla[i + 1][j].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i + 1][j], joc.circle1, joc.circle2) == false) &&
					(ctabla[i + 2][j].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i + 2][j], joc.circle1, joc.circle2) == false) &&
					(ctabla[i][j + 1].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i][j + 1], joc.circle1, joc.circle2) == false))
					count++;

			//Upside Down reversed L
			if (j - 1 >= 0 && i + 2 < tablaSize)
				if ((ctabla[i][j].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i][j], joc.circle1, joc.circle2) == false) &&
					(ctabla[i + 1][j].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i + 1][j], joc.circle1, joc.circle2) == false) &&
					(ctabla[i + 2][j].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i + 2][j], joc.circle1, joc.circle2) == false) &&
					(ctabla[i][j - 1].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i][j - 1], joc.circle1, joc.circle2) == false))
					count++;
			//L pa spate
			if (j + 2 < tablaSize && i - 1 > 0)
				if ((ctabla[i][j].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i][j], joc.circle1, joc.circle2) == false) &&
					(ctabla[i][j + 1].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i][j + 1], joc.circle1, joc.circle2) == false) &&
					(ctabla[i][j + 2].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i][j + 2], joc.circle1, joc.circle2) == false) &&
					(ctabla[i - 1][j + 2].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i - 1][j + 2], joc.circle1, joc.circle2) == false))
					count++;
			//L pa spate upside down
			if (j + 2 < tablaSize && i + 1 < tablaSize)
				if ((ctabla[i][j].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i][j], joc.circle1, joc.circle2) == false) &&
					(ctabla[i][j + 1].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i][j + 1], joc.circle1, joc.circle2) == false) &&
					(ctabla[i][j + 2].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i][j + 2], joc.circle1, joc.circle2) == false) &&
					(ctabla[i + 1][j + 2].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i + 1][j + 2], joc.circle1, joc.circle2) == false))
					count++;
			//L pa spate reversed
			if (j + 2 < tablaSize && i - 1 >= 0)
				if ((ctabla[i][j].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i][j], joc.circle1, joc.circle2) == false) &&
					(ctabla[i][j + 1].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i][j + 1], joc.circle1, joc.circle2) == false) &&
					(ctabla[i][j + 2].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i][j + 2], joc.circle1, joc.circle2) == false) &&
					(ctabla[i - 1][j].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i - 1][j], joc.circle1, joc.circle2) == false))
					count++;
			//L pa spate reversed upside down
			if (j + 2 < tablaSize && i + 1 < tablaSize)
				if ((ctabla[i][j].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i][j], joc.circle1, joc.circle2) == false) &&
					(ctabla[i][j + 1].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i][j + 1], joc.circle1, joc.circle2) == false) &&
					(ctabla[i][j + 2].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i][j + 2], joc.circle1, joc.circle2) == false) &&
					(ctabla[i + 1][j].getFillColor() == Color::White && CelulaOcupata(joc.tabla[i + 1][j], joc.circle1, joc.circle2) == false))
					count++;
		}
	return count - 1;
}

void displayMutariPosibile(sf::RenderWindow& window) {
	sf::Font font;
	if (font.loadFromFile("Candy Beans.otf") == 0) {
		cout << "Error loading font\n";
		return;
	}

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(40);
	text.setFillColor(sf::Color::White);
	text.setString("Mutari posibile: ");

	text.setPosition(windowWidth / 2 - text.getLocalBounds().width / 2, 50);

	window.draw(text);
}

void displayWinner(sf::RenderWindow& window, GameContent& joc, GameState& gameState) {

	sf::RectangleShape overlay(sf::Vector2f(windowWidth, windowHeight));
	overlay.setFillColor(sf::Color(0, 0, 0, 200));

	sf::Font font;
	if (!font.loadFromFile("Candy Beans.otf")) {
		std::cerr << "Error loading font\n";
		return;
	}

	Text felicitari;
	felicitari.setFont(font);
	felicitari.setCharacterSize(60);
	felicitari.setPosition(windowWidth / 2 - felicitari.getLocalBounds().width / 2 - 150, windowHeight / 2 - felicitari.getLocalBounds().height / 2 - 450);
	felicitari.setString("FELICITARI!!");

	sf::Text titlu;
	titlu.setFont(font);
	titlu.setCharacterSize(60);
	if (joc.Player.IsTurn == true)
		titlu.setString("JUCATORUL 1 A CASTIGAT!");
	else
		titlu.setString("JUCATORUL 2 A CASTIGAT!");
	titlu.setPosition(windowWidth / 2 - titlu.getLocalBounds().width / 2, windowHeight / 2 - titlu.getLocalBounds().height / 2 - 350);

	titlu.setFillColor(sf::Color::White);
	titlu.setStyle(sf::Text::Bold);

	felicitari.setFillColor(sf::Color::White);
	felicitari.setStyle(sf::Text::Bold);


	//Back

	sf::RectangleShape buttonInapoi(sf::Vector2f(220, 50));
	buttonInapoi.setFillColor(sf::Color(128, 128, 128)); // Grey
	buttonInapoi.setPosition(windowWidth / 2 - 100, windowHeight / 2 - 100);

	sf::Text textInapoi;
	textInapoi.setFont(font);
	textInapoi.setCharacterSize(20);
	textInapoi.setString("Meniul principal");
	textInapoi.setFillColor(sf::Color::White);
	textInapoi.setStyle(sf::Text::Bold);
	textInapoi.setPosition(buttonInapoi.getPosition().x + 30, buttonInapoi.getPosition().y + 10);

	//EXIT//

	sf::RectangleShape buttonExit(sf::Vector2f(220, 50));
	buttonExit.setFillColor(sf::Color(196, 30, 58));//red
	buttonExit.setPosition(windowWidth / 2 - 100, windowHeight / 2);

	sf::Text textExit;
	textExit.setFont(font);
	textExit.setCharacterSize(20);
	textExit.setString("Exit game");
	textExit.setFillColor(sf::Color::White);
	textExit.setStyle(sf::Text::Bold);
	textExit.setPosition(buttonExit.getPosition().x + 60, buttonExit.getPosition().y + 10);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					sf::Vector2i mousePos = sf::Mouse::getPosition(window);
					if (buttonExit.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
						window.close();
						return;
					}
					else if (buttonInapoi.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
						gameState = GameState::MENU;
						return;
					}
				}
			}
		}
		window.clear(joc.CuloareFundal);
		// Draw the current state of the game as the background
		for (int i = 0; i < tablaSize; ++i) {
			for (int j = 0; j < tablaSize; ++j) {
				window.draw(joc.tabla[i][j]);
			}
		}
		window.draw(joc.circle1);
		window.draw(joc.circle2);
		window.draw(overlay);
		window.draw(felicitari);
		window.draw(titlu);
		window.draw(buttonInapoi);
		window.draw(textInapoi);
		window.draw(buttonExit);
		window.draw(textExit);
		window.display();
	}
}

void RandomCPU(GameContent& joc) {

	int randRow, randCol, k = 0;
	while (k < 4) {
		do {
			randRow = rand() % tablaSize;
			randCol = rand() % tablaSize;
		} while (!CelulaGoala(joc.tabla[randRow][randCol], sf::Color::White));
		if (CelulaOcupata(joc.tabla[randRow][randCol], joc.circle1, joc.circle2) == false) {
			joc.tabla[randRow][randCol].setFillColor(Color::Blue);
			k++;
		}

	}
}

void RandomCPUNeutru(GameContent& joc) {

	int randRow, randCol, cerc;
	cerc = rand() % 2;
	do {
		randRow = rand() % tablaSize;
		randCol = rand() % tablaSize;

	} while (!CelulaGoala(joc.tabla[randRow][randCol], sf::Color::White));
	if (CelulaOcupata(joc.tabla[randRow][randCol], joc.circle1, joc.circle2) == false) {
		if (cerc == 1)
			joc.circle1.setPosition(joc.tabla[randRow][randCol].getPosition().x + cellSize.x / 2 - joc.circle1.getRadius(),
				joc.tabla[randRow][randCol].getPosition().y + cellSize.y / 2 - joc.circle1.getRadius());
		if (cerc == 0)
			joc.circle2.setPosition(joc.tabla[randRow][randCol].getPosition().x + cellSize.x / 2 - joc.circle2.getRadius(),
				joc.tabla[randRow][randCol].getPosition().y + cellSize.y / 2 - joc.circle2.getRadius());

	}
}

void MoveCPUstrategie(GameContent& joc) {
	golire(joc.tabla, Color::Blue);
	RandomCPU(joc);
	while (ValidMove(joc.tabla, Color::Blue, joc.undo) == false)
	{
		golire(joc.tabla, Color::Blue);
		RandomCPU(joc);
	}
	if (rand() % 2 == 0)
		RandomCPUNeutru(joc);
}

void Game() {
	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "BonoL", sf::Style::Titlebar | sf::Style::Close);
	sf::RectangleShape undoButton(sf::Vector2f(100, 50)); // Adjust size as needed
	sf::RectangleShape skipButton(sf::Vector2f(100, 50)); // Adjust size as needed
	undoButton.setPosition(10, 10); // Adjust position as needed
	skipButton.setPosition(120, 10); // Adjust position as needed
	undoButton.setFillColor(sf::Color::Green); // Adjust color as needed
	skipButton.setFillColor(sf::Color::Yellow); // Adjust color as needed
	GameContent joc;
	SoundBuffer buffer;
	SoundBuffer buffer_gameplay;
	SoundBuffer buffer_winner;

	if (!buffer.loadFromFile("meniu.ogg"))
		cout << "ERROR";
	if (!buffer_gameplay.loadFromFile("music.ogg"))
		cout << "ERROR";
	if (!buffer_winner.loadFromFile("winner.ogg"))
		cout << "ERROR";
	Sound sunet_meniu;
	Sound sunet_game;
	Sound sunet_winner;

	sunet_meniu.setBuffer(buffer);
	sunet_game.setBuffer(buffer_gameplay);
	sunet_winner.setBuffer(buffer_winner);
	joc.tabla;
	joc.undo;
	joc.circle1;
	joc.circle2;
	joc.circle1.setFillColor(sf::Color::Black);
	joc.circle2.setFillColor(sf::Color::Black);
	TablaInit(joc, cellSize, windowWidth, windowHeight);
	Copie(joc.tabla, joc.undo);
	displayTura(joc.Player.IsTurn, window);

	GameState gameState = GameState::MENU;
	vector<Button> buttons;

	sunet_meniu.setVolume(5);
	sunet_game.setVolume(3);
	sunet_winner.setVolume(50);

	// Inputs
	while (window.isOpen()) {
		switch (gameState) {
		case GameState::MENU:
			if (sunet_meniu.getStatus() != Sound::Playing) {
				sunet_meniu.play();
				sunet_game.stop();
				sunet_winner.stop();
			}
				
			displayMenu(window, gameState, buttons);
			break;
		case GameState::PLAYERVPLAYER:
			if (sunet_game.getStatus() != Sound::Playing) {
				sunet_game.play();
				sunet_meniu.stop();
				sunet_winner.stop();
			}
			sf::Event event;
			while (window.pollEvent(event)) {
				if (event.type == Event::Closed)
					window.close();

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
					sunet_game.setVolume(1);
					displayPause(window, joc, gameState);
					sunet_game.setVolume(5);
				}

				// Avem grija de inputs (mouse,tastatura etc)
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
					TablaInit(joc, cellSize, windowWidth, windowHeight);
					Copie(joc.tabla, joc.undo);
					joc.Player.IsTurn = true;
				}


				////////////
				//DESENARE//
				////////////

				if (event.type == sf::Event::MouseButtonPressed) {
					if (event.mouseButton.button == sf::Mouse::Left) {
						joc.AmColorat.red = 0;
						if (joc.Player.IsTurn == true) {
							Vector2i click = sf::Mouse::getPosition(window);
							for (int i = 0; i < tablaSize; ++i)
								for (int j = 0; j < tablaSize; ++j)
									if (joc.tabla[i][j].getGlobalBounds().contains(click.x, click.y))
										if (CelulaGoala(joc.tabla[i][j], Color::White) || CelulaGoala(joc.tabla[i][j], Color::Red))
											if (!CelulaOcupata(joc.tabla[i][j], joc.circle1, joc.circle2))
												if (!CelulaGoala(joc.tabla[i][j], Color::Blue))
													golire(joc.tabla, sf::Color::Red);
						}
						while (Mouse::isButtonPressed(sf::Mouse::Left) && joc.Player.IsTurn == true) {
							sf::Vector2i clickPos = sf::Mouse::getPosition(window);
							for (int i = 0; i < tablaSize; ++i)
								for (int j = 0; j < tablaSize; ++j)
									if (joc.tabla[i][j].getGlobalBounds().contains(clickPos.x, clickPos.y))
										if (CelulaGoala(joc.tabla[i][j], sf::Color::White) && !CelulaOcupata(joc.tabla[i][j], joc.circle1, joc.circle2))
										{
											joc.AmColorat.red = 1;
											joc.tabla[i][j].setFillColor(sf::Color::Red);
										}
							window.clear(joc.CuloareFundal);
							// Desenam pe ecran noua tabla
							for (int i = 0; i < tablaSize; ++i)
								for (int j = 0; j < tablaSize; ++j)
									window.draw(joc.tabla[i][j]);
							window.draw(joc.circle1);
							window.draw(joc.circle2);
							displayTura(joc.Player.IsTurn, window);
							window.display();

						}
					}
					if (event.mouseButton.button == sf::Mouse::Right) {
						joc.AmColorat.blue = 0;
						if (joc.Player.IsTurn == false)
						{
							sf::Vector2i click = sf::Mouse::getPosition(window);
							for (int i = 0; i < tablaSize; ++i)
								for (int j = 0; j < tablaSize; ++j)
									if (joc.tabla[i][j].getGlobalBounds().contains(click.x, click.y))
										if (CelulaGoala(joc.tabla[i][j], Color::White) || CelulaGoala(joc.tabla[i][j], Color::Blue))
											if (!CelulaOcupata(joc.tabla[i][j], joc.circle1, joc.circle2))
												if (!CelulaGoala(joc.tabla[i][j], Color::Red))
													golire(joc.tabla, sf::Color::Blue);
						}
						while (Mouse::isButtonPressed(sf::Mouse::Right) && joc.Player.IsTurn == false) {
							sf::Vector2i clickPos = sf::Mouse::getPosition(window);
							//Verificam daca cursorul este in vreo celula
							for (int i = 0; i < tablaSize; ++i)
								for (int j = 0; j < tablaSize; ++j)
									if (joc.tabla[i][j].getGlobalBounds().contains(clickPos.x, clickPos.y))
										if (CelulaGoala(joc.tabla[i][j], sf::Color::White) && !CelulaOcupata(joc.tabla[i][j], joc.circle1, joc.circle2))
										{
											joc.AmColorat.blue = 1;
											joc.tabla[i][j].setFillColor(sf::Color::Blue);
										}
							window.clear(joc.CuloareFundal);
							// Desenam noua tabla
							for (int i = 0; i < tablaSize; ++i)
								for (int j = 0; j < tablaSize; ++j)
									window.draw(joc.tabla[i][j]);
							window.draw(joc.circle1);
							displayTura(joc.Player.IsTurn, window);
							window.draw(joc.circle2);
							window.display();

						}
					}
				}

				/////////////////////////////////////////////
				//VERIFICARE MISCARE / MISCARE PIESA NEUTRA//
				/////////////////////////////////////////////


				if (event.type == sf::Event::MouseButtonReleased) {
					if (event.mouseButton.button == Mouse::Left && joc.Player.IsTurn == true) {
						if (ValidMove(joc.tabla, sf::Color::Red, joc.undo) == true && joc.AmColorat.red == 1) {

							bool decizie = false;
							while (decizie == false)
							{
								Button skipButton;
								Button undoButton;

								if (!click_buffer.loadFromFile("pop.ogg"))
									cout << "error";
								click_sound.setBuffer(click_buffer);

								Font font;
								if (font.loadFromFile("Candy Beans.otf") == 0) {
									cout << "Error loading font\n";
									return;
								}


								//BUTOANE////////////////////////////////////////////////////////////////////////////////////////////



								//Skip Button

								skipButton.hover = false;
								skipButton.shape.setFillColor(Color::White);
								skipButton.shape.setSize(sf::Vector2f(100, 50));
								skipButton.shape.setPosition(50, 380);

								skipButton.text.setFont(font);
								skipButton.text.setFillColor(Color::Black);
								skipButton.text.setString("SKIP");
								skipButton.text.setPosition(
									skipButton.shape.getPosition().x + (skipButton.shape.getSize().x - skipButton.text.getLocalBounds().width) / 2,
									skipButton.shape.getPosition().y + (skipButton.shape.getSize().y - skipButton.text.getLocalBounds().height) / 2
									- 5);
								//Undo Button
								undoButton.hover = false;
								undoButton.shape.setFillColor(Color::White);
								undoButton.shape.setSize(sf::Vector2f(100, 50));
								undoButton.shape.setPosition(50, 460);

								undoButton.text.setFont(font);
								undoButton.text.setFillColor(Color::Black);
								undoButton.text.setString("UNDO");
								undoButton.text.setPosition(
									undoButton.shape.getPosition().x + (undoButton.shape.getSize().x - undoButton.text.getLocalBounds().width) / 2,
									undoButton.shape.getPosition().y + (undoButton.shape.getSize().y - undoButton.text.getLocalBounds().height) / 2
									- 5);

								/////////////////////////////////////////////////////////////////////////////////////////////////////

								Event eventButton;
								while (window.pollEvent(eventButton) && decizie == false) {
									if (eventButton.type == Event::Closed)
										window.close();
									if (eventButton.type == Event::MouseButtonPressed)
										if (eventButton.mouseButton.button == sf::Mouse::Left)
										{
											Vector2i mousePos = Mouse::getPosition(window);
											if (skipButton.shape.getGlobalBounds().contains(mousePos.x, mousePos.y))
											{
												click_sound.play();
												joc.Player.IsTurn = !joc.Player.IsTurn;
												decizie = true;
												joc.circle1.setFillColor(Color::Black);
												joc.circle2.setFillColor(Color::Black);
											}
											if (undoButton.shape.getGlobalBounds().contains(mousePos.x, mousePos.y))
											{
												click_sound.play();
												UndoMove(joc.tabla, joc.undo);
												decizie = true;

												joc.circle1.setFillColor(Color::Black);
												joc.circle2.setFillColor(Color::Black);
											}
										}

									if (eventButton.type == Event::KeyPressed)
									{
										if (eventButton.key.code == Keyboard::Escape)
										{
											sunet_game.setVolume(1);
											displayPause(window, joc, gameState);
											sunet_game.setVolume(5);
										}
										if (eventButton.key.code == Keyboard::C)
										{
											joc.isCircle1Selected = !joc.isCircle1Selected;
											if (joc.isCircle1Selected == 1)
											{
												joc.circle1.setFillColor(Color::Yellow);
												joc.circle2.setFillColor(Color::Black);
												window.display();
											}
											if (joc.isCircle1Selected == 0)
											{
												joc.circle2.setFillColor(Color::Yellow);
												joc.circle1.setFillColor(Color::Black);
												window.display();
											}
										}
										if (eventButton.key.code == Keyboard::M)
										{
											Vector2i mousePos = Mouse::getPosition(window);
											//verificam daca este vreun cerc este selectat
											if (joc.circle1.getFillColor() == Color::Black && joc.circle2.getFillColor() == Color::Black)
											{
												sf::Text text;
												text.setFont(font);
												text.setCharacterSize(40);
												text.setFillColor(sf::Color::Red);
												text.setString("Nu ati selectat nicio piesa neutra");

												text.setPosition(windowWidth / 2 - text.getLocalBounds().width / 2, 90);

												window.draw(text);
												window.display();
												sleep(seconds(2));
											}
											else
											{
												for (int i = 0; i < tablaSize; ++i)
													for (int j = 0; j < tablaSize; ++j)
														if (joc.tabla[i][j].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
															if (!CelulaOcupata(joc.tabla[i][j], joc.circle1, joc.circle2))
																if (joc.tabla[i][j].getFillColor() == Color::White) {
																	if (joc.isCircle1Selected == 1) {
																		joc.circle1.setPosition(joc.tabla[i][j].getPosition().x + cellSize.x / 2 - joc.circle1.getRadius(),
																			joc.tabla[i][j].getPosition().y + cellSize.y / 2 - joc.circle1.getRadius());
																		joc.circle1.setFillColor(Color::Black);
																		decizie = true;
																		joc.Player.IsTurn = !joc.Player.IsTurn;
																	}

																	else {
																		joc.circle2.setPosition(joc.tabla[i][j].getPosition().x + cellSize.x / 2 - joc.circle2.getRadius(),
																			joc.tabla[i][j].getPosition().y + cellSize.y / 2 - joc.circle2.getRadius());
																		joc.circle2.setFillColor(Color::Black);
																		decizie = true;
																		joc.Player.IsTurn = !joc.Player.IsTurn;
																	}
																}
														}
											}
										}
									}
								}

								window.clear(joc.CuloareFundal);

								// Desenam tabla
								for (int i = 0; i < tablaSize; ++i) {
									for (int j = 0; j < tablaSize; ++j) {
										window.draw(joc.tabla[i][j]);
									}
								}
								window.draw(joc.circle1);
								window.draw(joc.circle2);
								displayTura(joc.Player.IsTurn, window);
								window.draw(skipButton.shape);
								window.draw(skipButton.text);
								window.draw(undoButton.shape);
								window.draw(undoButton.text);
								window.display();
							}

							Copie(joc.tabla, joc.undo);
							joc.AmColorat.red = 0;
							if (CountLShapes(joc, Color::Blue) == 0) {
								joc.Player.IsTurn = true;
								if (sunet_game.getStatus() == Sound::Playing)
								{
									sunet_game.stop();
									sunet_winner.play();
								}
								displayWinner(window, joc, gameState);
								TablaInit(joc, cellSize, windowWidth, windowHeight);
							}
						}
						else if (joc.AmColorat.red != 0) {
							displayInvalidMove(window);
							UndoMove(joc.tabla, joc.undo);
						}
					}
					if (event.mouseButton.button == Mouse::Right && joc.Player.IsTurn == false) {

						//Wininng condition
						/*displayMutariPosibile(window, CountLShapes(joc, Color::Blue));*/
						if (ValidMove(joc.tabla, sf::Color::Blue, joc.undo) == true) {

							bool decizie = false;
							while (decizie == false)
							{
								Button skipButton;
								Button undoButton;

								if (!click_buffer.loadFromFile("pop.ogg"))
									cout << "error";
								click_sound.setBuffer(click_buffer);

								Font font;
								if (font.loadFromFile("Candy Beans.otf") == 0) {
									cout << "Error loading font\n";
									return;
								}


								//BUTOANE////////////////////////////////////////////////////////////////////////////////////////////



								//Skip Button

								skipButton.hover = false;
								skipButton.shape.setFillColor(Color::White);
								skipButton.shape.setSize(sf::Vector2f(100, 50));
								skipButton.shape.setPosition(50, 380);

								skipButton.text.setFont(font);
								skipButton.text.setFillColor(Color::Black);
								skipButton.text.setString("SKIP");
								skipButton.text.setPosition(
									skipButton.shape.getPosition().x + (skipButton.shape.getSize().x - skipButton.text.getLocalBounds().width) / 2,
									skipButton.shape.getPosition().y + (skipButton.shape.getSize().y - skipButton.text.getLocalBounds().height) / 2
									- 5);
								//Undo Button
								undoButton.hover = false;
								undoButton.shape.setFillColor(Color::White);
								undoButton.shape.setSize(sf::Vector2f(100, 50));
								undoButton.shape.setPosition(50, 460);

								undoButton.text.setFont(font);
								undoButton.text.setFillColor(Color::Black);
								undoButton.text.setString("UNDO");
								undoButton.text.setPosition(
									undoButton.shape.getPosition().x + (undoButton.shape.getSize().x - undoButton.text.getLocalBounds().width) / 2,
									undoButton.shape.getPosition().y + (undoButton.shape.getSize().y - undoButton.text.getLocalBounds().height) / 2
									- 5);

								/////////////////////////////////////////////////////////////////////////////////////////////////////

								Event eventButton;
								while (window.pollEvent(eventButton) && decizie == false) {
									if (eventButton.type == Event::Closed)
										window.close();
									if (eventButton.type == Event::MouseButtonPressed)
										if (eventButton.mouseButton.button == sf::Mouse::Left)
										{
											Vector2i mousePos = Mouse::getPosition(window);
											if (skipButton.shape.getGlobalBounds().contains(mousePos.x, mousePos.y))
											{
												click_sound.play();
												joc.Player.IsTurn = !joc.Player.IsTurn;
												decizie = true;
												joc.circle1.setFillColor(Color::Black);
												joc.circle2.setFillColor(Color::Black);
											}
											if (undoButton.shape.getGlobalBounds().contains(mousePos.x, mousePos.y))
											{
												click_sound.play();
												UndoMove(joc.tabla, joc.undo);
												decizie = true;
												joc.circle1.setFillColor(Color::Black);
												joc.circle2.setFillColor(Color::Black);
											}
										}

									if (eventButton.type == Event::KeyPressed)
									{
										if (eventButton.key.code == Keyboard::Escape)
										{
											sunet_game.setVolume(1);
											displayPause(window, joc, gameState);
											sunet_game.setVolume(5);
										}
										if (eventButton.key.code == Keyboard::C)
										{
											joc.isCircle1Selected = !joc.isCircle1Selected;
											if (joc.isCircle1Selected == 1)
											{
												joc.circle1.setFillColor(Color::Yellow);
												joc.circle2.setFillColor(Color::Black);
												window.display();
											}
											if (joc.isCircle1Selected == 0)
											{
												joc.circle2.setFillColor(Color::Yellow);
												joc.circle1.setFillColor(Color::Black);
												window.display();
											}
										}
										if (eventButton.key.code == Keyboard::M)
										{
											Vector2i mousePos = Mouse::getPosition(window);
											//verificam daca este vreun cerc este selectat
											if (joc.circle1.getFillColor() == Color::Black && joc.circle2.getFillColor() == Color::Black)
											{
												sf::Text text;
												text.setFont(font);
												text.setCharacterSize(40);
												text.setFillColor(sf::Color::Red);
												text.setString("Nu ati selectat nicio piesa neutra");

												text.setPosition(windowWidth / 2 - text.getLocalBounds().width / 2, 90);

												window.draw(text);
												window.display();
												sleep(seconds(2));
											}
											else
											{
												for (int i = 0; i < tablaSize; ++i)
													for (int j = 0; j < tablaSize; ++j)
														if (joc.tabla[i][j].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
															if (!CelulaOcupata(joc.tabla[i][j], joc.circle1, joc.circle2))
																if (joc.tabla[i][j].getFillColor() == Color::White) {
																	if (joc.isCircle1Selected == 1) {
																		joc.circle1.setPosition(joc.tabla[i][j].getPosition().x + cellSize.x / 2 - joc.circle1.getRadius(),
																			joc.tabla[i][j].getPosition().y + cellSize.y / 2 - joc.circle1.getRadius());
																		joc.circle1.setFillColor(Color::Black);
																		decizie = true;
																		joc.Player.IsTurn = !joc.Player.IsTurn;
																	}

																	else {
																		joc.circle2.setPosition(joc.tabla[i][j].getPosition().x + cellSize.x / 2 - joc.circle2.getRadius(),
																			joc.tabla[i][j].getPosition().y + cellSize.y / 2 - joc.circle2.getRadius());
																		joc.circle2.setFillColor(Color::Black);
																		decizie = true;
																		joc.Player.IsTurn = !joc.Player.IsTurn;
																	}
																}
														}
											}
										}
									}
								}

								window.clear(joc.CuloareFundal);

								// Desenam tabla
								for (int i = 0; i < tablaSize; ++i) {
									for (int j = 0; j < tablaSize; ++j) {
										window.draw(joc.tabla[i][j]);
									}
								}
								window.draw(joc.circle1);
								window.draw(joc.circle2);
								displayTura(joc.Player.IsTurn, window);
								window.draw(skipButton.shape);
								window.draw(skipButton.text);
								window.draw(undoButton.shape);
								window.draw(undoButton.text);
								window.display();
							}

							Copie(joc.tabla, joc.undo);
							if (CountLShapes(joc, Color::Red) == 0) {
								joc.Player.IsTurn = false;
								if (sunet_game.getStatus() == Sound::Playing)
								{
									sunet_game.stop();
									sunet_winner.play();
								}
								displayWinner(window, joc, gameState);
								TablaInit(joc, cellSize, windowWidth, windowHeight);
							}
						}
						else {
							displayInvalidMove(window);
							UndoMove(joc.tabla, joc.undo);
						}
					}
				}

			}

			window.clear(joc.CuloareFundal);

			// Desenam tabla
			for (int i = 0; i < tablaSize; ++i) {
				for (int j = 0; j < tablaSize; ++j) {
					window.draw(joc.tabla[i][j]);
				}
			}
			window.draw(joc.circle1);
			window.draw(joc.circle2);
			displayTura(joc.Player.IsTurn, window);
			/*displayMutariPosibile(window);*/
			window.display();

			break;
		case GameState::PLAYERVCPU:
			if (sunet_game.getStatus() != Sound::Playing) {
				sunet_game.play();
				sunet_meniu.stop();
				sunet_winner.stop();
			}
			sf::Event eventCPU;
			joc.isgameover = 0;
			while (window.pollEvent(eventCPU)) {
				if (eventCPU.type == sf::Event::Closed)
					window.close();
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				{
					sunet_game.setVolume(1);
					displayPause(window, joc, gameState);
					sunet_game.setVolume(5);
				}

				// Avem grija de inputs (mouse,tastatura etc)
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
					TablaInit(joc, cellSize, windowWidth, windowHeight);
					Copie(joc.tabla, joc.undo);
					joc.Player.IsTurn = true;
				}


				////////////
				//DESENARE//
				////////////

				if (eventCPU.type == sf::Event::MouseButtonPressed) {
					if (eventCPU.mouseButton.button == sf::Mouse::Left) {
						joc.AmColorat.red = 0;
						if (joc.Player.IsTurn == true) {
							Vector2i click = sf::Mouse::getPosition(window);
							for (int i = 0; i < tablaSize; ++i)
								for (int j = 0; j < tablaSize; ++j)
									if (joc.tabla[i][j].getGlobalBounds().contains(click.x, click.y))
										if (CelulaGoala(joc.tabla[i][j], Color::White) || CelulaGoala(joc.tabla[i][j], Color::Red))
											if (!CelulaOcupata(joc.tabla[i][j], joc.circle1, joc.circle2))
												if (!CelulaGoala(joc.tabla[i][j], Color::Blue))
													golire(joc.tabla, sf::Color::Red);
						}
						while (Mouse::isButtonPressed(sf::Mouse::Left) && joc.Player.IsTurn == true) {
							sf::Vector2i clickPos = sf::Mouse::getPosition(window);
							for (int i = 0; i < tablaSize; ++i)
								for (int j = 0; j < tablaSize; ++j)
									if (joc.tabla[i][j].getGlobalBounds().contains(clickPos.x, clickPos.y))
										if (CelulaGoala(joc.tabla[i][j], sf::Color::White) && !CelulaOcupata(joc.tabla[i][j], joc.circle1, joc.circle2))
										{
											joc.AmColorat.red = 1;
											joc.tabla[i][j].setFillColor(sf::Color::Red);
										}
							window.clear(joc.CuloareFundal);
							// Desenam pe ecran noua tabla
							for (int i = 0; i < tablaSize; ++i)
								for (int j = 0; j < tablaSize; ++j)
									window.draw(joc.tabla[i][j]);
							window.draw(joc.circle1);
							window.draw(joc.circle2);
							displayTura(joc.Player.IsTurn, window);
							window.display();

						}
					}
				}

				/////////////////////////////////////////////
				//VERIFICARE MISCARE / MISCARE PIESA NEUTRA//
				/////////////////////////////////////////////


				if (eventCPU.type == sf::Event::MouseButtonReleased) {
					if (eventCPU.mouseButton.button == Mouse::Left && joc.Player.IsTurn == true) {
						if (ValidMove(joc.tabla, sf::Color::Red, joc.undo) == true && joc.AmColorat.red == 1) {

							bool decizie = false;
							while (decizie == false)
							{
								Button skipButton;
								Button undoButton;

								if (!click_buffer.loadFromFile("pop.ogg"))
									cout << "error";
								click_sound.setBuffer(click_buffer);

								Font font;
								if (font.loadFromFile("Candy Beans.otf") == 0) {
									cout << "Error loading font\n";
									return;
								}

								//BUTOANE////////////////////////////////////////////////////////////////////////////////////////////

								//Skip Button

								skipButton.hover = false;
								skipButton.shape.setFillColor(Color::White);
								skipButton.shape.setSize(sf::Vector2f(100, 50));
								skipButton.shape.setPosition(50, 380);

								skipButton.text.setFont(font);
								skipButton.text.setFillColor(Color::Black);
								skipButton.text.setString("SKIP");
								skipButton.text.setPosition(
									skipButton.shape.getPosition().x + (skipButton.shape.getSize().x - skipButton.text.getLocalBounds().width) / 2,
									skipButton.shape.getPosition().y + (skipButton.shape.getSize().y - skipButton.text.getLocalBounds().height) / 2
									- 5);
								//Undo Button
								undoButton.hover = false;
								undoButton.shape.setFillColor(Color::White);
								undoButton.shape.setSize(sf::Vector2f(100, 50));
								undoButton.shape.setPosition(50, 460);

								undoButton.text.setFont(font);
								undoButton.text.setFillColor(Color::Black);
								undoButton.text.setString("UNDO");
								undoButton.text.setPosition(
									undoButton.shape.getPosition().x + (undoButton.shape.getSize().x - undoButton.text.getLocalBounds().width) / 2,
									undoButton.shape.getPosition().y + (undoButton.shape.getSize().y - undoButton.text.getLocalBounds().height) / 2
									- 5);

								/////////////////////////////////////////////////////////////////////////////////////////////////////

								Event eventButton;
								while (window.pollEvent(eventButton) && decizie == false) {
									if (eventButton.type == Event::Closed)
										window.close();
									if (eventButton.type == Event::MouseButtonPressed)
										if (eventButton.mouseButton.button == sf::Mouse::Left)
										{
											Vector2i mousePos = Mouse::getPosition(window);
											if (skipButton.shape.getGlobalBounds().contains(mousePos.x, mousePos.y))
											{
												click_sound.play();
												joc.Player.IsTurn = !joc.Player.IsTurn;
												decizie = true;
												joc.circle1.setFillColor(Color::Black);
												joc.circle2.setFillColor(Color::Black);
											}
											if (undoButton.shape.getGlobalBounds().contains(mousePos.x, mousePos.y))
											{
												click_sound.play();
												UndoMove(joc.tabla, joc.undo);
												decizie = true;

												joc.circle1.setFillColor(Color::Black);
												joc.circle2.setFillColor(Color::Black);
											}
										}

									if (eventButton.type == Event::KeyPressed)
									{
										if (eventButton.key.code == Keyboard::Escape)
										{
											sunet_game.setVolume(1);
											displayPause(window, joc, gameState);
											sunet_game.setVolume(5);
										}
										if (eventButton.key.code == Keyboard::C)
										{
											joc.isCircle1Selected = !joc.isCircle1Selected;
											if (joc.isCircle1Selected == 1)
											{
												joc.circle1.setFillColor(Color::Yellow);
												joc.circle2.setFillColor(Color::Black);
												window.display();
											}
											if (joc.isCircle1Selected == 0)
											{
												joc.circle2.setFillColor(Color::Yellow);
												joc.circle1.setFillColor(Color::Black);
												window.display();
											}
										}
										if (eventButton.key.code == Keyboard::M)
										{
											Vector2i mousePos = Mouse::getPosition(window);
											//verificam daca este vreun cerc este selectat
											if (joc.circle1.getFillColor() == Color::Black && joc.circle2.getFillColor() == Color::Black)
											{
												sf::Text text;
												text.setFont(font);
												text.setCharacterSize(40);
												text.setFillColor(sf::Color::Red);
												text.setString("Nu ati selectat nicio piesa neutra");

												text.setPosition(windowWidth / 2 - text.getLocalBounds().width / 2, 90);

												window.draw(text);
												window.display();
												sleep(seconds(1.5));
											}
											else
											{
												for (int i = 0; i < tablaSize; ++i)
													for (int j = 0; j < tablaSize; ++j)
														if (joc.tabla[i][j].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
															if (!CelulaOcupata(joc.tabla[i][j], joc.circle1, joc.circle2))
																if (joc.tabla[i][j].getFillColor() == Color::White) {
																	if (joc.isCircle1Selected == 1) {
																		joc.circle1.setPosition(joc.tabla[i][j].getPosition().x + cellSize.x / 2 - joc.circle1.getRadius(),
																			joc.tabla[i][j].getPosition().y + cellSize.y / 2 - joc.circle1.getRadius());
																		joc.circle1.setFillColor(Color::Black);
																		decizie = true;
																		joc.Player.IsTurn = !joc.Player.IsTurn;
																	}

																	else {
																		joc.circle2.setPosition(joc.tabla[i][j].getPosition().x + cellSize.x / 2 - joc.circle2.getRadius(),
																			joc.tabla[i][j].getPosition().y + cellSize.y / 2 - joc.circle2.getRadius());
																		joc.circle2.setFillColor(Color::Black);
																		decizie = true;
																		joc.Player.IsTurn = !joc.Player.IsTurn;
																	}
																}
														}
											}
										}
									}
								}

								window.clear(joc.CuloareFundal);

								// Desenam tabla
								for (int i = 0; i < tablaSize; ++i) {
									for (int j = 0; j < tablaSize; ++j) {
										window.draw(joc.tabla[i][j]);
									}
								}
								window.draw(joc.circle1);
								window.draw(joc.circle2);
								displayTura(joc.Player.IsTurn, window);
								window.draw(skipButton.shape);
								window.draw(skipButton.text);
								window.draw(undoButton.shape);
								window.draw(undoButton.text);
								window.display();
							}

							Copie(joc.tabla, joc.undo);
							joc.AmColorat.red = 0;
							if (CountLShapes(joc, Color::Blue) == 0) {
								joc.isgameover = 1;
								joc.Player.IsTurn = true;
								if (sunet_game.getStatus() == Sound::Playing)
								{
									sunet_game.stop();
									sunet_winner.play();
								}
								displayWinner(window, joc, gameState);
								TablaInit(joc, cellSize, windowWidth, windowHeight);
							}
						}
						else if (joc.AmColorat.red != 0) {
							displayInvalidMove(window);
							UndoMove(joc.tabla, joc.undo);
						}
						if (joc.isgameover == 0 && joc.Player.IsTurn == false) {
							MoveCPUstrategie(joc);
							Copie(joc.tabla, joc.undo);
							joc.Player.IsTurn = !joc.Player.IsTurn;
						}
					}
				}

			}

			window.clear(joc.CuloareFundal);

			// Desenam tabla
			for (int i = 0; i < tablaSize; ++i) {
				for (int j = 0; j < tablaSize; ++j) {
					window.draw(joc.tabla[i][j]);
				}
			}
			window.draw(joc.circle1);
			window.draw(joc.circle2);
			displayTura(joc.Player.IsTurn, window);
			/*displayMutariPosibile(window);*/
			window.display();

			break;
		case GameState::HELP:
			displayHelp(window, gameState);
			break;
		case GameState::EXIT:
			break;
		}
	}

}

int main() {
	Game();
	cout << "Va pup made by Beleua :D ";
	return 0;
}
