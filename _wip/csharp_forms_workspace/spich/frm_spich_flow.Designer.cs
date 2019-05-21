namespace csharp_forms_workspace.spich
{
    partial class frm_spich_flow
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
            this.bkOK = new System.Windows.Forms.Button();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.label5 = new System.Windows.Forms.Label();
            this.groupBox8 = new System.Windows.Forms.GroupBox();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this.button3 = new System.Windows.Forms.Button();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.chk_cycle_frames = new System.Windows.Forms.CheckBox();
            this.rdb_cycle_manual = new System.Windows.Forms.RadioButton();
            this.rdb_cycle_free_run = new System.Windows.Forms.RadioButton();
            this.rdb_cycle_single = new System.Windows.Forms.RadioButton();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.label3 = new System.Windows.Forms.Label();
            this.cb_slot_command = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.txt_port_a = new System.Windows.Forms.TextBox();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.btn_add_slot_to_frame = new System.Windows.Forms.Button();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.btn_cycle_manual = new System.Windows.Forms.Button();
            this.groupBox7 = new System.Windows.Forms.GroupBox();
            this.tb_cycle_rate_hz = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.btn_frame_load = new System.Windows.Forms.Button();
            this.btn_frame_save = new System.Windows.Forms.Button();
            this.btn_cycle_stop = new System.Windows.Forms.Button();
            this.btn_cycle_start = new System.Windows.Forms.Button();
            this.btn_frame_clear = new System.Windows.Forms.Button();
            this.btn_slot_remove = new System.Windows.Forms.Button();
            this.list_Outbound_Opcode = new System.Windows.Forms.ListBox();
            this.groupBox6.SuspendLayout();
            this.groupBox8.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.groupBox7.SuspendLayout();
            this.SuspendLayout();
            // 
            // bkOK
            // 
            this.bkOK.Location = new System.Drawing.Point(476, 602);
            this.bkOK.Name = "bkOK";
            this.bkOK.Size = new System.Drawing.Size(75, 23);
            this.bkOK.TabIndex = 27;
            this.bkOK.Text = "Close";
            this.bkOK.UseVisualStyleBackColor = true;
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.label5);
            this.groupBox6.Controls.Add(this.groupBox8);
            this.groupBox6.Location = new System.Drawing.Point(12, 396);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(544, 200);
            this.groupBox6.TabIndex = 26;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "Inbound Data";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(41, 37);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(176, 52);
            this.label5.TabIndex = 15;
            this.label5.Text = "Post the data returned via the SPI\r\nport here.  Could be a read only text\r\nbox.  " +
                "Prefer something we can copy\r\nfrom to other applications.";
            // 
            // groupBox8
            // 
            this.groupBox8.Controls.Add(this.checkBox1);
            this.groupBox8.Controls.Add(this.button3);
            this.groupBox8.Location = new System.Drawing.Point(464, 10);
            this.groupBox8.Name = "groupBox8";
            this.groupBox8.Size = new System.Drawing.Size(74, 73);
            this.groupBox8.TabIndex = 14;
            this.groupBox8.TabStop = false;
            this.groupBox8.Text = "Frame Log";
            // 
            // checkBox1
            // 
            this.checkBox1.AutoSize = true;
            this.checkBox1.Location = new System.Drawing.Point(6, 51);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(65, 17);
            this.checkBox1.TabIndex = 15;
            this.checkBox1.Text = "Enabled";
            this.checkBox1.UseVisualStyleBackColor = true;
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(6, 19);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(58, 26);
            this.button3.TabIndex = 14;
            this.button3.Text = "File";
            this.button3.UseVisualStyleBackColor = true;
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.chk_cycle_frames);
            this.groupBox5.Controls.Add(this.rdb_cycle_manual);
            this.groupBox5.Controls.Add(this.rdb_cycle_free_run);
            this.groupBox5.Controls.Add(this.rdb_cycle_single);
            this.groupBox5.Location = new System.Drawing.Point(464, 12);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(92, 117);
            this.groupBox5.TabIndex = 25;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Cycling";
            // 
            // chk_cycle_frames
            // 
            this.chk_cycle_frames.AutoSize = true;
            this.chk_cycle_frames.Location = new System.Drawing.Point(6, 94);
            this.chk_cycle_frames.Name = "chk_cycle_frames";
            this.chk_cycle_frames.Size = new System.Drawing.Size(60, 17);
            this.chk_cycle_frames.TabIndex = 5;
            this.chk_cycle_frames.Text = "Frames";
            this.chk_cycle_frames.UseVisualStyleBackColor = true;
            // 
            // rdb_cycle_manual
            // 
            this.rdb_cycle_manual.AutoSize = true;
            this.rdb_cycle_manual.Location = new System.Drawing.Point(6, 19);
            this.rdb_cycle_manual.Name = "rdb_cycle_manual";
            this.rdb_cycle_manual.Size = new System.Drawing.Size(60, 17);
            this.rdb_cycle_manual.TabIndex = 4;
            this.rdb_cycle_manual.TabStop = true;
            this.rdb_cycle_manual.Text = "Manual";
            this.rdb_cycle_manual.UseVisualStyleBackColor = true;
            // 
            // rdb_cycle_free_run
            // 
            this.rdb_cycle_free_run.AutoSize = true;
            this.rdb_cycle_free_run.Location = new System.Drawing.Point(6, 66);
            this.rdb_cycle_free_run.Name = "rdb_cycle_free_run";
            this.rdb_cycle_free_run.Size = new System.Drawing.Size(69, 17);
            this.rdb_cycle_free_run.TabIndex = 1;
            this.rdb_cycle_free_run.TabStop = true;
            this.rdb_cycle_free_run.Text = "Free Run";
            this.rdb_cycle_free_run.UseVisualStyleBackColor = true;
            // 
            // rdb_cycle_single
            // 
            this.rdb_cycle_single.AutoSize = true;
            this.rdb_cycle_single.Location = new System.Drawing.Point(6, 43);
            this.rdb_cycle_single.Name = "rdb_cycle_single";
            this.rdb_cycle_single.Size = new System.Drawing.Size(54, 17);
            this.rdb_cycle_single.TabIndex = 0;
            this.rdb_cycle_single.TabStop = true;
            this.rdb_cycle_single.Text = "Single";
            this.rdb_cycle_single.UseVisualStyleBackColor = true;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.label3);
            this.groupBox3.Controls.Add(this.cb_slot_command);
            this.groupBox3.Controls.Add(this.label2);
            this.groupBox3.Controls.Add(this.label1);
            this.groupBox3.Controls.Add(this.txt_port_a);
            this.groupBox3.Controls.Add(this.textBox1);
            this.groupBox3.Controls.Add(this.label4);
            this.groupBox3.Controls.Add(this.btn_add_slot_to_frame);
            this.groupBox3.Location = new System.Drawing.Point(12, 12);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(446, 160);
            this.groupBox3.TabIndex = 24;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Slot Build";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(9, 21);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(57, 13);
            this.label3.TabIndex = 32;
            this.label3.Text = "Command:";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // cb_slot_command
            // 
            this.cb_slot_command.FormattingEnabled = true;
            this.cb_slot_command.Items.AddRange(new object[] {
            "NOP",
            "SELECT",
            "BATCH",
            "FIFO",
            "DATA",
            "DELAY"});
            this.cb_slot_command.Location = new System.Drawing.Point(12, 37);
            this.cb_slot_command.Name = "cb_slot_command";
            this.cb_slot_command.Size = new System.Drawing.Size(78, 21);
            this.cb_slot_command.TabIndex = 31;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(21, 94);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(283, 52);
            this.label2.TabIndex = 30;
            this.label2.Text = "NOTES:\r\n1. outbound uint16_t = <op code: upper 8-bit>< data 8-bit>\r\n2. op-codes: " +
                "0x00, 0x10, 0x20, 0x40, 0x80.  See Readme.\r\n3. data: 0x00 - 0xFF\r\n";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(105, 21);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(57, 13);
            this.label1.TabIndex = 29;
            this.label1.Text = "Data Byte:";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // txt_port_a
            // 
            this.txt_port_a.Location = new System.Drawing.Point(108, 37);
            this.txt_port_a.Name = "txt_port_a";
            this.txt_port_a.Size = new System.Drawing.Size(64, 20);
            this.txt_port_a.TabIndex = 28;
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(65, 65);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(375, 20);
            this.textBox1.TabIndex = 7;
            this.textBox1.Text = "Buffer is now full.";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(19, 66);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(40, 13);
            this.label4.TabIndex = 6;
            this.label4.Text = "Status:";
            this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // btn_add_slot_to_frame
            // 
            this.btn_add_slot_to_frame.Location = new System.Drawing.Point(255, 30);
            this.btn_add_slot_to_frame.Name = "btn_add_slot_to_frame";
            this.btn_add_slot_to_frame.Size = new System.Drawing.Size(84, 27);
            this.btn_add_slot_to_frame.TabIndex = 4;
            this.btn_add_slot_to_frame.Text = "Add to Frame";
            this.btn_add_slot_to_frame.UseVisualStyleBackColor = true;
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.btn_cycle_manual);
            this.groupBox4.Controls.Add(this.groupBox7);
            this.groupBox4.Controls.Add(this.btn_frame_load);
            this.groupBox4.Controls.Add(this.btn_frame_save);
            this.groupBox4.Controls.Add(this.btn_cycle_stop);
            this.groupBox4.Controls.Add(this.btn_cycle_start);
            this.groupBox4.Controls.Add(this.btn_frame_clear);
            this.groupBox4.Controls.Add(this.btn_slot_remove);
            this.groupBox4.Controls.Add(this.list_Outbound_Opcode);
            this.groupBox4.Location = new System.Drawing.Point(12, 178);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(544, 212);
            this.groupBox4.TabIndex = 23;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Outbound Frame";
            // 
            // btn_cycle_manual
            // 
            this.btn_cycle_manual.Location = new System.Drawing.Point(474, 156);
            this.btn_cycle_manual.Name = "btn_cycle_manual";
            this.btn_cycle_manual.Size = new System.Drawing.Size(58, 36);
            this.btn_cycle_manual.TabIndex = 14;
            this.btn_cycle_manual.Text = "Cycle\r\nManual";
            this.btn_cycle_manual.UseVisualStyleBackColor = true;
            // 
            // groupBox7
            // 
            this.groupBox7.Controls.Add(this.tb_cycle_rate_hz);
            this.groupBox7.Controls.Add(this.label10);
            this.groupBox7.Location = new System.Drawing.Point(452, 104);
            this.groupBox7.Name = "groupBox7";
            this.groupBox7.Size = new System.Drawing.Size(86, 46);
            this.groupBox7.TabIndex = 13;
            this.groupBox7.TabStop = false;
            this.groupBox7.Text = "Cycle Rate";
            // 
            // tb_cycle_rate_hz
            // 
            this.tb_cycle_rate_hz.Location = new System.Drawing.Point(7, 19);
            this.tb_cycle_rate_hz.Name = "tb_cycle_rate_hz";
            this.tb_cycle_rate_hz.Size = new System.Drawing.Size(58, 20);
            this.tb_cycle_rate_hz.TabIndex = 13;
            this.tb_cycle_rate_hz.Text = "100";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(66, 19);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(20, 13);
            this.label10.TabIndex = 14;
            this.label10.Text = "Hz";
            // 
            // btn_frame_load
            // 
            this.btn_frame_load.Location = new System.Drawing.Point(474, 60);
            this.btn_frame_load.Name = "btn_frame_load";
            this.btn_frame_load.Size = new System.Drawing.Size(58, 38);
            this.btn_frame_load.TabIndex = 10;
            this.btn_frame_load.Text = "Frame\r\nLoad";
            this.btn_frame_load.UseVisualStyleBackColor = true;
            // 
            // btn_frame_save
            // 
            this.btn_frame_save.Location = new System.Drawing.Point(474, 17);
            this.btn_frame_save.Name = "btn_frame_save";
            this.btn_frame_save.Size = new System.Drawing.Size(58, 38);
            this.btn_frame_save.TabIndex = 9;
            this.btn_frame_save.Text = "Frame\r\nSave";
            this.btn_frame_save.UseVisualStyleBackColor = true;
            // 
            // btn_cycle_stop
            // 
            this.btn_cycle_stop.Location = new System.Drawing.Point(388, 166);
            this.btn_cycle_stop.Name = "btn_cycle_stop";
            this.btn_cycle_stop.Size = new System.Drawing.Size(58, 36);
            this.btn_cycle_stop.TabIndex = 8;
            this.btn_cycle_stop.Text = "Cycle\r\nStop";
            this.btn_cycle_stop.UseVisualStyleBackColor = true;
            // 
            // btn_cycle_start
            // 
            this.btn_cycle_start.Location = new System.Drawing.Point(388, 119);
            this.btn_cycle_start.Name = "btn_cycle_start";
            this.btn_cycle_start.Size = new System.Drawing.Size(58, 41);
            this.btn_cycle_start.TabIndex = 7;
            this.btn_cycle_start.Text = "Cycle\r\nStart\r\n";
            this.btn_cycle_start.UseVisualStyleBackColor = true;
            // 
            // btn_frame_clear
            // 
            this.btn_frame_clear.Location = new System.Drawing.Point(388, 60);
            this.btn_frame_clear.Name = "btn_frame_clear";
            this.btn_frame_clear.Size = new System.Drawing.Size(58, 38);
            this.btn_frame_clear.TabIndex = 6;
            this.btn_frame_clear.Text = "Frame\r\nClear";
            this.btn_frame_clear.UseVisualStyleBackColor = true;
            // 
            // btn_slot_remove
            // 
            this.btn_slot_remove.Location = new System.Drawing.Point(388, 17);
            this.btn_slot_remove.Name = "btn_slot_remove";
            this.btn_slot_remove.Size = new System.Drawing.Size(58, 35);
            this.btn_slot_remove.TabIndex = 5;
            this.btn_slot_remove.Text = "Slot\r\nRemove";
            this.btn_slot_remove.UseVisualStyleBackColor = true;
            // 
            // list_Outbound_Opcode
            // 
            this.list_Outbound_Opcode.FormattingEnabled = true;
            this.list_Outbound_Opcode.Items.AddRange(new object[] {
            "1 - 0x0000",
            "2 - 0x1001       <--- chip select CS0 active",
            "4  - 0x4012      <--- data sent 0x12",
            "5  - 0x4034      <--- data sent 0x34",
            "6  - 0x1000        <--- chip select CS0 inactive",
            "7",
            "8",
            "9",
            "10",
            "11",
            "12",
            "13",
            "14",
            "15",
            "16 - slot position #4  - maximum number of slots shall be 16."});
            this.list_Outbound_Opcode.Location = new System.Drawing.Point(6, 16);
            this.list_Outbound_Opcode.Name = "list_Outbound_Opcode";
            this.list_Outbound_Opcode.Size = new System.Drawing.Size(376, 186);
            this.list_Outbound_Opcode.TabIndex = 4;
            // 
            // spich_flow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(561, 633);
            this.Controls.Add(this.bkOK);
            this.Controls.Add(this.groupBox6);
            this.Controls.Add(this.groupBox5);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox4);
            this.Name = "spich_flow";
            this.Text = "spich";
            this.groupBox6.ResumeLayout(false);
            this.groupBox6.PerformLayout();
            this.groupBox8.ResumeLayout(false);
            this.groupBox8.PerformLayout();
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox7.ResumeLayout(false);
            this.groupBox7.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button bkOK;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.GroupBox groupBox8;
        private System.Windows.Forms.CheckBox checkBox1;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.CheckBox chk_cycle_frames;
        private System.Windows.Forms.RadioButton rdb_cycle_manual;
        private System.Windows.Forms.RadioButton rdb_cycle_free_run;
        private System.Windows.Forms.RadioButton rdb_cycle_single;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox cb_slot_command;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txt_port_a;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button btn_add_slot_to_frame;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Button btn_cycle_manual;
        private System.Windows.Forms.GroupBox groupBox7;
        private System.Windows.Forms.TextBox tb_cycle_rate_hz;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Button btn_frame_load;
        private System.Windows.Forms.Button btn_frame_save;
        private System.Windows.Forms.Button btn_cycle_stop;
        private System.Windows.Forms.Button btn_cycle_start;
        private System.Windows.Forms.Button btn_frame_clear;
        private System.Windows.Forms.Button btn_slot_remove;
        private System.Windows.Forms.ListBox list_Outbound_Opcode;
    }
}