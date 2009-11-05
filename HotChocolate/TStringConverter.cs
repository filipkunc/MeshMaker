using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Globalization;

namespace HotChocolate
{
    public abstract class TStringConverter<T>
    {
        static DefaultTStringConverter<T> defaultConverter = null;

        public static TStringConverter<T> Default
        {
            get
            {
                if (defaultConverter == null)
                    defaultConverter = new DefaultTStringConverter<T>();
                return defaultConverter;
            }
        }

        public static CultureInfo DefaultCulture
        {
            get { return CultureInfo.CurrentUICulture; }
        }

        public abstract string ToString(T obj, IFormatProvider provider);
        public abstract T Parse(string s, IFormatProvider provider);

        public string ToString(T obj)
        {
            return ToString(obj, DefaultCulture);
        }

        public T Parse(string s)
        {
            return Parse(s, DefaultCulture);
        }

        public bool TryParse(string s, out T result, IFormatProvider provider)
        {
            try
            {
                result = Parse(s, provider);
                return true;
            }
            catch
            {
                result = default(T);
                return false;
            }
        }

        public bool TryParse(string s, out T result)
        {
            return TryParse(s, out result, DefaultCulture);
        }
    }
}
