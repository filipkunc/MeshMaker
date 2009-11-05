using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HotChocolate.Bindings
{
    public abstract class TransformPropertyObserver<TModel, TView> : IPropertyObserver<TView>
    {
        IPropertyObserver<TModel> observer = null;

        public event EventHandler DidChange;

        public TransformPropertyObserver()
        {

        }

        public TransformPropertyObserver(IPropertyObserver<TModel> observer)
        {
            Init(observer);
        }

        public void Init(IPropertyObserver<TModel> observer)
        {
            if (observer == null)
                throw new ArgumentNullException("observer");

            this.observer = observer;
            observer.DidChange += new EventHandler(observer_DidChange);
        }

        void observer_DidChange(object sender, EventArgs e)
        {
            if (DidChange != null)
                DidChange(sender, EventArgs.Empty);
        }

        protected abstract TView GetTransform(TModel value);
        protected abstract bool SetTransform(TView value, out TModel transformed);

        protected IPropertyObserver<TModel> ModelObserver
        {
            get { return observer; }
        }

        public object Target
        {
            get { return observer.Target; }
        }

        public string Name
        {
            get { return observer.Name; }
        }

        public TView Value
        {
            get
            {
                return GetTransform(observer.Value);
            }
            set
            {
                TModel transformed;
                if (SetTransform(value, out transformed))
                    observer.Value = transformed;
            }
        }

        public void ClearAllListeners()
        {
            observer.ClearAllListeners();
        }

        public void RaiseDidChange()
        {
            observer.RaiseDidChange();
        }
    }
}
