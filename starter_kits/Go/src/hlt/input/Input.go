package input

import (
	"bufio"
	"io"
	"os"
	"regexp"
	"strconv"
	"sync"

	"../log"
)

// Input - Reads from the game runner, helps with parsing
type Input struct {
	Scanner  *bufio.Scanner
	Buffer   []string
	Position int
}

var instance *Input
var loadErr error
var once sync.Once

// GetInstance - Creates an instance of the game constants
func GetInstance() *Input {
	if instance == nil {
		once.Do(func() {
			scanner := bufio.NewScanner(os.Stdin)
			if loadErr != nil {
				instance = nil
			}
			instance = &Input{scanner, nil, -1}
		})
	}
	return instance
}

func deleteEmpty(s []string) []string {
	var r []string
	for _, str := range s {
		if str != "" {
			r = append(r, str)
		}
	}
	return r
}

func (i *Input) readLine() error {
	if !i.Scanner.Scan() {
		log.GetInstance().Printf("Input connection from server closed. Exiting...")
		os.Exit(0)
	}
	var nextLine = i.Scanner.Text()
	splitter := regexp.MustCompile(" +")
	i.Buffer = deleteEmpty(splitter.Split(nextLine, -1))
	i.Position = 0
	return nil
}

// GetString -
func (i *Input) GetString() (string, error) {
	if i.Buffer == nil || i.Position >= len(i.Buffer) {
		var err = i.readLine()
		if err == io.EOF {
			return "", err
		}
	}
	var nextToken = i.Buffer[i.Position]
	i.Position++
	return nextToken, nil
}

// GetInt -
func (i *Input) GetInt() (int, error) {
	var nextToken, err = i.GetString()
	if err != nil {
		return -1, err
	}
	return strconv.Atoi(nextToken)
}

// GetFloat -
func (i *Input) GetFloat() (float64, error) {
	var nextToken, err = i.GetString()
	if err != nil {
		return -1, err
	}
	return strconv.ParseFloat(nextToken, 64)
}

// GetBool -
func (i *Input) GetBool() (bool, error) {
	var nextToken, err = i.GetString()
	if err != nil {
		return false, err
	}
	return strconv.ParseBool(nextToken)
}
