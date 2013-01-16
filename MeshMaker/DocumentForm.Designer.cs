namespace MeshMaker
{
    partial class DocumentForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(DocumentForm));
            this.frontView = new MeshMakerCppCLI.OpenGLSceneView();
            this.mainSplit = new System.Windows.Forms.SplitContainer();
            this.topSplit = new System.Windows.Forms.SplitContainer();
            this.leftView = new MeshMakerCppCLI.OpenGLSceneView();
            this.topView = new MeshMakerCppCLI.OpenGLSceneView();
            this.bottomSplit = new System.Windows.Forms.SplitContainer();
            this.perspectiveView = new MeshMakerCppCLI.OpenGLSceneView();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveAsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.editToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.undoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.redoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator6 = new System.Windows.Forms.ToolStripSeparator();
            this.duplicateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.deleteToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator7 = new System.Windows.Forms.ToolStripSeparator();
            this.selectAllToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.invertSelectionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator8 = new System.Windows.Forms.ToolStripSeparator();
            this.hideSelectedToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.unhideAllToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator9 = new System.Windows.Forms.ToolStripSeparator();
            this.addToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.planeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.cubeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.cylinderToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.sphereToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.icosahedronToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator10 = new System.Windows.Forms.ToolStripSeparator();
            this.mergeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.splitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.flipToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.subdivisionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.detachToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.extrudeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.triangulateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.viewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toggleOneViewFourViewsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripTop = new System.Windows.Forms.ToolStrip();
            this.toolStripButtonSelect = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonTranslate = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonRotate = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonScale = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripButtonAddPlane = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonAddCube = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonAddCylinder = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonAddSphere = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonAddIcosahedron = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripComboBoxEditMode = new System.Windows.Forms.ToolStripComboBox();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripButtonSplit = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonMerge = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonSubdivide = new System.Windows.Forms.ToolStripButton();
            this.toolStripBottom = new System.Windows.Forms.ToolStrip();
            this.toolStripLabel4 = new System.Windows.Forms.ToolStripLabel();
            this.toolStripComboBoxViewMode = new System.Windows.Forms.ToolStripComboBox();
            this.toolStripSeparator11 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripLabel5 = new System.Windows.Forms.ToolStripLabel();
            this.toolStripButtonColor = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator12 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripLabel1 = new System.Windows.Forms.ToolStripLabel();
            this.toolStripTextBoxSelectionX = new System.Windows.Forms.ToolStripTextBox();
            this.toolStripLabel2 = new System.Windows.Forms.ToolStripLabel();
            this.toolStripTextBoxSelectionY = new System.Windows.Forms.ToolStripTextBox();
            this.toolStripLabel3 = new System.Windows.Forms.ToolStripLabel();
            this.toolStripTextBoxSelectionZ = new System.Windows.Forms.ToolStripTextBox();
            this.mainSplit.Panel1.SuspendLayout();
            this.mainSplit.Panel2.SuspendLayout();
            this.mainSplit.SuspendLayout();
            this.topSplit.Panel1.SuspendLayout();
            this.topSplit.Panel2.SuspendLayout();
            this.topSplit.SuspendLayout();
            this.bottomSplit.Panel1.SuspendLayout();
            this.bottomSplit.Panel2.SuspendLayout();
            this.bottomSplit.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.toolStripTop.SuspendLayout();
            this.toolStripBottom.SuspendLayout();
            this.SuspendLayout();
            // 
            // frontView
            // 
            this.frontView.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.frontView.CurrentCameraMode = CameraMode.Perspective;
            this.frontView.Delegate = null;
            this.frontView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.frontView.Location = new System.Drawing.Point(0, 0);
            this.frontView.Name = "frontView";
            this.frontView.SharedContextView = null;
            this.frontView.Size = new System.Drawing.Size(395, 165);
            this.frontView.TabIndex = 0;
            // 
            // mainSplit
            // 
            this.mainSplit.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mainSplit.Location = new System.Drawing.Point(0, 78);
            this.mainSplit.Name = "mainSplit";
            this.mainSplit.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // mainSplit.Panel1
            // 
            this.mainSplit.Panel1.Controls.Add(this.topSplit);
            // 
            // mainSplit.Panel2
            // 
            this.mainSplit.Panel2.Controls.Add(this.bottomSplit);
            this.mainSplit.Size = new System.Drawing.Size(843, 400);
            this.mainSplit.SplitterDistance = 231;
            this.mainSplit.TabIndex = 1;
            // 
            // topSplit
            // 
            this.topSplit.Dock = System.Windows.Forms.DockStyle.Fill;
            this.topSplit.Location = new System.Drawing.Point(0, 0);
            this.topSplit.Name = "topSplit";
            // 
            // topSplit.Panel1
            // 
            this.topSplit.Panel1.Controls.Add(this.leftView);
            // 
            // topSplit.Panel2
            // 
            this.topSplit.Panel2.Controls.Add(this.topView);
            this.topSplit.Size = new System.Drawing.Size(843, 231);
            this.topSplit.SplitterDistance = 395;
            this.topSplit.TabIndex = 2;
            this.topSplit.SplitterMoving += new System.Windows.Forms.SplitterCancelEventHandler(this.topSplit_SplitterMoving);
            this.topSplit.SplitterMoved += new System.Windows.Forms.SplitterEventHandler(this.topSplit_SplitterMoved);
            // 
            // leftView
            // 
            this.leftView.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.leftView.CurrentCameraMode = CameraMode.Perspective;
            this.leftView.Delegate = null;
            this.leftView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.leftView.Location = new System.Drawing.Point(0, 0);
            this.leftView.Name = "leftView";
            this.leftView.SharedContextView = null;
            this.leftView.Size = new System.Drawing.Size(395, 231);
            this.leftView.TabIndex = 0;
            // 
            // topView
            // 
            this.topView.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.topView.CurrentCameraMode = CameraMode.Perspective;
            this.topView.Delegate = null;
            this.topView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.topView.Location = new System.Drawing.Point(0, 0);
            this.topView.Name = "topView";
            this.topView.SharedContextView = null;
            this.topView.Size = new System.Drawing.Size(444, 231);
            this.topView.TabIndex = 0;
            // 
            // bottomSplit
            // 
            this.bottomSplit.Dock = System.Windows.Forms.DockStyle.Fill;
            this.bottomSplit.Location = new System.Drawing.Point(0, 0);
            this.bottomSplit.Name = "bottomSplit";
            // 
            // bottomSplit.Panel1
            // 
            this.bottomSplit.Panel1.Controls.Add(this.frontView);
            // 
            // bottomSplit.Panel2
            // 
            this.bottomSplit.Panel2.Controls.Add(this.perspectiveView);
            this.bottomSplit.Size = new System.Drawing.Size(843, 165);
            this.bottomSplit.SplitterDistance = 395;
            this.bottomSplit.TabIndex = 2;
            this.bottomSplit.SplitterMoving += new System.Windows.Forms.SplitterCancelEventHandler(this.bottomSplit_SplitterMoving);
            this.bottomSplit.SplitterMoved += new System.Windows.Forms.SplitterEventHandler(this.bottomSplit_SplitterMoved);
            // 
            // perspectiveView
            // 
            this.perspectiveView.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.perspectiveView.CurrentCameraMode = CameraMode.Perspective;
            this.perspectiveView.Delegate = null;
            this.perspectiveView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.perspectiveView.Location = new System.Drawing.Point(0, 0);
            this.perspectiveView.Name = "perspectiveView";
            this.perspectiveView.SharedContextView = null;
            this.perspectiveView.Size = new System.Drawing.Size(444, 165);
            this.perspectiveView.TabIndex = 0;
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.editToolStripMenuItem,
            this.viewToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(843, 24);
            this.menuStrip1.TabIndex = 2;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newToolStripMenuItem,
            this.openToolStripMenuItem,
            this.toolStripSeparator4,
            this.saveToolStripMenuItem,
            this.saveAsToolStripMenuItem,
            this.toolStripSeparator5,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // newToolStripMenuItem
            // 
            this.newToolStripMenuItem.Name = "newToolStripMenuItem";
            this.newToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.N)));
            this.newToolStripMenuItem.Size = new System.Drawing.Size(155, 22);
            this.newToolStripMenuItem.Text = "New";
            this.newToolStripMenuItem.Click += new System.EventHandler(this.newToolStripMenuItem_Click);
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
            this.openToolStripMenuItem.Size = new System.Drawing.Size(155, 22);
            this.openToolStripMenuItem.Text = "Open...";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(152, 6);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(155, 22);
            this.saveToolStripMenuItem.Text = "Save";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // saveAsToolStripMenuItem
            // 
            this.saveAsToolStripMenuItem.Name = "saveAsToolStripMenuItem";
            this.saveAsToolStripMenuItem.Size = new System.Drawing.Size(155, 22);
            this.saveAsToolStripMenuItem.Text = "Save As...";
            this.saveAsToolStripMenuItem.Click += new System.EventHandler(this.saveAsToolStripMenuItem_Click);
            // 
            // toolStripSeparator5
            // 
            this.toolStripSeparator5.Name = "toolStripSeparator5";
            this.toolStripSeparator5.Size = new System.Drawing.Size(152, 6);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(155, 22);
            this.exitToolStripMenuItem.Text = "Exit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // editToolStripMenuItem
            // 
            this.editToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.undoToolStripMenuItem,
            this.redoToolStripMenuItem,
            this.toolStripSeparator6,
            this.duplicateToolStripMenuItem,
            this.deleteToolStripMenuItem,
            this.toolStripSeparator7,
            this.selectAllToolStripMenuItem,
            this.invertSelectionToolStripMenuItem,
            this.toolStripSeparator8,
            this.hideSelectedToolStripMenuItem,
            this.unhideAllToolStripMenuItem,
            this.toolStripSeparator9,
            this.addToolStripMenuItem,
            this.toolStripSeparator10,
            this.mergeToolStripMenuItem,
            this.splitToolStripMenuItem,
            this.flipToolStripMenuItem,
            this.subdivisionToolStripMenuItem,
            this.detachToolStripMenuItem,
            this.extrudeToolStripMenuItem,
            this.triangulateToolStripMenuItem});
            this.editToolStripMenuItem.Name = "editToolStripMenuItem";
            this.editToolStripMenuItem.Size = new System.Drawing.Size(39, 20);
            this.editToolStripMenuItem.Text = "Edit";
            // 
            // undoToolStripMenuItem
            // 
            this.undoToolStripMenuItem.Name = "undoToolStripMenuItem";
            this.undoToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Z)));
            this.undoToolStripMenuItem.Size = new System.Drawing.Size(209, 22);
            this.undoToolStripMenuItem.Text = "Undo";
            this.undoToolStripMenuItem.Click += new System.EventHandler(this.undoToolStripMenuItem_Click);
            // 
            // redoToolStripMenuItem
            // 
            this.redoToolStripMenuItem.Name = "redoToolStripMenuItem";
            this.redoToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Y)));
            this.redoToolStripMenuItem.Size = new System.Drawing.Size(209, 22);
            this.redoToolStripMenuItem.Text = "Redo";
            this.redoToolStripMenuItem.Click += new System.EventHandler(this.redoToolStripMenuItem_Click);
            // 
            // toolStripSeparator6
            // 
            this.toolStripSeparator6.Name = "toolStripSeparator6";
            this.toolStripSeparator6.Size = new System.Drawing.Size(206, 6);
            // 
            // duplicateToolStripMenuItem
            // 
            this.duplicateToolStripMenuItem.Name = "duplicateToolStripMenuItem";
            this.duplicateToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.D)));
            this.duplicateToolStripMenuItem.Size = new System.Drawing.Size(209, 22);
            this.duplicateToolStripMenuItem.Text = "Duplicate";
            this.duplicateToolStripMenuItem.Click += new System.EventHandler(this.duplicateToolStripMenuItem_Click);
            // 
            // deleteToolStripMenuItem
            // 
            this.deleteToolStripMenuItem.Name = "deleteToolStripMenuItem";
            this.deleteToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteToolStripMenuItem.Size = new System.Drawing.Size(209, 22);
            this.deleteToolStripMenuItem.Text = "Delete";
            this.deleteToolStripMenuItem.Click += new System.EventHandler(this.deleteToolStripMenuItem_Click);
            // 
            // toolStripSeparator7
            // 
            this.toolStripSeparator7.Name = "toolStripSeparator7";
            this.toolStripSeparator7.Size = new System.Drawing.Size(206, 6);
            // 
            // selectAllToolStripMenuItem
            // 
            this.selectAllToolStripMenuItem.Name = "selectAllToolStripMenuItem";
            this.selectAllToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.A)));
            this.selectAllToolStripMenuItem.Size = new System.Drawing.Size(209, 22);
            this.selectAllToolStripMenuItem.Text = "Select All";
            this.selectAllToolStripMenuItem.Click += new System.EventHandler(this.selectAllToolStripMenuItem_Click);
            // 
            // invertSelectionToolStripMenuItem
            // 
            this.invertSelectionToolStripMenuItem.Name = "invertSelectionToolStripMenuItem";
            this.invertSelectionToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.I)));
            this.invertSelectionToolStripMenuItem.Size = new System.Drawing.Size(209, 22);
            this.invertSelectionToolStripMenuItem.Text = "Invert Selection";
            this.invertSelectionToolStripMenuItem.Click += new System.EventHandler(this.invertSelectionToolStripMenuItem_Click);
            // 
            // toolStripSeparator8
            // 
            this.toolStripSeparator8.Name = "toolStripSeparator8";
            this.toolStripSeparator8.Size = new System.Drawing.Size(206, 6);
            // 
            // hideSelectedToolStripMenuItem
            // 
            this.hideSelectedToolStripMenuItem.Name = "hideSelectedToolStripMenuItem";
            this.hideSelectedToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.H)));
            this.hideSelectedToolStripMenuItem.Size = new System.Drawing.Size(209, 22);
            this.hideSelectedToolStripMenuItem.Text = "Hide Selected";
            this.hideSelectedToolStripMenuItem.Click += new System.EventHandler(this.hideSelectedToolStripMenuItem_Click);
            // 
            // unhideAllToolStripMenuItem
            // 
            this.unhideAllToolStripMenuItem.Name = "unhideAllToolStripMenuItem";
            this.unhideAllToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.U)));
            this.unhideAllToolStripMenuItem.Size = new System.Drawing.Size(209, 22);
            this.unhideAllToolStripMenuItem.Text = "Unhide All";
            this.unhideAllToolStripMenuItem.Click += new System.EventHandler(this.unhideAllToolStripMenuItem_Click);
            // 
            // toolStripSeparator9
            // 
            this.toolStripSeparator9.Name = "toolStripSeparator9";
            this.toolStripSeparator9.Size = new System.Drawing.Size(206, 6);
            // 
            // addToolStripMenuItem
            // 
            this.addToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.planeToolStripMenuItem,
            this.cubeToolStripMenuItem,
            this.cylinderToolStripMenuItem,
            this.sphereToolStripMenuItem,
            this.icosahedronToolStripMenuItem});
            this.addToolStripMenuItem.Name = "addToolStripMenuItem";
            this.addToolStripMenuItem.Size = new System.Drawing.Size(209, 22);
            this.addToolStripMenuItem.Text = "Add";
            // 
            // planeToolStripMenuItem
            // 
            this.planeToolStripMenuItem.Image = global::MeshMaker.Properties.Resources.PlaneTemplate;
            this.planeToolStripMenuItem.Name = "planeToolStripMenuItem";
            this.planeToolStripMenuItem.Size = new System.Drawing.Size(139, 22);
            this.planeToolStripMenuItem.Text = "Plane";
            this.planeToolStripMenuItem.Click += new System.EventHandler(this.addItem);
            // 
            // cubeToolStripMenuItem
            // 
            this.cubeToolStripMenuItem.Image = global::MeshMaker.Properties.Resources.CubeTemplate;
            this.cubeToolStripMenuItem.Name = "cubeToolStripMenuItem";
            this.cubeToolStripMenuItem.Size = new System.Drawing.Size(139, 22);
            this.cubeToolStripMenuItem.Text = "Cube";
            this.cubeToolStripMenuItem.Click += new System.EventHandler(this.addItem);
            // 
            // cylinderToolStripMenuItem
            // 
            this.cylinderToolStripMenuItem.Image = global::MeshMaker.Properties.Resources.CylinderTemplate;
            this.cylinderToolStripMenuItem.Name = "cylinderToolStripMenuItem";
            this.cylinderToolStripMenuItem.Size = new System.Drawing.Size(139, 22);
            this.cylinderToolStripMenuItem.Text = "Cylinder";
            this.cylinderToolStripMenuItem.Click += new System.EventHandler(this.addItem);
            // 
            // sphereToolStripMenuItem
            // 
            this.sphereToolStripMenuItem.Image = global::MeshMaker.Properties.Resources.SphereTemplate;
            this.sphereToolStripMenuItem.Name = "sphereToolStripMenuItem";
            this.sphereToolStripMenuItem.Size = new System.Drawing.Size(139, 22);
            this.sphereToolStripMenuItem.Text = "Sphere";
            this.sphereToolStripMenuItem.Click += new System.EventHandler(this.addItem);
            // 
            // icosahedronToolStripMenuItem
            // 
            this.icosahedronToolStripMenuItem.Image = global::MeshMaker.Properties.Resources.IcosahedronTemplate;
            this.icosahedronToolStripMenuItem.Name = "icosahedronToolStripMenuItem";
            this.icosahedronToolStripMenuItem.Size = new System.Drawing.Size(139, 22);
            this.icosahedronToolStripMenuItem.Text = "Icosahedron";
            this.icosahedronToolStripMenuItem.Click += new System.EventHandler(this.addItem);
            // 
            // toolStripSeparator10
            // 
            this.toolStripSeparator10.Name = "toolStripSeparator10";
            this.toolStripSeparator10.Size = new System.Drawing.Size(206, 6);
            // 
            // mergeToolStripMenuItem
            // 
            this.mergeToolStripMenuItem.Image = global::MeshMaker.Properties.Resources.MergeTemplate;
            this.mergeToolStripMenuItem.Name = "mergeToolStripMenuItem";
            this.mergeToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift)
                        | System.Windows.Forms.Keys.M)));
            this.mergeToolStripMenuItem.Size = new System.Drawing.Size(209, 22);
            this.mergeToolStripMenuItem.Text = "Merge";
            this.mergeToolStripMenuItem.Click += new System.EventHandler(this.mergeToolStripMenuItem_Click);
            // 
            // splitToolStripMenuItem
            // 
            this.splitToolStripMenuItem.Image = global::MeshMaker.Properties.Resources.SplitTemplate;
            this.splitToolStripMenuItem.Name = "splitToolStripMenuItem";
            this.splitToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift)
                        | System.Windows.Forms.Keys.S)));
            this.splitToolStripMenuItem.Size = new System.Drawing.Size(209, 22);
            this.splitToolStripMenuItem.Text = "Split";
            this.splitToolStripMenuItem.Click += new System.EventHandler(this.splitToolStripMenuItem_Click);
            // 
            // flipToolStripMenuItem
            // 
            this.flipToolStripMenuItem.Name = "flipToolStripMenuItem";
            this.flipToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift)
                        | System.Windows.Forms.Keys.F)));
            this.flipToolStripMenuItem.Size = new System.Drawing.Size(209, 22);
            this.flipToolStripMenuItem.Text = "Flip";
            this.flipToolStripMenuItem.Click += new System.EventHandler(this.flipToolStripMenuItem_Click);
            // 
            // subdivisionToolStripMenuItem
            // 
            this.subdivisionToolStripMenuItem.Image = global::MeshMaker.Properties.Resources.SubdivisionTemplate;
            this.subdivisionToolStripMenuItem.Name = "subdivisionToolStripMenuItem";
            this.subdivisionToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift)
                        | System.Windows.Forms.Keys.U)));
            this.subdivisionToolStripMenuItem.Size = new System.Drawing.Size(209, 22);
            this.subdivisionToolStripMenuItem.Text = "Subdivision";
            this.subdivisionToolStripMenuItem.Click += new System.EventHandler(this.subdivisionToolStripMenuItem_Click);
            // 
            // detachToolStripMenuItem
            // 
            this.detachToolStripMenuItem.Name = "detachToolStripMenuItem";
            this.detachToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift)
                        | System.Windows.Forms.Keys.D)));
            this.detachToolStripMenuItem.Size = new System.Drawing.Size(209, 22);
            this.detachToolStripMenuItem.Text = "Detach";
            this.detachToolStripMenuItem.Click += new System.EventHandler(this.detachToolStripMenuItem_Click);
            // 
            // extrudeToolStripMenuItem
            // 
            this.extrudeToolStripMenuItem.Name = "extrudeToolStripMenuItem";
            this.extrudeToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift)
                        | System.Windows.Forms.Keys.E)));
            this.extrudeToolStripMenuItem.Size = new System.Drawing.Size(209, 22);
            this.extrudeToolStripMenuItem.Text = "Extrude";
            this.extrudeToolStripMenuItem.Click += new System.EventHandler(this.extrudeToolStripMenuItem_Click);
            // 
            // triangulateToolStripMenuItem
            // 
            this.triangulateToolStripMenuItem.Name = "triangulateToolStripMenuItem";
            this.triangulateToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift)
                        | System.Windows.Forms.Keys.T)));
            this.triangulateToolStripMenuItem.Size = new System.Drawing.Size(209, 22);
            this.triangulateToolStripMenuItem.Text = "Triangulate";
            this.triangulateToolStripMenuItem.Click += new System.EventHandler(this.triangulateToolStripMenuItem_Click);
            // 
            // viewToolStripMenuItem
            // 
            this.viewToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toggleOneViewFourViewsToolStripMenuItem});
            this.viewToolStripMenuItem.Name = "viewToolStripMenuItem";
            this.viewToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
            this.viewToolStripMenuItem.Text = "View";
            // 
            // toggleOneViewFourViewsToolStripMenuItem
            // 
            this.toggleOneViewFourViewsToolStripMenuItem.Name = "toggleOneViewFourViewsToolStripMenuItem";
            this.toggleOneViewFourViewsToolStripMenuItem.ShortcutKeyDisplayString = "Space";
            this.toggleOneViewFourViewsToolStripMenuItem.Size = new System.Drawing.Size(264, 22);
            this.toggleOneViewFourViewsToolStripMenuItem.Text = "Toggle One View/Four Views";
            this.toggleOneViewFourViewsToolStripMenuItem.Click += new System.EventHandler(this.toggleOneViewFourViewMenuItem_Click);
            // 
            // toolStripTop
            // 
            this.toolStripTop.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.toolStripTop.ImageScalingSize = new System.Drawing.Size(32, 32);
            this.toolStripTop.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButtonSelect,
            this.toolStripButtonTranslate,
            this.toolStripButtonRotate,
            this.toolStripButtonScale,
            this.toolStripSeparator1,
            this.toolStripButtonAddPlane,
            this.toolStripButtonAddCube,
            this.toolStripButtonAddCylinder,
            this.toolStripButtonAddSphere,
            this.toolStripButtonAddIcosahedron,
            this.toolStripSeparator2,
            this.toolStripComboBoxEditMode,
            this.toolStripSeparator3,
            this.toolStripButtonSplit,
            this.toolStripButtonMerge,
            this.toolStripButtonSubdivide});
            this.toolStripTop.Location = new System.Drawing.Point(0, 24);
            this.toolStripTop.Name = "toolStripTop";
            this.toolStripTop.Size = new System.Drawing.Size(843, 54);
            this.toolStripTop.TabIndex = 3;
            this.toolStripTop.Text = "toolStrip1";
            // 
            // toolStripButtonSelect
            // 
            this.toolStripButtonSelect.Checked = true;
            this.toolStripButtonSelect.CheckState = System.Windows.Forms.CheckState.Checked;
            this.toolStripButtonSelect.Image = global::MeshMaker.Properties.Resources.SelectTemplate;
            this.toolStripButtonSelect.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonSelect.Name = "toolStripButtonSelect";
            this.toolStripButtonSelect.Size = new System.Drawing.Size(42, 51);
            this.toolStripButtonSelect.Text = "Select";
            this.toolStripButtonSelect.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripButtonSelect.Click += new System.EventHandler(this.currentManipulatorClicked);
            // 
            // toolStripButtonTranslate
            // 
            this.toolStripButtonTranslate.Image = global::MeshMaker.Properties.Resources.TranslateTemplate;
            this.toolStripButtonTranslate.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonTranslate.Name = "toolStripButtonTranslate";
            this.toolStripButtonTranslate.Size = new System.Drawing.Size(59, 51);
            this.toolStripButtonTranslate.Text = "Translate";
            this.toolStripButtonTranslate.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripButtonTranslate.Click += new System.EventHandler(this.currentManipulatorClicked);
            // 
            // toolStripButtonRotate
            // 
            this.toolStripButtonRotate.Image = global::MeshMaker.Properties.Resources.RotateTemplate;
            this.toolStripButtonRotate.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonRotate.Name = "toolStripButtonRotate";
            this.toolStripButtonRotate.Size = new System.Drawing.Size(45, 51);
            this.toolStripButtonRotate.Text = "Rotate";
            this.toolStripButtonRotate.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripButtonRotate.Click += new System.EventHandler(this.currentManipulatorClicked);
            // 
            // toolStripButtonScale
            // 
            this.toolStripButtonScale.Image = global::MeshMaker.Properties.Resources.ScaleTemplate;
            this.toolStripButtonScale.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonScale.Name = "toolStripButtonScale";
            this.toolStripButtonScale.Size = new System.Drawing.Size(38, 51);
            this.toolStripButtonScale.Text = "Scale";
            this.toolStripButtonScale.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripButtonScale.Click += new System.EventHandler(this.currentManipulatorClicked);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 54);
            // 
            // toolStripButtonAddPlane
            // 
            this.toolStripButtonAddPlane.Image = global::MeshMaker.Properties.Resources.PlaneTemplate;
            this.toolStripButtonAddPlane.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonAddPlane.Name = "toolStripButtonAddPlane";
            this.toolStripButtonAddPlane.Size = new System.Drawing.Size(65, 51);
            this.toolStripButtonAddPlane.Tag = "";
            this.toolStripButtonAddPlane.Text = "Add Plane";
            this.toolStripButtonAddPlane.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripButtonAddPlane.Click += new System.EventHandler(this.addItem);
            // 
            // toolStripButtonAddCube
            // 
            this.toolStripButtonAddCube.Image = global::MeshMaker.Properties.Resources.CubeTemplate;
            this.toolStripButtonAddCube.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonAddCube.Name = "toolStripButtonAddCube";
            this.toolStripButtonAddCube.Size = new System.Drawing.Size(64, 51);
            this.toolStripButtonAddCube.Text = "Add Cube";
            this.toolStripButtonAddCube.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripButtonAddCube.Click += new System.EventHandler(this.addItem);
            // 
            // toolStripButtonAddCylinder
            // 
            this.toolStripButtonAddCylinder.Image = global::MeshMaker.Properties.Resources.CylinderTemplate;
            this.toolStripButtonAddCylinder.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonAddCylinder.Name = "toolStripButtonAddCylinder";
            this.toolStripButtonAddCylinder.Size = new System.Drawing.Size(80, 51);
            this.toolStripButtonAddCylinder.Text = "Add Cylinder";
            this.toolStripButtonAddCylinder.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripButtonAddCylinder.Click += new System.EventHandler(this.addItem);
            // 
            // toolStripButtonAddSphere
            // 
            this.toolStripButtonAddSphere.Image = global::MeshMaker.Properties.Resources.SphereTemplate;
            this.toolStripButtonAddSphere.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonAddSphere.Name = "toolStripButtonAddSphere";
            this.toolStripButtonAddSphere.Size = new System.Drawing.Size(72, 51);
            this.toolStripButtonAddSphere.Text = "Add Sphere";
            this.toolStripButtonAddSphere.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripButtonAddSphere.Click += new System.EventHandler(this.addItem);
            // 
            // toolStripButtonAddIcosahedron
            // 
            this.toolStripButtonAddIcosahedron.Image = global::MeshMaker.Properties.Resources.IcosahedronTemplate;
            this.toolStripButtonAddIcosahedron.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonAddIcosahedron.Name = "toolStripButtonAddIcosahedron";
            this.toolStripButtonAddIcosahedron.Size = new System.Drawing.Size(101, 51);
            this.toolStripButtonAddIcosahedron.Text = "Add Icosahedron";
            this.toolStripButtonAddIcosahedron.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripButtonAddIcosahedron.Click += new System.EventHandler(this.addItem);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 54);
            // 
            // toolStripComboBoxEditMode
            // 
            this.toolStripComboBoxEditMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.toolStripComboBoxEditMode.Name = "toolStripComboBoxEditMode";
            this.toolStripComboBoxEditMode.Size = new System.Drawing.Size(100, 54);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(6, 54);
            // 
            // toolStripButtonSplit
            // 
            this.toolStripButtonSplit.Image = global::MeshMaker.Properties.Resources.SplitTemplate;
            this.toolStripButtonSplit.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonSplit.Name = "toolStripButtonSplit";
            this.toolStripButtonSplit.Size = new System.Drawing.Size(36, 51);
            this.toolStripButtonSplit.Text = "Split";
            this.toolStripButtonSplit.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripButtonSplit.Click += new System.EventHandler(this.splitToolStripMenuItem_Click);
            // 
            // toolStripButtonMerge
            // 
            this.toolStripButtonMerge.Image = global::MeshMaker.Properties.Resources.MergeTemplate;
            this.toolStripButtonMerge.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonMerge.Name = "toolStripButtonMerge";
            this.toolStripButtonMerge.Size = new System.Drawing.Size(45, 51);
            this.toolStripButtonMerge.Text = "Merge";
            this.toolStripButtonMerge.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripButtonMerge.Click += new System.EventHandler(this.mergeToolStripMenuItem_Click);
            // 
            // toolStripButtonSubdivide
            // 
            this.toolStripButtonSubdivide.Image = global::MeshMaker.Properties.Resources.SubdivisionTemplate;
            this.toolStripButtonSubdivide.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonSubdivide.Name = "toolStripButtonSubdivide";
            this.toolStripButtonSubdivide.Size = new System.Drawing.Size(72, 51);
            this.toolStripButtonSubdivide.Text = "Subdivision";
            this.toolStripButtonSubdivide.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripButtonSubdivide.Click += new System.EventHandler(this.subdivisionToolStripMenuItem_Click);
            // 
            // toolStripBottom
            // 
            this.toolStripBottom.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.toolStripBottom.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.toolStripBottom.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripLabel4,
            this.toolStripComboBoxViewMode,
            this.toolStripSeparator11,
            this.toolStripLabel5,
            this.toolStripButtonColor,
            this.toolStripSeparator12,
            this.toolStripLabel1,
            this.toolStripTextBoxSelectionX,
            this.toolStripLabel2,
            this.toolStripTextBoxSelectionY,
            this.toolStripLabel3,
            this.toolStripTextBoxSelectionZ});
            this.toolStripBottom.Location = new System.Drawing.Point(0, 478);
            this.toolStripBottom.Name = "toolStripBottom";
            this.toolStripBottom.Size = new System.Drawing.Size(843, 25);
            this.toolStripBottom.TabIndex = 4;
            this.toolStripBottom.Text = "toolStrip2";
            // 
            // toolStripLabel4
            // 
            this.toolStripLabel4.Name = "toolStripLabel4";
            this.toolStripLabel4.Size = new System.Drawing.Size(69, 22);
            this.toolStripLabel4.Text = "View Mode:";
            // 
            // toolStripComboBoxViewMode
            // 
            this.toolStripComboBoxViewMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.toolStripComboBoxViewMode.Name = "toolStripComboBoxViewMode";
            this.toolStripComboBoxViewMode.Size = new System.Drawing.Size(121, 25);
            // 
            // toolStripSeparator11
            // 
            this.toolStripSeparator11.Name = "toolStripSeparator11";
            this.toolStripSeparator11.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStripLabel5
            // 
            this.toolStripLabel5.Name = "toolStripLabel5";
            this.toolStripLabel5.Size = new System.Drawing.Size(39, 22);
            this.toolStripLabel5.Text = "Color:";
            // 
            // toolStripButtonColor
            // 
            this.toolStripButtonColor.BackColor = System.Drawing.Color.DarkGray;
            this.toolStripButtonColor.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.None;
            this.toolStripButtonColor.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonColor.Image")));
            this.toolStripButtonColor.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonColor.Name = "toolStripButtonColor";
            this.toolStripButtonColor.Size = new System.Drawing.Size(23, 22);
            this.toolStripButtonColor.Text = "Color";
            this.toolStripButtonColor.Click += new System.EventHandler(this.toolStripButtonColor_Click);
            // 
            // toolStripSeparator12
            // 
            this.toolStripSeparator12.Name = "toolStripSeparator12";
            this.toolStripSeparator12.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStripLabel1
            // 
            this.toolStripLabel1.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold);
            this.toolStripLabel1.ForeColor = System.Drawing.Color.DarkRed;
            this.toolStripLabel1.Name = "toolStripLabel1";
            this.toolStripLabel1.Size = new System.Drawing.Size(17, 22);
            this.toolStripLabel1.Text = "X:";
            // 
            // toolStripTextBoxSelectionX
            // 
            this.toolStripTextBoxSelectionX.Name = "toolStripTextBoxSelectionX";
            this.toolStripTextBoxSelectionX.Size = new System.Drawing.Size(100, 25);
            this.toolStripTextBoxSelectionX.Leave += new System.EventHandler(this.selectionLeave);
            this.toolStripTextBoxSelectionX.KeyDown += new System.Windows.Forms.KeyEventHandler(this.selectionKeyDown);
            // 
            // toolStripLabel2
            // 
            this.toolStripLabel2.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold);
            this.toolStripLabel2.ForeColor = System.Drawing.Color.Green;
            this.toolStripLabel2.Name = "toolStripLabel2";
            this.toolStripLabel2.Size = new System.Drawing.Size(17, 22);
            this.toolStripLabel2.Text = "Y:";
            // 
            // toolStripTextBoxSelectionY
            // 
            this.toolStripTextBoxSelectionY.Name = "toolStripTextBoxSelectionY";
            this.toolStripTextBoxSelectionY.Size = new System.Drawing.Size(100, 25);
            this.toolStripTextBoxSelectionY.Leave += new System.EventHandler(this.selectionLeave);
            this.toolStripTextBoxSelectionY.KeyDown += new System.Windows.Forms.KeyEventHandler(this.selectionKeyDown);
            // 
            // toolStripLabel3
            // 
            this.toolStripLabel3.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold);
            this.toolStripLabel3.ForeColor = System.Drawing.Color.Blue;
            this.toolStripLabel3.Name = "toolStripLabel3";
            this.toolStripLabel3.Size = new System.Drawing.Size(17, 22);
            this.toolStripLabel3.Text = "Z:";
            // 
            // toolStripTextBoxSelectionZ
            // 
            this.toolStripTextBoxSelectionZ.Name = "toolStripTextBoxSelectionZ";
            this.toolStripTextBoxSelectionZ.Size = new System.Drawing.Size(100, 25);
            this.toolStripTextBoxSelectionZ.Leave += new System.EventHandler(this.selectionLeave);
            this.toolStripTextBoxSelectionZ.KeyDown += new System.Windows.Forms.KeyEventHandler(this.selectionKeyDown);
            // 
            // DocumentForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(843, 503);
            this.Controls.Add(this.mainSplit);
            this.Controls.Add(this.toolStripBottom);
            this.Controls.Add(this.toolStripTop);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "DocumentForm";
            this.Text = "MeshMaker";
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.DocumentForm_KeyDown);
            this.mainSplit.Panel1.ResumeLayout(false);
            this.mainSplit.Panel2.ResumeLayout(false);
            this.mainSplit.ResumeLayout(false);
            this.topSplit.Panel1.ResumeLayout(false);
            this.topSplit.Panel2.ResumeLayout(false);
            this.topSplit.ResumeLayout(false);
            this.bottomSplit.Panel1.ResumeLayout(false);
            this.bottomSplit.Panel2.ResumeLayout(false);
            this.bottomSplit.ResumeLayout(false);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.toolStripTop.ResumeLayout(false);
            this.toolStripTop.PerformLayout();
            this.toolStripBottom.ResumeLayout(false);
            this.toolStripBottom.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private MeshMakerCppCLI.OpenGLSceneView frontView;
        private System.Windows.Forms.SplitContainer mainSplit;
        private System.Windows.Forms.SplitContainer topSplit;
        private MeshMakerCppCLI.OpenGLSceneView leftView;
        private MeshMakerCppCLI.OpenGLSceneView topView;
        private System.Windows.Forms.SplitContainer bottomSplit;
        private MeshMakerCppCLI.OpenGLSceneView perspectiveView;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStrip toolStripTop;
        private System.Windows.Forms.ToolStrip toolStripBottom;
        private System.Windows.Forms.ToolStripButton toolStripButtonSelect;
        private System.Windows.Forms.ToolStripButton toolStripButtonTranslate;
        private System.Windows.Forms.ToolStripButton toolStripButtonRotate;
        private System.Windows.Forms.ToolStripButton toolStripButtonScale;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton toolStripButtonAddPlane;
        private System.Windows.Forms.ToolStripButton toolStripButtonAddCube;
        private System.Windows.Forms.ToolStripButton toolStripButtonAddCylinder;
        private System.Windows.Forms.ToolStripButton toolStripButtonAddSphere;
        private System.Windows.Forms.ToolStripButton toolStripButtonAddIcosahedron;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveAsToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator5;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem viewToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem toggleOneViewFourViewsToolStripMenuItem;
        private System.Windows.Forms.ToolStripComboBox toolStripComboBoxEditMode;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripButton toolStripButtonSplit;
        private System.Windows.Forms.ToolStripButton toolStripButtonMerge;
        private System.Windows.Forms.ToolStripButton toolStripButtonSubdivide;
        private System.Windows.Forms.ToolStripLabel toolStripLabel1;
        private System.Windows.Forms.ToolStripTextBox toolStripTextBoxSelectionX;
        private System.Windows.Forms.ToolStripLabel toolStripLabel2;
        private System.Windows.Forms.ToolStripTextBox toolStripTextBoxSelectionY;
        private System.Windows.Forms.ToolStripLabel toolStripLabel3;
        private System.Windows.Forms.ToolStripTextBox toolStripTextBoxSelectionZ;
        private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem undoToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem redoToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator6;
        private System.Windows.Forms.ToolStripMenuItem duplicateToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem deleteToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator7;
        private System.Windows.Forms.ToolStripMenuItem selectAllToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem invertSelectionToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator8;
        private System.Windows.Forms.ToolStripMenuItem hideSelectedToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem unhideAllToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator9;
        private System.Windows.Forms.ToolStripMenuItem addToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem planeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem cubeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem cylinderToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem sphereToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem icosahedronToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator10;
        private System.Windows.Forms.ToolStripMenuItem mergeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem splitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem flipToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem subdivisionToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem detachToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem extrudeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem triangulateToolStripMenuItem;
        private System.Windows.Forms.ToolStripLabel toolStripLabel4;
        private System.Windows.Forms.ToolStripComboBox toolStripComboBoxViewMode;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator11;
        private System.Windows.Forms.ToolStripLabel toolStripLabel5;
        private System.Windows.Forms.ToolStripButton toolStripButtonColor;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator12;
    }
}

