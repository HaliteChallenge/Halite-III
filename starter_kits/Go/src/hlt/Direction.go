package hlt

import (
	"fmt"
)

// Direction - Struct for holding cardinal directions
type Direction struct {
	charValue byte
}

// Single character abbreviations for directions
const (
	NORTH byte = 'n'
	SOUTH byte = 's'
	EAST  byte = 'e'
	WEST  byte = 'w'
	STILL byte = 'o'
)

// AllDirections - Array of all directions
var AllDirections = [...]*Direction{North(), South(), East(), West(), Still()}

// North -
func North() *Direction {
	return &Direction{NORTH}
}

// South -
func South() *Direction {
	return &Direction{SOUTH}
}

// East -
func East() *Direction {
	return &Direction{EAST}
}

// West -
func West() *Direction {
	return &Direction{WEST}
}

// Still - Effectively a no-op
func Still() *Direction {
	return &Direction{STILL}
}

// Inverse - Returns the opposite of a direction
func (d *Direction) Inverse() (*Direction, error) {
	switch d.charValue {
	case NORTH:
		return South(), nil
	case SOUTH:
		return North(), nil
	case EAST:
		return West(), nil
	case WEST:
		return East(), nil
	case STILL:
		return Still(), nil
	}
	return nil, fmt.Errorf("Invalid direction %c", d.charValue)
}
