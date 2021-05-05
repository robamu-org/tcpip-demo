package tcp

import (
	"bufio"
	"fmt"
	"net"
	"strings"
	"time"

	"tcpip-go/config"
)

func Test() {
	fmt.Print("TEATEATEA")
}
func Server() {
	PORT := ":" + config.PORT
	listener, err := net.Listen("tcp", PORT)
	if err != nil {
		fmt.Println(err)
		return
	}
	defer listener.Close()

	conn, err := listener.Accept()
	if err != nil {
		fmt.Println(err)
		return
	}

	for {
		netData, err := bufio.NewReader(conn).ReadString('\n')
		if err != nil {
			fmt.Println(err)
			return
		}
		if strings.TrimSpace(string(netData)) == "STOP" {
			fmt.Println("Exiting TCP server!")
			return
		}

		fmt.Print("Server >> ", string(netData))
		t := time.Now()
		myTime := t.Format(time.RFC3339) + "\n"
		conn.Write([]byte(myTime))
	}
}
