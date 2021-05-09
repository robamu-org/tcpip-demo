package tcpip.java;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

public class TcpEchoClient extends Thread {
    private ServerSocket socket;
    private InetAddress address;

    private byte[] buf;

    public TcpEchoClient(int port) throws IOException {
	try {
	    socket = new ServerSocket(port);
	} catch (SocketException e) {
	    e.printStackTrace();
	    System.exit(1);
	}

	try {
	    address = InetAddress.getByName("localhost");
	} catch (UnknownHostException e) {
	    e.printStackTrace();
	    System.exit(1);
	}
    }

    @Override
    public void run () {
//	try {
//	    String msg = "Hello, this is a UDP test";
//	    buf = msg.getBytes();
//	    DatagramPacket packet = new DatagramPacket(buf, buf.length, address, App.SERVER_PORT);
//
//	    socket.send(packet);
//	    packet = new DatagramPacket(buf, buf.length);
//
//	    socket.receive(packet);
//
//	    String received = new String(packet.getData(), 0, packet.getLength());
//	    System.out.println(Util.AnsiColors.ANSI_CYAN + "Client: Received back " + 
//		    packet.getLength() + " bytes: " + received);
//	} catch (IOException e) {
//	    e.printStackTrace();
//	}
    }
}
