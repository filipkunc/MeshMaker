namespace MeshMaker
{
    partial class ScriptEditor
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ScriptEditor));
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.toolStripButtonNewScript = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonRunScript = new System.Windows.Forms.ToolStripButton();
            this.textBoxOutput = new System.Windows.Forms.TextBox();
            this.listBoxScripts = new System.Windows.Forms.ListBox();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.textBoxCode = new FastColoredTextBoxNS.FastColoredTextBox();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.toolStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.textBoxCode)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.SuspendLayout();
            // 
            // toolStrip1
            // 
            this.toolStrip1.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButtonNewScript,
            this.toolStripButtonRunScript});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(851, 25);
            this.toolStrip1.TabIndex = 0;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // toolStripButtonNewScript
            // 
            this.toolStripButtonNewScript.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonNewScript.Image")));
            this.toolStripButtonNewScript.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonNewScript.Name = "toolStripButtonNewScript";
            this.toolStripButtonNewScript.Size = new System.Drawing.Size(84, 22);
            this.toolStripButtonNewScript.Text = "New Script";
            this.toolStripButtonNewScript.Click += new System.EventHandler(this.toolStripButtonNewScript_Click);
            // 
            // toolStripButtonRunScript
            // 
            this.toolStripButtonRunScript.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonRunScript.Image")));
            this.toolStripButtonRunScript.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonRunScript.Name = "toolStripButtonRunScript";
            this.toolStripButtonRunScript.Size = new System.Drawing.Size(81, 22);
            this.toolStripButtonRunScript.Text = "Run Script";
            this.toolStripButtonRunScript.Click += new System.EventHandler(this.toolStripButtonRunScript_Click);
            // 
            // textBoxOutput
            // 
            this.textBoxOutput.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textBoxOutput.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.textBoxOutput.Location = new System.Drawing.Point(0, 0);
            this.textBoxOutput.Multiline = true;
            this.textBoxOutput.Name = "textBoxOutput";
            this.textBoxOutput.ReadOnly = true;
            this.textBoxOutput.Size = new System.Drawing.Size(686, 112);
            this.textBoxOutput.TabIndex = 2;
            this.textBoxOutput.Text = "Output:";
            // 
            // listBoxScripts
            // 
            this.listBoxScripts.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listBoxScripts.FormattingEnabled = true;
            this.listBoxScripts.Location = new System.Drawing.Point(0, 0);
            this.listBoxScripts.Name = "listBoxScripts";
            this.listBoxScripts.Size = new System.Drawing.Size(161, 553);
            this.listBoxScripts.TabIndex = 3;
            this.listBoxScripts.SelectedIndexChanged += new System.EventHandler(this.listBoxScripts_SelectedIndexChanged);
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.textBoxCode);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.textBoxOutput);
            this.splitContainer1.Size = new System.Drawing.Size(686, 553);
            this.splitContainer1.SplitterDistance = 437;
            this.splitContainer1.TabIndex = 4;
            // 
            // textBoxCode
            // 
            this.textBoxCode.AutoScrollMinSize = new System.Drawing.Size(27, 14);
            this.textBoxCode.BackBrush = null;
            this.textBoxCode.Cursor = System.Windows.Forms.Cursors.IBeam;
            this.textBoxCode.DisabledColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(180)))), ((int)(((byte)(180)))), ((int)(((byte)(180)))));
            this.textBoxCode.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textBoxCode.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.textBoxCode.IsReplaceMode = false;
            this.textBoxCode.Language = FastColoredTextBoxNS.Language.JS;
            this.textBoxCode.Location = new System.Drawing.Point(0, 0);
            this.textBoxCode.Name = "textBoxCode";
            this.textBoxCode.Paddings = new System.Windows.Forms.Padding(0);
            this.textBoxCode.SelectionColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))), ((int)(((byte)(255)))));
            this.textBoxCode.Size = new System.Drawing.Size(686, 437);
            this.textBoxCode.TabIndex = 1;
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.Location = new System.Drawing.Point(0, 25);
            this.splitContainer2.Name = "splitContainer2";
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.listBoxScripts);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.splitContainer1);
            this.splitContainer2.Size = new System.Drawing.Size(851, 553);
            this.splitContainer2.SplitterDistance = 161;
            this.splitContainer2.TabIndex = 5;
            // 
            // ScriptEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(851, 578);
            this.Controls.Add(this.splitContainer2);
            this.Controls.Add(this.toolStrip1);
            this.Name = "ScriptEditor";
            this.Text = "Script Editor";
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.textBoxCode)).EndInit();
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
            this.splitContainer2.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton toolStripButtonNewScript;
        private System.Windows.Forms.ToolStripButton toolStripButtonRunScript;
        private System.Windows.Forms.TextBox textBoxOutput;
        private System.Windows.Forms.ListBox listBoxScripts;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private FastColoredTextBoxNS.FastColoredTextBox textBoxCode;
    }
}