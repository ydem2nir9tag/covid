#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdio.h>      
#include <stdlib.h>     
#include <time.h> 
#include <string>
#include "infect.h"
/**
main() starts our window and checks for input, allows for input of the initial numbers of our simulation. it even accepts backspace 
@return 0 
*/
int main(){
	sf::RenderWindow window(sf::VideoMode(600,600), "SIR Model");	//create a window with title SIR Model
	//load in our font
	sf::Font f;	
	f.loadFromFile("sansation.ttf");
	//configure our messages, mainly the category to be inputted into. all have size 20 and are white
	sf::Text msg1;
	msg1.setFont(f);
	msg1.setCharacterSize(20);
	msg1.setFillColor(sf::Color::White);
	msg1.setString("Normal: ");

	sf::Text msg2;
	msg2.setFont(f);
	msg2.setCharacterSize(20);
	msg2.setFillColor(sf::Color::White);
	msg2.move({ 0,30 });

	sf::Text msg3;
	msg3.setFont(f);
	msg3.setCharacterSize(20);
	msg3.setFillColor(sf::Color::White);
	msg3.move({ 0,60 });
	//create some strings to represent the categories when they change
	std::string input = "Normal: ";
	std::string num = "";	//the number we are inputting
	//counters of our initial values to be passed into our simulation constructor
	size_t init_norm;
	size_t init_moron;
	size_t init_sick;
	size_t i = 0;		//this tells us what category we are inputting into. 0 for normal people, 1 for morons, and 2 for sick people. once i is passed 2, the simulation starts
	bool enter = false;	//tells us if the enter key is pressed
	bool sim = false;	//tells us if the simulation should be started

	while (window.isOpen())	//while our window is open
	{	
		window.clear(sf::Color::Black);	//clear it and fill with black
		sf::Event event;				//create an event
		while (window.pollEvent(event))	//and poll that event
		{
			if (event.type == sf::Event::Closed)	//if the windows should be closed, then close it
			{
				window.close();
			}
			else if (event.type == sf::Event::TextEntered)	//if there is text entered
			{
				if (event.text.unicode == 8)	//and the unicode is 8, then backspace is pressed and we should pop back the string
				{
					if (!num.empty())	//if the string is not empty, then pop back
					{
						num.pop_back();
					}
				}
				else if (event.text.unicode < 58 && event.text.unicode > 47)	//if it's a number
				{
					
					num += static_cast<char>(event.text.unicode);	//then cast that unicode to a char and add it to our num string
				}	
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))	//and if the enter key is pressed
			{	
				enter = true;	//then set enter to true
			}
			if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && enter) //and if our enter key is not pressed, but enter is set to true, then we've released our enter key
			{
				i++;	//increment i
				if (i == 1)	//if i is 1, then we should start inputting to moron, enter has been pressed and released once
				{
					init_norm = std::stoi(num);	//store our number into the initial normal
					input = "Moron: ";	//and change the input message to moron
					
				}
				else if (i == 2)	//if i is 2, we've pressed and released enter twice
				{
					init_moron = std::stoi(num);	//store the number into the initial moron counter
					input = "Sick: ";	//change our input message to sick
				}
				else	//if i is greater than 2
				{	
					init_sick = std::stoi(num);	//store the number into the initial sick counter
					sim = true;	//set sim to true, saying we can start our simulation
				}
				num = "";		//reset num after each enter press
				enter = false;	//enter is no longer pressed
			}
			switch (i)	//based on the value of i
			{
			case 0:
				msg1.setString(input + num);	//we are inputting to msg1
				break;
			case 1:
				msg2.setString(input + num);	//or msg 2 if i is 1
				break;
			case 2:
				msg3.setString(input + num);	//and msg3 if i is 2
				break;
			default:	//otherwise do nothing
				break;
			}		
		}

		if (sim)	//if we should start our simulation
		{
			static simulation COVID19(&window, init_norm, init_moron, init_sick);	//create a static simulation with our initial counters. static so we only run one simulation
			COVID19.run();	//and then run the simulation and draw the bar graph
		}
		//draw the 3 messages and display everything
		window.draw(msg1);
		window.draw(msg2);
		window.draw(msg3);
		window.display();

	}


	return 0;
}

