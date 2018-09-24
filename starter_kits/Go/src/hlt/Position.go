package hlt

import "fmt"

// Position - Location on the game map
type Position struct {
	x int
	y int
}

func (p *Position) String() string {
	return fmt.Sprintf("Pos{x=%d,y=%d}", p.x, p.y)
}

// DirectionalOffset - Returns the position of a move in the direction
func (p *Position) DirectionalOffset(d *Direction) (*Position, error) {
	switch d.charValue {
	case NORTH:
		return &Position{p.x, p.y - 1}, nil
	case SOUTH:
		return &Position{p.x, p.y + 1}, nil
	case EAST:
		return &Position{p.x + 1, p.y}, nil
	case WEST:
		return &Position{p.x - 1, p.y}, nil
	case STILL:
		return &Position{p.x, p.y}, nil
	}
	return nil, fmt.Errorf("Invalid direction %c", d.charValue)
}
