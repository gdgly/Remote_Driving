namespace SkySensor905
{
    partial class FormSensor905
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.BtnRS232Open = new System.Windows.Forms.Button();
            this.BtnRS232Stop = new System.Windows.Forms.Button();
            this.BtnSocketOpen = new System.Windows.Forms.Button();
            this.BtnSocketClose = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.lbRX_Num = new System.Windows.Forms.Label();
            this.lbSend_Num = new System.Windows.Forms.Label();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.label3 = new System.Windows.Forms.Label();
            this.lbClientNum = new System.Windows.Forms.Label();
            this.lbTcp = new System.Windows.Forms.Label();
            this.lbRS232 = new System.Windows.Forms.Label();
            this.serialPort2 = new System.IO.Ports.SerialPort(this.components);
            this.btnRS232CabinOpen = new System.Windows.Forms.Button();
            this.btnRS232CabinClose = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.lbRX_Cabin_Num = new System.Windows.Forms.Label();
            this.lbRS232Cabin = new System.Windows.Forms.Label();
            this.btnUDPOpen = new System.Windows.Forms.Button();
            this.btnUDPClose = new System.Windows.Forms.Button();
            this.label5 = new System.Windows.Forms.Label();
            this.lbUDPSend_Num = new System.Windows.Forms.Label();
            this.lbUDP = new System.Windows.Forms.Label();
            this.cbEnableSave = new System.Windows.Forms.CheckBox();
            this.btnTest = new System.Windows.Forms.Button();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // serialPort1
            // 
            this.serialPort1.BaudRate = 115200;
            this.serialPort1.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serialPort1_DataReceived);
            // 
            // BtnRS232Open
            // 
            this.BtnRS232Open.Location = new System.Drawing.Point(35, 24);
            this.BtnRS232Open.Name = "BtnRS232Open";
            this.BtnRS232Open.Size = new System.Drawing.Size(83, 23);
            this.BtnRS232Open.TabIndex = 0;
            this.BtnRS232Open.Text = "RS232Open";
            this.BtnRS232Open.UseVisualStyleBackColor = true;
            this.BtnRS232Open.Click += new System.EventHandler(this.BtnRS232Open_Click);
            // 
            // BtnRS232Stop
            // 
            this.BtnRS232Stop.Location = new System.Drawing.Point(35, 64);
            this.BtnRS232Stop.Name = "BtnRS232Stop";
            this.BtnRS232Stop.Size = new System.Drawing.Size(83, 23);
            this.BtnRS232Stop.TabIndex = 1;
            this.BtnRS232Stop.Text = "RS232Close";
            this.BtnRS232Stop.UseVisualStyleBackColor = true;
            this.BtnRS232Stop.Click += new System.EventHandler(this.BtnRS232Stop_Click);
            // 
            // BtnSocketOpen
            // 
            this.BtnSocketOpen.Location = new System.Drawing.Point(35, 143);
            this.BtnSocketOpen.Name = "BtnSocketOpen";
            this.BtnSocketOpen.Size = new System.Drawing.Size(83, 23);
            this.BtnSocketOpen.TabIndex = 2;
            this.BtnSocketOpen.Text = "SocketOpen";
            this.BtnSocketOpen.UseVisualStyleBackColor = true;
            this.BtnSocketOpen.Visible = false;
            this.BtnSocketOpen.Click += new System.EventHandler(this.BtnSocketOpen_Click);
            // 
            // BtnSocketClose
            // 
            this.BtnSocketClose.Location = new System.Drawing.Point(35, 187);
            this.BtnSocketClose.Name = "BtnSocketClose";
            this.BtnSocketClose.Size = new System.Drawing.Size(83, 23);
            this.BtnSocketClose.TabIndex = 3;
            this.BtnSocketClose.Text = "SocketClose";
            this.BtnSocketClose.UseVisualStyleBackColor = true;
            this.BtnSocketClose.Visible = false;
            this.BtnSocketClose.Click += new System.EventHandler(this.BtnSocketClose_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(153, 35);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(23, 12);
            this.label1.TabIndex = 5;
            this.label1.Text = "RX:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(153, 154);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(35, 12);
            this.label2.TabIndex = 6;
            this.label2.Text = "Send:";
            this.label2.Visible = false;
            // 
            // lbRX_Num
            // 
            this.lbRX_Num.AutoSize = true;
            this.lbRX_Num.Location = new System.Drawing.Point(196, 35);
            this.lbRX_Num.Name = "lbRX_Num";
            this.lbRX_Num.Size = new System.Drawing.Size(11, 12);
            this.lbRX_Num.TabIndex = 7;
            this.lbRX_Num.Text = "0";
            // 
            // lbSend_Num
            // 
            this.lbSend_Num.AutoSize = true;
            this.lbSend_Num.Location = new System.Drawing.Point(196, 154);
            this.lbSend_Num.Name = "lbSend_Num";
            this.lbSend_Num.Size = new System.Drawing.Size(11, 12);
            this.lbSend_Num.TabIndex = 8;
            this.lbSend_Num.Text = "0";
            this.lbSend_Num.Visible = false;
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(153, 198);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(53, 12);
            this.label3.TabIndex = 9;
            this.label3.Text = "Clients:";
            this.label3.Visible = false;
            // 
            // lbClientNum
            // 
            this.lbClientNum.AutoSize = true;
            this.lbClientNum.Location = new System.Drawing.Point(212, 198);
            this.lbClientNum.Name = "lbClientNum";
            this.lbClientNum.Size = new System.Drawing.Size(11, 12);
            this.lbClientNum.TabIndex = 10;
            this.lbClientNum.Text = "0";
            this.lbClientNum.Visible = false;
            // 
            // lbTcp
            // 
            this.lbTcp.AutoSize = true;
            this.lbTcp.Location = new System.Drawing.Point(33, 229);
            this.lbTcp.Name = "lbTcp";
            this.lbTcp.Size = new System.Drawing.Size(29, 12);
            this.lbTcp.TabIndex = 11;
            this.lbTcp.Text = "Tcp:";
            this.lbTcp.Visible = false;
            // 
            // lbRS232
            // 
            this.lbRS232.AutoSize = true;
            this.lbRS232.Location = new System.Drawing.Point(33, 105);
            this.lbRS232.Name = "lbRS232";
            this.lbRS232.Size = new System.Drawing.Size(41, 12);
            this.lbRS232.TabIndex = 12;
            this.lbRS232.Text = "RS232:";
            // 
            // serialPort2
            // 
            this.serialPort2.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serialPort2_DataReceived);
            // 
            // btnRS232CabinOpen
            // 
            this.btnRS232CabinOpen.Location = new System.Drawing.Point(370, 24);
            this.btnRS232CabinOpen.Name = "btnRS232CabinOpen";
            this.btnRS232CabinOpen.Size = new System.Drawing.Size(102, 23);
            this.btnRS232CabinOpen.TabIndex = 13;
            this.btnRS232CabinOpen.Text = "RS232CabinOpen";
            this.btnRS232CabinOpen.UseVisualStyleBackColor = true;
            this.btnRS232CabinOpen.Click += new System.EventHandler(this.btnRS232CabinOpen_Click);
            // 
            // btnRS232CabinClose
            // 
            this.btnRS232CabinClose.Location = new System.Drawing.Point(370, 64);
            this.btnRS232CabinClose.Name = "btnRS232CabinClose";
            this.btnRS232CabinClose.Size = new System.Drawing.Size(102, 23);
            this.btnRS232CabinClose.TabIndex = 14;
            this.btnRS232CabinClose.Text = "RS232CabinClose";
            this.btnRS232CabinClose.UseVisualStyleBackColor = true;
            this.btnRS232CabinClose.Click += new System.EventHandler(this.btnRS232CabinClose_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(503, 35);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(23, 12);
            this.label4.TabIndex = 15;
            this.label4.Text = "RX:";
            // 
            // lbRX_Cabin_Num
            // 
            this.lbRX_Cabin_Num.AutoSize = true;
            this.lbRX_Cabin_Num.Location = new System.Drawing.Point(543, 35);
            this.lbRX_Cabin_Num.Name = "lbRX_Cabin_Num";
            this.lbRX_Cabin_Num.Size = new System.Drawing.Size(11, 12);
            this.lbRX_Cabin_Num.TabIndex = 16;
            this.lbRX_Cabin_Num.Text = "0";
            // 
            // lbRS232Cabin
            // 
            this.lbRS232Cabin.AutoSize = true;
            this.lbRS232Cabin.Location = new System.Drawing.Point(370, 106);
            this.lbRS232Cabin.Name = "lbRS232Cabin";
            this.lbRS232Cabin.Size = new System.Drawing.Size(41, 12);
            this.lbRS232Cabin.TabIndex = 17;
            this.lbRS232Cabin.Text = "RS232:";
            // 
            // btnUDPOpen
            // 
            this.btnUDPOpen.Location = new System.Drawing.Point(372, 143);
            this.btnUDPOpen.Name = "btnUDPOpen";
            this.btnUDPOpen.Size = new System.Drawing.Size(75, 23);
            this.btnUDPOpen.TabIndex = 18;
            this.btnUDPOpen.Text = "UDPOpen";
            this.btnUDPOpen.UseVisualStyleBackColor = true;
            this.btnUDPOpen.Click += new System.EventHandler(this.btnUDPOpen_Click);
            // 
            // btnUDPClose
            // 
            this.btnUDPClose.Location = new System.Drawing.Point(372, 188);
            this.btnUDPClose.Name = "btnUDPClose";
            this.btnUDPClose.Size = new System.Drawing.Size(75, 23);
            this.btnUDPClose.TabIndex = 19;
            this.btnUDPClose.Text = "UDPClose";
            this.btnUDPClose.UseVisualStyleBackColor = true;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(505, 154);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(41, 12);
            this.label5.TabIndex = 20;
            this.label5.Text = "Send: ";
            // 
            // lbUDPSend_Num
            // 
            this.lbUDPSend_Num.AutoSize = true;
            this.lbUDPSend_Num.Location = new System.Drawing.Point(545, 155);
            this.lbUDPSend_Num.Name = "lbUDPSend_Num";
            this.lbUDPSend_Num.Size = new System.Drawing.Size(11, 12);
            this.lbUDPSend_Num.TabIndex = 21;
            this.lbUDPSend_Num.Text = "0";
            // 
            // lbUDP
            // 
            this.lbUDP.AutoSize = true;
            this.lbUDP.Location = new System.Drawing.Point(372, 230);
            this.lbUDP.Name = "lbUDP";
            this.lbUDP.Size = new System.Drawing.Size(29, 12);
            this.lbUDP.TabIndex = 22;
            this.lbUDP.Text = "UDP:";
            // 
            // cbEnableSave
            // 
            this.cbEnableSave.AutoSize = true;
            this.cbEnableSave.Location = new System.Drawing.Point(241, 150);
            this.cbEnableSave.Name = "cbEnableSave";
            this.cbEnableSave.Size = new System.Drawing.Size(84, 16);
            this.cbEnableSave.TabIndex = 23;
            this.cbEnableSave.Text = "EnableSave";
            this.cbEnableSave.UseVisualStyleBackColor = true;
            this.cbEnableSave.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
            // 
            // btnTest
            // 
            this.btnTest.Location = new System.Drawing.Point(241, 226);
            this.btnTest.Name = "btnTest";
            this.btnTest.Size = new System.Drawing.Size(75, 23);
            this.btnTest.TabIndex = 24;
            this.btnTest.Text = "Test";
            this.btnTest.UseVisualStyleBackColor = true;
            this.btnTest.Click += new System.EventHandler(this.btnTest_Click);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(76, 6);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(23, 12);
            this.label6.TabIndex = 25;
            this.label6.Text = "Car";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(405, 6);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(35, 12);
            this.label7.TabIndex = 26;
            this.label7.Text = "Cabin";
            // 
            // FormSensor905
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(713, 261);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.btnTest);
            this.Controls.Add(this.cbEnableSave);
            this.Controls.Add(this.lbUDP);
            this.Controls.Add(this.lbUDPSend_Num);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.btnUDPClose);
            this.Controls.Add(this.btnUDPOpen);
            this.Controls.Add(this.lbRS232Cabin);
            this.Controls.Add(this.lbRX_Cabin_Num);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.btnRS232CabinClose);
            this.Controls.Add(this.btnRS232CabinOpen);
            this.Controls.Add(this.lbRS232);
            this.Controls.Add(this.lbTcp);
            this.Controls.Add(this.lbClientNum);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.lbSend_Num);
            this.Controls.Add(this.lbRX_Num);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.BtnSocketClose);
            this.Controls.Add(this.BtnSocketOpen);
            this.Controls.Add(this.BtnRS232Stop);
            this.Controls.Add(this.BtnRS232Open);
            this.MaximizeBox = false;
            this.Name = "FormSensor905";
            this.Text = "HWT905";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.FormSensor905_FormClosed);
            this.Load += new System.EventHandler(this.FormSensor905_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.IO.Ports.SerialPort serialPort1;
        private System.Windows.Forms.Button BtnRS232Open;
        private System.Windows.Forms.Button BtnRS232Stop;
        private System.Windows.Forms.Button BtnSocketOpen;
        private System.Windows.Forms.Button BtnSocketClose;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label lbRX_Num;
        private System.Windows.Forms.Label lbSend_Num;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label lbClientNum;
        private System.Windows.Forms.Label lbTcp;
        private System.Windows.Forms.Label lbRS232;
        private System.IO.Ports.SerialPort serialPort2;
        private System.Windows.Forms.Button btnRS232CabinOpen;
        private System.Windows.Forms.Button btnRS232CabinClose;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label lbRX_Cabin_Num;
        private System.Windows.Forms.Label lbRS232Cabin;
        private System.Windows.Forms.Button btnUDPOpen;
        private System.Windows.Forms.Button btnUDPClose;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label lbUDPSend_Num;
        private System.Windows.Forms.Label lbUDP;
        private System.Windows.Forms.CheckBox cbEnableSave;
        private System.Windows.Forms.Button btnTest;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
    }
}

