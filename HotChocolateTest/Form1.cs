using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using HotChocolate;
using HotChocolate.Bindings;

namespace HotChocolateTest
{
    public partial class Form1 : Form
    {
        SimpleModel model = new SimpleModel();

        public Form1()
        {
            InitializeComponent();
            if (this.DesignMode)
                return;

            trackBar1.Bind<int>("Value", model, "Value");
            trackBar2.Bind<int>("Value", model, "Value");
            textBox1.BindString<int>("Text", model, "Value");
        }

        delegate int SomeDelegate(int a, int b);

        private void button1_Click(object sender, EventArgs e)
        {
            var del = this.GetDelegate<SomeDelegate>("Add");
            MessageBox.Show("Delegate test 10 + 20 = " + del(10, 20).ToString());
        }

        int Add(int a, int b)
        {
            return a + b;
        }
    }

    public class SimpleModel
    {
        int modelValue;
        public int Value 
        {
            get
            {
                return modelValue;
            }
            set
            {
                modelValue = value;
                modelValue = Math.Min(10, modelValue);
                modelValue = Math.Max(0, modelValue);
            }
        }

        public SimpleModel()
        {
            Value = 5;
        }
    }
}
