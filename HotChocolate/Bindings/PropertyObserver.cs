using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;

namespace HotChocolate.Bindings
{
    public delegate T Getter<T>();
    public delegate void Setter<T>(T value);

    public interface IPropertyObserverBase
    {
        event EventHandler DidChange;
        void RaiseDidChange();

        object Target { get; }
        string Name { get; }
        void ClearAllListeners();
    }

    public interface IPropertyObserver<T> : IPropertyObserverBase
    {
        T Value { get; set; }
    }

    public sealed class PropertyObserver<T> : IPropertyObserver<T>
    {
        object target;
        string name;
        Getter<T> getter;
        Setter<T> setter;

        public event EventHandler WillChange;
        public event EventHandler DidChange;

        public PropertyObserver(object target, string name)
        {
            if (target == null)
                throw new ArgumentNullException("target");
            if (string.IsNullOrEmpty(name))
                throw new ArgumentNullException("name");

            this.target = target;
            this.name = name;

            Type t = target.GetType();
            PropertyInfo info = t.GetProperty(name, BindingFlags.Public | BindingFlags.Instance);
            var getterMi = info.GetGetMethod();
            var setterMi = info.GetSetMethod();

            getter = target.GetDelegate<Getter<T>>(getterMi.Name);
            if (setterMi != null)
                setter = target.GetDelegate<Setter<T>>(setterMi.Name);
            else
                setter = null;
        }

        public object Target
        {
            get { return target; }
        }

        public string Name
        {
            get { return name; }
        }

        public T Value
        {
            get
            {
                return getter();
            }
            set
            {
                if (setter == null)
                    return;

                T newValue = value;
                RaiseWillChange();
                setter(newValue);
                RaiseDidChange();
            }
        }

        public void RaiseWillChange()
        {
            if (WillChange != null)
                WillChange(this, EventArgs.Empty);
        }

        public void RaiseDidChange()
        {
            if (DidChange != null)
                DidChange(this, EventArgs.Empty);
        }

        public void ClearAllListeners()
        {
            DidChange = null;
            WillChange = null;
        }
    }
}
