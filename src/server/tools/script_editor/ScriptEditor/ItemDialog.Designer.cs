namespace ScriptEditor
{
    partial class ItemDialog
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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.Comment = new System.Windows.Forms.RichTextBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.DeleteOption = new System.Windows.Forms.LinkLabel();
            this.AddOption = new System.Windows.Forms.LinkLabel();
            this.OptionDataGrid = new System.Windows.Forms.DataGridView();
            this.CommandName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Next = new System.Windows.Forms.Button();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.Rule = new System.Windows.Forms.RichTextBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.OptionDataGrid)).BeginInit();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.Comment);
            this.groupBox1.Location = new System.Drawing.Point(12, 214);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(451, 191);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "模板Id";
            // 
            // Comment
            // 
            this.Comment.Location = new System.Drawing.Point(6, 20);
            this.Comment.Name = "Comment";
            this.Comment.Size = new System.Drawing.Size(439, 165);
            this.Comment.TabIndex = 0;
            this.Comment.Text = "";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.DeleteOption);
            this.groupBox2.Controls.Add(this.AddOption);
            this.groupBox2.Controls.Add(this.OptionDataGrid);
            this.groupBox2.Location = new System.Drawing.Point(12, 421);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(451, 174);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "选项";
            // 
            // DeleteOption
            // 
            this.DeleteOption.AutoSize = true;
            this.DeleteOption.Location = new System.Drawing.Point(65, 155);
            this.DeleteOption.Name = "DeleteOption";
            this.DeleteOption.Size = new System.Drawing.Size(53, 12);
            this.DeleteOption.TabIndex = 3;
            this.DeleteOption.TabStop = true;
            this.DeleteOption.Text = "删除选项";
            this.DeleteOption.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.DeleteOption_LinkClicked);
            // 
            // AddOption
            // 
            this.AddOption.AutoSize = true;
            this.AddOption.Location = new System.Drawing.Point(6, 155);
            this.AddOption.Name = "AddOption";
            this.AddOption.Size = new System.Drawing.Size(53, 12);
            this.AddOption.TabIndex = 2;
            this.AddOption.TabStop = true;
            this.AddOption.Text = "增加选项";
            this.AddOption.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.AddOption_LinkClicked);
            // 
            // OptionDataGrid
            // 
            this.OptionDataGrid.AllowUserToAddRows = false;
            this.OptionDataGrid.AllowUserToDeleteRows = false;
            this.OptionDataGrid.AllowUserToResizeColumns = false;
            this.OptionDataGrid.AllowUserToResizeRows = false;
            this.OptionDataGrid.BackgroundColor = System.Drawing.SystemColors.Window;
            this.OptionDataGrid.BorderStyle = System.Windows.Forms.BorderStyle.None;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("SimSun", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.OptionDataGrid.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
            this.OptionDataGrid.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.OptionDataGrid.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.CommandName});
            this.OptionDataGrid.EnableHeadersVisualStyles = false;
            this.OptionDataGrid.Location = new System.Drawing.Point(6, 20);
            this.OptionDataGrid.MultiSelect = false;
            this.OptionDataGrid.Name = "OptionDataGrid";
            this.OptionDataGrid.RowHeadersVisible = false;
            this.OptionDataGrid.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.DisableResizing;
            this.OptionDataGrid.RowTemplate.Height = 25;
            this.OptionDataGrid.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.OptionDataGrid.ShowCellErrors = false;
            this.OptionDataGrid.ShowCellToolTips = false;
            this.OptionDataGrid.ShowEditingIcon = false;
            this.OptionDataGrid.ShowRowErrors = false;
            this.OptionDataGrid.Size = new System.Drawing.Size(439, 132);
            this.OptionDataGrid.TabIndex = 1;
            this.OptionDataGrid.CellDoubleClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.OptionDataGrid_CellDoubleClick);
            // 
            // CommandName
            // 
            this.CommandName.DataPropertyName = "TemplateId";
            this.CommandName.HeaderText = "物品Id";
            this.CommandName.Name = "CommandName";
            this.CommandName.Width = 250;
            // 
            // Next
            // 
            this.Next.Location = new System.Drawing.Point(388, 602);
            this.Next.Name = "Next";
            this.Next.Size = new System.Drawing.Size(75, 23);
            this.Next.TabIndex = 0;
            this.Next.Text = "下一步";
            this.Next.UseVisualStyleBackColor = true;
            this.Next.Click += new System.EventHandler(this.Next_Click);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.Rule);
            this.groupBox3.Location = new System.Drawing.Point(12, 12);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(451, 196);
            this.groupBox3.TabIndex = 3;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "条件";
            // 
            // Rule
            // 
            this.Rule.Location = new System.Drawing.Point(6, 20);
            this.Rule.Name = "Rule";
            this.Rule.Size = new System.Drawing.Size(437, 170);
            this.Rule.TabIndex = 0;
            this.Rule.Text = "";
            // 
            // ItemDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(475, 637);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.Next);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ItemDialog";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "提示信息（带物品）";
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.OptionDataGrid)).EndInit();
            this.groupBox3.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RichTextBox Comment;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button Next;
        private System.Windows.Forms.LinkLabel AddOption;
        private System.Windows.Forms.DataGridView OptionDataGrid;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.RichTextBox Rule;
        private System.Windows.Forms.LinkLabel DeleteOption;
        private System.Windows.Forms.DataGridViewTextBoxColumn CommandName;
    }
}