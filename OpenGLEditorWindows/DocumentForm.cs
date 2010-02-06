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
using System.IO;
using WeifenLuo.WinFormsUI.Docking;

namespace OpenGLEditorWindows
{
    public partial class DocumentForm : Form, OpenGLSceneViewDelegate
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
        List<OpenGLSceneView> views;

        string lastFileName = null;
        string fileDialogFilter = "Native format (*.model3D)|*.model3D" +
                                  "|Bullet (*.bullet)|*.bullet";

        OpenGLSceneView openGLSceneViewLeft = null;
        OpenGLSceneView openGLSceneViewTop = null;
        OpenGLSceneView openGLSceneViewFront = null;
        OpenGLSceneView openGLSceneViewPerspective = null;

        SplitContainer mainSplit = null;
        SplitContainer topSplit = null;
        SplitContainer bottomSplit = null;

        PropertyGrid propertyGrid = null;
        TextBox logTextBox = null;
        TreeView sceneGraphTreeView = null;

        DockFourViews fourViewDock = null;
        DockPropertyPanel propertyPanel = null;
        DockHierarchyPanel hierarchyPanel = null;
        DockLogPanel logPanel = null;

        StringWriter logWriter;

        ExperimentalBulletWrapper bulletWrapper = null;
        Timer simulationTimer = null;
        
        public DocumentForm()
        {
            InitializeComponent();

            if (this.DesignMode)
                return;

            items = new ItemCollection();
            itemsController = new OpenGLManipulatingController();
            meshController = new OpenGLManipulatingController();
            undo = new UndoManager();

            fourViewDock = new DockFourViews();
            propertyPanel = new DockPropertyPanel();
            hierarchyPanel = new DockHierarchyPanel();
            logPanel = new DockLogPanel();

            openGLSceneViewLeft = fourViewDock.openGLSceneViewLeft;
            openGLSceneViewTop = fourViewDock.openGLSceneViewTop;
            openGLSceneViewFront = fourViewDock.openGLSceneViewFront;
            openGLSceneViewPerspective = fourViewDock.openGLSceneViewPerspective;

            mainSplit = fourViewDock.mainSplit;
            topSplit = fourViewDock.topSplit;
            bottomSplit = fourViewDock.bottomSplit;

            logTextBox = logPanel.logTextBox;
            sceneGraphTreeView = hierarchyPanel.sceneGraphTreeView;
            propertyGrid = propertyPanel.propertyGrid;

            logWriter = new StringWriter();
            Trace.Listeners.Add(new TextWriterTraceListener(logWriter));
            
            logTextBox.Text = logWriter.ToString();
            
            topSplit.SplitterMoving += new SplitterCancelEventHandler(topSplit_SplitterMoving);
            bottomSplit.SplitterMoving += new SplitterCancelEventHandler(bottomSplit_SplitterMoving);

            topSplit.SplitterMoved += new SplitterEventHandler(topSplit_SplitterMoved);
            bottomSplit.SplitterMoved += new SplitterEventHandler(bottomSplit_SplitterMoved);

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

            textBoxX.Bind<float>("Number", this, "SelectionX");
            textBoxY.Bind<float>("Number", this, "SelectionY");
            textBoxZ.Bind<float>("Number", this, "SelectionZ");

            BindSelectionXYZ(itemsController);
            BindSelectionXYZ(meshController);

            observerSelectionX = this.ObserveProperty<float>("SelectionX");
            observerSelectionY = this.ObserveProperty<float>("SelectionY");
            observerSelectionZ = this.ObserveProperty<float>("SelectionZ");

            manipulationFinished = true;
            oldManipulations = null;
            oldMeshManipulation = null;

            undo.NeedsSaveChanged += new EventHandler(undo_NeedsSaveChanged);
            this.FormClosing += new FormClosingEventHandler(Form1_FormClosing);

            views = new List<OpenGLSceneView>();
            OnEachViewDo(view => views.Add(view));

            this.KeyDown += new KeyEventHandler(DocumentForm_KeyDown);

            fourViewDock.Show(dockPanel1);
            propertyPanel.Show(dockPanel1);
            hierarchyPanel.Show(dockPanel1);
            logPanel.Show(dockPanel1);

            dockPanel1.DockLeftPortion = 0.15;   // 15 percent of dock space
            dockPanel1.DockRightPortion = 0.15;  // 25 percent is default

            Manipulated = itemsController;
            propertyGrid.PropertyValueChanged += new PropertyValueChangedEventHandler(propertyGrid_PropertyValueChanged);

            simulationTimer = new Timer();
            simulationTimer.Interval = 1000 / 60;
            simulationTimer.Tick += new EventHandler(simulationTimer_Tick);
            simulationTimer.Start();
        }

        void simulationTimer_Tick(object sender, EventArgs e)
        {
            if (bulletWrapper != null)
            {
                bulletWrapper.StepSimulation(1.0f / 60.0f);
                OnEachViewDo(view => view.Invalidate());
            }
        }

        void propertyGrid_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            OnEachViewDo(view => view.Invalidate());
        }

        bool IsSaveQuestionCancelled()
        {
            if (undo.NeedsSave)
            {
                DialogResult result = MessageBox.Show(
                    "Do you want to save this document?",
                    "Question",
                    MessageBoxButtons.YesNoCancel);

                switch (result)
                {
                    case DialogResult.Cancel:
                        return true;
                    case DialogResult.Yes:
                        saveToolStripMenuItem_Click(this, EventArgs.Empty);
                        return false;
                    case DialogResult.No:
                    default:
                        return false;
                }
            }
            return false;
        }

        void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (IsSaveQuestionCancelled())
                e.Cancel = true;
        }

        void undo_NeedsSaveChanged(object sender, EventArgs e)
        {
            StringBuilder formTitle = new StringBuilder();
            formTitle.Append("OpenGL Editor - ");
            if (string.IsNullOrEmpty(lastFileName))
                formTitle.Append("Untitled");
            else
                formTitle.Append(Path.GetFileNameWithoutExtension(lastFileName));
            if (undo.NeedsSave)
                formTitle.Append(" *");
            this.Text = formTitle.ToString();

            logTextBox.Text = logWriter.ToString();
            if (logTextBox.Text.Length - 1 >= 0)
            {
                logTextBox.Select(logTextBox.Text.Length - 1, 0);
                logTextBox.ScrollToCaret();
            }
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
                ManipulationStarted(null);
                Manipulated.SelectionX = value;
                ManipulationEnded(null);
            }
        }

        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public float SelectionY
        {
            get { return Manipulated.SelectionY; }
            set
            {
                ManipulationStarted(null);
                Manipulated.SelectionY = value;
                ManipulationEnded(null);
            }
        }

        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public float SelectionZ
        {
            get { return Manipulated.SelectionZ; }
            set
            {
                ManipulationStarted(null);
                Manipulated.SelectionZ = value;
                ManipulationEnded(null);
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
                propertyGrid.SelectedObject = value;

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

            undo.PrepareUndo(string.Format("Add {0}", type.ToDisplayString()),
                Invocation.Create(type, steps, RemoveItem));
        }

        private void RemoveItem(MeshType type, uint steps)
        {
            itemsController.ChangeSelection(0);
            items.RemoveAt((int)items.Count - 1);
            OnEachViewDo(view => view.Invalidate());

            // simple test for undo/redo
            undo.PrepareUndo(string.Format("Add {0}", type.ToDisplayString()),
                Invocation.Create(type, steps, AddItem));
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

            undo.PrepareUndo("Manipulations",
                Invocation.Create(current, old, SwapManipulations));

            itemsController.UpdateSelection();
            Manipulated = itemsController;
        }

        void SwapMeshManipulation(MeshManipulationState old,
            MeshManipulationState current)
        {
            Trace.WriteLine("swapMeshManipulationWithOld:current:");

            items.CurrentMeshManipulation = old;

            undo.PrepareUndo("Mesh Manipulation",
                Invocation.Create(current, old, SwapMeshManipulation));

            itemsController.UpdateSelection();
            meshController.UpdateSelection();
            Manipulated = meshController;
        }

        void SwapAllItems(List<Item> old, List<Item> current, string actionName)
        {
            Trace.WriteLine("swapAllItemsWithOld:current:actionName:");

            Trace.WriteLine(string.Format("items count before set = {0}", items.Count));
            items.AllItems = old;
            Trace.WriteLine(string.Format("items count after set = {0}", items.Count));

            undo.PrepareUndo(actionName,
                Invocation.Create(current, old, actionName, SwapAllItems));

            itemsController.UpdateSelection();
            Manipulated = itemsController;
        }

        void SwapMeshFullState(MeshFullState old, MeshFullState current, string actionName)
        {
            Trace.WriteLine("swapMeshFullStateWithOld:current:actionName:");

            items.CurrentMeshFull = old;

            undo.PrepareUndo(actionName,
                Invocation.Create(current, old, actionName, SwapMeshFullState));

            itemsController.UpdateSelection();
            meshController.UpdateSelection();
            Manipulated = meshController;
        }

        void AllItemsAction(string actionName, Action action)
        {
            var oldItems = items.AllItems;
            Trace.WriteLine(string.Format("oldItems count = {0}", oldItems.Count));

            action();

            var currentItems = items.AllItems;
            Trace.WriteLine(string.Format("currentItems count = {0}", currentItems.Count));

            undo.PrepareUndo(actionName,
                Invocation.Create(oldItems, currentItems, actionName, SwapAllItems));
        }

        void FullMeshAction(string actionName, Action action)
        {
            MeshFullState oldState = items.CurrentMeshFull;

            action();

            MeshFullState currentState = items.CurrentMeshFull;
            undo.PrepareUndo(actionName,
                Invocation.Create(oldState, currentState, actionName, SwapMeshFullState));
        }

        public void ManipulationStarted(OpenGLSceneView view)
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

        public void ManipulationEnded(OpenGLSceneView view)
        {
            Trace.WriteLine("manipulationEnded");
            manipulationFinished = true;

            if (Manipulated == itemsController)
            {
                undo.PrepareUndo("Manipulations",
                    Invocation.Create(oldManipulations,
                        items.CurrentManipulations, SwapManipulations));

                oldManipulations = null;
            }
            else if (Manipulated == meshController)
            {
                undo.PrepareUndo("Mesh Manipulation",
                    Invocation.Create(oldMeshManipulation,
                        items.CurrentMeshManipulation, SwapMeshManipulation));

                oldMeshManipulation = null;
            }

            propertyGrid.Refresh();

            OnEachViewDo(v => { if (v != view) v.Invalidate(); });
        }

        public void SelectionChanged(OpenGLSceneView view)
        {
            propertyGrid.Refresh();
            //propertyGrid.SelectedObject = this.items;
            OnEachViewDo(v => { if (v != view) v.Invalidate(); });
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
                ManipulationEnded(null);
            }

            if (Manipulated == itemsController)
            {
                var selection = items.CurrentSelection;
                undo.PrepareUndo("Clone",
                    Invocation.Create(selection, UndoCloneSelected));

                Manipulated.CloneSelected();
            }
            else
            {
                FullMeshAction("Clone", () => Manipulated.CloneSelected());
            }

            OnEachViewDo(view => view.Invalidate());

            if (startManipulation)
            {
                ManipulationStarted(null);
            }
        }

        void RedoCloneSelected(List<uint> selection)
        {
            Trace.WriteLine("redoCloneSelected:");

            Manipulated = itemsController;
            items.CurrentSelection = selection;
            Manipulated.CloneSelected();

            undo.PrepareUndo("Clone",
                Invocation.Create(selection, UndoCloneSelected));

            itemsController.UpdateSelection();
            OnEachViewDo(view => view.Invalidate());
        }

        void UndoCloneSelected(List<uint> selection)
        {
            Trace.WriteLine("undoCloneSelected:");

            Manipulated = itemsController;
            items.RemoveRange((int)items.Count - selection.Count, selection.Count);
            items.CurrentSelection = selection;

            undo.PrepareUndo("Clone",
                Invocation.Create(selection, RedoCloneSelected));

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
                undo.PrepareUndo("Delete",
                    Invocation.Create(currentItems, UndoDeleteSelected));

                Manipulated.RemoveSelected();
            }
            else
            {
                FullMeshAction("Delete", () => Manipulated.RemoveSelected());
            }

            OnEachViewDo(view => view.Invalidate());
        }

        void RedoDeleteSelected(List<IndexedItem> selectedItems)
        {
            Trace.WriteLine("redoDeleteSelected:");

            Manipulated = itemsController;
            items.SetSelectionFromIndexedItems(selectedItems);
            Manipulated.RemoveSelected();

            undo.PrepareUndo("Delete",
                Invocation.Create(selectedItems, UndoDeleteSelected));

            itemsController.UpdateSelection();
            OnEachViewDo(view => view.Invalidate());
        }

        void UndoDeleteSelected(List<IndexedItem> selectedItems)
        {
            Trace.WriteLine("undoDeleteSelected:");
            Manipulated = itemsController;
            items.CurrentItems = selectedItems;

            undo.PrepareUndo("Delete",
                Invocation.Create(selectedItems, RedoDeleteSelected));

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
                AllItemsAction("Merge", () => items.MergeSelectedItems());
            }
            else
            {
                FullMeshAction("Merge", () => CurrentMesh.MergeSelected());
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
                FullMeshAction("Split", () => CurrentMesh.SplitSelected());
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
                FullMeshAction("Flip", () => CurrentMesh.FlipSelected());
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

        private void editToolStripMenuItem_DropDownOpening(object sender, EventArgs e)
        {
            undoToolStripMenuItem.Text = undo.CanUndo ?
                "Undo " + undo.UndoName : "Undo";

            redoToolStripMenuItem.Text = undo.CanRedo ?
                "Redo " + undo.RedoName : "Redo";

            undoToolStripMenuItem.Enabled = undo.CanUndo;
            redoToolStripMenuItem.Enabled = undo.CanRedo;
        }

        #region Splitter sync

        bool ignoreSplitterMoved = true;

        private void topSplit_SplitterMoved(object sender, SplitterEventArgs e)
        {
            if (ignoreSplitterMoved)
                return;

            bottomSplit.SplitterDistance = e.SplitX;
            ignoreSplitterMoved = true;
        }

        private void bottomSplit_SplitterMoved(object sender, SplitterEventArgs e)
        {
            if (ignoreSplitterMoved)
                return;

            topSplit.SplitterDistance = e.SplitX;
            ignoreSplitterMoved = true;
        }

        private void topSplit_SplitterMoving(object sender, SplitterCancelEventArgs e)
        {
            ignoreSplitterMoved = false;
        }

        private void bottomSplit_SplitterMoving(object sender, SplitterCancelEventArgs e)
        {
            ignoreSplitterMoved = false;
        }

        private void toggleOneViewFourViewMenuItem_Click(object sender, EventArgs e)
        {
            // one view -> four views
            if (mainSplit.Panel1Collapsed)
            {
                mainSplit.Panel1Collapsed = false;
                bottomSplit.Panel1Collapsed = false;
                openGLSceneViewPerspective.CurrentCameraMode = CameraMode.CameraModePerspective;
            }
            else // four views -> one view
            {
                Point screenPoint = Control.MousePosition;
                foreach (OpenGLSceneView view in views)
                {
                    Point clientPoint = view.PointToClient(screenPoint);
                    if (view.ClientRectangle.Contains(clientPoint))
                    {
                        openGLSceneViewPerspective.CurrentCameraMode = view.CurrentCameraMode;
                        mainSplit.Panel1Collapsed = true;
                        bottomSplit.Panel1Collapsed = true;
                        return;
                    }
                }

                Trace.WriteLine("No view is under mouse");
            }
        }

        #endregion

        #region File menu

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (IsSaveQuestionCancelled())
                return;

            items = new ItemCollection();
            itemsController.Model = items;
            itemsController.UpdateSelection();
            undo.Clear();
            OnEachViewDo(view => view.Invalidate());
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (IsSaveQuestionCancelled())
                return;

            using (OpenFileDialog dlg = new OpenFileDialog())
            {
                dlg.Filter = fileDialogFilter;
                dlg.RestoreDirectory = true;
                if (dlg.ShowDialog() != DialogResult.OK)
                    return;

                lastFileName = dlg.FileName;
            }

            if (Path.GetExtension(lastFileName) == ".bullet")
            {
                bulletWrapper = new ExperimentalBulletWrapper(lastFileName);
                items = null;
                itemsController.Model = bulletWrapper;
            }
            else
            {
                items = new ItemCollection();
                items.ReadFromFile(lastFileName);
                itemsController.Model = items;
            }
            itemsController.UpdateSelection();
            undo.Clear();
            OnEachViewDo(view => view.Invalidate());
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(lastFileName))
            {
                using (SaveFileDialog dlg = new SaveFileDialog())
                {
                    dlg.Filter = fileDialogFilter;
                    dlg.RestoreDirectory = true;
                    if (dlg.ShowDialog() != DialogResult.OK)
                        return;

                    lastFileName = dlg.FileName;
                }
            }
            items.WriteToFile(lastFileName);
            undo.DocumentSaved();
        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (SaveFileDialog dlg = new SaveFileDialog())
            {
                dlg.Filter = fileDialogFilter;
                dlg.RestoreDirectory = true;
                if (dlg.ShowDialog() != DialogResult.OK)
                    return;

                lastFileName = dlg.FileName;
            }
            items.WriteToFile(lastFileName);
            undo.DocumentSaved();
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        #endregion

        private void manipulatorToolStripMenuItem_DropDownItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            int tag = int.Parse(e.ClickedItem.Tag.ToString());
            SetManipulator((ManipulatorType)tag);
        }

        #region Keyboard

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            // this allows to process keys in DocumentForm_KeyDown
            foreach (var view in views)
            {
                if (view.Focused)
                    base.ProcessKeyMessage(ref msg);
            }

            // but we also allow standard processing
            return base.ProcessCmdKey(ref msg, keyData);
        }

        void DocumentForm_KeyDown(object sender, KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.Space:
                    toggleOneViewFourViewMenuItem_Click(this, EventArgs.Empty);
                    break;
                case Keys.Q:
                    manipulatorToolStripMenuItem_DropDownItemClicked(this,
                        new ToolStripItemClickedEventArgs(selectToolStripMenuItem));
                    break;
                case Keys.W:
                    manipulatorToolStripMenuItem_DropDownItemClicked(this,
                        new ToolStripItemClickedEventArgs(translateToolStripMenuItem));
                    break;
                case Keys.E:
                    manipulatorToolStripMenuItem_DropDownItemClicked(this,
                        new ToolStripItemClickedEventArgs(rotateToolStripMenuItem));
                    break;
                case Keys.R:
                    manipulatorToolStripMenuItem_DropDownItemClicked(this,
                        new ToolStripItemClickedEventArgs(scaleToolStripMenuItem));
                    break;
            }
        }
      
        #endregion
    }
}
