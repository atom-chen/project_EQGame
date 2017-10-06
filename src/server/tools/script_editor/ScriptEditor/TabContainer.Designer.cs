namespace ScriptEditor
{
    partial class TabContainer
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.FunctionTabContainer = new System.Windows.Forms.TabControl();
            this.SuspendLayout();
            // 
            // FunctionTabContainer
            // 
            this.FunctionTabContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.FunctionTabContainer.Location = new System.Drawing.Point(0, 0);
            this.FunctionTabContainer.Name = "FunctionTabContainer";
            this.FunctionTabContainer.SelectedIndex = 0;
            this.FunctionTabContainer.Size = new System.Drawing.Size(638, 515);
            this.FunctionTabContainer.TabIndex = 1;
            // 
            // TabContainer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.FunctionTabContainer);
            this.Name = "TabContainer";
            this.Size = new System.Drawing.Size(638, 515);
            this.Load += new System.EventHandler(this.TabContainer_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl FunctionTabContainer;


    }
}
