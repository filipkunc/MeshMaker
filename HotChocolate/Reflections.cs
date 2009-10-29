using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;
using System.ComponentModel;

namespace HotChocolate
{
    public static class Reflections
    {
        public static TDelegate GetStaticDelegate<TDelegate, TStatic>(string name) where TDelegate : class
        {
            return Delegate.CreateDelegate(typeof(TDelegate), typeof(TStatic), name) as TDelegate;
        }

        public static TDelegate GetDelegate<TDelegate>(this object target, string name) where TDelegate : class
        {   
            return Delegate.CreateDelegate(typeof(TDelegate), target, name) as TDelegate;
        }
    }
}
