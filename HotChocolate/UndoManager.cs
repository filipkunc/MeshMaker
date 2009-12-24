using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HotChocolate
{
    public class UndoManager
    {
        Stack<UndoableAction> undoStack;
        Stack<UndoableAction> redoStack;
        bool undoing;
        UndoableAction lastSavedAction;

        public UndoManager()
        {
            undoStack = new Stack<UndoableAction>();
            redoStack = new Stack<UndoableAction>();
            undoing = false;
            lastSavedAction = null;
        }

        public bool NeedsSave
        {
            get 
            {
                if (CanUndo && lastSavedAction == undoStack.Peek())
                    return true;
                return false;
            }
        }

        public bool CanUndo
        {
            get { return undoStack.Count > 0; }
        }

        public bool CanRedo
        {
            get { return redoStack.Count > 0; }
        }

        public string UndoName
        {
            get { return undoStack.Peek().Name; }
        }

        public string RedoName
        {
            get { return redoStack.Peek().Name; }
        }

        public void PrepareUndo(string name, Invocation invocation)
        {
            UndoableAction undoableAction = new UndoableAction(name, invocation);
            if (undoing)
                redoStack.Push(undoableAction);
            else
                undoStack.Push(undoableAction);
        }

        public void Undo()
        {
            if (CanUndo)
            {
                var undoableAction = undoStack.Pop();
                undoing = true;
                undoableAction.Perform();
                undoing = false;
            }
        }

        public void Redo()
        {
            if (CanRedo)
            {
                var undoableAction = redoStack.Pop();
                undoableAction.Perform();
            }
        }

        public void Clear()
        {
            undoStack.Clear();
            redoStack.Clear();
            undoing = false;
        }

        public void DocumentSaved()
        {
            if (CanUndo)
                lastSavedAction = undoStack.Peek();
            else
                lastSavedAction = null;
        }
    }
}
