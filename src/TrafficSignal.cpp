#include "../include/TrafficSignal.h"
#include <iostream>
using namespace std;

TrafficSignal::TrafficSignal(int id, int nodeID, const sf::Vector2<float> &pos, float greenTime, float redTime, bool automatic)
{
    signalID = id;
    intersectionID = nodeID;
    position = pos;
    greenDuration = greenTime;
    redDuration = redTime;
    isAuto = automatic;
    state = SignalState::GREEN;
    timer = greenTime;

    cout << "Traffic Signal " << signalID << " created at intersection " << intersectionID << endl;
}

void TrafficSignal::toggleLight()
{
    if (state == SignalState::RED)
    {
        state = SignalState::GREEN;
        timer = greenDuration;
        releaseAllVehicles(); // Release waiting cars
        cout << "Signal " << signalID << " switched to GREEN" << endl;
    }
    else if (state == SignalState::GREEN)
    {
        state = SignalState::RED;
        timer = redDuration;
        cout << "Signal " << signalID << " switched to RED" << endl;
    }
}

void TrafficSignal::updateTimer(float deltaTime)
{
    if (!isAuto)
        return;

    timer -= deltaTime;

    if (timer <= 0.0f)
    {
        switchState();
    }
}

void TrafficSignal::switchState()
{
    if (state == SignalState::GREEN)
    {
        state = SignalState::RED;
        timer = redDuration;
        cout << "Signal " << signalID << " auto-switched to RED" << endl;
    }
    else if (state == SignalState::RED)
    {
        state = SignalState::GREEN;
        timer = greenDuration;
        releaseAllVehicles(); // Release waiting vehicles
        cout << "Signal " << signalID << " auto-switched to GREEN" << endl;
    }
}

void TrafficSignal::addVehicleToQueue(Vehicle *car)
{
    if (car == nullptr)
    {
        cout << "Warning: Attempted to add null vehicle to queue!" << endl;
        return;
    }

    waitingQueue.push(car);
    car->setStatus(VehicleStatus::WAITING);

    cout << "Vehicle " << car->getID() << " added to queue at Signal " << signalID << " (Queue size: " << waitingQueue.size() << ")" << endl;
}

Vehicle *TrafficSignal::dequeueVehicle()
{
    if (waitingQueue.empty())
        return nullptr;

    Vehicle *car = waitingQueue.front();
    waitingQueue.pop();

    car->setStatus(VehicleStatus::MOVING);

    cout << "Vehicle " << car->getID() << " released from Signal " << signalID << endl;

    return car;
}

void TrafficSignal::releaseAllVehicles()
{
    if (state != SignalState::GREEN)
        return;

    int released = 0;
    while (!waitingQueue.empty())
    {
        Vehicle *car = dequeueVehicle();
        if (car != nullptr)
            released++;
    }

    if (released > 0)
        cout << "Signal " << signalID << " released " << released << " vehicle(s)" << endl;
}

bool TrafficSignal::isQueueEmpty() const { return waitingQueue.empty(); }
int TrafficSignal::getQueueSize() const { return waitingQueue.size(); }

int TrafficSignal::getID() const { return signalID; }
int TrafficSignal::getIntersectionID() const { return intersectionID; }
SignalState TrafficSignal::getState() const { return state; }
sf::Vector2<float> TrafficSignal::getPosition() const { return position; }
float TrafficSignal::getTimer() const { return timer; }
bool TrafficSignal::isAutoMode() const { return isAuto; }

sf::Color TrafficSignal::getColor() const
{
    if (state == SignalState::RED)
        return sf::Color::Red;
    else
        return sf::Color::Green;
}

void TrafficSignal::setState(SignalState newState)
{
    state = newState;

    if (state == SignalState::GREEN)
    {
        timer = greenDuration;
        releaseAllVehicles();
    }
    else if (state == SignalState::RED)
        timer = redDuration;
}

void TrafficSignal::setAutoMode(bool automatic) { isAuto = automatic; }
void TrafficSignal::setGreenDuration(float duration) { greenDuration = duration; }
void TrafficSignal::setRedDuration(float duration) { redDuration = duration; }

void TrafficSignal::reset()
{
    state = SignalState::GREEN;
    timer = greenDuration;

    while (!waitingQueue.empty())
        waitingQueue.pop();

    cout << "Signal " << signalID << " reset" << endl;
}

void TrafficSignal::printInfo() const
{
    cout << "\n--- Traffic Signal " << signalID << " Info ---" << endl;
    cout << "Intersection: " << intersectionID << endl;
    cout << "State: ";

    if (state == SignalState::RED)
        cout << "RED" << endl;
    else if (state == SignalState::GREEN)
        cout << "GREEN" << endl;

    cout << "Timer: " << timer << "s" << endl;
    cout << "Mode: " << (isAuto ? "AUTOMATIC" : "MANUAL") << endl;
    cout << "Queue Size: " << waitingQueue.size() << " vehicle(s)" << endl;
    cout << "Green Duration: " << greenDuration << "s" << endl;
    cout << "Red Duration: " << redDuration << "s" << endl;
    cout << "----------------------------\n" << endl;
}