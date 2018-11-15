(ns hlt.state)

(defonce game (atom nil))

;; certain constants have different names than provided in JSON
(def remap-constants
  {:MAX_HALITE :MAX_ENERGY
   :SHIP_COST :NEW_ENTITY_ENERGY_COST})

(defn constants
  "Return value for the game constant. Constants include:
   :SHIP_COST - The cost to build a single ship.
   :DROPOFF_COST - The cost to build a dropoff.
   :MAX_HALITE - The maximum amount of halite a ship can carry.
   :MAX_TURNS - The maximum number of turns a game can last. This reflects
      the fact that smaller maps play for fewer turns.
   :EXTRACT_RATIO - 1/EXTRACT_RATIO halite (truncated) is collected from
      a square per turn.
   :MOVE_COST_RATIO - 1/MOVE_COST_RATIO halite (truncated) is needed to move
      off a cell.
   :INSPIRATION_ENABLED - Whether inspiration is enabled.
   :INSPIRATION_RADIUS - A ship is inspired if at least INSPIRATION_SHIP_COUNT
     opponent ships are within this Manhattan distance.
   :INSPIRATION_SHIP_COUNT - A ship is inspired if at least this many
     opponent ships are within this Manhattan distance.
   :INSPIRED_EXTRACT_RATIO - An inspired ship mines 1/X halite from a cell
     per turn instead.
   :INSPIRED_BONUS_MULTIPLIER - An inspired ship that removes Y halite from a
     cell collects X*Y additional halite.
   :INSPIRED_MOVE_COST_RATIO - An inspired ship instead spends 1/X% halite
    to move."
  [constant-kw]
  (let [constant (get remap-constants constant-kw constant-kw)]
    (get-in @game [:constants constant])))
