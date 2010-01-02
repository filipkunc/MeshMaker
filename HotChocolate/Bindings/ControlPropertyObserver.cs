using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Reflection;

namespace HotChocolate.Bindings
{
    public class ControlPropertyObserver<T>
    {
        IPropertyObserver<T> observer;
        Getter<T> getter;
        Setter<T> setter;
        bool ignoreChange = false;

        public ControlPropertyObserver(object target, IPropertyObserver<T> observer, string name)
        {
            if (target == null)
                throw new ArgumentNullException("target");
            if (observer == null)
                throw new ArgumentNullException("observer");
            if (string.IsNullOrEmpty(name))
                throw new ArgumentNullException("name");

            Type t = target.GetType();

            this.observer = observer;

            PropertyInfo info = t.GetProperty(name, BindingFlags.Public | BindingFlags.Instance);
            var getterMi = info.GetGetMethod();
            var setterMi = info.GetSetMethod();

            getter = target.GetDelegate<Getter<T>>(getterMi.Name);
            setter = target.GetDelegate<Setter<T>>(setterMi.Name);
                
            setter(observer.Value);

            EventInfo ei = t.GetEvent(name + "Changed", BindingFlags.Instance | BindingFlags.Public);
            if (ei != null)
                ei.AddEventHandler(target, new EventHandler(ControlPropertyChanged));

            observer.DidChange += new EventHandler(observer_DidChange);
        }

        void observer_DidChange(object sender, EventArgs e)
        {
            ignoreChange = true;
            setter(observer.Value);
            ignoreChange = false;
        }

        private void ControlPropertyChanged(object sender, EventArgs e)
        {
            if (ignoreChange)
                return;

            observer.Value = getter();
        }
    }
}
