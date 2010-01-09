namespace OpenGLEditorWindows
{
    partial class DockFourViews
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
            this.mainSplit = new System.Windows.Forms.SplitContainer();
            this.topSplit = new System.Windows.Forms.SplitContainer();
            this.openGLSceneViewLeft = new ManagedCpp.OpenGLSceneView();
            this.openGLSceneViewTop = new ManagedCpp.OpenGLSceneView();
            this.bottomSplit = new System.Windows.Forms.SplitContainer();
            this.openGLSceneViewFront = new ManagedCpp.OpenGLSceneView();
            this.openGLSceneViewPerspective = new ManagedCpp.OpenGLSceneView();
            this.mainSplit.Panel1.SuspendLayout();
            this.mainSplit.Panel2.SuspendLayout();
            this.mainSplit.SuspendLayout();
            this.topSplit.Panel1.SuspendLayout();
            this.topSplit.Panel2.SuspendLayout();
            this.topSplit.SuspendLayout();
            this.bottomSplit.Panel1.SuspendLayout();
            this.bottomSplit.Panel2.SuspendLayout();
            this.bottomSplit.SuspendLayout();
            this.SuspendLayout();
            // 
            // mainSplit
            // 
            this.mainSplit.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mainSplit.Location = new System.Drawing.Point(0, 0);
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
            this.mainSplit.Size = new System.Drawing.Size(292, 266);
            this.mainSplit.SplitterDistance = 130;
            this.mainSplit.TabIndex = 6;
            // 
            // topSplit
            // 
            this.topSplit.Dock = System.Windows.Forms.DockStyle.Fill;
            this.topSplit.Location = new System.Drawing.Point(0, 0);
            this.topSplit.Name = "topSplit";
            // 
            // topSplit.Panel1
            // 
            this.topSplit.Panel1.Controls.Add(this.openGLSceneViewLeft);
            // 
            // topSplit.Panel2
            // 
            this.topSplit.Panel2.Controls.Add(this.openGLSceneViewTop);
            this.topSplit.Size = new System.Drawing.Size(292, 130);
            this.topSplit.SplitterDistance = 144;
            this.topSplit.TabIndex = 6;
            // 
            // openGLSceneViewLeft
            // 
            this.openGLSceneViewLeft.BackColor = System.Drawing.Color.Salmon;
            this.openGLSceneViewLeft.Dock = System.Windows.Forms.DockStyle.Fill;
            this.openGLSceneViewLeft.Location = new System.Drawing.Point(0, 0);
            this.openGLSceneViewLeft.Name = "openGLSceneViewLeft";
            this.openGLSceneViewLeft.Size = new System.Drawing.Size(144, 130);
            this.openGLSceneViewLeft.TabIndex = 5;
            // 
            // openGLSceneViewTop
            // 
            this.openGLSceneViewTop.BackColor = System.Drawing.Color.LightGreen;
            this.openGLSceneViewTop.Dock = System.Windows.Forms.DockStyle.Fill;
            this.openGLSceneViewTop.Location = new System.Drawing.Point(0, 0);
            this.openGLSceneViewTop.Name = "openGLSceneViewTop";
            this.openGLSceneViewTop.Size = new System.Drawing.Size(144, 130);
            this.openGLSceneViewTop.TabIndex = 6;
            // 
            // bottomSplit
            // 
            this.bottomSplit.Dock = System.Windows.Forms.DockStyle.Fill;
            this.bottomSplit.Location = new System.Drawing.Point(0, 0);
            this.bottomSplit.Name = "bottomSplit";
            // 
            // bottomSplit.Panel1
            // 
            this.bottomSplit.Panel1.Controls.Add(this.openGLSceneViewFront);
            // 
            // bottomSplit.Panel2
            // 
            this.bottomSplit.Panel2.Controls.Add(this.openGLSceneViewPerspective);
            this.bottomSplit.Size = new System.Drawing.Size(292, 132);
            this.bottomSplit.SplitterDistance = 144;
            this.bottomSplit.TabIndex = 7;
            // 
            // openGLSceneViewFront
            // 
            this.openGLSceneViewFront.BackColor = System.Drawing.Color.DarkKhaki;
            this.openGLSceneViewFront.Dock = System.Windows.Forms.DockStyle.Fill;
            this.openGLSceneViewFront.Location = new System.Drawing.Point(0, 0);
            this.openGLSceneViewFront.Name = "openGLSceneViewFront";
            this.openGLSceneViewFront.Size = new System.Drawing.Size(144, 132);
            this.openGLSceneViewFront.TabIndex = 7;
            // 
            // openGLSceneViewPerspective
            // 
            this.openGLSceneViewPerspective.BackColor = System.Drawing.Color.SlateBlue;
            this.openGLSceneViewPerspective.Dock = System.Windows.Forms.DockStyle.Fill;
            this.openGLSceneViewPerspective.Location = new System.Drawing.Point(0, 0);
            this.openGLSceneViewPerspective.Name = "openGLSceneViewPerspective";
            this.openGLSceneViewPerspective.Size = new System.Drawing.Size(144, 132);
            this.openGLSceneViewPerspective.TabIndex = 8;
            // 
            // DockFourViews
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(292, 266);
            this.Controls.Add(this.mainSplit);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.Name = "DockFourViews";
            this.Text = "DockFourViews";
            this.mainSplit.Panel1.ResumeLayout(false);
            this.mainSplit.Panel2.ResumeLayout(false);
            this.mainSplit.ResumeLayout(false);
            this.topSplit.Panel1.ResumeLayout(false);
            this.topSplit.Panel2.ResumeLayout(false);
            this.topSplit.ResumeLayout(false);
            this.bottomSplit.Panel1.ResumeLayout(false);
            this.bottomSplit.Panel2.ResumeLayout(false);
            this.bottomSplit.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        public System.Windows.Forms.SplitContainer mainSplit;
        public System.Windows.Forms.SplitContainer topSplit;
        public ManagedCpp.OpenGLSceneView openGLSceneViewLeft;
        public ManagedCpp.OpenGLSceneView openGLSceneViewTop;
        public System.Windows.Forms.SplitContainer bottomSplit;
        public ManagedCpp.OpenGLSceneView openGLSceneViewFront;
        public ManagedCpp.OpenGLSceneView openGLSceneViewPerspective;
    }
}