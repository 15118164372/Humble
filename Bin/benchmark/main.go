package main

import (
	"fmt"
	"os"

	"gopkg.in/urfave/cli.v2"
)

func main() {
	app := &cli.App{
		Name:    "benchmark",
		Usage:   "benchmark --server=127.0.0.1:16000 --runtime=60 --link=1000",
		Version: "1.0",
		Flags: []cli.Flag{
			&cli.StringFlag{
				Name:  "server",
				Value: "127.0.0.1:16000",
				Usage: "server address:port",
			},
			&cli.IntFlag{
				Name:  "runtime",
				Value: 60,
				Usage: "run time(sec)",
			},
			&cli.IntFlag{
				Name:  "link",
				Value: 1000,
				Usage: "link number",
			},
		},
		Action: func(c *cli.Context) error {
			fmt.Println("server:", c.String("server"))
			fmt.Println("runtime:", c.Int("runtime"))
			fmt.Println("link:", c.Int("link"))

			sv := Server{cli: c}

			sv.Service()
			return nil
		},
	}
	app.Run(os.Args)
}
