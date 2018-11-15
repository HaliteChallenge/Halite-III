(ns hlt.shipyard
  (:require [hlt.commands :as commands]))

(defn id
  "Get id for a shipyard"
  [shipyard]
  (::id shipyard))

(defn owner
  "Get owner for a shipyard"
  [shipyard]
  (:hlt.player/id shipyard))

(defn position
  "Get position of a shipyard"
  [shipyard]
  (:position shipyard))

(defn spawn
  "returns the command to spawn a ship"
  []
  commands/generate)

(defn new-shipyard
  "New shipyard"
  [owner shipyard-id position]
  {::id shipyard-id
   :hlt.player/id owner
   :position position})
