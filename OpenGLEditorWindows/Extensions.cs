using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ManagedCpp;

namespace OpenGLEditorWindows
{
    public static class Extensions
    {
        public static string ToDisplayString(this MeshType type)
        {
            return type.ToString().Replace("MeshType", "");
        }
    }
}
