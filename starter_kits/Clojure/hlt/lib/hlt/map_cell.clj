(ns hlt.map-cell
  (:require [hlt.input :refer [read-line-ints]]
            [hlt.shipyard :as shipyard]
            [hlt.dropoff :as dropoff]
            [hlt.position :refer [new-position]]))

(defn position
  "Get position for a map cell"
  [cell]
  (:position cell))

(defn halite-amount
  "Get halite amount for a map cell"
  [cell]
  (:halite cell))

(defn ship
  "Get ship in a map cell"
  [cell]
  (:ship cell))

(defn structure
  "Get structure for a map cell"
  [cell]
  (:structure cell))

(defn has-structure?
  "Is there a dropoff or shipyard on this cell?"
  [cell]
  (boolean (:structure cell)))

(defn structure-type
  "Get structure-type for a map cell"
  [cell]
  (let [structure (:structure cell)]
    (cond
      (::dropoff/id structure) :dropoff
      (::shipyard/id structure) :shipyard
      :else
      nil)))

(defn is-occupied?
  "Is cell occupied?"
  [cell]
  (boolean (:ship cell)))

(defn is-empty?
  "Is cell empty?"
  [cell]
  (not (is-occupied? cell)))

(defn mark-unsafe
  "Place a ship in a cell"
  [ship cell]
  (assoc cell :ship ship))

(defn new-map-cell
  "New map-cell"
  [position halite ship structure]
  {:position position
   :halite halite
   :ship ship
   :structure structure})

(defn read-map-cells
  "Read the game map cells"
  [width height]
  (loop [map-cells (sorted-map) y 0]
    (if (= y height)
      map-cells
      (let [row (read-line-ints)
            new-cell (fn [x halite]
                       (let [position (new-position x y)
                             cell (new-map-cell position halite nil nil)]
                         [position cell]))
            row-cells (map new-cell (range) row)]
        (recur (merge map-cells (into {} row-cells)) (inc y))))))
