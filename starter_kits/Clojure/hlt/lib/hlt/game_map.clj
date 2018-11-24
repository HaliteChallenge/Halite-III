(ns hlt.game-map
  (:require [hlt.state :refer [game]]
            [hlt.input :refer [read-line-ints]]
            [hlt.direction :refer [invert]]
            [hlt.position :refer [new-position abs-position sub-position
                                  get-position directional-offset]]
            [hlt.map-cell :as map-cell :refer [read-map-cells]]
            [hlt.direction :as direction]))

(defn game-map
  "Return the game map"
  []
  (:game-map @game))

(defn at
  "Get cell at position"
  [position-or-entity]
  (get-in @game [:game-map :map-cells (get-position position-or-entity)]))

(defn is-occupied?
  "Is cell occupied?"
  [position-or-entity]
  (let [cell (at position-or-entity)]
    (map-cell/is-occupied? cell)))

(defn is-empty?
  "Is cell empty?"
  [position-or-entity]
  (let [cell (at position-or-entity)]
    (map-cell/is-empty? cell)))

(defn normalize
  "A method that normalizes a position within the bounds of the
  toroidal map. Useful for handling the wraparound modulus arithmetic
  on x and y. For example, if a ship at (x = 31, y = 4) moves to the
  east on a 32x32 map, the normalized position would be (x = 0, y =
  4), rather than the off-the-map position of (x = 32, y = 4)."
  [position]
  (let [[x y] position
        {:keys [width height]} (game-map)]
    [(mod x width) (mod y height)]))

(defn calculate-distance
  "A method that computes the Manhattan distance between two locations,
  and accounts for the toroidal wraparound."
  [source target]
  (let [source (-> source get-position normalize)
        target (-> target get-position normalize)
        [x y] (abs-position (sub-position source target))
        {:keys [width height]} (game-map)]
    (+ (min x (- width x)) (min y (- height y)))))

(defn get-target-direction
  "Returns where in the cardinality spectrum the target is from source.
  e.g.: North, East; South, West; etc."
  [source target]
  (let [[sx sy] (-> source get-position)
        [tx ty] (-> target get-position)
        cx (if (> tx sx)
             direction/east
             (if (< tx sx)
               direction/west))
        cy (if (> ty sy)
             direction/south
             (if (< ty sy)
               direction/north))]
    [cx cy]))

(defn get-unsafe-moves
  "A method that returns a list of direction(s) to move closer to a
  target disregarding collision possibilities. Returns an empty list
  if the source and destination are the same."
  [source destination]
  (let [{:keys [width height]} (game-map)
        source (-> source get-position normalize)
        destination (-> destination get-position normalize)
        [dx dy] (abs-position (sub-position destination source))
        [cx cy] (get-target-direction source destination)
        m0 (if-not (zero? dx)
             (if (< dx (/ width 2))
               cx
               (if cx (invert cx))))
        m1 (if-not (zero? dy)
             (if (< dy (/ height 2))
               cy
               (if cy (invert cy))))]
    (if m0
      (if m1
        [m0 m1]
        [m0])
      (if m1
        [m1]
        []))))

(defn naive-navigate
  "A method that returns a direction to move closer to a target
  without colliding with other entities. Returns a direction of
  still if no such move exists."
  [ship destination]
  (let [ship-position (get-position ship)
        destination-position (get-position destination)
        unsafe-moves (get-unsafe-moves ship-position destination-position)
        dir (reduce
              (fn [_ direction]
                (if (is-empty?
                      (directional-offset ship-position direction))
                  (reduced direction)))
              unsafe-moves)]
    (or dir direction/still)))

(defn read-game-map
  "Read the game map"
  []
  (let [[width height] (read-line-ints)
        map-cells (read-map-cells width height)]
    {:width width
     :height height
     :map-cells map-cells}))

(defn update-game-map
  [game-map]
  (let [{:keys [map-cells]} game-map
        remove-ship (fn [cells [position cell]]
                      (assoc cells
                        position (assoc cell
                                   :ship nil)))
        map-cells (reduce remove-ship
                    map-cells
                    (seq map-cells))
        [update-count] (read-line-ints)
        update-cell (fn [cells [x y halite]]
                      (let [position (new-position x y)
                            cell (get cells position)]
                        (assoc cells
                          position (assoc cell
                                     :halite halite))))
        map-cells (reduce update-cell
                    map-cells
                    (repeatedly update-count read-line-ints))]
    (assoc game-map
      :map-cells map-cells)))
