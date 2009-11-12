using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace OpenGLEditorWindows
{
    public partial class AddItemWithStepsDialog : Form
    {
        public AddItemWithStepsDialog()
        {
            InitializeComponent();
        }

        public uint Steps
        {
            get 
            {
                uint steps;
                if (uint.TryParse(textBoxSteps.Text, out steps))
                    return steps;
                return 5;
            }
        }
    }
}
