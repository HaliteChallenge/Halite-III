package log

import (
	"fmt"
	"log"
	"os"
)

// FileLogger - singleton
type FileLogger struct {
	Logger *log.Logger
	file   *os.File
}

var defaultLogger *FileLogger

// GetInstance - Gets the logger instance
func GetInstance() *FileLogger {
	return defaultLogger
}

// NewFileLogger - Initializes the logger
func NewFileLogger(botID int) *FileLogger {
	if defaultLogger == nil {
		var logfileName = fmt.Sprintf("bot-%d.log", botID)
		f, err := os.OpenFile(logfileName, os.O_WRONLY|os.O_CREATE|os.O_APPEND, 0644)
		if err != nil {
			log.Fatal(err)
		}

		var logger = log.New(f, "halite", log.Lshortfile|log.LstdFlags)
		defaultLogger = &FileLogger{logger, f}
	} else {
		defaultLogger.Printf("Error: log: tried to open( %d ) but we have already opened before.", botID)
	}
	return defaultLogger
}

// Close -
func (fl *FileLogger) Close() {
	fl.file.Close()
}

// Printf - Passthrough to log.Logger
func (fl *FileLogger) Printf(format string, v ...interface{}) {
	fl.Logger.Printf(format, v...)
}
