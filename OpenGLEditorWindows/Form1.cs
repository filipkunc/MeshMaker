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
using System.Diagnostics;

namespace OpenGLEditorWindows
{
    public partial class Form1 : Form, OpenGLSceneViewDelegate
    {
        ItemCollection items;
        OpenGLManipulatingController itemsController;
        OpenGLManipulatingController meshController;
        UndoManager undo;

        PropertyObserver<float> observerSelectionX;
        PropertyObserver<float> observerSelectionY;
        PropertyObserver<float> observerSelectionZ;

        bool manipulationFinished;
        List<ItemManipulationState> oldManipulations;
        MeshManipulationState oldMeshManipulation;

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

            OnEachViewDo(view =>
                {
                    view.Displayed = view.Manipulated = itemsController;
                    view.TheDelegate = this;
                });

            textBoxX.TextBox.BindString<float>("Text", this, "SelectionX");
            textBoxY.TextBox.BindString<float>("Text", this, "SelectionY");
            textBoxZ.TextBox.BindString<float>("Text", this, "SelectionZ");

            BindSelectionXYZ(itemsController);
            BindSelectionXYZ(meshController);

            observerSelectionX = this.ObserveProperty<float>("SelectionX");
            observerSelectionY = this.ObserveProperty<float>("SelectionY");
            observerSelectionZ = this.ObserveProperty<float>("SelectionZ");

            manipulationFinished = true;
            oldManipulations = null;
            oldMeshManipulation = null;
        }

        #region Bindings magic

        void BindSelectionXYZ(OpenGLManipulatingController controller)
        {
            controller.ObserverSelectionX.WillChange += new EventHandler(ObserverSelectionX_WillChange);
            controller.ObserverSelectionY.WillChange += new EventHandler(ObserverSelectionY_WillChange);
            controller.ObserverSelectionZ.WillChange += new EventHandler(ObserverSelectionZ_WillChange);
            controller.ObserverSelectionX.DidChange += new EventHandler(ObserverSelectionX_DidChange);
            controller.ObserverSelectionY.DidChange += new EventHandler(ObserverSelectionY_DidChange);
            controller.ObserverSelectionZ.DidChange += new EventHandler(ObserverSelectionZ_DidChange);
        }

        void ObserverSelectionX_WillChange(object sender, EventArgs e)
        {
            observerSelectionX.RaiseWillChange();
        }

        void ObserverSelectionY_WillChange(object sender, EventArgs e)
        {
            observerSelectionY.RaiseWillChange();
        }

        void ObserverSelectionZ_WillChange(object sender, EventArgs e)
        {
            observerSelectionZ.RaiseWillChange();
        }

        void ObserverSelectionX_DidChange(object sender, EventArgs e)
        {
            observerSelectionX.RaiseDidChange();
        }

        void ObserverSelectionY_DidChange(object sender, EventArgs e)
        {
            observerSelectionY.RaiseDidChange();
        }

        void ObserverSelectionZ_DidChange(object sender, EventArgs e)
        {
            observerSelectionZ.RaiseDidChange();
        }

        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public float SelectionX
        {
            get { return Manipulated.SelectionX; }
            set
            {
                ManipulationStarted();
                Manipulated.SelectionX = value;
                ManipulationEnded();
            }
        }

        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public float SelectionY
        {
            get { return Manipulated.SelectionY; }
            set
            {
                ManipulationStarted();
                Manipulated.SelectionY = value;
                ManipulationEnded();
            }
        }

        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public float SelectionZ
        {
            get { return Manipulated.SelectionZ; }
            set
            {
                ManipulationStarted();
                Manipulated.SelectionZ = value;
                ManipulationEnded();
            }
        }

        #endregion

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

        OpenGLManipulatingController Manipulated
        {
            get { return openGLSceneViewLeft.Manipulated as OpenGLManipulatingController; }
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

        private void AddItem(MeshType type, uint steps)
        {
            Item item = new Item();
            item.GetMesh().MakeMesh(type, steps);

            itemsController.ChangeSelection(0);
            item.Selected = 1;
            items.AddItem(item);
            itemsController.UpdateSelection();
            OnEachViewDo(view => view.Invalidate());

            undo.PrepareUndo(Invocation.Create(type, steps, RemoveItem));
        }

        private void RemoveItem(MeshType type, uint steps)
        {
            itemsController.ChangeSelection(0);
            items.RemoveAt((int)items.Count - 1);
            OnEachViewDo(view => view.Invalidate());

            // simple test for undo/redo
            undo.PrepareUndo(Invocation.Create(type, steps, AddItem));
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
            AddItem(MeshType.MeshTypeCube, 1);
        }

        private void AddItemDialog(MeshType type)
        {
            using (AddItemWithStepsDialog dlg = new AddItemWithStepsDialog())
            {
                if (dlg.ShowDialog() == DialogResult.OK)
                {
                    AddItem(type, dlg.Steps);
                }
            }
        }

        private void btnAddCylinder_Click(object sender, EventArgs e)
        {
            AddItemDialog(MeshType.MeshTypeCylinder);
        }

        private void btnAddSphere_Click(object sender, EventArgs e)
        {
            AddItemDialog(MeshType.MeshTypeSphere);
        }

        private void undoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            undo.Undo();
        }

        private void redoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            undo.Redo();
        }

        void SwapManipulations(List<ItemManipulationState> old,
            List<ItemManipulationState> current)
        {
            Trace.WriteLine("swapManipulationsWithOld:current:");
            Trace.Assert(old.Count == current.Count, "old.Count == current.Count");
            items.CurrentManipulations = old;

            undo.PrepareUndo(Invocation.Create(current, old, SwapManipulations));

            itemsController.UpdateSelection();
            Manipulated = itemsController;
        }

        void SwapMeshManipulation(MeshManipulationState old,
            MeshManipulationState current)
        {
            Trace.WriteLine("swapMeshManipulationWithOld:current:");

            items.CurrentMeshManipulation = old;

            undo.PrepareUndo(Invocation.Create(current, old, SwapMeshManipulation));

            itemsController.UpdateSelection();
            meshController.UpdateSelection();
            Manipulated = meshController;
        }

        void SwapAllItems(List<Item> old, List<Item> current)
        {
            Trace.WriteLine("swapAllItemsWithOld:current:actionName:");

            Trace.WriteLine(string.Format("items count before set = {0}", items.Count));
            items.AllItems = old;
            Trace.WriteLine(string.Format("items count after set = {0}", items.Count));

            undo.PrepareUndo(Invocation.Create(current, old, SwapAllItems));

            itemsController.UpdateSelection();
            Manipulated = itemsController;
        }

        void SwapMeshFullState(MeshFullState old, MeshFullState current)
        {
            Trace.WriteLine("swapMeshFullStateWithOld:current:actionName:");

            items.CurrentMeshFull = old;

            undo.PrepareUndo(Invocation.Create(current, old, SwapMeshFullState));

            itemsController.UpdateSelection();
            meshController.UpdateSelection();
            Manipulated = meshController;
        }

        void AllItemsAction(Action action)
        {
            var oldItems = items.AllItems;
            Trace.WriteLine(string.Format("oldItems count = {0}", oldItems.Count));

            action();

            var currentItems = items.AllItems;
            Trace.WriteLine(string.Format("currentItems count = {0}", currentItems.Count));

            undo.PrepareUndo(Invocation.Create(oldItems, currentItems, SwapAllItems));
        }

        void FullMeshAction(Action action)
        {
            MeshFullState oldState = items.CurrentMeshFull;

            action();

            MeshFullState currentState = items.CurrentMeshFull;
            undo.PrepareUndo(Invocation.Create(oldState, currentState, SwapMeshFullState));
        }

        public void ManipulationStarted()
        {
            Trace.WriteLine("manipulationStarted");
            manipulationFinished = false;

            if (Manipulated == itemsController)
            {
                oldManipulations = items.CurrentManipulations;
            }
            else if (Manipulated == meshController)
            {
                oldMeshManipulation = items.CurrentMeshManipulation;
            }
        }

        public void ManipulationEnded()
        {
            Trace.WriteLine("manipulationEnded");
            manipulationFinished = true;

            if (Manipulated == itemsController)
            {
                undo.PrepareUndo(Invocation.Create(oldManipulations,
                    items.CurrentManipulations, SwapManipulations));

                oldManipulations = null;
            }
            else if (Manipulated == meshController)
            {
                undo.PrepareUndo(Invocation.Create(oldMeshManipulation,
                    items.CurrentMeshManipulation, SwapMeshManipulation));

                oldMeshManipulation = null;
            }
        }

        private void cloneToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Trace.WriteLine("cloneSelected:");
            if (Manipulated.SelectedCount <= 0)
                return;

            bool startManipulation = false;
            if (!manipulationFinished)
            {
                startManipulation = true;
                ManipulationEnded();
            }

            if (Manipulated == itemsController)
            {
                var selection = items.CurrentSelection;
                undo.PrepareUndo(Invocation.Create(selection, UndoCloneSelected));
                Manipulated.CloneSelected();
            }
            else
            {
                FullMeshAction(() => { Manipulated.CloneSelected(); });
            }

            OnEachViewDo(view => view.Invalidate());

            if (startManipulation)
            {
                ManipulationStarted();
            }
        }

        void RedoCloneSelected(List<uint> selection)
        {
            Trace.WriteLine("redoCloneSelected:");

            Manipulated = itemsController;
            items.CurrentSelection = selection;
            Manipulated.CloneSelected();

            undo.PrepareUndo(Invocation.Create(selection, UndoCloneSelected));
            itemsController.UpdateSelection();
            OnEachViewDo(view => view.Invalidate());
        }

        void UndoCloneSelected(List<uint> selection)
        {
            Trace.WriteLine("undoCloneSelected:");

            Manipulated = itemsController;
            items.RemoveRange((int)items.Count - selection.Count, selection.Count);
            items.CurrentSelection = selection;

            undo.PrepareUndo(Invocation.Create(selection, RedoCloneSelected));

            itemsController.UpdateSelection();
            OnEachViewDo(view => view.Invalidate());
        }

        private void deleteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Trace.WriteLine("deleteSelected:");
            if (Manipulated.SelectedCount <= 0)
                return;

            if (Manipulated == itemsController)
            {
                var currentItems = items.CurrentItems;
                undo.PrepareUndo(Invocation.Create(currentItems, UndoDeleteSelected));
                Manipulated.RemoveSelected();
            }
            else
            {
                FullMeshAction(() => { Manipulated.RemoveSelected(); });
            }

            OnEachViewDo(view => view.Invalidate());
        }

        void RedoDeleteSelected(List<IndexedItem> selectedItems)
        {
            Trace.WriteLine("redoDeleteSelected:");

            Manipulated = itemsController;
            items.SetSelectionFromIndexedItems(selectedItems);
            Manipulated.RemoveSelected();

            undo.PrepareUndo(Invocation.Create(selectedItems, UndoDeleteSelected));

            itemsController.UpdateSelection();
            OnEachViewDo(view => view.Invalidate());
        }

        void UndoDeleteSelected(List<IndexedItem> selectedItems)
        {
            Trace.WriteLine("undoDeleteSelected:");
            Manipulated = itemsController;
            items.CurrentItems = selectedItems;

            undo.PrepareUndo(Invocation.Create(selectedItems, RedoDeleteSelected));

            itemsController.UpdateSelection();
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
            Trace.WriteLine("mergeSelected:");
            if (Manipulated.SelectableCount <= 0)
                return;

            if (Manipulated == itemsController)
            {
                AllItemsAction(() => { items.MergeSelectedItems(); });
            }
            else
            {
                FullMeshAction(() => { CurrentMesh.MergeSelected(); });
            }

            Manipulated.UpdateSelection();
            OnEachViewDo(view => view.Invalidate());
        }

        private void splitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Trace.WriteLine("splitSelected:");
            if (Manipulated.SelectedCount <= 0)
                return;

            if (Manipulated == meshController)
            {
                FullMeshAction(() => { CurrentMesh.SplitSelected(); });
            }

            Manipulated.UpdateSelection();
            OnEachViewDo(view => view.Invalidate());
        }

        private void flipToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Trace.WriteLine("flipSelected:");
            if (Manipulated.SelectedCount <= 0)
                return;

            if (Manipulated == meshController)
            {
                FullMeshAction(() => { CurrentMesh.FlipSelected(); });
            }

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

        private void dropDownEditMode_DropDownItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            string tag = e.ClickedItem.Tag.ToString();
            int parsed = int.Parse(tag);
            dropDownEditMode.Text = e.ClickedItem.Text;
            ChangeEditMode((EditMode)parsed);
        }

        #region Splitter magic

        bool ignoreSplitterMoved = true;

        private void splitContainer2_SplitterMoved(object sender, SplitterEventArgs e)
        {
            if (ignoreSplitterMoved)
                return;

            splitContainer3.SplitterDistance = e.SplitX;
            ignoreSplitterMoved = true;
        }

        private void splitContainer3_SplitterMoved(object sender, SplitterEventArgs e)
        {
            if (ignoreSplitterMoved)
                return;

            splitContainer2.SplitterDistance = e.SplitX;
            ignoreSplitterMoved = true;
        }

        private void splitContainer2_SplitterMoving(object sender, SplitterCancelEventArgs e)
        {
            ignoreSplitterMoved = false;
        }

        private void splitContainer3_SplitterMoving(object sender, SplitterCancelEventArgs e)
        {
            ignoreSplitterMoved = false;
        }

        private void oneViewMenuItem_Click(object sender, EventArgs e)
        {
            splitContainer1.Panel1Collapsed = true;
            splitContainer3.Panel1Collapsed = true;
            oneViewMenuItem.Checked = true;
            fourViewsMenuItem.Checked = false;
        }

        private void fourViewsMenuItem_Click(object sender, EventArgs e)
        {
            splitContainer1.Panel1Collapsed = false;
            splitContainer3.Panel1Collapsed = false;
            oneViewMenuItem.Checked = false;
            fourViewsMenuItem.Checked = true;
        }

        #endregion
    }
}
