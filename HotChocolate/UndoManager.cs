using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HotChocolate
{
    public class UndoManager
    {
        Stack<Invocation> undoStack;
        Stack<Invocation> redoStack;
        bool undoing;

        public UndoManager()
        {
            undoStack = new Stack<Invocation>();
            redoStack = new Stack<Invocation>();
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

        public void PrepareUndo(Invocation invocation)
        {
            if (undoing)
                redoStack.Push(invocation);
            else
                undoStack.Push(invocation);
        }

        public void Undo()
        {
            if (CanUndo)
            {
                Invocation invocation = undoStack.Pop();
                undoing = true;
                invocation.Perform();
                undoing = false;
            }
        }

        public void Redo()
        {
            if (CanRedo)
            {
                Invocation invocation = redoStack.Pop();
                invocation.Perform();
            }
        }
    }
}
