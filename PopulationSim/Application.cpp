#include <iostream>
# define size 1024

struct Creature {

};

struct World {

	Creature locations[size][size];
public:
	int population;

	World() {
		this->population = 1;
		Creature c = {};
		Creature cc[size][size] = malloc(sizeof(Creature) * (size*size));
		this->locations = new Creature[size][size];

		// 
		//struct v* data = (struct v*)malloc(sizeof(struct v));

	}
};


int main() {

	World w = World();
	std::cout << w.population;
	return 1;
}