using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using ManagedCpp;

namespace OpenGLEditorWindows
{
    public partial class Form1 : Form
    {
        ItemCollection items;
        OpenGLManipulatingController controller;

        public Form1()
        {
            InitializeComponent();

            items = new ItemCollection();
            Item item = new Item();
            item.GetMesh().MakeCube();
            items.AddItem(item);
            controller = new OpenGLManipulatingController();
            controller.Model = items;
            openGLSceneView1.CurrentManipulator = ManipulatorType.ManipulatorTypeTranslation;
            controller.CurrentManipulator = openGLSceneView1.CurrentManipulator;
            openGLSceneView1.Displayed = openGLSceneView1.Manipulated = controller;
        }
    }
}
