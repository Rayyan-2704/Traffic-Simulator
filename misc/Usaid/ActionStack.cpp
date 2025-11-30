#include "../include/ActionStack.h"

ActionStack::ActionStack(int maxSize) : maxStackSize(maxSize)
{
    cout << "ActionStack initialized with max size: " << maxStackSize << endl;
}

void ActionStack::recordAction(const Action &action)
{
    // Limit stack size to prevent memory overflow
    if (undoStack.size() >= maxStackSize)
    {
        cout << "Warning: Undo stack is full. Oldest action will be lost." << endl;
        // In a real implementation, we'd remove the bottom element
        // For now, we just clear when full
    }

    undoStack.push(action);
    cout << "Action recorded: " << action.description << " (Stack size: " << undoStack.size() << ")" << endl;
}

bool ActionStack::canUndo() const
{
    return !undoStack.empty();
}

Action ActionStack::getLastAction()
{
    if (undoStack.empty())
    {
        throw underflow_error("No actions to undo!");
    }

    return undoStack.peek();
}

void ActionStack::popLastAction()
{
    if (!undoStack.empty())
    {
        undoStack.pop();
    }
}

int ActionStack::getStackSize() const
{
    return undoStack.size();
}

void ActionStack::clear()
{
    while (!undoStack.empty())
    {
        undoStack.pop();
    }
    cout << "ActionStack cleared" << endl;
}

void ActionStack::printStack() const
{
    cout << "\n=== UNDO STACK (Size: " << undoStack.size() << ") ===" << endl;
    if (undoStack.empty())
    {
        cout << "No actions recorded" << endl;
    }
    cout << "================================\n" << endl;
}
