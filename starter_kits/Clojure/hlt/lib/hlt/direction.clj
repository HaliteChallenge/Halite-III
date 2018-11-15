(ns hlt.direction
  (:require [hlt.commands :as commands]))

(def north [0 -1])
(def south [0 1])
(def east [1 0])
(def west [-1 0])

(def still [0 0])

(def all-cardinals [north south east west])

(defn get-all-cardinals
  []
  all-cardinals)

(defn convert
  "Converts from this direction tuple notation to the engine's string notation"
  [direction]
  (cond
    (= direction north) commands/north
    (= direction south) commands/south
    (= direction east) commands/east
    (= direction west) commands/west
    :else
    commands/stay-still))

(defn invert
  "Returns the opposite cardinal direction given a direction"
  [direction]
  (cond
    (= direction north) south
    (= direction south) north
    (= direction east) west
    (= direction west) east
    :else
    still))
