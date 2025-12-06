#include "../include/Simulation.h"
#include <iostream>
using namespace std;

int main()
{
    // Create simulation
    Simulation sim;
    
    // Load map
    if (!sim.loadMap("data/roads.txt"))
    {
        cerr << "Failed to load map file!" << endl;
        return 1;
    }
    
    // Set frame rate
    sim.setFrameRate(60);
    
    // Run simulation
    sim.run();
    
    return 0;
}