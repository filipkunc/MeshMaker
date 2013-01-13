using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MeshMakerCppCLI;
using System.Diagnostics;

namespace MeshMaker
{
    public partial class DocumentForm : Form
    {
        MyDocument document = new MyDocument();
        ToolStripButton[] manipulatorButtons;

        public DocumentForm()
        {
            InitializeComponent();
            this.Load += new EventHandler(DocumentForm_Load);
        }

        void DocumentForm_Load(object sender, EventArgs e)
        {
            if (DesignMode)
                return;

            toolStripButtonAddCube.Tag = MeshType.Cube;
            toolStripButtonAddCylinder.Tag = MeshType.Cylinder;
            toolStripButtonAddIcosahedron.Tag = MeshType.Icosahedron;
            toolStripButtonAddPlane.Tag = MeshType.Plane;
            toolStripButtonAddSphere.Tag = MeshType.Sphere;

            toolStripButtonSelect.Tag = ManipulatorType.Default;
            toolStripButtonTranslate.Tag = ManipulatorType.Translation;
            toolStripButtonRotate.Tag = ManipulatorType.Rotation;
            toolStripButtonScale.Tag = ManipulatorType.Scale;

            manipulatorButtons = new ToolStripButton[] { toolStripButtonSelect, 
                toolStripButtonTranslate, toolStripButtonRotate, toolStripButtonScale };

            document.setViews(leftView, topView, frontView, perspectiveView);
        }

        private void addItem(object sender, EventArgs e)
        {
            var button = (ToolStripButton)sender;
            document.addItem((MeshType)button.Tag, 20);
        }

        private void currentManipulatorClicked(object sender, EventArgs e)
        {
            foreach (var button in manipulatorButtons)
            {
                if (button == sender)
                {
                    button.Checked = true;
                    document.CurrentManipulator = (ManipulatorType)button.Tag;
                }
                else
                {
                    button.Checked = false;
                }
            }
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
            //// one view -> four views
            if (mainSplit.Panel1Collapsed)
            {
                mainSplit.Panel1Collapsed = false;
                bottomSplit.Panel1Collapsed = false;
                perspectiveView.CurrentCameraMode = CameraMode.Perspective;
            }
            else // four views -> one view
            {
                Point screenPoint = Control.MousePosition;
                foreach (OpenGLSceneView view in document.Views)
                {
                    Point clientPoint = view.PointToClient(screenPoint);
                    if (view.ClientRectangle.Contains(clientPoint))
                    {
                        perspectiveView.CurrentCameraMode = view.CurrentCameraMode;
                        mainSplit.Panel1Collapsed = true;
                        bottomSplit.Panel1Collapsed = true;
                        return;
                    }
                }

                Trace.WriteLine("No view is under mouse");
            }
        }

        #endregion
        
        #region Keyboard

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            // this allows to process keys in DocumentForm_KeyDown
            foreach (var view in document.Views)
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
            }
        }

        #endregion      
    }
}
