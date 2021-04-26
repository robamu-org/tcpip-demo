package tcpip.java;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;


public class UdpEchoServer extends Thread {
    private DatagramSocket udpSocket;
    private byte[] buf = new byte[App.BUFFER_SIZE];

    public UdpEchoServer() {
	try {
	    udpSocket = new DatagramSocket(App.SERVER_PORT);
	} catch (SocketException e) {
	    e.printStackTrace();
	    System.exit(1); 
	}
    }

    @Override 
    public void run() {
	try {
	    DatagramPacket packet = new DatagramPacket(buf, buf.length);

	    udpSocket.receive(packet);

	    InetAddress address = packet.getAddress();

	    String received = new String(packet.getData(), 0, packet.getLength());
	    System.out.println(Util.AnsiColors.ANSI_GREEN + "Server: Received " 
		    + packet.getLength() + " bytes: " + received);

	    int port = packet.getPort();
	    packet = new DatagramPacket(buf, buf.length, address, port);
	    udpSocket.send(packet);
	} catch (IOException e) {
	    e.printStackTrace();
	    System.exit(1); 
	}

	udpSocket.close();
    }
}
