#include <track.hpp>

bool Track::Barrier::testConstrains(const Car& contextCar)
{
	auto carPosition = contextCar.getPosition();

	if(	testSingleConstrain(xConstrains, carPosition.x) &&
			testSingleConstrain(yConstrains, carPosition.y))
		return true;
	else
		return false;
}

void Track::add_barrier(const sf::RectangleShape& rs)
{
	sf::Vector2f xConstrains = sf::Vector2f(rs.getPosition().x,
																				 	rs.getPosition().x + rs.getSize().x) ;
	sf::Vector2f yConstrains = sf::Vector2f(rs.getPosition().y,
																					rs.getPosition().y + rs.getSize().y) ;
	
	barriers.push_back(std::unique_ptr<Barrier>(new TexturedBarrier{xConstrains, yConstrains}));
}

void Track::add_barrier(sf::Vector2f xCon, sf::Vector2f yCon)
{
	barriers.push_back(std::unique_ptr<Barrier>{new TexturedBarrier(xCon, yCon)}); //TODO throw exce if xCon.x < xCon.y etc.
}

void Track::add_finish_barrier(sf::Vector2f xCon, sf::Vector2f yCon)
{
	barriers.push_back(std::unique_ptr<Barrier>{new FinishBarrier(xCon,yCon)});
	lastFinishBarrierRect = sf::RectangleShape(sf::Vector2f(xCon.y-xCon.x,yCon.y-yCon.x));
	lastFinishBarrierRect.setPosition(xCon.x,yCon.x);
}

void Track::trackMove(Car& contextCar)
{
	for(auto &b : barriers)
	{	
		b->interactWithBarrier(contextCar);
	}
}

int Track::trackMoveUntilBarrier(Car& contextCar, int limit) // returns
{
	int index = 0;
	while(limit-- > 0)
	{
		index++;
		for(auto &b : barriers)
		{	
			if(b->testConstrains(contextCar))
			{
				b->interactWithBarrier(contextCar);
				contextCar.calculateNewPosition();
				return index;
			}
		}
		contextCar.calculateNewPosition();
 	}
	return index;
}

void Track::Barrier::interactWithBarrier(Car& contextCar)
{
		auto aux_acc = sf::Vector2f(0,0);

		if(this->testConstrains(contextCar))
		{
			bool presentInX = testSingleConstrain(xConstrains, contextCar.getLastPosition().x); //x1<x<x2
			bool presentInY = testSingleConstrain(yConstrains, contextCar.getLastPosition().y); //y1<y<y2

			if( presentInX )
			{
				if( presentInY ) return; // [(x1<x<x2) && (y1<y<y2)] is true
				aux_acc.y = -2*contextCar.getVelocity().y; // collision with upper or downer ?XD? edge
			}

			else if( presentInY )
			{
				aux_acc.x = -2*contextCar.getVelocity().x; //collision with left or right edge
			}
			else //each condition is false: [!(x1<x<x2) && !(y1<y<y2)] is true
			{
				aux_acc.x = -2*contextCar.getVelocity().x;
				aux_acc.y = -2*contextCar.getVelocity().y;
			}
		}

	contextCar.accelerate(sf::Vector2f(0,0), aux_acc); 
}

void Track::FinishBarrier::interactWithBarrier(Car& contextCar)
{
	if(this->testConstrains(contextCar))	
	{
		contextCar.markAsFinished();
	}
}

sf::Vector2f Track::getVectorToLastFinishBarrier(const Car& contextCar)
{
	sf::Vector2f vecToFinish(0,0);
	
	auto diff_x = lastFinishBarrierRect.getPosition().x	- contextCar.getPosition().x;		
	if(diff_x > 0)
		vecToFinish.x = diff_x;
	else if(diff_x + lastFinishBarrierRect.getSize().x < 0)
		vecToFinish.x = diff_x + lastFinishBarrierRect.getSize().x;
	else 
		vecToFinish.x = 0;

	auto diff_y = lastFinishBarrierRect.getPosition().y	- contextCar.getPosition().y;		
	if(diff_y > 0)
		vecToFinish.y = diff_y;
	else if(diff_y + lastFinishBarrierRect.getSize().y < 0)
		vecToFinish.y = diff_y + lastFinishBarrierRect.getSize().y;
	else 
		vecToFinish.y = 0;

	return vecToFinish;
}
