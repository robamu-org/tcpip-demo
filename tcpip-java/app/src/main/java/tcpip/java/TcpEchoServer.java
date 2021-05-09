package tcpip.java;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;


public class TcpEchoServer extends Thread {
    private ServerSocket socket;

    public TcpEchoServer() throws IOException {
	socket = new ServerSocket();
    }

    @Override
    public void run () {
	while(true) {
	    try {
		Socket client = socket.accept();
		/* Assuming that the client sends string data for now */
		EchoHandler handler = new EchoHandler(client, true);
		handler.start();
	    }
	    catch(IOException e) {
		e.printStackTrace();
		System.exit(1);
	    }
	}
    }
}


class EchoHandler extends Thread {
    Socket client;
    boolean stringData;

    EchoHandler (Socket client, boolean stringData) {
	this.client = client;
	this.stringData = stringData;
    }

    public void run () {
	try {
	    if(!stringData) {
		handleBytes();
	    }
	    else {
		handleStrings();
	    }
	}
	catch (IOException e) {
	    e.printStackTrace();
	    System.exit(1);
	}
	finally {
	    try {
		client.close();
	    }
	    catch (IOException e) {
		e.printStackTrace();
		System.exit(1);
	    }
	}
    }

    private void handleBytes() throws IOException {
	InputStream input = client.getInputStream();
	OutputStream output = client.getOutputStream();
	while(true) {
	    byte[] bytes = input.readAllBytes();
	    if (bytes.length > 0) {
		String data = new String(bytes, 0, bytes.length);
		System.out.println(Util.AnsiColors.ANSI_GREEN + "Server: Received " 
			+ bytes.length + " bytes: " + data);
		output.write(bytes);
	    }
	    else if (bytes.length == 0) {
		client.shutdownInput();
	    }
	    else {
		// Should not happen
	    }
	}
    }

    /**
     * Handle TCP client connection which only send strings.
     */
    private void handleStrings() throws IOException {
	PrintWriter output = new PrintWriter(client.getOutputStream(), true);
	BufferedReader input = new BufferedReader(new InputStreamReader(client.getInputStream()));
	String data = input.readLine();
	while(data != null && data.length() > 0) {
	    System.out.println(Util.AnsiColors.ANSI_GREEN + "Server: Received " 
		    + data.length() + " bytes: " + data);
	    output.println(data);
	}
	output.close();
	input.close();
    }
}
