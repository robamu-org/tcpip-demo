package tcp

import (
	"bufio"
	"fmt"
	"net"
	"os"
	"strings"

	"tcpip-go/config"
)

func Client() {

	CONNECT := "127.0.0.1:" + config.PORT
	conn, err := net.Dial("tcp", CONNECT)
	if err != nil {
		fmt.Println(err)
		return
	}

	for {
		reader := bufio.NewReader(os.Stdin)
		fmt.Print("Client Message >> ")
		text, _ := reader.ReadString('\n')
		fmt.Fprintf(conn, text+"\n")

		message, _ := bufio.NewReader(conn).ReadString('\n')
		fmt.Print("->: " + message)
		if strings.TrimSpace(string(text)) == "STOP" {
			fmt.Println("TCP client exiting...")
			return
		}
	}
}
