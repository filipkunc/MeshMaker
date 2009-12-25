using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HotChocolate
{
    public sealed class UndoManager
    {
        Stack<UndoableAction> undoStack;
        Stack<UndoableAction> redoStack;
        bool undoing;
        int lastUndoStackCount;

        public event EventHandler NeedsSaveChanged;

        public UndoManager()
        {
            undoStack = new Stack<UndoableAction>();
            redoStack = new Stack<UndoableAction>();
            undoing = false;
            lastUndoStackCount = 0;
        }

        public bool NeedsSave
        {
            get 
            {
                if (CanUndo && lastUndoStackCount != undoStack.Count)
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
            OnNeedsSaveChanged();
        }

        public void Undo()
        {
            if (CanUndo)
            {
                var undoableAction = undoStack.Pop();
                undoing = true;
                undoableAction.Perform();
                undoing = false;
                OnNeedsSaveChanged();
            }
        }

        public void Redo()
        {
            if (CanRedo)
            {
                var undoableAction = redoStack.Pop();
                undoableAction.Perform();
                OnNeedsSaveChanged();
            }
        }

        public void Clear()
        {
            undoStack.Clear();
            redoStack.Clear();
            undoing = false;
            lastUndoStackCount = undoStack.Count;
            OnNeedsSaveChanged();
        }

        public void DocumentSaved()
        {
            lastUndoStackCount = undoStack.Count;
            OnNeedsSaveChanged();
        }

        private void OnNeedsSaveChanged()
        {
            if (NeedsSaveChanged != null)
                NeedsSaveChanged(this, EventArgs.Empty);
        }
    }
}
