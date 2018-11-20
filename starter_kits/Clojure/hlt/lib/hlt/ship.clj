(ns hlt.ship
  (:require [hlt.state :refer [constants]]
            [hlt.input :refer [read-line-ints]]
            [hlt.position :refer [new-position position?]]
            [hlt.commands :as commands]
            [hlt.direction :as direction]))

(defn id
  "Get id for a ship"
  [ship]
  (::id ship))

(defn owner
  "Get owner for a ship"
  [ship]
  (:hlt.player/id ship))

(defn position
  "Get position of a ship"
  [ship]
  (:position ship))

(defn halite-amount
  "Get halite-amount for a ship"
  [ship]
  (:halite ship))

(defn is-full?
  "Returns true if the ship is full of halite"
  [ship]
  (>= (:halite ship) (constants :MAX_HALITE)))

(defn move
  "Move"
  [ship dir-or-offset]
  (let [dir (if (position? dir-or-offset)
              (direction/convert dir-or-offset)
              dir-or-offset)]
    (str commands/move " " (::id ship) " " dir)))

(defn stay-still
  "Stay Still"
  [ship]
  (move ship direction/still))

(defn make-dropoff
  "Convert to dropoff"
  [ship]
  (str commands/construct " " (::id ship)))

(defn new-ship
  "New ship"
  [player-id ship-id position halite]
  {::id ship-id
   :hlt.player/id player-id
   :position position
   :halite halite})

(defn read-ship
  "Read ship"
  [player-id]
  (let [[ship-id x y halite] (read-line-ints)
        position (new-position x y)
        ship (new-ship player-id ship-id position halite)]
    [ship-id ship]))
