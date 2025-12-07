#include "../include/Simulation.h"
#include <iostream>
using namespace std;

int main()
{
    Simulation sim;
    
    if (!sim.loadMap("data/roads.txt"))
    {
        cerr << "Failed to load map file!" << endl;
        return 1;
    }
    
    sim.setFrameRate(60);
    
    sim.run();
    
    return 0;
}