#ifndef CARD_H_
#define CARD_H_

using namespace std;

namespace QParser{


class Card
{
public:
	Card();
	virtual ~Card();
	static int next();
};
}

#endif /*CARD_H_*/
