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
        UndoManager undo;

        public Form1()
        {
            InitializeComponent();

            if (this.DesignMode)
                return;

            items = new ItemCollection();
            itemsController = new OpenGLManipulatingController();
            meshController = new OpenGLManipulatingController();
            undo = new UndoManager();

            openGLSceneViewLeft.CurrentCameraMode = CameraMode.CameraModeLeft;
            openGLSceneViewTop.CurrentCameraMode = CameraMode.CameraModeTop;
            openGLSceneViewFront.CurrentCameraMode = CameraMode.CameraModeFront;
            openGLSceneViewPerspective.CurrentCameraMode = CameraMode.CameraModePerspective;

            itemsController.Model = items;

            OnEachViewDo(view => view.CurrentManipulator = ManipulatorType.ManipulatorTypeDefault);
            
            itemsController.CurrentManipulator = openGLSceneViewLeft.CurrentManipulator;

            OnEachViewDo(view => view.Displayed = view.Manipulated = itemsController);
            
            // need fix bindings
            textBoxX.TextBox.BindString<float>("Text", itemsController, "SelectionX");
            textBoxY.TextBox.BindString<float>("Text", itemsController, "SelectionY");
            textBoxZ.TextBox.BindString<float>("Text", itemsController, "SelectionZ");
        }

        private void OnEachViewDo(Action<OpenGLSceneView> actionOnView)
        {
            actionOnView(openGLSceneViewLeft);
            actionOnView(openGLSceneViewTop);
            actionOnView(openGLSceneViewFront);
            actionOnView(openGLSceneViewPerspective);
        }

        private void SetManipulator(ManipulatorType manipulator)
        {
            OnEachViewDo(view => view.CurrentManipulator = manipulator);
            Manipulated.CurrentManipulator = manipulator;
            OnEachViewDo(view => view.Invalidate());
            
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
            get { return openGLSceneViewLeft.Manipulated; }
            set
            {
                value.CurrentManipulator = openGLSceneViewLeft.CurrentManipulator;

                OnEachViewDo(view => view.Manipulated = value);
                OnEachViewDo(view => view.Invalidate());
            }
        }

        Mesh CurrentMesh
        {
            get { return meshController.Model as Mesh; }
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
            Mesh mesh = CurrentMesh;
            if (mesh != null)
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
            OnEachViewDo(view => view.Invalidate());
            
            // simple test for undo/redo
            undo.PrepareUndo(Invocation.Create(this, t => t.UndoAddItem(item)));
        }

        private void UndoAddItem(Item item)
        {
            itemsController.ChangeSelection(0);
            items.RemoveItem(item);
            OnEachViewDo(view => view.Invalidate());
            
            // simple test for undo/redo
            undo.PrepareUndo(Invocation.Create(this, t => t.AddItem(item)));
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
            undo.Undo();  
        }   

        private void redoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            undo.Redo();
        }

        private void cloneToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Manipulated.CloneSelected();
            OnEachViewDo(view => view.Invalidate());
        }

        private void deleteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Manipulated.RemoveSelected();
            OnEachViewDo(view => view.Invalidate());
        }

        private void selectAllToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Manipulated.ChangeSelection(1);
            OnEachViewDo(view => view.Invalidate());
        }

        private void invertSelectionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Manipulated.InvertSelection();
            OnEachViewDo(view => view.Invalidate());
        }

        private void mergeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (Manipulated == itemsController)
                items.MergeSelectedItems();
            else
                CurrentMesh.MergeSelected();

            Manipulated.UpdateSelection();
            OnEachViewDo(view => view.Invalidate());
        }

        private void splitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (Manipulated == meshController)
                CurrentMesh.SplitSelected();

            Manipulated.UpdateSelection();
            OnEachViewDo(view => view.Invalidate());
        }

        private void mergeVertexPairsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (Manipulated == meshController)
            {
                CurrentMesh.MergeVertexPairs();
                Manipulated.UpdateSelection();
                OnEachViewDo(view => view.Invalidate());
            }
        }

        private void turnEdgesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (Manipulated == meshController)
            {
                CurrentMesh.TurnSelectedEdges();
                Manipulated.UpdateSelection();
                OnEachViewDo(view => view.Invalidate());
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
