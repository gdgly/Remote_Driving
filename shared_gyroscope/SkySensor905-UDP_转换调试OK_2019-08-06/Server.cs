using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
//using System.Windows.Forms;       //for MessageBox


namespace SkySensor905
{

    public class Server
    {
        public string ipString = "127.0.0.1";   // 服务器端ip
        //public string ipString = "172.16.5.16";   // 服务器端ip
        public int port = 10200;                // 服务器端口
        
        public Socket socket;
        public Print print;                     // 运行时的信息输出方法

        public Dictionary<string, Socket> clients = new Dictionary<string, Socket>();   // 存储连接到服务器的客户端信息
        public bool started = false;            // 标识当前是否启动了服务

        public Server(Print print = null, string ipString = null, int port = -1)
        {
            this.print = print;
            if (ipString != null) this.ipString = ipString;
            if (port >= 0)this.port = port;
        }

        public Server(Print print = null, string ipString = null, string port = "-1")
        {
            this.print = print;
            if (ipString != null) this.ipString = ipString;

            int port_int = Int32.Parse(port);
            if (port_int >= 0) this.port = port_int;
        }

        /// <summary>
        /// Print用于输出Server的输出信息
        /// </summary>
        public delegate void Print(string info);

        /// <summary>
        /// 启动服务
        /// </summary>
        public void start()
        {
            try
            {
                IPAddress address = IPAddress.Parse(ipString);
                socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                socket.Bind(new IPEndPoint(address, port));   
                socket.Listen(10);                              //最大连接数

                if (print != null)
                {
                    try { print("启动服务【" + socket.LocalEndPoint.ToString() + "】成功"); }
                    catch { print = null; }
                }
                started = true;

                new Thread(listenClientConnect).Start(socket);  // 在新的线程中监听客户端连接
            }
            catch (Exception exception)
            {
                if (print != null)
                {
                    print("启动服务失败 " + exception.ToString());
                }
                started = false;
            }
        }

        /// <summary>
        /// 监听客户端的连接
        /// </summary>
        private void listenClientConnect(object obj)
        {
            Socket socket_server = (Socket) obj;
            while (true)
            {
                Socket clientScoket = null;
                try
                {
                    clientScoket = socket_server.Accept();
                }
                catch (Exception ex)
                {
                    //MessageBox.Show("监听异常断开", "提醒"); //for debug
                    return;
                }

                if (print != null)
                {
                    print("客户端" + clientScoket.RemoteEndPoint.ToString() + "已连接");
                }
                new Thread(receiveData).Start(clientScoket);   // 在新的线程中接收客户端信息

                Thread.Sleep(1000);                            // 延时1秒后，接收连接请求

                if (!started) return;
            }
        }


        #region  Send函数

        //遍历客户端
        public int Send(byte[] data, int count)
        {
            try
            {
                foreach (string key in clients.Keys)
                {
                    Send(data, count, key);
                }
            }
            catch (Exception ex)
            {
                if (print != null) print("遍历错误");
                //MessageBox.Show("遍历错误", "提醒");    //for debug
            }
            return clients.Count;
        }

        private void Send(byte[] data, int count, string id)
        {
            if (clients.ContainsKey(id))
            {
                Socket socket_tx = clients[id];

                try
                {
                    Send(socket_tx, data, count);
                }
                catch (Exception ex)
                {
                    clients.Remove(id);

                    socket_tx.Shutdown(SocketShutdown.Both);   //add by zhj 2019-07-17
                    socket_tx.Close();

                    if (print != null) print("客户端已断开，【" + id + "】");
                   // MessageBox.Show("客户端已断开，【" + id + "】", "串口操作提醒"); //for debug
                }
            }
        }


        /// <summary>
        /// 发送信息
        /// </summary>
        /// 
        public void Send(string info, string id)
        {
            if (clients.ContainsKey(id))
            {
                Socket socket_tx = clients[id];

                try 
                { 
                    Send(socket_tx, info); 
                }
                catch(Exception ex)
                {
                    clients.Remove(id);
                    if (print != null) print("客户端已断开，【" + id + "】");
                }
            }
        }


        private void Send(Socket socket_tx, byte[] data, int count)
        {
            if (socket_tx != null && data != null && !data.Equals(""))
            {
                socket_tx.Send(data, count, SocketFlags.None);
            }
        }


        /// <summary>
        /// 通过socket发送数据data
        /// </summary>
        private void Send(Socket socket_tx, string data)
        {
            if (socket_tx != null && data != null && !data.Equals(""))
            {
                byte[] bytes = Encoding.UTF8.GetBytes(data);   // 将data转化为byte数组
                socket_tx.Send(bytes);                            // 
            }
        }

        #endregion

        private string clientIp = "";
        /// <summary>
        /// 输出Server的输出信息到客户端
        /// </summary>
        public void PrintOnClient(string info)
        {
            Send(info, clientIp);
        }

        /// <summary>
        /// 接收通过socket发送过来的数据
        /// </summary>
        private void receiveData(object obj)
        {
            Socket socket_rx = (Socket) obj;

            string clientIp = socket_rx.RemoteEndPoint.ToString();                 // 获取客户端标识 ip和端口
            if (!clients.ContainsKey(clientIp)) clients.Add(clientIp, socket_rx);  // 将连接的客户端socket添加到clients中保存
            else clients[clientIp] = socket_rx;

            while (true)
            {
                try
                {
                    string str = Receive(socket_rx);

                    //str处理
                }
                catch (Exception exception)
                {
                    if (print != null) print("连接已自动断开，【" + clientIp + "】" + exception.Message);
                    //MessageBox.Show("2客户端已断开，【" + clientIp + "】", "提醒"); //for debug

                    try
                    {
                        socket_rx.Shutdown(SocketShutdown.Both);       //add by zhj 2019-07-17
                        socket_rx.Close();
                        //MessageBox.Show("接收异常关闭客户端1，【" + clientIp + "】", "提醒"); //for debug
                    }
                    catch (Exception exception2)
                    {
                        //MessageBox.Show("关闭SOCKET异常关闭客户端，【" + clientIp + "】", "提醒"); //for debug
                    }


                    if (clients.ContainsKey(clientIp)) clients.Remove(clientIp);

                    return;
                    }

                if (!started)
                {
                    socket_rx.Shutdown(SocketShutdown.Both);       //add by zhj 2019-07-17
                    socket_rx.Close();
                    //MessageBox.Show("线程正常退出关闭客户端，【" + clientIp + "】", "提醒"); //for debug
                    return;
                }
                Thread.Sleep(200);      // 延时0.2秒后再接收客户端发送的消息
            }
        }


        /// <summary>
        /// 从socket接收数据
        /// </summary>
        private string Receive(Socket socket)
        {
            string data = "";

            byte[] bytes = null;
            int len = socket.Available;
            if (len > 0)
            {
                bytes = new byte[len];
                int receiveNumber = socket.Receive(bytes);
                
                data = Encoding.UTF8.GetString(bytes, 0, receiveNumber);    //如不是明文，不能直接转字符串
            }

            return data;
        }

        /// <summary>
        /// 停止服务
        /// </summary>
        public void stop()
        {
            if (started)
            {
                started = false;
                //socket.Shutdown(SocketShutdown.Receive);
                socket.Close();
                clients.Clear();
            }
        }



    }
}
