using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HotChocolate
{
    public class UndoManager
    {
        Stack<KeyValuePair<string, Invocation>> undoStack;
        Stack<KeyValuePair<string, Invocation>> redoStack;
        bool undoing;

        public UndoManager()
        {
            undoStack = new Stack<KeyValuePair<string, Invocation>>();
            redoStack = new Stack<KeyValuePair<string, Invocation>>();
            undoing = false;
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
            get { return undoStack.Peek().Key; }
        }

        public string RedoName
        {
            get { return redoStack.Peek().Key; }
        }

        public void PrepareUndo(string name, Invocation invocation)
        {
            if (undoing)
                redoStack.Push(new KeyValuePair<string, Invocation>(name, invocation));
            else
                undoStack.Push(new KeyValuePair<string, Invocation>(name, invocation));
        }

        public void Undo()
        {
            if (CanUndo)
            {
                var keyValuePair = undoStack.Pop();
                undoing = true;
                keyValuePair.Value.Perform();
                undoing = false;
            }
        }

        public void Redo()
        {
            if (CanRedo)
            {
                var keyValuePair = redoStack.Pop();
                keyValuePair.Value.Perform();
            }
        }
    }
}
