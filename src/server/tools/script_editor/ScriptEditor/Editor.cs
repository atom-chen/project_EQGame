using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Logic;
using Common;
using System.IO;
using System.Runtime.InteropServices;
using Model;
using System.Text.RegularExpressions;
using System.Diagnostics;

namespace ScriptEditor
{
    public partial class Editor : Form
    {
        private CommandManager commandMgr = new CommandManager();
        private OperateRecordManager operaterecordMgr;
        private FileManager fileMgr = new FileManager();
        private RtfManager RtfMgr = new RtfManager();
        private FunctionManager functionMgr = new FunctionManager();
        private int searchIndex = 0;
        private string FileName;
        [Flags]
        enum MouseEventFlag : uint
        {
            Move = 0x0001,
            LeftDown = 0x0002,
            LeftUp = 0x0004,
            RightDown = 0x0008,
            RightUp = 0x0010,
            MiddleDown = 0x0020,
            MiddleUp = 0x0040,
            XDown = 0x0080,
            XUp = 0x0100,
            Wheel = 0x0800,
            VirtualDesk = 0x4000,
            Absolute = 0x8000
        }
        [DllImport("user32.dll")]
        static extern void mouse_event(MouseEventFlag flags, int dx, int dy, uint data, UIntPtr extraInfo);


        public Editor()
        {
            InitializeComponent();
        }

        private void toolStripAdd_Click(object sender, EventArgs e)
        {
            NewFile();
        }

        private void NewFile()
        {
            operaterecordMgr = new OperateRecordManager();
            Map map = new Map(operaterecordMgr);
            map.ShowDialog();
            if (operaterecordMgr != null && operaterecordMgr.OperateList.Count > 0)
                this.ContentTextBox.Text = commandMgr.Render(operaterecordMgr);
        }

        private void toolStripSave_Click(object sender, EventArgs e)
        {
            try
            {
                if (!string.IsNullOrEmpty(ContentTextBox.Text))
                {
                    SaveFile();
                    SaveNpcDef();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void SaveFile()
        {
            if (operaterecordMgr != null && !string.IsNullOrEmpty(ContentTextBox.Text))
            {
                this.FileName = operaterecordMgr.NpcKey + ".erl";
                fileMgr.Save(@"Gen_File\" + this.FileName, ContentTextBox.Text);
                MessageBox.Show("保存成功!");
            }
            //else if (!string.IsNullOrEmpty(FileName))
            //{

            //    fileMgr.Save(@"Gen_File\" + FileName, ContentTextBox.Text);
            //    MessageBox.Show("保存成功!");
            //}
            else
            {
                DialogResult dialogResult = saveFileDialog1.ShowDialog();
                if (dialogResult == DialogResult.OK)
                {
                    string fileName = saveFileDialog1.FileName;
                    int Index = fileName.LastIndexOf("\\");
                    this.FileName = fileName.Substring(Index + 1, fileName.Length - Index - 1);
                    if (!string.IsNullOrEmpty(fileName))
                    {
                        fileMgr.Save1(fileName, ContentTextBox.Text);
                        MessageBox.Show("保存成功!");
                    }
                }
            }
        }

        private void SaveNpcDef()
        { 
            if (operaterecordMgr != null && !string.IsNullOrEmpty(ContentTextBox.Text))
            {
                string content = fileMgr.Load(DelegateUtils.GetVirtualPath() + @"Template\npc_def.tplt");
                if (content.IndexOf(operaterecordMgr.NpcKey) <= 0)
                {
                    int index1 = content.IndexOf("[");
                    int index2 = content.IndexOf("]");
                    if(index1 + 1 == index2)
                        content = content.Insert(index2, "{" + operaterecordMgr.NpcKey + ", " + operaterecordMgr.NpcKey + "}");
                    else
                        content = content.Insert(index2, ", {" + operaterecordMgr.NpcKey + ", " + operaterecordMgr.NpcKey + "}");
                }

                fileMgr.Save(@"Gen_File\npc_def.hrl", content);
                fileMgr.Save(@"Template\npc_def.tplt", content);
            }
        }

        private void toolStripComplie_Click(object sender, EventArgs e)
        {
            if (!string.IsNullOrEmpty(ContentTextBox.Text))
                ComplieFile();
        }

        private void ComplieFile()
        {
            try
            {
                this.SaveFile();
                //this.SaveNpcDef();

                System.Diagnostics.ProcessStartInfo Info = new System.Diagnostics.ProcessStartInfo();
                System.Diagnostics.Process Proc = new System.Diagnostics.Process();
                Info.FileName = DelegateUtils.GetVirtualPath() + @"Gen_File\make.bat";
                Info.Arguments = DelegateUtils.GetVirtualPath() + @"Gen_File, " + this.FileName;
                Proc = System.Diagnostics.Process.Start(Info);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void toolStripOpen_Click(object sender, EventArgs e)
        {
            OpenFile();
        }

        private void OpenFile()
        {
            try
            {
                DialogResult dialogResult = openFileDialog1.ShowDialog();
                string fileName = openFileDialog1.FileName;
                if (!string.IsNullOrEmpty(fileName))
                {
                    string content = fileMgr.Load(fileName);
                    ContentTextBox.Text = content;
                    this.FileName = openFileDialog1.SafeFileName;
                }
            }
            catch (Exception ex)
            {
                FileName = "";
                MessageBox.Show(ex.Message);
            }
        }

        private void File_New_Click(object sender, EventArgs e)
        {
            NewFile();
        }

        private void File_Open_Click(object sender, EventArgs e)
        {
            OpenFile();
        }

        private void File_Save_Click(object sender, EventArgs e)
        {
            try
            {
                SaveFile();
                
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void File_Complie_Click(object sender, EventArgs e)
        {
            ComplieFile();
        }

        private void ContentTextBox_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                mouse_event(MouseEventFlag.LeftDown, 0, 0, 0, UIntPtr.Zero);
                mouse_event(MouseEventFlag.LeftUp, 0, 0, 0, UIntPtr.Zero);
            }
        }

        private void ContentTextBox_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            Rules rules = new Rules();
            rules.ShowDialog();
        }

        private void functionContainer1_ClickButton(FunctionInfo functionInfo)
        {
            int startIndex = ContentTextBox.SelectionStart;
            string content = functionInfo.Name;

            RichTextBox tempRtb = new RichTextBox();
            Dictionary<int, int> dict = new Dictionary<int, int>();
            content = replace(dict, content);
            tempRtb.AppendText(content);
            foreach (KeyValuePair<int, int> pair in dict)
            {
                tempRtb.Select(pair.Key, pair.Value - pair.Key);
                tempRtb.SelectionColor = Color.Red; 
            }
            

            ContentTextBox.SelectionStart = startIndex;
            ContentTextBox.SelectedRtf = tempRtb.Rtf;
            ContentTextBox.Focus();
        }

        private string replace(Dictionary<int, int> dict, string content)
        {
            if (content.IndexOf("*") > 0)
            {
                int leftIndex = content.IndexOf("*");
                content = content.Remove(leftIndex, 1);
                int rightIndex = content.IndexOf("*");
                content = content.Remove(rightIndex, 1);
                dict.Add(leftIndex, rightIndex);
                content = replace(dict, content);
            }

            return content;
        }

        private void HighLightText(List<string> wordList, Color color)
        {
            foreach (string word in wordList)
            {
                Regex r = new Regex(word, RegexOptions.IgnoreCase);

                foreach (Match m in r.Matches(ContentTextBox.Text))
                {
                    ContentTextBox.Select(m.Index, m.Length);
                    ContentTextBox.SelectionColor = color;
                    ContentTextBox.Select(0, 0);
                }
            }
        }

        private void ContentTextBox_TextChanged(object sender, EventArgs e)
        {
            //int startIndex = ContentTextBox.SelectionStart;
            //ContentTextBox.Text = ContentTextBox.Text;
            //HighLightText(functionMgr.GetKeyWord(), Color.Red);
            //ContentTextBox.Select(0, 0);
            //ContentTextBox.SelectionStart = startIndex;
        }

        private void Search_Click(object sender, EventArgs e)
        {
            string searchText = SearchTextBox.Text;
            string content = ContentTextBox.Text;
            if (!string.IsNullOrEmpty(searchText) && !string.IsNullOrEmpty(content))
            {
                Regex regex = new Regex(searchText, RegexOptions.IgnoreCase);
                foreach (Match match in regex.Matches(ContentTextBox.Text))
                {
                    if (searchIndex >= match.Index)
                    {
                        continue;
                    }
                    else
                    {
                        searchIndex = match.Index;
                        ContentTextBox.Select(match.Index, match.Length);
                        return;
                    }
                }
            }

            searchIndex = 0;
        }

        private void Location_Click(object sender, EventArgs e)
        {
            ContentTextBox.Focus();
            int rowIndex = Utils.TryToInt(LocationTextBox.Text, 0);
            int startIndex = 0;
            if (rowIndex > 0)
            {
                for (int i = 0; i < rowIndex - 1; i++)
                {
                    startIndex += ContentTextBox.Lines[i].Length + "\n".Length;
                }
            }

            ContentTextBox.SelectionStart = startIndex;
            ContentTextBox.ScrollToCaret();
        }

        private void toolStripTemplate_Click(object sender, EventArgs e)
        {
            string fileName = DelegateUtils.GetVirtualPath() + @"Template\template.tplt";
            string content = fileMgr.Load(fileName);
            ContentTextBox.Text = content;
        }

        private void toolStripButton1_Click(object sender, EventArgs e)
        {
            string fileName = DelegateUtils.GetVirtualPath() + @"Template\task.tplt";
            string content = fileMgr.Load(fileName);
            ContentTextBox.Text = content;
        }
    }
}
