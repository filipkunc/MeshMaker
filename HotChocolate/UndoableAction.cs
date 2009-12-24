using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HotChocolate
{
    public class UndoableAction
    {
        string name;
        Invocation invocation;

        public string Name { get { return name; } }

        public void Perform()
        {
            invocation.Perform();
        }

        public UndoableAction(string name, Invocation invocation)
        {
            this.name = name;
            this.invocation = invocation;
        }
    }
}
