#ifndef TRAFFICSIGNAL_H
#define TRAFFICSIGNAL_H

#include "Queue.h"
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include "Vehicle.h"
using namespace std;

enum class SignalState
{
    RED,
    GREEN
};

class TrafficSignal
{
private:
    int signalID;
    int intersectionID; // ID of the node which has this signal
    SignalState state;

    sf::Vector2<float> position;
    Queue<Vehicle *> waitingQueue; // Queue of cars waiting at red light
    float timer;
    float greenDuration; // How long green light lasts (seconds)
    float redDuration;   // How long red light lasts (seconds)
    bool isAuto;         // True = auto-switch, False = manual control

public:
    // Constructor
    TrafficSignal(int id, int nodeID, const sf::Vector2<float> &pos, float greenTime = 5.0f, float redTime = 5.0f, bool automatic = true);

    // Signal control
    void toggleLight();
    void updateTimer(float deltaTime); // for automatic switch of signals
    void switchState();

    // Queue management
    void addVehicleToQueue(Vehicle *car);
    Vehicle *dequeueVehicle();
    void releaseAllVehicles();
    bool isQueueEmpty() const;
    int getQueueSize() const;

    // Getters
    int getID() const;
    int getIntersectionID() const;
    SignalState getState() const;
    sf::Vector2<float> getPosition() const;
    sf::Color getColor() const;
    float getTimer() const;
    bool isAutoMode() const;

    // Setters
    void setState(SignalState newState);
    void setAutoMode(bool automatic);
    void setGreenDuration(float duration);
    void setRedDuration(float duration);

    // For debugging
    void reset(); // reset the signal to its default state
    void printInfo() const;
};

#endif