package tcpip.java;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

public class UdpEchoClient extends Thread {
    private DatagramSocket socket;
    private InetAddress address;

    private byte[] buf;

    public UdpEchoClient() throws SocketException, UnknownHostException {
	socket = new DatagramSocket();
	address = InetAddress.getByName("localhost");
    }

    @Override
    public void run () {
	try {
	    String msg = "Hello, this is a UDP test";
	    buf = msg.getBytes();
	    System.out.println(Util.AnsiColors.ANSI_CYAN + "Client: Sending " + 
		    buf.length + " bytes");

	    DatagramPacket packet = new DatagramPacket(buf, buf.length, address, App.SERVER_PORT);

	    socket.send(packet);
	    packet = new DatagramPacket(buf, buf.length);

	    socket.receive(packet);

	    String received = new String(packet.getData(), 0, packet.getLength());
	    System.out.println(Util.AnsiColors.ANSI_CYAN + "Client: Received back " + 
		    packet.getLength() + " bytes: " + received);
	} catch (IOException e) {
	    e.printStackTrace();
	}
    }
}
