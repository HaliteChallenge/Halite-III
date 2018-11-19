(ns hlt.dropoff
  (:require [hlt.input :refer [read-line-ints]]
            [hlt.position :refer [new-position]]))

(defn id
  "Get id for a dropoff"
  [dropoff]
  (::id dropoff))

(defn owner
  "Get owner for a dropoff"
  [dropoff]
  (:hlt.player/id dropoff))

(defn position
  "Get position of a dropoff"
  [dropoff]
  (:position dropoff))

(defn new-dropoff
  "New dropoff"
  [player-id dropoff-id position]
  {::id dropoff-id
   :hlt.player/id player-id
   :position position})

(defn read-dropoff
  "Read dropoff"
  [player-id]
  (let [[dropoff-id x y] (read-line-ints)
        position (new-position x y)
        dropoff (new-dropoff player-id dropoff-id position)]
    [dropoff-id dropoff]))
