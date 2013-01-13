using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Chocolate
{
    public static class Tools
    {
        static RunningVisualStudio _runningVS = null;

        static RunningVisualStudio RunningVS
        {
            get
            {
                if (_runningVS != null)
                    return _runningVS;

                lock (typeof(Tools))
                {
                    if (_runningVS == null)
                        _runningVS = new RunningVisualStudio();
                    return _runningVS;
                }
            }
        }

        public static bool SafeDesignMode
        {
            get
            {
                return RunningVS.IsRunningVisualStudio;
            }
        }

    }
}
