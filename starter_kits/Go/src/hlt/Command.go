package hlt

import "fmt"

// Command - Interface for generating text commands to print out
type Command interface {
	CommandString() string
}

// SpawnShip - Command to spawn a ship
type SpawnShip struct {
}

// CommandString - Returns the text string for the TransformToDropoff command
func (t SpawnShip) CommandString() string {
	return fmt.Sprintf("g")
}

// TransformToDropoff - Command to transform ship into a dropoff point
type TransformToDropoff struct {
	id int
}

// CommandString - Returns the text string for the TransformToDropoff command
func (t TransformToDropoff) CommandString() string {
	return fmt.Sprintf("c %d", t.id)
}

// Move - A command that moves an entity a direction
type Move struct {
	id        int
	direction *Direction
}

// CommandString - Returns the text string for the Move command
func (m Move) CommandString() string {
	return fmt.Sprintf("m %d %c", m.id, m.direction.charValue)
}
