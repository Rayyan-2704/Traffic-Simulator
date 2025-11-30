#include "../include/ActionStack.h"
#include <iostream>
#include <stdexcept>

ActionStack::ActionStack(int maxCapacity) : maxSize(maxCapacity), totalTime(0.0f)
{
    cout << "ActionStack initialized with max capacity: " << maxSize << endl;
}

void ActionStack::push(const Action &action)
{
    if (history.size() >= maxSize)
    {
        cout << "Error: ActionStack is full! Cannot add more actions." << endl;
        return;
    }

    history.push(action);
    cout << "Action pushed to stack (Stack size: " << history.size() << ")" << endl;
}

Action ActionStack::pop()
{
    if (history.empty())
        throw runtime_error("Cannot pop from empty ActionStack!");

    Action action = history.top();
    history.pop();

    cout << "Action popped from stack (Stack size: " << history.size() << ")" << endl;
    return action;
}

Action ActionStack::peek() const
{
    if (history.empty())
        throw runtime_error("Cannot peek empty ActionStack!");

    return history.top();
}

bool ActionStack::canUndo() const { return !history.empty(); }

// Undo last action (same as pop, but with logging)
Action ActionStack::undo()
{
    if (!canUndo())
        throw runtime_error("Cannot undo - no actions in history!");

    Action action = history.top();
    history.pop();

    cout << "\n=== UNDO ACTION ===" << endl;

    switch (action.type)
    {
    case ActionType::TOGGLE_SIGNAL:
        cout << "Undoing signal toggle for Signal " << action.targetID << endl;
        cout << "Reverting from ";

        cout << ((action.newState == SignalState::RED) ? "RED" : "GREEN");
        cout << " back to ";
        cout << ((action.previousState == SignalState::RED) ? "RED" : "GREEN") << endl;

        break;

    case ActionType::SPAWN_VEHICLE:
        cout << "Undoing vehicle spawn (Vehicle ID: " << action.targetID << ")" << endl;
        break;
    }

    cout << "Timestamp: " << action.timestamp << "s" << endl;
    cout << "Stack size after undo: " << history.size() << endl;
    cout << "===================\n" << endl;

    return action;
}

void ActionStack::clear()
{
    while (!history.empty())
        history.pop();

    cout << "ActionStack cleared" << endl;
}

int ActionStack::size() const { return history.size(); }
bool ActionStack::isEmpty() const { return history.empty(); }
void ActionStack::setTime(float time) { totalTime = time; }

void ActionStack::printLastAction() const
{
    if (history.empty())
    {
        cout << "No actions in history" << endl;
        return;
    }

    const Action &action = history.top();
    cout << "Last action: ";

    if (action.type == ActionType::TOGGLE_SIGNAL)
    {
        cout << "Toggled Signal " << action.targetID << " at t=" << action.timestamp << "s" << endl;
    }
    else if (action.type == ActionType::SPAWN_VEHICLE)
    {
        cout << "Spawned Vehicle " << action.targetID << " at t=" << action.timestamp << "s" << endl;
    }
}