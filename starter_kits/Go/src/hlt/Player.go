package hlt

import (
	"errors"
	"fmt"

	"./input"
)

// Player - Structure to hold all information about a player
type Player struct {
	ID       int
	Shipyard *Shipyard
	Halite   int
	Ships    map[int]*Ship
	Dropoffs map[int]*Dropoff
}

func (p *Player) String() string {
	return fmt.Sprintf("Player{ID=%d,Halite=%d}", p.ID, p.Halite)
}

// NewPlayer - Creates a new player from input data
func NewPlayer() *Player {
	var input = input.GetInstance()
	var playerID, _ = input.GetInt()
	var x, _ = input.GetInt()
	var y, _ = input.GetInt()
	return &Player{playerID, NewShipyard(playerID, &Position{x, y}), 0, nil, nil}
}

// GetShip - Get a specific Ship by its ID.
func (p *Player) GetShip(shipId int) (*Ship, error) {
	if ship, ok := p.Ships[shipId]; ok {
		return ship, nil
	}
	return nil, errors.New(fmt.Sprintf("No ship with ID %d", shipId))
}

// Update - Updates the player, reading the ships and dropoffs data
func (p *Player) Update(numShips int, numDropoffs int, halite int) {
	p.Halite = halite
	p.Ships = make(map[int]*Ship)
	p.Dropoffs = make(map[int]*Dropoff)
	for i := 0; i < numShips; i++ {
		var nextShip = NewShip(p.ID)
		p.Ships[nextShip.E.id] = nextShip
	}
	for i := 0; i < numDropoffs; i++ {
		var nextDropoff = NewDropoff(p.ID)
		p.Dropoffs[nextDropoff.E.id] = nextDropoff
	}
}
