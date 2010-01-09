using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace HotChocolate
{
    public class FloatTextBox : ToolStripTextBox
    {
        public float Number
        {
            get
            {
                float result = 0.0f;
                if (float.TryParse(Text, out result))
                    return result;
                return 0.0f;
            }
            set
            {
                Text = value.ToString();
            }
        }

        public event EventHandler NumberChanged;

        protected virtual void OnNumberChanged(EventArgs e)
        {
            if (NumberChanged != null)
                NumberChanged(this, e);
        }

        protected override void OnLeave(EventArgs e)
        {
            base.OnLeave(e);
            OnNumberChanged(EventArgs.Empty);
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Return)
                OnNumberChanged(EventArgs.Empty);
            base.OnKeyDown(e);
        }
    }
}
