using System;
using System.IO;

namespace DataProcess
{
    #region 结构体定义

    public struct UdpData
    {
        public byte command;           //命令

        public float angular_v_x;      //X角速度
        public float angular_v_z;      //Z角速度
        public float angular_v_y;      //Y角速度

        public float angle_x;          //X角度
        public float angle_z;          //Z角度
        public float angle_y;          //Y角度

        public float angular_a_x;      //X角加速度
        public float angular_a_z;      //Z角加速度
        public float angular_a_y;      //Y角加速度
    }

    #endregion

    public class WT905DataProcess
    {
        int m_Pointer=0;
        byte[] ReceiveData = new byte[10000];
        public UdpData udp_frame_data = new UdpData();

        public byte CaluSum(byte[] data, int start_pos, int len)
        {
            int i;
            byte sum = 0;

            for (i = 0; i < len; i++)
                sum += data[start_pos + i];

            return sum;
        }


        public void WriteOneFrame(byte [] byte_buf, UdpData data)
        {
            byte[] buf;
            int i;
            byte bb;

            //报文头   
            byte_buf[0] = 0x6b;
            byte_buf[1] = 0x6c;

            //命令
            byte_buf[3] = data.command;

            //方位角速度 z
            buf = BitConverter.GetBytes(data.angular_v_z);
            for (i = 0; i < 4; i++)
                byte_buf[4 + i] = buf[i];
            //俯仰角速度 x
            buf = BitConverter.GetBytes(data.angular_v_x);
            for (i = 0; i < 4; i++)
                byte_buf[8 + i] = buf[i];
            //侧倾角速度 y
            buf = BitConverter.GetBytes(data.angular_v_y);
            for (i = 0; i < 4; i++)
                byte_buf[12 + i] = buf[i];


            //方位角度, z 
            buf = BitConverter.GetBytes(data.angle_z);
            for (i = 0; i < 4; i++)
                byte_buf[16 + i] = buf[i];
            //俯仰角度, x
            buf = BitConverter.GetBytes(data.angle_x);
            for (i = 0; i < 4; i++)
                byte_buf[20 + i] = buf[i];
            //侧倾角度, y
            buf = BitConverter.GetBytes(data.angle_y);
            for (i = 0; i < 4; i++)
                byte_buf[24 + i] = buf[i];

            //纵向加速度 y
            buf = BitConverter.GetBytes(data.angular_a_y);
            for (i = 0; i < 4; i++)
                byte_buf[28 + i] = buf[i];
            //横向加速度 x
            buf = BitConverter.GetBytes(data.angular_a_x);
            for (i = 0; i < 4; i++)
                byte_buf[32 + i] = buf[i];
            //垂向加速度 z
            buf = BitConverter.GetBytes(data.angular_a_z);
            for (i = 0; i < 4; i++)
                byte_buf[36 + i] = buf[i];

            //其他没有的数据，以及保留的字节，填0
            for (i = 40; i < 75; i++)
                byte_buf[i] = 0;

            //帧尾
            byte_buf[76] = 0x54;
            byte_buf[77] = 0x6f;
            byte_buf[78] = 0x43;
            byte_buf[79] = 0x41;

            //计算校验字节
            bb = 0;
            for (i = 3; i < 76; i++)
                //bb = bb + (byte) byte_buf[i];
                bb += byte_buf[i];
            byte_buf[2] = bb;
        }

        float GetValueByByte(byte low, byte high)
        {
            short s_tmp;
            float f_tmp;

            s_tmp = 0;
            s_tmp = (short)(s_tmp ^ high);
            s_tmp = (short)(s_tmp << 8);
            s_tmp = (short)(s_tmp ^ low);
            f_tmp = s_tmp;

            return f_tmp;
        }

        public void AddStringToBuf(byte[] data, int len)
        {

            if((len + m_Pointer) > 10000)       //如果数据太多，则会丢失
            {
                len = 10000 - m_Pointer;

            }

            Array.Copy(data, 0 , ReceiveData , m_Pointer, len); //data[0] to ReceiveData[m_Pointer], length = len

            m_Pointer += len;
        }


        public bool SearchOneFrameData()
        {
            int i,len;

            if (m_Pointer < 33)
                return false;


            len = m_Pointer - 32;

            for(i=0;i< len;i++)
            {
                if(   (ReceiveData[i + 0 ] == 0x55) && (ReceiveData[i + 1 ] == 0x51) 
                    &&(ReceiveData[i + 11] == 0x55) && (ReceiveData[i + 12] == 0x52)
                    &&(ReceiveData[i + 22] == 0x55) && (ReceiveData[i + 23] == 0x53))
                {
                    //检查累加和
                    if (ReceiveData[i + 10] != CaluSum(ReceiveData, i, 10))
                        continue;
                    if (ReceiveData[i + 21] != CaluSum(ReceiveData, i+11, 10))
                        continue;
                    if (ReceiveData[i + 32] != CaluSum(ReceiveData, i+22, 10))
                        continue;

                    //提取原始数据
                    udp_frame_data.angular_a_x = GetValueByByte(ReceiveData[i + 2], ReceiveData[i + 3]);
                    udp_frame_data.angular_a_y = GetValueByByte(ReceiveData[i + 4], ReceiveData[i + 5]);
                    udp_frame_data.angular_a_z = GetValueByByte(ReceiveData[i + 6], ReceiveData[i + 7]);

                    udp_frame_data.angular_v_x = GetValueByByte(ReceiveData[i + 13], ReceiveData[i + 14]);
                    udp_frame_data.angular_v_x = GetValueByByte(ReceiveData[i + 15], ReceiveData[i + 16]);
                    udp_frame_data.angular_v_x = GetValueByByte(ReceiveData[i + 17], ReceiveData[i + 18]);

                    udp_frame_data.angle_x = GetValueByByte(ReceiveData[i + 24], ReceiveData[i + 25]);
                    udp_frame_data.angle_y = GetValueByByte(ReceiveData[i + 26], ReceiveData[i + 27]);
                    udp_frame_data.angle_z = GetValueByByte(ReceiveData[i + 28], ReceiveData[i + 29]);

                    //数据转换
                    udp_frame_data.angular_a_x = udp_frame_data.angular_a_x * 16 * 9.8f / 32767;
                    udp_frame_data.angular_a_y = udp_frame_data.angular_a_y * 16 * 9.8f / 32767;
                    udp_frame_data.angular_a_z = udp_frame_data.angular_a_z * 16 * 9.8f / 32767;

                    udp_frame_data.angular_v_x = udp_frame_data.angular_v_x * 2000 / 32767;
                    udp_frame_data.angular_v_y = udp_frame_data.angular_v_y * 2000 / 32767;
                    udp_frame_data.angular_v_z = udp_frame_data.angular_v_z * 2000 / 32767;

                    udp_frame_data.angle_x = udp_frame_data.angle_x * 180 / 32767;
                    udp_frame_data.angle_y = udp_frame_data.angle_y * 180 / 32767;
                    udp_frame_data.angle_z = udp_frame_data.angle_z * 180 / 32767;

                    //更新剩余的字符数量
                    m_Pointer = m_Pointer - (i + 33);
                    //移动剩余字符
                    Array.Copy(ReceiveData,i+33,ReceiveData,0,m_Pointer);

                    return true;
                }
            }

            //没搜到数据，也要更新剩余的字符数量
            m_Pointer = m_Pointer - i;
            //移动剩余字符
            Array.Copy(ReceiveData, i, ReceiveData, 0, m_Pointer);

            return false;
        }

        /// 保存数据data到文件的处理过程；
        /// </summary>
        /// <param name="data"></param>


        public void SavaProcess(string file_name)
        {
            string data = "";

            data += udp_frame_data.angle_x.ToString() + ",";
            data += udp_frame_data.angle_y.ToString() + ",";
            data += udp_frame_data.angle_z.ToString() + ",";

            data += udp_frame_data.angular_v_x.ToString() + ",";
            data += udp_frame_data.angular_v_y.ToString() + ",";
            data += udp_frame_data.angular_v_z.ToString() + ",";

            data += udp_frame_data.angular_a_x.ToString() + ",";
            data += udp_frame_data.angular_a_y.ToString() + ",";
            data += udp_frame_data.angular_a_z.ToString();


            FileStream fs = new FileStream(@file_name, FileMode.OpenOrCreate, FileAccess.Write);
            StreamWriter m_streamWriter = new StreamWriter(fs);
            m_streamWriter.BaseStream.Seek(0, SeekOrigin.End);
            //m_streamWriter.WriteLine(string.Format("Service Start at {0}", DateTime.Now.ToString()));
            m_streamWriter.WriteLine(data);
            m_streamWriter.Flush();
            m_streamWriter.Close();

            //System.IO.StreamWriter file = new System.IO.StreamWriter(file_name, false);
            //file
            //保存数据到文件
            //file.Write(data);
            //关闭文件
            //file.Close();
            //释放对象
            //file.Dispose();
        }
    }

}


