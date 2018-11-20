(ns hlt.player
  (:require [hlt.input :refer [read-line-ints]]
            [hlt.position :refer [new-position]]
            [hlt.ship :refer [read-ship]]
            [hlt.dropoff :refer [read-dropoff]]
            [hlt.shipyard :refer [new-shipyard]]))

(defn id
  "Get id for a player"
  [player]
  (::id player))

(defn shipyard
  "Get shipyard for a player"
  [player]
  (:shipyard player))

(defn halite-amount
  "Get halite-amount for a player"
  [player]
  (:halite player))

(defn get-ships
  "Get ships for a player"
  [player]
  (:ships player))

(defn get-ship
  "Get ship ship-id for a player"
  [player ship-id]
  (get-in player [:ships ship-id]))

(defn has-ship?
  "Does this player have ship ship-id?"
  [player ship-id]
  (boolean (get-ship player ship-id)))

(defn get-dropoffs
  "Get dropoffs for a player"
  [player]
  (:dropoffs player))

(defn get-dropoff
  "Get dropoff dropoff-id for a player"
  [player dropoff-id]
  (get-in player [:dropoffs dropoff-id]))

(defn read-player
  "Read the shipyard positions for a player"
  []
  (let [[player-id x y] (read-line-ints)
        shipyard (new-shipyard player-id -1 (new-position x y))
        player {::id player-id
                :shipyard shipyard
                :halite 0
                :ships nil
                :dropoffs nil}]
    [player-id player]))

(defn read-players
  "Read the shipyard positions for all the players"
  [num-players]
  (into {} (repeatedly num-players read-player)))

(defn update-player
  [players [player-id num-ships num-dropoffs halite]]
  (assoc players
    player-id
    (let [player (get players player-id)
          ships (into (sorted-map)
                  (repeatedly num-ships #(read-ship player-id)))
          dropoffs (into (sorted-map)
                     (repeatedly num-dropoffs #(read-dropoff player-id)))]
      (assoc player
        :ships ships
        :dropoffs dropoffs
        :halite halite))))

(defn update-players
  [num-players players]
  (reduce update-player
    players
    (repeatedly num-players read-line-ints)))
