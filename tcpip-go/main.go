package main

import (
	"tcpip-go/tcp"
)

func main() {
	go tcp.Server()
	tcp.Client()
}
