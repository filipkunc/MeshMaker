#pragma once

#include <string>
#include <stack>
#include <functional>
#include <memory>

// An undoable action stores a name and a function to perform the undo
class UndoableAction {
public:
    UndoableAction(const std::string& name, std::function<void()> action)
        : m_name(name), m_action(std::move(action)) {}
    
    const std::string& getName() const { return m_name; }
    void perform() const { m_action(); }
    
private:
    std::string m_name;
    std::function<void()> m_action;
};

// UndoManager maintains undo and redo stacks
class UndoManager {
public:
    UndoManager() = default;
    
    // Register an undo action. If we're currently undoing, it goes to redo stack.
    void prepareUndo(const std::string& name, std::function<void()> action) {
        auto undoableAction = std::make_unique<UndoableAction>(name, std::move(action));
        if (m_isUndoing) {
            m_redoStack.push(std::move(undoableAction));
        } else {
            m_undoStack.push(std::move(undoableAction));
            // Clear redo stack when a new action is registered (not during undo/redo)
            if (!m_isRedoing) {
                clearRedoStack();
            }
        }
    }
    
    bool canUndo() const { return !m_undoStack.empty(); }
    bool canRedo() const { return !m_redoStack.empty(); }
    
    std::string getUndoName() const {
        return canUndo() ? m_undoStack.top()->getName() : "";
    }
    
    std::string getRedoName() const {
        return canRedo() ? m_redoStack.top()->getName() : "";
    }
    
    void undo() {
        if (!canUndo()) return;
        
        auto action = std::move(m_undoStack.top());
        m_undoStack.pop();
        
        m_isUndoing = true;
        action->perform();
        m_isUndoing = false;
    }
    
    void redo() {
        if (!canRedo()) return;
        
        auto action = std::move(m_redoStack.top());
        m_redoStack.pop();
        
        // When redoing, the action will call prepareUndo which will push to undo stack
        // We set m_isRedoing to prevent clearing the remaining redo stack
        m_isRedoing = true;
        action->perform();
        m_isRedoing = false;
    }
    
    void clear() {
        clearUndoStack();
        clearRedoStack();
    }
    
    size_t getUndoCount() const { return m_undoStack.size(); }
    size_t getRedoCount() const { return m_redoStack.size(); }
    
private:
    void clearUndoStack() {
        while (!m_undoStack.empty()) m_undoStack.pop();
    }
    
    void clearRedoStack() {
        while (!m_redoStack.empty()) m_redoStack.pop();
    }
    
    std::stack<std::unique_ptr<UndoableAction>> m_undoStack;
    std::stack<std::unique_ptr<UndoableAction>> m_redoStack;
    bool m_isUndoing = false;
    bool m_isRedoing = false;
};
