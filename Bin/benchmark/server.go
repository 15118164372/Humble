package main

import (
	"encoding/binary"
	"fmt"
	"gokit/misc/packet"
	"gokit/utils"
	"io"
	"net"
	"strconv"
	"time"

	"sync/atomic"

	"gopkg.in/urfave/cli.v2"
)

var g_bQuit = false
var g_ProtoCount = uint16(40)

type Server struct {
	PackCount    int64
	Excption     int32
	RunningCount int32
	SendPackLens int64
	cli          *cli.Context
	tcpAddr      *net.TCPAddr
	msg          []string
}

func (s *Server) ReadMsg(conn *net.TCPConn, nextChan chan bool) {
	header := make([]byte, 4)

	for !g_bQuit {
		conn.SetReadDeadline(time.Now().Add(time.Duration(60) * time.Second))
		_, err := io.ReadFull(conn, header)
		if err != nil {
			fmt.Println(err)
			if !g_bQuit {
				atomic.AddInt32(&s.Excption, 1)
			}
			return
		}

		size := binary.BigEndian.Uint32(header)
		payload := make([]byte, size)
		_, err = io.ReadFull(conn, payload)
		if err != nil {
			fmt.Println(err)
			if !g_bQuit {
				atomic.AddInt32(&s.Excption, 1)
			}
			return
		}

		nextChan <- true
	}
}

func (s *Server) SendMsg(conn *net.TCPConn) int64 {
	proto := utils.RandAB(1, uint32(g_ProtoCount))
	sendMsg := s.msg[utils.RandAB(0, uint32(len(s.msg)-1))]

	writer := packet.Writer()
	iLens := len(sendMsg) + 2

	writer.WriteU32(uint32(iLens))
	writer.WriteU16(uint16(proto))
	writer.WriteRawBytes([]byte(sendMsg))
	conn.Write(writer.Data())

	return int64(iLens) + 4
}

func (s *Server) worker() {
	conn, err := net.DialTCP("tcp", nil, s.tcpAddr)
	if err != nil {
		fmt.Println(err)
		atomic.AddInt32(&s.RunningCount, -1)
		atomic.AddInt32(&s.Excption, 1)
		return
	}

	packLens := int64(0)
	packCount := int64(0)
	defer func() {
		conn.Close()
		atomic.AddInt64(&s.PackCount, packCount)
		atomic.AddInt64(&s.SendPackLens, packLens)
		atomic.AddInt32(&s.RunningCount, -1)
	}()

	sendChan := make(chan bool, 10)

	go s.ReadMsg(conn, sendChan)
	packLens += s.SendMsg(conn)
	packCount++

	for !g_bQuit {
		select {
		case _ = <-sendChan:
			packLens += s.SendMsg(conn)
			packCount++
		}
	}
}

func (s *Server) Service() {
	msgString := []string{"a", "b", "c", "d", "e", "f", "d", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t"}
	for i := 100; i <= 1024; i++ {
		var msg string
		for j := 0; j < i; j++ {
			msg += msgString[utils.RandAB(0, uint32(len(msgString)-1))]
		}
		msg += strconv.Itoa(i)
		s.msg = append(s.msg, msg)
	}

	tcpAddr, err := net.ResolveTCPAddr("tcp", s.cli.String("server"))
	if err != nil {
		fmt.Println(err)
		return
	}
	s.tcpAddr = tcpAddr
	s.RunningCount = int32(s.cli.Int("link"))
	for i := int32(0); i < s.RunningCount; i++ {
		go s.worker()
	}

	tickRun := time.NewTicker(time.Duration(s.cli.Int("runtime")) * time.Second)
	defer func() {
		tickRun.Stop()
	}()

	for !g_bQuit {
		select {
		case <-tickRun.C:
			g_bQuit = true
		}
	}

	for atomic.LoadInt32(&s.RunningCount) > 0 {
		time.Sleep(100 * time.Millisecond)
	}

	fmt.Println("total link:", s.cli.Int("link"))
	fmt.Println("excption link:", s.Excption)
	fmt.Println("pack count:", s.PackCount)
	fmt.Println("run time(sec):", s.cli.Int("runtime"))
	fmt.Println("per second count:", s.PackCount/int64(s.cli.Int("runtime")))
	fmt.Println("send pack lens(B):", s.SendPackLens)
	fmt.Println("per second send(B):", s.SendPackLens/int64(s.cli.Int("runtime")))

	time.Sleep(3 * time.Second)
}
