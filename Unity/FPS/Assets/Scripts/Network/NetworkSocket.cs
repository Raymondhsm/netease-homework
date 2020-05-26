using UnityEngine;
using System.Collections.Generic;
using System;
using System.IO;
using System.Web;
using System.Net.Sockets;
 

public class NetworkSocket : MonoBehaviour
{
    public static int NET_HEAD_LENGTH_SIZE	= 4;		// 4 bytes little endian (x86)

    public String host = "localhost";
    public Int32 port = 8765;
    public bool fightServer = false;

    internal Boolean socket_ready = false;
    TcpClient tcp_socket;
    NetworkStream net_stream;

    StreamWriter socket_writer;
    StreamReader socket_reader;

    Queue<Recv_data> recv_buff;

    public struct Recv_data
    {
        public int command;
        public string data;
    }

    private Controller m_controller;

    void Start()
    {
        m_controller = GameObject.Find("Controller").GetComponent<Controller>();
        recv_buff = new Queue<Recv_data>();
        if(fightServer){
            send(Config.COMMAND_ATTEND_GAME, "{\"sessionID\":\""+m_controller.sessionID+"\"}");
        }
    }

    void Update()
    {
        readSocket();
        while(recv_buff.Count > 0){
            Recv_data recv = read();
            Service.Instance().ServiceProcess(recv.command, recv.data);
        }
    }


    void Awake()
    {
        setupSocket();
    }

    void OnApplicationQuit()
    {
        closeSocket();
    }

    public void setupSocket()
    {
        try
        {
            tcp_socket = new TcpClient(host, port);

            net_stream = tcp_socket.GetStream();
            socket_writer = new StreamWriter(net_stream);
            socket_reader = new StreamReader(net_stream);

            socket_ready = true;
        }
        catch (Exception e)
        {
        	// Something went wrong
            Debug.Log("Socket error: " + e);
        }
    }

    public void writeSocket(byte[] msg)
    {
        if (!socket_ready)
            return;
            
        int size = msg.Length + NET_HEAD_LENGTH_SIZE;
        byte[] sizeBuff = BitConverter.GetBytes(size);     // 将 int 转换成字节数组
        byte[] sendData = Utils.MergeByteArray(sizeBuff, msg); 

        net_stream.Write(sendData, 0, sendData.Length);
        net_stream.Flush();

        // socket_writer.Write(data);
        // socket_writer.Flush();
    }

    public void send(int command, string msg)
    {
        if (!socket_ready)
            return;

        byte[] commandBuff = BitConverter.GetBytes(command);
        byte[] data = System.Text.Encoding.Default.GetBytes(msg);
        writeSocket(Utils.MergeByteArray(commandBuff, data));
    }

    public void readSocket()
    {
        if (!socket_ready)return;

        while (net_stream.DataAvailable)
        {
            byte[] head = new byte[NET_HEAD_LENGTH_SIZE];
            net_stream.Read(head, 0, head.Length);

            byte[] command = new byte[Config.COMMAND_LENGTH_SIZE];
            net_stream.Read(command, 0, command.Length);
            int com = BitConverter.ToInt32(command, 0);

            int dataLen = BitConverter.ToInt32(head, 0) - NET_HEAD_LENGTH_SIZE - Config.COMMAND_LENGTH_SIZE;
            byte[] data = new byte[dataLen];
            net_stream.Read(data, 0, data.Length);

            Recv_data recv_data;
            recv_data.command = com;
            recv_data.data = System.Text.Encoding.Default.GetString(data);

            recv_buff.Enqueue(recv_data);
        }
    }

    public Recv_data read()
    {
        return recv_buff.Dequeue();
    }

    public void closeSocket()
    {
        if (!socket_ready)
            return;

        socket_writer.Close();
        socket_reader.Close();
        tcp_socket.Close();
        socket_ready = false;
    }

}