using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace MeshMaker
{
    public partial class AddItemWithStepsForm : Form
    {
        public AddItemWithStepsForm()
        {
            InitializeComponent();
            numericUpDownSteps.Select(0, numericUpDownSteps.Text.Length);
        }

        public uint steps
        {
            get { return (uint)numericUpDownSteps.Value; }
        }
    }
}
