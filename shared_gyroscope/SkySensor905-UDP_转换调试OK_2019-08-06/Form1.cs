using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;


using System.Net.Sockets;
using System.Net;
using System.Threading;

using SkyOperationIni;

using DataProcess;

using System.IO;

namespace SkySensor905
{
    public partial class FormSensor905 : Form
    {
        private long received_count = 0;            //接收计数   
        private long cabin_received_count = 0;      //接收计数   
        private long send_count = 0;                //发送计数 
        private int client_num = 0;

        Server server;
        private UdpClient udpcSend;
        private IPEndPoint g_ipep;

        private WT905DataProcess WT905_car = new WT905DataProcess();
        private WT905DataProcess WT905_cabin = new WT905DataProcess();

        public FormSensor905()
        {
            InitializeComponent();

            //BtnRS232Open_Click(null, null);
            //BtnSocketOpen_Click(null, null);
        }

        private void BtnSocketOpen_Click(object sender, EventArgs e)
        {
            string server_ip, server_port;

            string file_name = System.Environment.CurrentDirectory;
            file_name += "\\config.ini";

            //server_ip = OperateIniFile.ReadIniData("TCPIP", "IP", "127.0.0.1", "config.ini");
            //server_ip = OperateIniFile.ReadIniData("TCPIP", "IP", "192.168.0.11", "config.ini");
            server_ip = OperateIniFile.ReadIniData("TCPIP", "IP", "127.0.0.1", file_name);
            server_port = OperateIniFile.ReadIniData("TCPIP", "TCP_PORT", "10200", file_name);

            if (server == null)
                server = new Server(null, server_ip, server_port);
            else
            {
                server.ipString = server_ip;
                server.port = Int32.Parse(server_port);
            }

            if (server.started)
            {
                MessageBox.Show("TCP Server is listening!", "TCP操作提醒");
            }
            else
            {
                server.start();
            }

            if (server.started)
                lbTcp.Text = "TCP: " + server_ip + " / " + server_port + " is listening!";
            else
                lbTcp.Text = "TCP: " + server_ip + " / " + server_port + " can not open!";

        }

        #region HWT串口操作
        private void BtnRS232Open_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                MessageBox.Show("RS232 Port is open!","串口操作提醒");
                return;
            }

            string file_name = System.Environment.CurrentDirectory;
            file_name += "\\config.ini";

            serialPort1.PortName = OperateIniFile.ReadIniData("RS232", "PORT", "COM1", file_name);
            serialPort1.BaudRate = Convert.ToInt32(OperateIniFile.ReadIniData("RS232", "Baud", "115200", file_name));
            //serialPort1.PortName = "COM1";
            //serialPort1.BaudRate = 115200;


            serialPort1.Open();

            if (serialPort1.IsOpen)
                lbRS232.Text = "RS232: " + serialPort1.PortName + " " + serialPort1.BaudRate + " is opened!";
            else
                lbRS232.Text = "RS232: " + serialPort1.PortName + " " + serialPort1.BaudRate + " can not open!";

        }
        #endregion

        private void serialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            int received_num;
            received_num = serialPort1.BytesToRead;
            received_count += received_num;

            byte[] udp_buf = new byte[100];

            byte[] bytesRecvData = new byte[serialPort1.ReadBufferSize + 1];
            serialPort1.Read(bytesRecvData, 0, received_num); //数据不读取出，会导致一直触发事件

            WT905_car.AddStringToBuf(bytesRecvData, received_num);

            while (WT905_car.SearchOneFrameData())
            {
                WT905_car.WriteOneFrame(udp_buf, WT905_car.udp_frame_data);
                if(udpcSend != null)
                {
                    udpcSend.Send(udp_buf, 80, g_ipep);
                    send_count += 80; 
                }

                if (cbEnableSave.Checked)
                    WT905_car.SavaProcess("Car_data.txt");
            }

            //server.Send(bytesRecvData, serialPort1.BytesToRead);
            //if (server != null)
            //{ 
            //    if(server.Send(bytesRecvData, received_num) >0)
            //        send_count += received_num;
            //}


        }

        private void BtnRS232Stop_Click(object sender, EventArgs e)
        {
            if(serialPort1.IsOpen)
            {
                serialPort1.Close();
                lbRS232.Text = "RS232: " + serialPort1.PortName + " " + serialPort1.BaudRate + " closed!";
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            lbRX_Num.Text = Convert.ToString(received_count);
            lbSend_Num.Text = Convert.ToString(send_count);
            lbUDPSend_Num.Text = Convert.ToString(send_count);
            lbRX_Cabin_Num.Text = Convert.ToString(cabin_received_count);

            if (server != null)
            {
                lbClientNum.Text = Convert.ToString(server.clients.Count);
            }
           //private int client_num = 0;
        }

    private void BtnSocketClose_Click(object sender, EventArgs e)
        {
            if (server != null)
                if(server.started)
                {
                    server.stop();
                    lbTcp.Text = "TCP:" + " closed!";
                }
        }

        private void FormSensor905_FormClosed(object sender, FormClosedEventArgs e)
        {
            BtnSocketClose_Click(null, null);
            BtnRS232Stop_Click(null, null);
        }

        private void btnRS232CabinOpen_Click(object sender, EventArgs e)
        {
            if (serialPort2.IsOpen)
            {
                MessageBox.Show("RS232 Cabin Port is open!", "串口操作提醒");
                return;
            }

            string file_name = System.Environment.CurrentDirectory;
            file_name += "\\config.ini";

            serialPort2.PortName = OperateIniFile.ReadIniData("RS232", "PORT_CABIN", "COM2", file_name);
            serialPort2.BaudRate = Convert.ToInt32(OperateIniFile.ReadIniData("RS232", "Baud_CABIN", "115200", file_name));
            //serialPort1.PortName = "COM1";
            //serialPort1.BaudRate = 115200;


            serialPort2.Open();

            if (serialPort2.IsOpen)
                lbRS232Cabin.Text = "RS232: " + serialPort2.PortName + " " + serialPort2.BaudRate + " is opened!";
            else
                lbRS232Cabin.Text = "RS232: " + serialPort2.PortName + " " + serialPort2.BaudRate + " can not open!";

        }

        private void btnRS232CabinClose_Click(object sender, EventArgs e)
        {
            if (serialPort2.IsOpen)
            {
                serialPort2.Close();
                lbRS232Cabin.Text = "RS232: " + serialPort2.PortName + " " + serialPort2.BaudRate + " closed!";
            }
        }

        private void serialPort2_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            int received_num;
            received_num = serialPort2.BytesToRead;
            cabin_received_count += received_num;

            byte[] udp_buf = new byte[100];

            byte[] bytesRecvData = new byte[serialPort2.ReadBufferSize + 1];
            serialPort2.Read(bytesRecvData, 0, received_num); //数据不读取出，会导致一直触发事件


            WT905_cabin.AddStringToBuf(bytesRecvData, received_num);

            while (WT905_cabin.SearchOneFrameData())
            {
                if(cbEnableSave.Checked)
                    WT905_cabin.SavaProcess("Cabin_data.txt");
            }
        }

        private void btnUDPOpen_Click(object sender, EventArgs e)
        {
            string udp_server_ip, udp_server_port;

            string file_name = System.Environment.CurrentDirectory;
            file_name += "\\config.ini";

            udp_server_ip = OperateIniFile.ReadIniData("UDP", "UDP_IP", "127.0.0.1", file_name);
            udp_server_port = OperateIniFile.ReadIniData("UDP", "UDP_PORT", "10200", file_name);

            if(udpcSend == null)
            {
                g_ipep = new IPEndPoint(IPAddress.Parse(udp_server_ip), Convert.ToInt16(udp_server_port));

                // 匿名发送
                udpcSend = new UdpClient(0);             // 自动分配本地IPv4地址
                // 实名发送
                //IPEndPoint localIpep = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 12345); // 本机IP，指定的端口号
                //udpcSend = new UdpClient(localIpep);
            }
            /*
             byte[] b = System.Text.Encoding.UTF8.GetBytes ("這是'客戶端'傳送的訊息 ~ " + i);            
			udpcSend.Send (b, b.Length, ipep);
            */
            lbUDP.Text = "UDP: client is running, " + udp_server_ip + " / " + udp_server_port + " !";
        }

        private void FormSensor905_Load(object sender, EventArgs e)
        {

        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if(cbEnableSave.Checked)
            {
                //删除文件
                try
                {
                    if(File.Exists("Car_data.txt"))
                        File.Delete("Car_data.txt");

                    if (File.Exists("Cabin_data.txt"))
                        File.Delete("Cabin_data.txt");
                }
                catch (System.IO.IOException ee )
                {
                    Console.WriteLine(ee.Message);
                }
            }
        }

        private void btnTest_Click(object sender, EventArgs e)
        {
            byte[] udp_buf = new byte[100];

            WT905_car.udp_frame_data.command = 1;

            WT905_car.udp_frame_data.angle_x = 2872.21631f;
            WT905_car.udp_frame_data.angle_y = 2.2f;
            WT905_car.udp_frame_data.angle_z = 3.3f;

            WT905_car.udp_frame_data.angular_v_x = 4.4f;
            WT905_car.udp_frame_data.angular_v_y = 5.5f;
            WT905_car.udp_frame_data.angular_v_z = 6.6f;

            WT905_car.udp_frame_data.angular_a_x = 7.7f;
            WT905_car.udp_frame_data.angular_a_y = 8.8f;
            WT905_car.udp_frame_data.angular_a_z = 9.9f;

            WT905_car.SavaProcess("a.txt");

            WT905_car.WriteOneFrame(udp_buf, WT905_car.udp_frame_data);

            if (udpcSend != null)
            {
                udpcSend.Send(udp_buf, 80, g_ipep);
            }
        }
        }

}
