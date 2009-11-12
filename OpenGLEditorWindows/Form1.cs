using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using ManagedCpp;
using HotChocolate;
using HotChocolate.Bindings;

namespace OpenGLEditorWindows
{
    public partial class Form1 : Form
    {
        ItemCollection items;
        OpenGLManipulatingController itemsController;
        OpenGLManipulatingController meshController;

        public Form1()
        {
            InitializeComponent();

            if (this.DesignMode)
                return;

            items = new ItemCollection();
            itemsController = new OpenGLManipulatingController();
            meshController = new OpenGLManipulatingController();
            itemsController.Model = items;
            openGLSceneView1.CurrentManipulator = ManipulatorType.ManipulatorTypeDefault;
            itemsController.CurrentManipulator = openGLSceneView1.CurrentManipulator;
            openGLSceneView1.Displayed = openGLSceneView1.Manipulated = itemsController;

            // need fix bindings
            textBoxX.TextBox.BindString<float>("Text", itemsController, "SelectionX");
            textBoxY.TextBox.BindString<float>("Text", itemsController, "SelectionY");
            textBoxZ.TextBox.BindString<float>("Text", itemsController, "SelectionZ");
        }

        private void SetManipulator(ManipulatorType manipulator)
        {
            openGLSceneView1.CurrentManipulator = manipulator;
            Manipulated.CurrentManipulator = manipulator;
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

        OpenGLManipulating Manipulated
        {
            get { return openGLSceneView1.Manipulated; }
            set
            {
                value.CurrentManipulator = openGLSceneView1.CurrentManipulator;
                openGLSceneView1.Manipulated = value;
                openGLSceneView1.Invalidate();
            }
        }

        Mesh CurrentMesh
        {
            get { return (Mesh)meshController.Model; }
        }

        private void EditMesh(MeshSelectionMode mode)
        {
            int index = itemsController.LastSelectedIndex;
            if (index > -1)
            {
                Item item = items.GetItem((uint)index);
                item.GetMesh().SelectionMode = mode;
                meshController.Model = item.GetMesh();
                meshController.SetTransform(item);
                Manipulated = meshController;
            }
        }

        private void EditItems()
        {
            Mesh mesh = (Mesh)meshController.Model;
            mesh.SelectionMode = MeshSelectionMode.MeshSelectionModeVertices;
            itemsController.Model = items;
            itemsController.SetTransform(null);
            Manipulated = itemsController;
        }

        private void ChangeEditMode(EditMode editMode)
        {
            switch (editMode)
            {
                case EditMode.EditModeItems:
                    EditItems();
                    break;
                case EditMode.EditModeTriangles:
                    EditMesh(MeshSelectionMode.MeshSelectionModeTriangles);
                    break;
                case EditMode.EditModeVertices:
                    EditMesh(MeshSelectionMode.MeshSelectionModeVertices);
                    break;
                case EditMode.EditModeEdges:
                    EditMesh(MeshSelectionMode.MeshSelectionModeEdges);
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
            using (AddItemWithStepsDialog dlg = new AddItemWithStepsDialog())
            {
                if (dlg.ShowDialog() == DialogResult.OK)
                {
                    item.GetMesh().MakeCylinder(dlg.Steps);
                    AddItem(item);
                }
            }
        }

        private void btnAddSphere_Click(object sender, EventArgs e)
        {
            Item item = new Item();
            using (AddItemWithStepsDialog dlg = new AddItemWithStepsDialog())
            {
                if (dlg.ShowDialog() == DialogResult.OK)
                {
                    item.GetMesh().MakeSphere(dlg.Steps);
                    AddItem(item);
                }
            }
        }

        private void undoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show("Undo is not implemented yet");   
        }   

        private void redoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show("Redo is not implemented yet");
        }

        private void cloneToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Manipulated.CloneSelected();
            openGLSceneView1.Invalidate();
        }

        private void deleteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Manipulated.RemoveSelected();
            openGLSceneView1.Invalidate();
        }

        private void selectAllToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Manipulated.ChangeSelection(1);
            openGLSceneView1.Invalidate();
        }

        private void invertSelectionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Manipulated.InvertSelection();
            openGLSceneView1.Invalidate();
        }

        private void mergeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (Manipulated == itemsController)
                items.MergeSelectedItems();
            else
                CurrentMesh.MergeSelected();
                
            Manipulated.UpdateSelection();
            openGLSceneView1.Invalidate();
        }

        private void splitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (Manipulated == meshController)
                CurrentMesh.SplitSelected();

            Manipulated.UpdateSelection();
            openGLSceneView1.Invalidate();
        }

        private void mergeVertexPairsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (Manipulated == meshController)
            {
                CurrentMesh.MergeVertexPairs();
                Manipulated.UpdateSelection();
                openGLSceneView1.Invalidate();
            }
        }

        private void turnEdgesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (Manipulated == meshController)
            {
                CurrentMesh.TurnSelectedEdges();
                Manipulated.UpdateSelection();
                openGLSceneView1.Invalidate();
            }
        }

        private void dropDownEditMode_DropDownItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            string tag = e.ClickedItem.Tag.ToString();
            int parsed = int.Parse(tag);
            dropDownEditMode.Text = e.ClickedItem.Text;
            ChangeEditMode((EditMode)parsed);
        }
    }
}
