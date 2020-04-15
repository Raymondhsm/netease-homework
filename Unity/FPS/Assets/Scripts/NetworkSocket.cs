using UnityEngine;
using System.Collections;
using System;
using System.IO;
using System.Net.Sockets;

 

public class NetworkSocket : MonoBehaviour
{
    public String host = "localhost";
    public Int32 port = 8765;

    internal Boolean socket_ready = false;
    internal String input_buffer = "";
    TcpClient tcp_socket;
    NetworkStream net_stream;

    StreamWriter socket_writer;
    StreamReader socket_reader;



    void Update()
    {
        string received_data = readSocket();
        // string key_stroke = Input.inputString;

        // // Collects keystrokes into a buffer
        // if (key_stroke != ""){
        //     input_buffer += key_stroke;

        //     if (key_stroke == "\n"){
        //     	// Send the buffer, clean it
        //     	Debug.Log("Sending: " + input_buffer);
        //     	writeSocket(input_buffer);
        //     	input_buffer = "";
        //     }

        // }

        // writeSocket("kasjdhakjhdjkashjkdhasjkdhkjashdjkahdjkahdjkhajkshdjka");


        if (received_data != "")
        {
        	// Do something with the received data,
        	// print it in the log for now
            Debug.Log(received_data);
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

    public void writeSocket(string line)
    {
        if (!socket_ready)
            return;
            
        line = line + "\r\n";
        socket_writer.Write(line);
        socket_writer.Flush();
    }

    public String readSocket()
    {
        if (!socket_ready)
            return "";

        if (net_stream.DataAvailable){
            // string data = "";
            // for(int i=0; i<2 && !socket_reader.EndOfStream; i++)
            //     data += socket_reader.ReadLine();
            // return data;
            // return socket_reader.ReadLine();
            byte[] b = new byte[1024];
            net_stream.Read(b,0,1024);
            Debug.Log(System.Text.Encoding.UTF8.GetString(b));
            return System.Text.Encoding.ASCII.GetString(b);

        }

        return "";
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