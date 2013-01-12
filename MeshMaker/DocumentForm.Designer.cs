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
            this.sceneView = new MeshMakerCppCLI.OpenGLSceneView();
            this.SuspendLayout();
            // 
            // sceneView
            // 
            this.sceneView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.sceneView.Location = new System.Drawing.Point(0, 0);
            this.sceneView.Name = "sceneView";
            this.sceneView.Size = new System.Drawing.Size(787, 456);
            this.sceneView.TabIndex = 0;
            // 
            // DocumentForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(787, 456);
            this.Controls.Add(this.sceneView);
            this.Name = "DocumentForm";
            this.Text = "MeshMaker";
            this.ResumeLayout(false);

        }

        #endregion

        private MeshMakerCppCLI.OpenGLSceneView sceneView;
    }
}

