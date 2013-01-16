using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Chocolate
{
    public class Invocation
    {
        Delegate _action;
        object[] _arguments;

        public Invocation(Delegate action, params object[] arguments)
        {
            _action = action;
            _arguments = arguments;
        }

        public void Perform()
        {
            _action.DynamicInvoke(_arguments);
        }       
    }
}
