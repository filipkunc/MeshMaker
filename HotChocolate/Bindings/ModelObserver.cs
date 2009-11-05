using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HotChocolate.Bindings
{
    public class ModelObserver
    {
        Dictionary<string, IPropertyObserverBase> observers;
        Dictionary<string, string[]> dependencies;
        object target;
        bool ignoreDidChange = false;

        public ModelObserver(object target)
        {
            this.target = target;
            observers = new Dictionary<string, IPropertyObserverBase>();
            dependencies = new Dictionary<string, string[]>();
        }

        public PropertyObserver<T> CreateOrGetObserver<T>(string property)
        {
            IPropertyObserverBase observer;
            PropertyObserver<T> newObserver;
            if (!observers.TryGetValue(property, out observer))
            {
                newObserver = new PropertyObserver<T>(target, property);
                observers.Add(property, newObserver);
            }
            else
            {
                newObserver = (PropertyObserver<T>)observer;
            }
            newObserver.DidChange += new EventHandler(newObserver_DidChange);
            return newObserver;
        }

        
        void newObserver_DidChange(object sender, EventArgs e)
        {
            if (ignoreDidChange)
                return;

            ignoreDidChange = true;

            IPropertyObserverBase observer = (IPropertyObserverBase)sender;
            string[] dependentProperties;
            if (dependencies.TryGetValue(observer.Name, out dependentProperties))
            {
                foreach (string property in dependentProperties)
                {
                    IPropertyObserverBase dependentObserver;
                    if (observers.TryGetValue(property, out dependentObserver) && observer != dependentObserver)
                        dependentObserver.RaiseDidChange();
                }
            }

            ignoreDidChange = false;
        }

        public void SetDependencies(string[] names)
        {
            foreach (string dependency in names)
            {
                this.dependencies[dependency] = names;
            }
        }

        public void ClearAllListeners()
        {
            foreach (var observer in observers.Values)
            {
                observer.ClearAllListeners();
            }
        }

        public void ClearAll()
        {
            ClearAllListeners();
            observers.Clear();
        }
    }
}
