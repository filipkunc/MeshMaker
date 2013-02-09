using MeshMakerCppCLI;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Noesis.Javascript;

namespace MeshMaker
{
    public partial class ScriptEditor : Form
    {
        MyDocument document;

        class JSWindow
        {
            ScriptEditor _editor;

            public JSWindow(ScriptEditor editor) { _editor = editor; }

            public void alert(string text)
            {
                _editor.textBoxOutput.AppendText(text + Environment.NewLine);
            }
        }

        public ScriptEditor(MyDocument document)
        {
            InitializeComponent();
            this.document = document;
        }

        private void toolStripButtonNewScript_Click(object sender, EventArgs e)
        {

        }

        private void toolStripButtonRunScript_Click(object sender, EventArgs e)
        {
            textBoxOutput.Text = "Output:" + Environment.NewLine;

            document.runScriptAction(items =>
                {
                    using (JavascriptContext context = new JavascriptContext())
                    {
                        context.SetParameter("window", new JSWindow(this));
                        context.SetParameter("items", items);
                        context.Run(textBoxCode.Text);
                    }
                });
        }

        private void listBoxScripts_SelectedIndexChanged(object sender, EventArgs e)
        {

        }
    }
}
