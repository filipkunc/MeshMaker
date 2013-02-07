using MeshMakerCppCLI;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace MeshMaker
{
    public partial class TextureBrowser : Form
    {
        MyDocument document;

        public TextureBrowser(MyDocument document)
        {
            InitializeComponent();
            this.document = document;
            this.Load += TextureBrowser_Load;
        }

        void TextureBrowser_Load(object sender, EventArgs e)
        {
            for (uint i = 0; i < document.textureCount(); i++)
            {
                listBoxTextures.Items.Add(document.textureNameAtIndex(i));
            }
        }

        private void toolStripButtonAdd_Click(object sender, EventArgs e)
        {
            using (OpenFileDialog dlg = new OpenFileDialog())
            {
                dlg.Filter = "Image files |*.png;*.bmp;*.jpeg;*.jpg";
                if (dlg.ShowDialog() == DialogResult.OK)
                {
                    document.addTexture(dlg.FileName);
                    listBoxTextures.Items.Add(document.textureNameAtIndex(document.textureCount() - 1));
                }
            }
        }

        private void toolStripButtonRemove_Click(object sender, EventArgs e)
        {
            var index = listBoxTextures.SelectedIndex;

            if (index >= 0 && index < document.textureCount())
            {
                document.removeTextureAtIndex((uint)index);
                listBoxTextures.Items.RemoveAt(index);
            }
        }

        private void toolStripButtonSet_Click(object sender, EventArgs e)
        {
             var index = listBoxTextures.SelectedIndex;

             if (index >= 0 && index < document.textureCount())
             {
                 document.setTextureAtIndex((uint)index);
             }
        }

        private void listBoxTextures_SelectedIndexChanged(object sender, EventArgs e)
        {
            var index = listBoxTextures.SelectedIndex;

            if (index >= 0 && index < document.textureCount())
            {
                var image = document.textureImageAtIndex((uint)index);
                pictureBoxTexture.Image = (Bitmap)image.Clone();
            }
            else
            {
                pictureBoxTexture.Image = null;
            }
        }
    }
}
