using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HotChocolate
{
    public delegate T ParseDelegate<T>(string s, IFormatProvider provider);

    public class DefaultTStringConverter<T> : TStringConverter<T>
    {
        bool isString;
        bool isEnum;
        ParseDelegate<T> parse;

        public DefaultTStringConverter()
        {
            Type type = typeof(T);

            if (type == typeof(string))
                isString = true;
            else if (type.IsEnum)
                isEnum = true;
            else
            {
                isEnum = false;
                parse = Reflections.GetStaticDelegate<ParseDelegate<T>, T>("Parse");
            }
        }

        public override string ToString(T obj, IFormatProvider provider)
        {
            if (isString)
                return obj.ToString();
            IFormattable formattable = obj as IFormattable;
            if (formattable != null)
                return formattable.ToString(null, provider);
            return obj.ToString();
        }

        public override T Parse(string s, IFormatProvider provider)
        {
            if (isString)
            {
                object obj = s;
                return (T)obj;
            }
            if (isEnum)
                return (T)Enum.Parse(typeof(T), s);
            return parse(s, provider);
        }
    }
}
