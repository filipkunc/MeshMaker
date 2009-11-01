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
        OpenGLManipulatingController itemsController;

        public Form1()
        {
            InitializeComponent();

            items = new ItemCollection();
            itemsController = new OpenGLManipulatingController();
            itemsController.Model = items;
            openGLSceneView1.CurrentManipulator = ManipulatorType.ManipulatorTypeDefault;
            itemsController.CurrentManipulator = openGLSceneView1.CurrentManipulator;
            openGLSceneView1.Displayed = openGLSceneView1.Manipulated = itemsController;
        }

        private void SetManipulator(ManipulatorType manipulator)
        {
            openGLSceneView1.CurrentManipulator = manipulator;
            itemsController.CurrentManipulator = manipulator;
            openGLSceneView1.Invalidate();

            btnSelect.Checked = btnTranslate.Checked = btnRotate.Checked = btnScale.Checked = false;
            switch (manipulator)
            {
                case ManipulatorType.ManipulatorTypeDefault:
                    btnSelect.Checked = true;
                    break;
                case ManipulatorType.ManipulatorTypeRotation:
                    btnRotate.Checked = true;
                    break;
                case ManipulatorType.ManipulatorTypeScale:
                    btnScale.Checked = true;
                    break;
                case ManipulatorType.ManipulatorTypeTranslation:
                    btnTranslate.Checked = true;
                    break;
                default:
                    break;
            }
        }

        private void AddItem(Item item)
        {
            itemsController.ChangeSelection(0);
            item.Selected = 1;
            items.AddItem(item);
            itemsController.UpdateSelection();
            openGLSceneView1.Invalidate();
        }

        private void btnSelect_Click(object sender, EventArgs e)
        {
            SetManipulator(ManipulatorType.ManipulatorTypeDefault);
        }

        private void btnTranslate_Click(object sender, EventArgs e)
        {
            SetManipulator(ManipulatorType.ManipulatorTypeTranslation);
        }

        private void btnRotate_Click(object sender, EventArgs e)
        {
            SetManipulator(ManipulatorType.ManipulatorTypeRotation);
        }

        private void btnScale_Click(object sender, EventArgs e)
        {
            SetManipulator(ManipulatorType.ManipulatorTypeScale);
        }

        private void btnAddCube_Click(object sender, EventArgs e)
        {
            Item item = new Item();
            item.GetMesh().MakeCube();
            AddItem(item);
        }

        private void btnAddCylinder_Click(object sender, EventArgs e)
        {
            Item item = new Item();
            item.GetMesh().MakeCylinder(5);
            AddItem(item);
        }

        private void btnAddSphere_Click(object sender, EventArgs e)
        {
            Item item = new Item();
            item.GetMesh().MakeSphere(5);
            AddItem(item);
        }

        private void undoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            
        }

        private void redoToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void cloneToolStripMenuItem_Click(object sender, EventArgs e)
        {
            itemsController.CloneSelected();
            openGLSceneView1.Invalidate();
        }

        private void deleteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            itemsController.RemoveSelected();
            openGLSceneView1.Invalidate();
        }

        private void selectAllToolStripMenuItem_Click(object sender, EventArgs e)
        {
            itemsController.ChangeSelection(1);
            openGLSceneView1.Invalidate();
        }

        private void invertSelectionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            itemsController.InvertSelection();
            openGLSceneView1.Invalidate();
        }

        private void mergeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            items.MergeSelectedItems();
            itemsController.UpdateSelection();
            openGLSceneView1.Invalidate();
        }

        private void splitToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void mergeVertexPairsToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void turnEdgesToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }
    }
}
