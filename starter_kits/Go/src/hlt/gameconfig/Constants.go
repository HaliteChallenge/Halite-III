package gameconfig

import (
	"bytes"
	"fmt"
	"regexp"
	"strconv"
	"sync"

	"../log"
)

// Game setting keys
const (
	ShipCost                 string = "NEW_ENTITY_ENERGY_COST"    /** The maximum amount of halite a ship can carry. */
	DropoffCost              string = "DROPOFF_COST"              /** The cost to build a single ship. */
	MaxHalite                string = "MAX_ENERGY"                /** The cost to build a dropoff. */
	MaxTurns                 string = "MAX_TURNS"                 /** The maximum number of turns a game can last. */
	ExtractRatio             string = "EXTRACT_RATIO"             /** 1/EXTRACT_RATIO halite (rounded) is collected from a square per turn. */
	MoveCostRatio            string = "MOVE_COST_RATIO"           /** 1/MOVE_COST_RATIO halite (rounded) is needed to move off a cell. */
	InspirationEnabled       string = "INSPIRATION_ENABLED"       /** Whether inspiration is enabled. */
	InspirationRadius        string = "INSPIRATION_RADIUS"        /** A ship is inspired if at least INSPIRATION_SHIP_COUNT opponent ships are within this Manhattan distance. */
	InspirationShipCount     string = "INSPIRATION_SHIP_COUNT"    /** A ship is inspired if at least this many opponent ships are within INSPIRATION_RADIUS distance. */
	InspiredExtractRatio     string = "INSPIRED_EXTRACT_RATIO"    /** An inspired ship mines 1/X halite from a cell per turn instead. */
	InspiredBonusMultiplayer string = "INSPIRED_BONUS_MULTIPLIER" /** An inspired ship that removes Y halite from a cell collects X*Y additional halite. */
	InspiredMoveCostRatio    string = "INSPIRED_MOVE_COST_RATIO"  /** An inspired ship instead spends 1/X% halite to move. */
)

// Constants - Holds all of the game constants
type Constants struct {
	values map[string]string
}

var constantsInstance *Constants
var loadErr error
var once sync.Once

// GetInstance - Returns the singleton instance of Constants
func GetInstance() *Constants {
	return constantsInstance
}

func (c *Constants) String() string {
	var buffer bytes.Buffer
	for key, val := range c.values {
		s := fmt.Sprintf("%s=%s\n", key, val)
		buffer.WriteString(s)
	}
	return buffer.String()
}

// Init - Creates an instance of the game constants
func Init(inputString string) error {
	if constantsInstance == nil {
		once.Do(func() {
			var constMap, err = parseMap(inputString)
			loadErr = err
			constantsInstance = &Constants{*constMap}
			if loadErr != nil {
				constantsInstance = nil
			}
		})
	}
	return loadErr
}

// GetInt - Returns value from the map as an int
func (c Constants) GetInt(key string) (int, error) {
	return strconv.Atoi(c.values[key])
}

// GetDouble - Returns value from the map as a double
func (c Constants) GetDouble(key string) (float64, error) {
	return strconv.ParseFloat(c.values[key], 64)
}

// GetBool - Returns value from the map as a bool
func (c Constants) GetBool(key string) (bool, error) {
	return strconv.ParseBool(c.values[key])
}

func parseMap(inputString string) (*map[string]string, error) {
	constMap := make(map[string]string)
	splitter := regexp.MustCompile("[{}, :\"]+")
	tokens := splitter.Split(inputString, -1)
	tokens = tokens[1 : len(tokens)-1]
	if len(tokens)%2 != 0 {
		log.GetInstance().Printf("Error: constants: expected even total number of key and value tokens from server.")
		return nil, fmt.Errorf("Invalid number of constant tokens %d", len(tokens))
	}
	for index := range tokens {
		if index%2 == 0 {
			var key = tokens[index]
			var val = tokens[index+1]
			constMap[key] = val
		}
	}
	return &constMap, nil
}
