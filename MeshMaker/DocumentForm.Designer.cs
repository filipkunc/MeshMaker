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
            this.viewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toggleOneViewFourViewsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
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
            this.toolStrip2 = new System.Windows.Forms.ToolStrip();
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
            this.toolStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // frontView
            // 
            this.frontView.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.frontView.CurrentCameraMode = CameraMode.Perspective;
            this.frontView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.frontView.Location = new System.Drawing.Point(0, 0);
            this.frontView.Name = "frontView";
            this.frontView.Size = new System.Drawing.Size(278, 124);
            this.frontView.TabIndex = 0;
            // 
            // mainSplit
            // 
            this.mainSplit.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mainSplit.Location = new System.Drawing.Point(0, 76);
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
            this.mainSplit.Size = new System.Drawing.Size(593, 306);
            this.mainSplit.SplitterDistance = 178;
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
            this.topSplit.Size = new System.Drawing.Size(593, 178);
            this.topSplit.SplitterDistance = 278;
            this.topSplit.TabIndex = 2;
            this.topSplit.SplitterMoving += new System.Windows.Forms.SplitterCancelEventHandler(this.topSplit_SplitterMoving);
            this.topSplit.SplitterMoved += new System.Windows.Forms.SplitterEventHandler(this.topSplit_SplitterMoved);
            // 
            // leftView
            // 
            this.leftView.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.leftView.CurrentCameraMode = CameraMode.Perspective;
            this.leftView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.leftView.Location = new System.Drawing.Point(0, 0);
            this.leftView.Name = "leftView";
            this.leftView.Size = new System.Drawing.Size(278, 178);
            this.leftView.TabIndex = 0;
            // 
            // topView
            // 
            this.topView.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.topView.CurrentCameraMode = CameraMode.Perspective;
            this.topView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.topView.Location = new System.Drawing.Point(0, 0);
            this.topView.Name = "topView";
            this.topView.Size = new System.Drawing.Size(311, 178);
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
            this.bottomSplit.Size = new System.Drawing.Size(593, 124);
            this.bottomSplit.SplitterDistance = 278;
            this.bottomSplit.TabIndex = 2;
            this.bottomSplit.SplitterMoving += new System.Windows.Forms.SplitterCancelEventHandler(this.bottomSplit_SplitterMoving);
            this.bottomSplit.SplitterMoved += new System.Windows.Forms.SplitterEventHandler(this.bottomSplit_SplitterMoved);
            // 
            // perspectiveView
            // 
            this.perspectiveView.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.perspectiveView.CurrentCameraMode = CameraMode.Perspective;
            this.perspectiveView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.perspectiveView.Location = new System.Drawing.Point(0, 0);
            this.perspectiveView.Name = "perspectiveView";
            this.perspectiveView.Size = new System.Drawing.Size(311, 124);
            this.perspectiveView.TabIndex = 0;
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.viewToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(593, 24);
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
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // newToolStripMenuItem
            // 
            this.newToolStripMenuItem.Name = "newToolStripMenuItem";
            this.newToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.N)));
            this.newToolStripMenuItem.Size = new System.Drawing.Size(163, 22);
            this.newToolStripMenuItem.Text = "New";
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
            this.openToolStripMenuItem.Size = new System.Drawing.Size(163, 22);
            this.openToolStripMenuItem.Text = "Open...";
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(160, 6);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(163, 22);
            this.saveToolStripMenuItem.Text = "Save";
            // 
            // saveAsToolStripMenuItem
            // 
            this.saveAsToolStripMenuItem.Name = "saveAsToolStripMenuItem";
            this.saveAsToolStripMenuItem.Size = new System.Drawing.Size(163, 22);
            this.saveAsToolStripMenuItem.Text = "Save As...";
            // 
            // toolStripSeparator5
            // 
            this.toolStripSeparator5.Name = "toolStripSeparator5";
            this.toolStripSeparator5.Size = new System.Drawing.Size(160, 6);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(163, 22);
            this.exitToolStripMenuItem.Text = "Exit";
            // 
            // viewToolStripMenuItem
            // 
            this.viewToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toggleOneViewFourViewsToolStripMenuItem});
            this.viewToolStripMenuItem.Name = "viewToolStripMenuItem";
            this.viewToolStripMenuItem.Size = new System.Drawing.Size(41, 20);
            this.viewToolStripMenuItem.Text = "View";
            // 
            // toggleOneViewFourViewsToolStripMenuItem
            // 
            this.toggleOneViewFourViewsToolStripMenuItem.Name = "toggleOneViewFourViewsToolStripMenuItem";
            this.toggleOneViewFourViewsToolStripMenuItem.ShortcutKeyDisplayString = "Space";
            this.toggleOneViewFourViewsToolStripMenuItem.Size = new System.Drawing.Size(257, 22);
            this.toggleOneViewFourViewsToolStripMenuItem.Text = "Toggle One View/Four Views";
            this.toggleOneViewFourViewsToolStripMenuItem.Click += new System.EventHandler(this.toggleOneViewFourViewMenuItem_Click);
            // 
            // toolStrip1
            // 
            this.toolStrip1.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.toolStrip1.ImageScalingSize = new System.Drawing.Size(32, 32);
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
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
            this.toolStripSeparator2});
            this.toolStrip1.Location = new System.Drawing.Point(0, 24);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(593, 52);
            this.toolStrip1.TabIndex = 3;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // toolStripButtonSelect
            // 
            this.toolStripButtonSelect.Checked = true;
            this.toolStripButtonSelect.CheckState = System.Windows.Forms.CheckState.Checked;
            this.toolStripButtonSelect.Image = global::MeshMaker.Properties.Resources.SelectTemplate;
            this.toolStripButtonSelect.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonSelect.Name = "toolStripButtonSelect";
            this.toolStripButtonSelect.Size = new System.Drawing.Size(40, 49);
            this.toolStripButtonSelect.Text = "Select";
            this.toolStripButtonSelect.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripButtonSelect.Click += new System.EventHandler(this.currentManipulatorClicked);
            // 
            // toolStripButtonTranslate
            // 
            this.toolStripButtonTranslate.Image = global::MeshMaker.Properties.Resources.TranslateTemplate;
            this.toolStripButtonTranslate.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonTranslate.Name = "toolStripButtonTranslate";
            this.toolStripButtonTranslate.Size = new System.Drawing.Size(56, 49);
            this.toolStripButtonTranslate.Text = "Translate";
            this.toolStripButtonTranslate.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripButtonTranslate.Click += new System.EventHandler(this.currentManipulatorClicked);
            // 
            // toolStripButtonRotate
            // 
            this.toolStripButtonRotate.Image = global::MeshMaker.Properties.Resources.RotateTemplate;
            this.toolStripButtonRotate.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonRotate.Name = "toolStripButtonRotate";
            this.toolStripButtonRotate.Size = new System.Drawing.Size(44, 49);
            this.toolStripButtonRotate.Text = "Rotate";
            this.toolStripButtonRotate.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripButtonRotate.Click += new System.EventHandler(this.currentManipulatorClicked);
            // 
            // toolStripButtonScale
            // 
            this.toolStripButtonScale.Image = global::MeshMaker.Properties.Resources.ScaleTemplate;
            this.toolStripButtonScale.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonScale.Name = "toolStripButtonScale";
            this.toolStripButtonScale.Size = new System.Drawing.Size(36, 49);
            this.toolStripButtonScale.Text = "Scale";
            this.toolStripButtonScale.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripButtonScale.Click += new System.EventHandler(this.currentManipulatorClicked);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 52);
            // 
            // toolStripButtonAddPlane
            // 
            this.toolStripButtonAddPlane.Image = global::MeshMaker.Properties.Resources.PlaneTemplate;
            this.toolStripButtonAddPlane.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonAddPlane.Name = "toolStripButtonAddPlane";
            this.toolStripButtonAddPlane.Size = new System.Drawing.Size(59, 49);
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
            this.toolStripButtonAddCube.Size = new System.Drawing.Size(58, 49);
            this.toolStripButtonAddCube.Text = "Add Cube";
            this.toolStripButtonAddCube.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripButtonAddCube.Click += new System.EventHandler(this.addItem);
            // 
            // toolStripButtonAddCylinder
            // 
            this.toolStripButtonAddCylinder.Image = global::MeshMaker.Properties.Resources.CylinderTemplate;
            this.toolStripButtonAddCylinder.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonAddCylinder.Name = "toolStripButtonAddCylinder";
            this.toolStripButtonAddCylinder.Size = new System.Drawing.Size(72, 49);
            this.toolStripButtonAddCylinder.Text = "Add Cylinder";
            this.toolStripButtonAddCylinder.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripButtonAddCylinder.Click += new System.EventHandler(this.addItem);
            // 
            // toolStripButtonAddSphere
            // 
            this.toolStripButtonAddSphere.Image = global::MeshMaker.Properties.Resources.SphereTemplate;
            this.toolStripButtonAddSphere.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonAddSphere.Name = "toolStripButtonAddSphere";
            this.toolStripButtonAddSphere.Size = new System.Drawing.Size(67, 49);
            this.toolStripButtonAddSphere.Text = "Add Sphere";
            this.toolStripButtonAddSphere.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripButtonAddSphere.Click += new System.EventHandler(this.addItem);
            // 
            // toolStripButtonAddIcosahedron
            // 
            this.toolStripButtonAddIcosahedron.Image = global::MeshMaker.Properties.Resources.IcosahedronTemplate;
            this.toolStripButtonAddIcosahedron.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonAddIcosahedron.Name = "toolStripButtonAddIcosahedron";
            this.toolStripButtonAddIcosahedron.Size = new System.Drawing.Size(93, 49);
            this.toolStripButtonAddIcosahedron.Text = "Add Icosahedron";
            this.toolStripButtonAddIcosahedron.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripButtonAddIcosahedron.Click += new System.EventHandler(this.addItem);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 52);
            // 
            // toolStrip2
            // 
            this.toolStrip2.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.toolStrip2.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.toolStrip2.Location = new System.Drawing.Point(0, 382);
            this.toolStrip2.Name = "toolStrip2";
            this.toolStrip2.Size = new System.Drawing.Size(593, 25);
            this.toolStrip2.TabIndex = 4;
            this.toolStrip2.Text = "toolStrip2";
            // 
            // DocumentForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(593, 407);
            this.Controls.Add(this.mainSplit);
            this.Controls.Add(this.toolStrip2);
            this.Controls.Add(this.toolStrip1);
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
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
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
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStrip toolStrip2;
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
    }
}

