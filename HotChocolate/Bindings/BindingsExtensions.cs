using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace HotChocolate.Bindings
{
    public static class BindingsExtensions
    {
        static Dictionary<object, ModelObserver> models = new Dictionary<object, ModelObserver>();

        public static ModelObserver Observe(this object target)
        {
            lock (typeof(BindingsExtensions))
            {
                ModelObserver model;
                if (!models.TryGetValue(target, out model))
                {
                    model = new ModelObserver(target);
                    models.Add(target, model);
                }
                return model;
            }
        }

        public static PropertyObserver<T> ObserveProperty<T>(this object target, string property)
        {
            return target.Observe().CreateOrGetObserver<T>(property);
        }

        public static void ClearAllListeners(this object target)
        {
            lock (typeof(BindingsExtensions))
            {
                ModelObserver model;
                if (models.TryGetValue(target, out model))
                    model.ClearAllListeners();
            }
        }

        public static void ClearAll(this object target)
        {
            lock (typeof(BindingsExtensions))
            {
                ModelObserver model;
                if (models.TryGetValue(target, out model))
                {
                    model.ClearAll();
                    models.Remove(target);
                }
            }
        }

        public static void Bind<T>(this Control control, string controlProperty, object model, string modelProperty)
        {
            var observer = model.ObserveProperty<T>(modelProperty);
            new ControlPropertyObserver<T>(control, observer, controlProperty);
        }

        public static void BindTransform<TModel, TView>(
            this Control control, string controlProperty,
            object model, string modelProperty,
            TransformPropertyObserver<TModel, TView> transform)
        {
            var observer = model.ObserveProperty<TModel>(modelProperty);
            transform.Init(observer);
            new ControlPropertyObserver<TView>(control, transform, controlProperty);
        }

        public static void BindString<T>(this Control control, string controlProperty, object model, string modelProperty)
        {
            BindTransform(control, controlProperty, model, modelProperty, new StringPropertyObserver<T>());
        }

        public static void BindTransformString<TModel, TTransformed>(
            this Control control, string controlProperty,
            object model, string modelProperty,
            TransformPropertyObserver<TModel, TTransformed> transform)
        {
            var observer = model.ObserveProperty<TModel>(modelProperty);
            transform.Init(observer);

            new ControlPropertyObserver<string>(control,
                new StringPropertyObserver<TTransformed>(transform),
                controlProperty);
        }

        public static void SetDependentProperties(this object target, params string[] dependencies)
        {
            var observer = target.Observe();
            observer.SetDependencies(dependencies);
        }
    }
}
