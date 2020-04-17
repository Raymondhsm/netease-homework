using UnityEngine;
using System.Collections.Generic;
using System;
using System.IO;
using System.Net.Sockets;
 

public class NetworkSocket : MonoBehaviour
{
    public static int NET_HEAD_LENGTH_SIZE	= 4;		// 4 bytes little endian (x86)

    public String host = "localhost";
    public Int32 port = 8765;

    internal Boolean socket_ready = false;
    TcpClient tcp_socket;
    NetworkStream net_stream;

    StreamWriter socket_writer;
    StreamReader socket_reader;

    Queue<string> recv_buff;


    void Start()
    {
        recv_buff = new Queue<string>();
    }

    void Update()
    {
        readSocket();
        while(recv_buff.Count > 0){
            Debug.Log(read());
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

    public void writeSocket(string msg)
    {
        if (!socket_ready)
            return;
            
        int size = msg.Length + NET_HEAD_LENGTH_SIZE;
        byte [] sizeBuff = BitConverter.GetBytes(size);     // 将 int 转换成字节数组
        string data = System.Text.Encoding.Default.GetString(sizeBuff) + msg;
        socket_writer.Write(data);
        socket_writer.Flush();
    }

    public void send(int command, string msg)
    {
        if (!socket_ready)
            return;

        byte[] commandBuff = BitConverter.GetBytes(command);
        string data = System.Text.Encoding.Default.GetString(commandBuff) + msg;
        writeSocket(data);
    }

    public void readSocket()
    {
        if (!socket_ready)return;

        while (net_stream.DataAvailable)
        {
            char[] head = new char[NET_HEAD_LENGTH_SIZE];
            socket_reader.Read(head, 0, head.Length);

            int dataLen = Utils.CharArrayToInt32(head) - NET_HEAD_LENGTH_SIZE;
            char[] data = new char[dataLen];
            socket_reader.Read(data, 0, data.Length);

            recv_buff.Enqueue(new String(data));
        }
    }

    public String read()
    {
        if(recv_buff.Count == 0) return "";

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