#ifndef _infect_h_
#define _infect_h_

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <list>
#include <chrono>
#include <thread>
#include <random>
#include <stdio.h>      
#include <stdlib.h>     
#include <time.h> 
/**
people is a class that takes care of each individual and their status. It also includes the connections to other people which change daily. 
@moron is a bool that tells us whether or not the person is a moron
@infected is a bool that tells us whether or not the person is infected
@susceptible is a bool that tells us whether or not the person is susceptible
@recovered is a bool that tells us whether or not the person is recovering
@immune is a bool that tells us whether or not the person is immune, and cannot get 
@recover_counter is a size_t that keeps track of how many days recovered without getting infected again
@others is a vector of references to people that keep track of who this person is in contact with
*/
class people {
private:
	bool moron;
	bool infected;
	bool susceptible;
	bool recovered;
	bool immune;
	size_t recover_counter;
	std::vector<std::reference_wrapper<people>> others;

public:
	/**
	people(bool m, bool s) is the constructor for our person, it sets whether or not they are moron and infected.
	@param bool m is the bool to tell if this person should be a moron.
	@param bool s is the bool to tell if this person is initially sick
	*/
	people(bool m, bool s) :moron(m), infected(s), susceptible(true), recovered(false) { recover_counter = 0; }	//all people are susceptilble unless sick, and not recovered
	//accessors 
	/**
	isMoron() const is an accessor that simply returns the moron bool
	@return bool true if moron, false otherwise
	*/
	bool isMoron() const { return moron; }
	/**
	isInfected() const is an accessor that returns the infected bool
	@return bool true if infected/sick, false otherwise
	*/
	bool isInfected() const { return infected; }
	/**
	isSus const is an accessor that returns the susceptible bool
	@return bool true if susecptible, false otherwise
	*/
	bool isSus() const { return susceptible; }
	/**
	isRec() const is an accessor taht returns the recovered bool
	@return bool true if recovered, false otherwise
	*/
	bool isRec() const { return recovered; }
	/**
	sick() will just change infected to true and susceptible to false. this is used when randomly picking people to be initially sick
	*/
	void sick() { infected = true; susceptible = false; }
	/**
	addOther(people& contact) will take in a person and push it back onto the others vector as a reference
	@param people& contact is the contact to add to the vector
	*/
	void addOther(people& contact)
	{
		if (this == &contact)	//if the person we are adding is ourself, then just return
		{
			return;
		}
		others.push_back(contact); //otherwise, pushback the contact onto our others vector
	}
	/**
	reset() simply clears the others vector, getting ready for another day of contacts
	*/
	void reset()
	{
		others.clear();	//clear everything in the vector to prepare for new contacts
	}
	/**
	recover() will use the equations and the rates to give the person a chance to start recovery
	people are infected for 14 days before they are fully recovered and immune
	*/
	void recover() 
	{
		if (!infected)	//if not infected, then we don't need to recover
		{
			return;
		}
		else if (recovered)	//if we are in the recovering stage
		{
			recover_counter++;	//we increase the recover counter, saying that we've recovered for another day
			if (recover_counter >= 14)	//if this counter gets to 14, then we have fully revoered
			{
				infected = false;	//which means we are no longer infected, susceptible, and are immune
				susceptible = false;
				immune = true;
			}
			return;	//we are done
		}
		else if (infected)	//if we are infected and not recovering yet
		{
			std::exponential_distribution<double> recover{ 0.071428571429 };	//create an exponential distribution variable with 1/14 as intensity
			std::random_device rand;	//create a random device
			double time = 14;	//and our time of recovery 14 days
			if ((recover(rand)) > time)	//if our person can recover in 14 days
			{
				recovered = true;	//then we start recovering, so recovery is true
				infected = true;	//but we are still infected
				susceptible = false;//no longer susceptible
				recover_counter = 0;//and we start recovering at 0 days
			}
		}
	}
	/**
	getSick() this is the function to determine if a person gets sick or not 
	*/
	void getSick()
	{
		if (immune)	//if our person is immune, they cannot get sick. return 
		{
			return;
		}
		size_t n = 0;	//create counters for normal sick people and morons sick people
		size_t m = 0;
		for (size_t i = 0; i < others.size(); ++i)	//for everyone in our contacts
		{
			if (others[i].get().isMoron() && others[i].get().isInfected())	//if the person is a moron and sick
			{	
				m++;	//increment the moron counter
			}
			else if (!others[i].get().isMoron() && others[i].get().isInfected())	//if the person is normal but sick
			{
				n++;	//increment the normal counter
			}
		}
		double sickRate = 0.02 * (0.34 * n + m);	//use equatinos to get our sickness probability using our counters
		double prob = 1 - std::exp(-sickRate);
		prob *= 100;
		double chance = (std::rand() % 100) + static_cast<double>(1);	//create a random variable to see if we got sick by chance
		if (chance <= prob)	//if our chance is withint our probability
		{
			infected = true;	//then set the person to infected, not susceptible or recovering
			susceptible = false;
			recovered = false;
		}
	}
};
/**
stub is a struct that stores a reference to a person. it is used to keep track of connections when creating our graph
@a is the person that we are storing in this stub
*/
struct stub {
	/**
	stub(people& person) is constructor, taking in a person by reference, and initializing a with it.
	*/
	stub(people& person) :a(person) {}
	people& a;	//the person we are storing
};
/**
simulation is the class that takes care of our population of people, our connections, and our bar graph
@population is our vector of pointers to people
@connection is a list of stubs, which is filled with all the stubs, used to connect people together
@w is a pointer to a window, specically the window initialized in main
@f is the font for our text
@day is the text telling us what day we are on
@side is the side words of our bar graph
@norm_sus is the bar representing the normal susceptible 
@norm_sick is the bar representing the normal sick 
@norm_rec is the bar representing the normal recovered
@mor_sus is the bar representing the moron susceptible
@mor_sick is the bar representing the moron sick
@mor_rec is the bar representing the moron recovered 
@t is the number representing the day
@total is the total number of people, normal or morons
@n_sus is the number of normal susceptible 
@n_sick is the number of normal sick
@n_rec is the number of normal recovered 
@m_sus is the number of moron susceptible
@m_sick is the number of moron sick 
@m_rec is the number of moron recovered 
*/
class simulation {
private:
	//containers
	std::vector<people*> population;
	std::list<stub> connection;
	//SFML 
	sf::RenderWindow* w;
	sf::Font f;
	sf::Text day;
	sf::Text side;
	sf::RectangleShape norm_sus;
	sf::RectangleShape norm_sick;
	sf::RectangleShape norm_rec;
	sf::RectangleShape mor_sus;
	sf::RectangleShape mor_sick;
	sf::RectangleShape mor_rec;
	//counters
	size_t t;	//number for day
	size_t total;
	size_t n_sus;
	size_t n_sick;
	size_t n_rec;
	size_t m_sus;
	size_t m_sick;
	size_t m_rec;



public:
	/**
	simulation(sf::RenderWindow* win, size_t norm, size_t mor, size_t sick) is the constructor that initializes most things, including all of our people, all text and our 
	initial bars. 
	@param sf::RenderWindow* win is a pointer to the window we want to draw on, which is the one in main
	@param size_t norm is the amount of normal people 
	@param size_t mor is the amount of morons
	@param size_t sick is the amount of initial sick people
	*/
	simulation(sf::RenderWindow* win, size_t norm, size_t mor, size_t sick)
		:w(win), t(0), n_sus(norm), m_sus(mor)	//we start at day 0, and initialize other member variables
	{
		total = norm + mor;					//our total is morons and normal people

		f.loadFromFile("sansation.ttf");	//load the font

		day.setFont(f);						//configure our day text
		day.setCharacterSize(20);
		day.setFillColor(sf::Color::White);
		day.setString("Day: ");
		day.move(0, 100);

		side.setFont(f);					//configure our side text
		side.setCharacterSize(20);	
		side.setFillColor(sf::Color::White);
		side.setString("Normal S\nNormal I\nNormal R\nMoron S\nMoron I\nMoron R");
		side.move(0, 150);

		norm_sus = sf::RectangleShape({ (sf::Vector2f(120.f, 20.f)) });	//configure our initial rectangles
		norm_sus.move(100.f, 153.f);
		norm_sus.setFillColor(sf::Color::Green);
		norm_sick = sf::RectangleShape({ (sf::Vector2f(120.f, 20.f)) });
		norm_sick.move(100.f, 175.f);
		norm_sick.setFillColor(sf::Color::Red);
		norm_rec = sf::RectangleShape({ (sf::Vector2f(120.f, 20.f)) });
		norm_rec.move(100.f, 197.f);
		norm_rec.setFillColor(sf::Color::Blue);

		mor_sus = sf::RectangleShape({ (sf::Vector2f(120.f, 20.f)) });
		mor_sus.move(100.f, 219.f);
		mor_sus.setFillColor(sf::Color::Green);
		mor_sick = sf::RectangleShape({ (sf::Vector2f(120.f, 20.f)) });
		mor_sick.move(100.f, 241.f);
		mor_sick.setFillColor(sf::Color::Red);
		mor_rec = sf::RectangleShape({ (sf::Vector2f(120.f, 20.f)) });
		mor_rec.move(100.f, 263.f);
		mor_rec.setFillColor(sf::Color::Blue);

		for (size_t i = 0; i < norm; ++i)	//add all our normal people
		{
			people* p_temp = new people(false, false);//create a temp person who has moron and sick set to false
			for (size_t j = 0; j < 9; ++j)
			{
				stub s_temp(*p_temp);			//then create 9 stubs per normal person
				connection.push_back(s_temp);
			}
			population.push_back(p_temp);
		}
		for (size_t i = 0; i < mor; ++i)	//add all our morons
		{
			people* p_temp = new people(true, false);//create a temp person who has moron set to true and sick set to false
			for (size_t j = 0; j < 20; ++j)
			{
				stub s_temp(*p_temp);			//then create 20 stubs per moron 
				connection.push_back(s_temp);
			}
			population.push_back(p_temp);
		}

		for (size_t i = 0; i < sick; ++i)	//we want sick number of initial sick people, so iterate that amount of times
		{
			size_t S = rand() % population.size();	//randomly generate an index
			population[S]->sick();	//and the person at that index is now sick
		}

		this->createGraph();	//then start creating the initial graph
	}
	/**
	simulation() is the destructor that removes everything in our containers
	*/
	~simulation()
	{
		population.clear();
		connection.clear();
	}
	/**
	reset() will reset everyone's connections and remake all the connections in our connection vector
	*/
	void reset()
	{
		for (size_t i = 0; i < population.size(); ++i)	//for everyone in our population
		{
			population[i]->reset();			//clears current connections
			if (population[i]->isMoron())	//if they are moron
			{
				for (size_t j = 0; j < 20; ++j)
				{
					stub s_temp(*population[i]);			//recreate the 20 stubs per moron
					connection.push_back(s_temp);
				}
			}
			else							//otherwise they are normal
			{
				for (size_t j = 0; j < 9; ++j)
				{
					stub s_temp(*population[i]);			//then recreate 9 stubs per normal person
					connection.push_back(s_temp);
				}
			}
		}
		//reset all our counters to zero, they will be counted up again anyways
		m_rec = 0;
		m_sick = 0;
		m_sus = 0;
		n_rec = 0;
		n_sick = 0;
		n_sus = 0;
	}
	/**
	createGraph() creates our connections by the stub method, constantly pairing people until there are no more connections to make
	*/
	void createGraph()
	{
		while (!connection.empty())		//while we still have connections to make
		{
			if (connection.size() == 1)	//if we only have one stub left, then we are done
			{
				break;
			}
			size_t p1 = std::rand() % connection.size();		//create a random index
			std::list<stub>::iterator it = connection.begin();	//create an iterator to the beginning of the list
			for (int i = 0; i < p1; ++i)	//and iterate it to the place of our index
			{
				++it;
			}
			it->a.addOther(connection.front().a);	//add the first person to this random person's connections
			connection.front().a.addOther(it->a);	//add the random person to the first person's connections
			connection.erase(it);					//erase the random stub
			connection.erase(connection.begin());	//and erase the first stub (making the first stub now different than before
		}
		connection.clear();	//clear any left over stubs, probably the one left over
	}
	/**
	configureRect() is the function that will take our numbers/counters of people by their groups, and then configure how long our bars should be
	it also takes care of their position and color
	*/
	void configureRect()
	{
		//configure all the people bars:
		//our max length is 220 pixels, so just take a fraction of that based on how many people there are of the group and divide it by the total
		//that fraction is then what we multiply our 220 by, to create a proportional bar.
		//each bar is 22 pixels from the other to line up with the text. 
		//susceptible people have a green bar, sick people are red, and recovered people are blue
		//This applies to all bars
		norm_sus = sf::RectangleShape({ (sf::Vector2f(220.f * n_sus / total, 20.f)) });	//
		norm_sus.move(100.f, 153.f);
		norm_sus.setFillColor(sf::Color::Green);
		norm_sick = sf::RectangleShape({ (sf::Vector2f(220.f * n_sick / total, 20.f)) });
		norm_sick.move(100.f, 175.f);
		norm_sick.setFillColor(sf::Color::Red);
		norm_rec = sf::RectangleShape({ (sf::Vector2f(220.f * n_rec / total, 20.f)) });
		norm_rec.move(100.f, 197.f);
		norm_rec.setFillColor(sf::Color::Blue);

		mor_sus = sf::RectangleShape({ (sf::Vector2f(220.f * m_sus / total , 20.f)) });
		mor_sus.move(100.f, 219.f);
		mor_sus.setFillColor(sf::Color::Green);
		mor_sick = sf::RectangleShape({ (sf::Vector2f(220.f * m_sick / total, 20.f)) });
		mor_sick.move(100.f, 241.f);
		mor_sick.setFillColor(sf::Color::Red);
		mor_rec = sf::RectangleShape({ (sf::Vector2f(220.f * m_rec / total, 20.f)) });
		mor_rec.move(100.f, 263.f);
		mor_rec.setFillColor(sf::Color::Blue);
	}
	/**
	run() is the where all the functions come together to create our model and graphic. it only runs once a second, creates a new graph each day, and counts up
	how many people there are of each group
	*/
	void run()
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));	//only run once a second
		this->reset();			//reset the population's connections each day. people meet different people each day anyways, and all the counters
		this->createGraph();	//create the graph with new connections
		for (size_t i = 0; i < population.size(); ++i)	//for everyone in our population
		{
			population[i]->getSick();	//give them the chance to get sick
			population[i]->recover();	//then give them the chance to recover
			if (population[i]->isMoron())	//if they are morons
			{		
				if (population[i]->isSus())	//and susceptible, then increment the m_sus counter
				{
					m_sus++;
				}
				else if (population[i]->isInfected())	//or if they're infected, then increment the infected counter
				{
					m_sick++;
				}
				else if (population[i]->isRec())	//and if they are recovered, then increment the recover counter
				{
					m_rec++;
				}
			}
			else	//if they are normal people
			{
				if (population[i]->isSus())	//and sceptible, increment the n_sus counter
				{
					n_sus++;
				}
				else if (population[i]->isInfected())	//or if they're sick, then increment the n_sick counter
				{
					n_sick++;
				}
				else if (population[i]->isRec())	//or if they are recovered, the increment the n_rec counter
				{
					n_rec++;
				}
			}
		}

		this->configureRect();	//configure our rectangles with our values of our counters

		t++;	//increment the day
		std::string days = std::to_string(t);	//change the day to a string
		day.setString("Day: " + days);	//and print it out
		w->draw(day);		//draw our days, our side text, and all of our bars
		w->draw(side);
		w->draw(norm_sus);
		w->draw(norm_sick);
		w->draw(norm_rec);
		w->draw(mor_sus);
		w->draw(mor_sick);
		w->draw(mor_rec);

	}
};

#endif // !_infect_h_
