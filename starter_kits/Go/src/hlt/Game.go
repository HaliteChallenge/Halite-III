package hlt

import (
	"fmt"

	"./gameconfig"
	"./input"
	"./log"
)

// Game - Structure holding all metadata for the game
type Game struct {
	NumPlayers int
	Me         *Player
	Players    []*Player
	Map        *GameMap
	TurnNumber int
}

func (g *Game) String() string {
	return fmt.Sprintf("Game{NumPlayers=%d,Me=%s,Players=%d,Map=%s}", g.NumPlayers, g.Me.String(), len(g.Players), g.Map.String())
}

// Ready - When run, notifies the server that the bot is ready to start
func (g *Game) Ready(name string) {
	fmt.Println(name)
}

// NewGame - Creates a new game for bot named by the parameter
func NewGame() *Game {
	var input = input.GetInstance()
	if !input.Scanner.Scan() {
		return nil
	}

	var constantsString = input.Scanner.Text()
	gameconfig.Init(constantsString)
	var NumPlayers, _ = input.GetInt()
	var myID, _ = input.GetInt()
	var Players = make([]*Player, NumPlayers)
	for i := range Players {
		Players[i] = NewPlayer()
	}
	var gameMap = GenerateGameMap()
	var me = Players[myID]
	return &Game{NumPlayers, me, Players, gameMap, 0}
}

// UpdateFrame - Runs a single turn in the game
func (g *Game) UpdateFrame() {
	var logger = log.GetInstance()
	var input = input.GetInstance()
	g.TurnNumber, _ = input.GetInt()
	logger.Printf("=============== TURN %d ================\n", g.TurnNumber)
	for i := range g.Players {
		// Player ID variable being read here. It isn't used, so we can't assign it in Go without error, but we still need to consume it from the input
		input.GetInt()
		var numShips, _ = input.GetInt()
		var numDropoffs, _ = input.GetInt()
		var halite, _ = input.GetInt()
		g.Players[i].Update(numShips, numDropoffs, halite)
	}
	g.Map.Update()
	for i := range g.Players {
		var player = g.Players[i]
		for j := range player.Ships {
			var ship = player.Ships[j]
			g.Map.AtEntity(ship.E).MarkUnsafe(ship)
		}
		for k := range player.Dropoffs {
			var dropoff = player.Dropoffs[k]
			g.Map.AtEntity(dropoff.E).structure = dropoff.E
		}
	}
}

// EndTurn -
func (g *Game) EndTurn(commands []Command) {
	for _, command := range commands {
		fmt.Print(command.CommandString())
		fmt.Print(" ")
	}
	fmt.Println()
}
