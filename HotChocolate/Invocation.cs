using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HotChocolate
{
    public class Invocation
    {
        object argument;
        Delegate action;

        public Invocation(object argument, Delegate action)
        {
            this.argument = argument;
            this.action = action;
        }

        public void Perform()
        {
            action.DynamicInvoke(argument);
        }

        public static Invocation Create<T>(T argument,
            Action<T> action)
        {
            return new Invocation(argument, action);
        }

        public static Invocation Create<T1, T2>(T1 arg1, T2 arg2,
            Action<T1, T2> action)
        {
            return Create(new { A1 = arg1, A2 = arg2 },
                args => action(args.A1, args.A2));
        }

        public static Invocation Create<T1, T2, T3>(T1 arg1, T2 arg2, T3 arg3,
            Action<T1, T2, T3> action)
        {
            return Create(new { A1 = arg1, A2 = arg2, A3 = arg3 },
                args => action(args.A1, args.A2, args.A3));
        }

        public static Invocation Create<T1, T2, T3, T4>(T1 arg1, T2 arg2, T3 arg3, T4 arg4,
            Action<T1, T2, T3, T4> action)
        {
            return Create(new { A1 = arg1, A2 = arg2, A3 = arg3, A4 = arg4 },
                args => action(args.A1, args.A2, args.A3, args.A4));
        }
    }
}
