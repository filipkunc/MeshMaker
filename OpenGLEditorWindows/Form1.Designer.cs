namespace OpenGLEditorWindows
{
    partial class Form1
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.openGLSceneView1 = new ManagedCpp.OpenGLSceneView();
            this.btnSelect = new System.Windows.Forms.ToolStripButton();
            this.btnTranslate = new System.Windows.Forms.ToolStripButton();
            this.btnRotate = new System.Windows.Forms.ToolStripButton();
            this.btnScale = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.btnAddCube = new System.Windows.Forms.ToolStripButton();
            this.btnAddCylinder = new System.Windows.Forms.ToolStripButton();
            this.btnAddSphere = new System.Windows.Forms.ToolStripButton();
            this.toolStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnSelect,
            this.btnTranslate,
            this.btnRotate,
            this.btnScale,
            this.toolStripSeparator1,
            this.btnAddCube,
            this.btnAddCylinder,
            this.btnAddSphere});
            this.toolStrip1.Location = new System.Drawing.Point(0, 24);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(1009, 25);
            this.toolStrip1.TabIndex = 1;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // menuStrip1
            // 
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(1009, 24);
            this.menuStrip1.TabIndex = 2;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // openGLSceneView1
            // 
            this.openGLSceneView1.BackColor = System.Drawing.Color.LightGray;
            this.openGLSceneView1.CurrentCameraMode = ManagedCpp.CameraMode.CameraModePerspective;
            this.openGLSceneView1.CurrentManipulator = ManagedCpp.ManipulatorType.ManipulatorTypeDefault;
            this.openGLSceneView1.CurrentViewMode = ManagedCpp.ViewMode.ViewModeSolid;
            this.openGLSceneView1.Displayed = null;
            this.openGLSceneView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.openGLSceneView1.Location = new System.Drawing.Point(0, 49);
            this.openGLSceneView1.Manipulated = null;
            this.openGLSceneView1.Name = "openGLSceneView1";
            this.openGLSceneView1.Size = new System.Drawing.Size(1009, 537);
            this.openGLSceneView1.TabIndex = 3;
            // 
            // btnSelect
            // 
            this.btnSelect.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.btnSelect.Image = ((System.Drawing.Image)(resources.GetObject("btnSelect.Image")));
            this.btnSelect.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnSelect.Name = "btnSelect";
            this.btnSelect.Size = new System.Drawing.Size(40, 22);
            this.btnSelect.Text = "Select";
            this.btnSelect.Click += new System.EventHandler(this.btnSelect_Click);
            // 
            // btnTranslate
            // 
            this.btnTranslate.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.btnTranslate.Image = ((System.Drawing.Image)(resources.GetObject("btnTranslate.Image")));
            this.btnTranslate.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnTranslate.Name = "btnTranslate";
            this.btnTranslate.Size = new System.Drawing.Size(56, 22);
            this.btnTranslate.Text = "Translate";
            this.btnTranslate.Click += new System.EventHandler(this.btnTranslate_Click);
            // 
            // btnRotate
            // 
            this.btnRotate.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.btnRotate.Image = ((System.Drawing.Image)(resources.GetObject("btnRotate.Image")));
            this.btnRotate.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnRotate.Name = "btnRotate";
            this.btnRotate.Size = new System.Drawing.Size(44, 22);
            this.btnRotate.Text = "Rotate";
            this.btnRotate.Click += new System.EventHandler(this.btnRotate_Click);
            // 
            // btnScale
            // 
            this.btnScale.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.btnScale.Image = ((System.Drawing.Image)(resources.GetObject("btnScale.Image")));
            this.btnScale.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnScale.Name = "btnScale";
            this.btnScale.Size = new System.Drawing.Size(36, 22);
            this.btnScale.Text = "Scale";
            this.btnScale.Click += new System.EventHandler(this.btnScale_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // btnAddCube
            // 
            this.btnAddCube.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.btnAddCube.Image = ((System.Drawing.Image)(resources.GetObject("btnAddCube.Image")));
            this.btnAddCube.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnAddCube.Name = "btnAddCube";
            this.btnAddCube.Size = new System.Drawing.Size(58, 22);
            this.btnAddCube.Text = "Add Cube";
            this.btnAddCube.Click += new System.EventHandler(this.btnAddCube_Click);
            // 
            // btnAddCylinder
            // 
            this.btnAddCylinder.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.btnAddCylinder.Image = ((System.Drawing.Image)(resources.GetObject("btnAddCylinder.Image")));
            this.btnAddCylinder.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnAddCylinder.Name = "btnAddCylinder";
            this.btnAddCylinder.Size = new System.Drawing.Size(72, 22);
            this.btnAddCylinder.Text = "Add Cylinder";
            this.btnAddCylinder.Click += new System.EventHandler(this.btnAddCylinder_Click);
            // 
            // btnAddSphere
            // 
            this.btnAddSphere.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.btnAddSphere.Image = ((System.Drawing.Image)(resources.GetObject("btnAddSphere.Image")));
            this.btnAddSphere.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnAddSphere.Name = "btnAddSphere";
            this.btnAddSphere.Size = new System.Drawing.Size(67, 22);
            this.btnAddSphere.Text = "Add Sphere";
            this.btnAddSphere.Click += new System.EventHandler(this.btnAddSphere_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1009, 586);
            this.Controls.Add(this.openGLSceneView1);
            this.Controls.Add(this.toolStrip1);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "Form1";
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private ManagedCpp.OpenGLSceneView openGLSceneView1;
        private System.Windows.Forms.ToolStripButton btnSelect;
        private System.Windows.Forms.ToolStripButton btnTranslate;
        private System.Windows.Forms.ToolStripButton btnRotate;
        private System.Windows.Forms.ToolStripButton btnScale;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton btnAddCube;
        private System.Windows.Forms.ToolStripButton btnAddCylinder;
        private System.Windows.Forms.ToolStripButton btnAddSphere;
    }
}

