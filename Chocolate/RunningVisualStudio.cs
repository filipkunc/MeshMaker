using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

namespace Chocolate
{
    public class RunningVisualStudio
    {
        public bool IsRunningVisualStudio { get; private set; }

        public RunningVisualStudio()
        {
            Process currentProcess = Process.GetCurrentProcess();
            if (string.Equals(currentProcess.ProcessName.ToLowerInvariant(), "devenv")) //!-!
                IsRunningVisualStudio = true;
            else
                IsRunningVisualStudio = false;            
        }
    }
}
