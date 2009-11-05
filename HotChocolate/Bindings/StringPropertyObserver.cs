using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HotChocolate.Bindings
{
    public sealed class StringPropertyObserver<TModel> : TransformPropertyObserver<TModel, string>
    {
        TStringConverter<TModel> stringConverter = TStringConverter<TModel>.Default;

        public StringPropertyObserver()
        {

        }

        public StringPropertyObserver(IPropertyObserver<TModel> observer)
            : base(observer)
        {
        }

        protected override string GetTransform(TModel value)
        {
            return stringConverter.ToString(value);
        }

        protected override bool SetTransform(string value, out TModel transformed)
        {
            return stringConverter.TryParse(value, out transformed);
        }
    }
}
