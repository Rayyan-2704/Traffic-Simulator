#ifndef ACTIONSTACK_H
#define ACTIONSTACK_H

#include "Stack.h"
#include "TrafficSignal.h"
#include <string>
#include <iostream>
using namespace std;

enum class ActionType
{
    TOGGLE_SIGNAL,
    ADD_VEHICLE,
    CHANGE_SIGNAL_MODE
};

struct Action
{
    ActionType type;
    int targetID; // Signal ID or Vehicle ID
    SignalState previousState;
    bool previousAutoMode;
    string description;

    Action(ActionType t, int id, const string &desc = "")
        : type(t), targetID(id), previousState(SignalState::RED),
          previousAutoMode(false), description(desc) {}

    Action(ActionType t, int id, SignalState prevState, const string &desc = "")
        : type(t), targetID(id), previousState(prevState),
          previousAutoMode(false), description(desc) {}

    Action(ActionType t, int id, bool prevAuto, const string &desc = "")
        : type(t), targetID(id), previousState(SignalState::RED),
          previousAutoMode(prevAuto), description(desc) {}
};

class ActionStack
{
    Stack<Action> undoStack;
    int maxStackSize;

public:
    ActionStack(int maxSize = 50);

    void recordAction(const Action &action);
    bool canUndo() const;
    Action getLastAction();
    void popLastAction();
    int getStackSize() const;
    void clear();

    void printStack() const;
};

#endif
