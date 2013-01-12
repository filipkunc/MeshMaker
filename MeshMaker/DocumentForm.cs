using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MeshMakerCppCLI;

namespace MeshMaker
{
    public partial class DocumentForm : Form
    {
        MyDocument document;

        public DocumentForm()
        {
            InitializeComponent();
            this.Load += new EventHandler(DocumentForm_Load);
        }

        void DocumentForm_Load(object sender, EventArgs e)
        {
            if (DesignMode)
                return;

            document = new MyDocument(sceneView);
        }
    }
}
