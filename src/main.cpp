#include<SFML/graphics.hpp>
#include<TGUI/TGUI.hpp>
#include<string>
#include<vector>
#include<random>

float random(int min, int max)// this function is use for finding random value for genreatig food
{
	return (float)min + rand() % (max - min);
}

class Game
{
private:
	sf::RenderWindow* window;
	tgui::GuiSFML gui;
	enum GameState {
		MENU,
		PLAY,
		SETTING,
		ABOUTUS,
		EXIT,
		PAUSE,
		RESULT
	} gameState;
	enum SnakeState {
		LEFT,
		RIGHT,
		UP,
		DOWN,
		OUT
	} playerState;
	
	tgui::ButtonRenderer buttonStyle;
	//tgui::Font font;
	double deltaTime;
	sf::Clock clock;
	std::vector<sf::Vector2i> playerPos;
	//for closing the window
	void Close()
	{
		//playerPos.clear();
		gui.removeAllWidgets();
		window->close();
	}
public:

	//Start the game from menu
	void Start()
	{

		//Used Variables
		bool init = true;//Variable for checking to initialize the game State
		sf::Event events;//for storing sfml events
		sf::RectangleShape playerRect(sf::Vector2f(20, 20));
		bool gameStarted = false;
		int score = 0;
		std::stringstream scoreString;
		sf::RectangleShape playbackrect(sf::Vector2f(580,300));
		tgui::BitmapButton::Ptr close = tgui::BitmapButton::create();//close button
		sf::RectangleShape food(sf::Vector2f(20.0f,20.0f));
		float playerVelocity = 9.0f;
		float playerAcceleration = 0.2f;
		float sumofVel = 0.0f;
		bool move;//player has to move or not
		sf::Vector2i moveVector;
		//varibles for moving base
		tgui::Button::Ptr movingBase = tgui::Button::create();//for the window moving base
		bool mousePressedMovingBase = false;
		bool pressEsc = false;
		sf::Mouse mouse;
		sf::Vector2i mouseWindowPos, mouseDesktopPos;

		//setting up the window
		sf::RenderWindow Window(sf::VideoMode(600, 400), "Snake Game", sf::Style::None);
		window = &Window;
		gui.setTarget(*window);//setting gui target to window
	
		//Setting up the button styles
		buttonStyle.setBackgroundColor(sf::Color::Transparent);
		buttonStyle.setBackgroundColorDisabled(sf::Color::Transparent);
		buttonStyle.setBackgroundColorHover(sf::Color::Transparent);
		buttonStyle.setBackgroundColorDownFocused(sf::Color::Transparent);
		buttonStyle.setTextColor(sf::Color::White);
		deltaTime = 0.0;

		//setting up the the moving base
		movingBase->setSize(550, 51);
		movingBase->setPosition(0, 0);
		movingBase->getRenderer()->setBackgroundColor(sf::Color::Transparent);
		movingBase->getRenderer()->setBackgroundColorDisabled(sf::Color::Transparent);
		movingBase->getRenderer()->setBackgroundColorHover(sf::Color::Transparent);
		movingBase->getRenderer()->setBackgroundColorDownFocused(sf::Color::Transparent);
		movingBase->getRenderer()->setBorders(tgui::Outline());
		movingBase->onMousePress([&]() {
			mouseWindowPos = mouse.getPosition(*window);
			mousePressedMovingBase = true;
		});
		movingBase->onMouseRelease([&]() {
			mousePressedMovingBase = false;
		});

		//setting up close button
		tgui::Texture closeIcon("assets/images/CloseIcon.png");
		close->setPosition(549, 0);
		close->setSize(51, 51);
		close->setImage(closeIcon);
		close->setRenderer(buttonStyle.clone());
		close->onMouseEnter([&]() {
			close->setImageScaling(1.2f);
			closeIcon.setColor(sf::Color::Red);
			close->setImage(closeIcon);
			});
		close->onMouseLeave([&]() {
			close->setImageScaling(1.0f);
			closeIcon.setColor(sf::Color::White);
			close->setImage(closeIcon);
			});
		close->onMouseRelease([&]() {
			Close();
			});
		//gui.add(close);

		//setting up font
		tgui::Font font("assets/Font/Roboto-Medium.ttf");
		gui.setFont(font);

		//setting gameState to menue in starting
		gameState = MENU;

		//setting up the background
		playbackrect.setFillColor(sf::Color(30, 30, 30));
		playbackrect.setPosition(10, 50);

		//setting up player window
		playerRect.setFillColor(sf::Color(64, 255, 83));

		//setting up the food
		food.setFillColor(sf::Color(255, 0, 0));
		food.setPosition(250, 190);

		//setting the snake state 
		playerState = LEFT;
		//Main Loop
		while (window->isOpen())
		{
			//for initalizing the state if it is new
			if (init)
			{
				switch (gameState)
				{
				case MENU: {
					tgui::Button::Ptr play = tgui::Button::create("PLAY");
					tgui::BitmapButton::Ptr info = tgui::BitmapButton::create();
					
					//setting up movingBase
					gui.add(movingBase);
					//setting up play button
					play->setPosition(180, 150);
					play->setRenderer(buttonStyle.clone());
					play->setTextSize(64);
					play->getRenderer()->setTextColorHover(sf::Color(164, 73, 255));
					play->onMouseRelease([&]() {
						gameState = PLAY;
						init = true;
						gui.removeAllWidgets();
					});
					gui.add(play);
					

					//setting up aboutus icon
					info->setPosition(21, 333);
					info->setSize(51, 51);
					info->setImage(tgui::Texture("assets/images/InfoIcon.png"));
					info->setRenderer(buttonStyle.clone());
					info->onMouseEnter([&]() {
						info->setImageScaling(1.2f);
						});
					info->onMouseLeave([&]() {
						info->setImageScaling(1.0f);
						});
					info->onMouseRelease([&]() {
						gameState = ABOUTUS;
						init = true;
						gui.removeAllWidgets();
					});
					gui.add(info);
					//gui.add(movingBase);
					gui.add(close);

					init = false;
				}break;
				case PLAY: {
					score = 0;
					scoreString.str("");
					while (!playerPos.empty())
					{
						playerPos.pop_back();
					}
					playerPos.push_back(sf::Vector2i(10, 50));
					move = 0;
					playerState = LEFT;
					moveVector.x = 20;
					moveVector.y = 0;
					scoreString << "SCORE : " << score;
					gameStarted = false;
					tgui::Label::Ptr scoreLabel = tgui::Label::create(scoreString.str());
					tgui::Label::Ptr pressKeyLabel = tgui::Label::create("PRESS ANY KEY TO START");
					gui.add(movingBase);
					gui.add(close);

					//setting up the score Label
					scoreLabel->setTextSize(24);
					scoreLabel->setPosition(245, 12);
					scoreLabel->getRenderer()->setTextColor(sf::Color::White);
					gui.add(scoreLabel, "scoreLabel");

					//setting up press any key to start label
					pressKeyLabel->setTextSize(24);
					pressKeyLabel->getRenderer()->setTextColor(sf::Color::White);
					pressKeyLabel->setPosition(153, 358);
					gui.add(pressKeyLabel, "pressKeyLabel");
					init = false;
					pressEsc = false;
				}break;
				case ABOUTUS:
				{
					tgui::Label::Ptr heading = tgui::Label::create("ABOUT US");
					tgui::Button::Ptr back = tgui::Button::create("<");
					tgui::Label::Ptr aboutme = tgui::Label::create("hi there my name is kuldeep.\nthis is a project work of mine\n\n\tvisit\n https://github.com/Kuldeep800singh \n\nfor more Projects");

					gui.add(movingBase);
					gui.add(close);

					//setting up heading
					heading->setPosition(180, 50);
					heading->setTextSize(44);
					heading->getRenderer()->setTextColor(sf::Color(164, 73, 255));
					heading->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
					gui.add(heading);

					//setting up about me
					aboutme->setPosition(100, 130);
					aboutme->setTextSize(24);
					aboutme->getRenderer()->setTextColor(sf::Color(164, 164, 164));
					aboutme->getRenderer()->setTextStyle(tgui::TextStyle::Italic);
					aboutme->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
					gui.add(aboutme);

					//setting up back button
					back->setPosition(10, 50);
					back->setTextSize(30);
					back->setRenderer(buttonStyle.clone());
					back->getRenderer()->setTextColorHover(sf::Color(255, 130, 130));
					back->onMouseEnter([&]() {
						back->setTextSize(33);
						});
					back->onMouseLeave([&]() {
						back->setTextSize(30);
						});
				//	back->onMouseEnter([&]() {
				//		sound.play(); });
					back->onMouseRelease([&]() {
						init = true;
						gameState = MENU;
						gui.removeAllWidgets();
						});
					gui.add(back);
					init = false;
				}break;
				case PAUSE:
				{
					gui.remove(gui.get("pressEscLabel"));
					tgui::Label::Ptr pressEscLabel = tgui::Label::create("PRESS ESC TO RESUME");
					pressEscLabel->setTextSize(24);
					pressEscLabel->getRenderer()->setTextColor(sf::Color::White);
					pressEscLabel->setPosition(153, 358);
					gui.add(pressEscLabel, "pressEscLabel");
					tgui::Button::Ptr retry = tgui::Button::create("RETRY");
					tgui::Button::Ptr exit = tgui::Button::create("EXIT");
					tgui::Label::Ptr pausedLabel = tgui::Label::create("PAUSED");
					pausedLabel->setTextSize(72);
					pausedLabel->setPosition(150, 100);
					gui.add(pausedLabel,"pausedLabel");

					//setting up retry button
					retry->setRenderer(buttonStyle.clone());
					retry->setTextSize(44);
					retry->setPosition(120, 200);
					retry->getRenderer()->setTextColorHover(sf::Color(164, 73, 255));
					retry->onMouseRelease([&]() {
						init = true;
						gameState = PLAY;
						gui.removeAllWidgets();
						gameStarted = false;
						});
					gui.add(retry, "Retry");

					//setting up the exit button
					exit->setRenderer(buttonStyle.clone());
					exit->setTextSize(44);
					exit->setPosition(330, 200);
					exit->getRenderer()->setTextColorHover(sf::Color(164, 73, 255));
					exit->onMouseRelease([&]() {
						init = true;
						gameState = MENU;
						gui.removeAllWidgets();
						});
					gui.add(exit, "ExitButton");
					init = false;

				}break;
				case RESULT:
				{
					tgui::Button::Ptr retry = tgui::Button::create("RETRY");
					tgui::Button::Ptr exit = tgui::Button::create("EXIT");
					tgui::Label::Ptr gameOverLabel = tgui::Label::create("GAME OVER");
					gameOverLabel->setTextSize(72);
					gameOverLabel->setPosition(100, 100);
					gui.add(gameOverLabel);

					//setting up retry button
					retry->setRenderer(buttonStyle.clone());
					retry->setTextSize(44);
					retry->setPosition(120, 200);
					retry->getRenderer()->setTextColorHover(sf::Color(164, 73, 255));
					retry->onMouseRelease([&]() {
						init = true;
						gameState = PLAY;
						gui.removeAllWidgets();
						gameStarted = false;
						});
					gui.add(retry, "Retry");
					
					//setting up the exit button
					exit->setRenderer(buttonStyle.clone());
					exit->setTextSize(44);
					exit->setPosition(330, 200);
					exit->getRenderer()->setTextColorHover(sf::Color(164, 73, 255));
					exit->onMouseRelease([&]() {
						init = true;
						gameState = MENU;
						gui.removeAllWidgets();
						});
					gui.add(exit, "ExitButton");
					gui.remove(gui.get("pressEscLabel"));
					init = false;
				}break;
				}
			}

			//Event polling
			while (window->pollEvent(events))
			{
				//Event handling
				gui.handleEvent(events);//tgui hadling events
				switch (gameState)
				{
				//case MENU:break;
				case PLAY:
				{
					if (gameStarted)
					{
						if (events.type == sf::Event::KeyReleased)
						{
							if (events.key.code == sf::Keyboard::Escape)
							{
								gameState = PAUSE;
								init = true;
								
							}
						}
						if (events.type == sf::Event::KeyPressed)
						{
							if(events.key.code == sf::Keyboard::Key::A)
							{
								if (playerState != LEFT)
								{
									moveVector = sf::Vector2i(-20, 0);
									playerState = RIGHT;

								}
							} if (events.key.code == sf::Keyboard::Key::D)
							{
								if (playerState != RIGHT)
								{
									moveVector = sf::Vector2i(20, 0);
									playerState = LEFT;
								}
							}
							 if (events.key.code == sf::Keyboard::Key::W)
							{
								if (playerState != DOWN)
								{
									moveVector = sf::Vector2i(0, -20);
									playerState = UP;

								}
							}
							 if (events.key.code == sf::Keyboard::Key::S)
							{
								if (playerState != UP)
								{
									moveVector = sf::Vector2i(0, 20);
									playerState = DOWN;

								}
							}
						}
					}
					else {
						if (events.type == sf::Event::KeyReleased)
						{
							gameStarted = true;
							gui.remove(gui.get("pressKeyLabel"));
						}
					}
				}break;
				case ABOUTUS:break;
				case PAUSE: {
					if (events.type == sf::Event::KeyReleased && events.key.code == sf::Keyboard::Escape)
					{
						gui.remove(gui.get("pressEscLabel"));
						gui.remove(gui.get("ExitButton"));
						gui.remove(gui.get("Retry"));
						gui.remove(gui.get("pausedLabel"));
						
						pressEsc = false;
						gameState = PLAY;


					}
				}break;
				default:break;
				}
				if (events.type == sf::Event::Closed)
				{
					Close();
				}
			}

			//For Drawing and clearing
			if (mousePressedMovingBase)
			{
				mouseDesktopPos = mouse.getPosition();
				window->setPosition(sf::Vector2i(mouseDesktopPos.x - mouseWindowPos.x, mouseDesktopPos.y - mouseWindowPos.y));
			}
			window->clear(sf::Color(48, 48, 48));
			switch (gameState)
			{
			case MENU: {
				gui.draw();
			}break;
			case PLAY:
			{
				if (gameStarted)
				{
					if (!pressEsc)
					{
						tgui::Label::Ptr pressEscLabel = tgui::Label::create("PRESS ESC TO PAUSE");
						pressEscLabel->setTextSize(24);
						pressEscLabel->getRenderer()->setTextColor(sf::Color::White);
						pressEscLabel->setPosition(153, 358);
						gui.add(pressEscLabel, "pressEscLabel");
						pressEsc = true;
					}
					//for checking the border edge
					if (playerPos[0].x < 10 || playerPos[0].x >= 580 || playerPos[0].y < 50 || playerPos[0].y >= 350)
					{
						gameState = RESULT;
						init = true;
					}
					//for checking if snake bite itself
					for (int i = 2; i < playerPos.size(); i++)
					{
						if (playerPos[0] == playerPos[i])
						{
							gameState = RESULT;
							init = true;
						}
					}
					//for checking if snake hit the food if yes ten setting next postion and increasing score
					if (food.getGlobalBounds().intersects(sf::FloatRect(playerPos[0].x, playerPos[0].y, 20, 20))) {
						score++;
						scoreString.str("");
						scoreString << "SCORE : " << score;
						tgui::Label::Ptr scorePtr = gui.get<tgui::Label>("scoreLabel");
						scorePtr->setText(scoreString.str());
						//choseing the random and postion of the food not on the snake in the  
						while (1) {
							int x = (10 +( random(0, 25) * 20)),y=(50 + (random(0, 15) * 20));
							for (sf::Vector2i pos : playerPos)
							{
								if (x == pos.x && y == pos.y)
								{
									continue;
								}
							}
							food.setPosition(x, y);
							break;
						}
						playerPos.push_back(playerPos[playerPos.size()-1]);
						playerVelocity += playerAcceleration;
					}
					sumofVel += (playerVelocity) * deltaTime;
					if (sumofVel >= 1)
					{
						sumofVel = 0;
						move = true;
						for (int i = playerPos.size() - 1; i >= 0; i--)
						{
							if (i == 0)
							{
								playerPos[i] += moveVector;
							}
							else {
								playerPos[i] = playerPos[i - 1];
							}
						}
					}
				}
				window->draw(playbackrect);
				//for drawing snake
				
				int countno = 0;
				for (sf::Vector2i pos : playerPos) {
					playerRect.setPosition((sf::Vector2f)pos);
					playerRect.setFillColor(sf::Color(64, 255 - countno, 83));
					score<50?countno+=5:countno-=5;
					window->draw(playerRect);
				}
				window->draw(food);
				gui.draw();
			}break;
			case ABOUTUS:
			{
				gui.draw();
			}break;
			case PAUSE:
			{
				window->draw(playbackrect);
				int countno = 0;
				for (sf::Vector2i pos : playerPos) {
					playerRect.setPosition((sf::Vector2f)pos);
					playerRect.setFillColor(sf::Color(64, 255 - countno, 83));
					score < 50 ? countno += 5 : countno -= 5;
					window->draw(playerRect);
				}
				window->draw(food);
				gui.draw();
			}break; 
			case RESULT:
			{
				window->draw(playbackrect);
				int countno = 0;
				for (sf::Vector2i pos : playerPos) {
					playerRect.setPosition((sf::Vector2f)pos);
					playerRect.setFillColor(sf::Color(64, 255 - countno, 83));
					score < 50 ? countno += 5 : countno -= 5;
					window->draw(playerRect);
				}
				window->draw(food);
				gui.draw();
			}break;
			}
			window->display();
			deltaTime = clock.getElapsedTime().asSeconds();
			clock.restart();
		}
	}
};

int main()
{
	Game game;
	game.Start();
	return 0;
}