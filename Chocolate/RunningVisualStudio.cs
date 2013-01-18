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
            var currentProcessName = currentProcess.ProcessName.ToLowerInvariant();
            if (string.Equals(currentProcessName, "devenv") ||
                string.Equals(currentProcessName, "wdexpress"))
                IsRunningVisualStudio = true;
            else
                IsRunningVisualStudio = false;
        }
    }
}
