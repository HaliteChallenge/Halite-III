(ns hlt.game
  (:require [clojure.string :as string]
            [clojure.data.json :as json]
            [hlt.state :refer [game]]
            [hlt.log :refer [log]]
            [hlt.input :refer [read-one-line read-line-ints exit]]
            [hlt.position :refer [position?]]
            [hlt.player :as player :refer [read-players update-players]]
            [hlt.map-cell :as map-cell]
            [hlt.game-map :refer [read-game-map update-game-map at]]))

(defn my-id
  "Return my player id"
  []
  (::player/id @game))

(defn me
  "Return my player information"
  [& ks]
  (let [{:keys [::player/id players]} @game
        player (get players id)]
    (if (seq ks)
      (get-in player ks)
      player)))

(defn game-map
  "Return the game map"
  []
  (:game-map @game))

(defn has-structure?
  "Is cell empty?"
  [position-or-entity]
  (let [cell (at position-or-entity)]
    (map-cell/has-structure? cell)))

(defn structure-type
  "Get structure-type for a map cell"
  [position-or-entity]
  (let [cell (at position-or-entity)]
    (map-cell/structure-type cell)))

(defn turn-number
  "Return the turn-number"
  []
  (:turn-number @game))

(defn new-game
  "Initialize the game"
  []
  (let [constants (json/read-json (read-one-line) true)
        [num-players id] (read-line-ints)
        log-file (str "bot-" id ".log")
        players (read-players num-players)
        game-map (read-game-map)]
    (swap! game assoc
      ::player/id id
      :num-players num-players
      :log-file log-file
      :constants constants
      :players players
      :game-map game-map
      :turn-number 0
      :commands [])))

(defn ready
  "Game initialization"
  [bot-name]
  (println bot-name))

(defn update-frame
  "Update game state"
  []
  (let [{:keys [num-players players game-map]} @game
        [turn-number] (read-line-ints)
        players (update-players num-players players)
        game-map (update-game-map game-map)]
    (log "=============== TURN" turn-number "================")
    (swap! game assoc
      :players players
      :game-map game-map
      :turn-number turn-number
      :commands [])
    (doseq [[_ player] (seq players)]
      (let [{:keys [::player/id shipyard ships dropoffs]} player]
        (swap! game update-in
          [:game-map :map-cells (:position shipyard)]
          assoc :structure shipyard)
        (doseq [[_ ship] (seq ships)]
          (swap! game update-in
            [:game-map :map-cells (:position ship)]
            (partial map-cell/mark-unsafe ship)))
        (doseq [[_ dropoff] (seq dropoffs)]
          (swap! game update-in
            [:game-map :map-cells (:position dropoff)]
            assoc :structure dropoff))))
    nil))

(defn command
  "queue an engine command"
  [cmd]
  (swap! game update-in [:commands] conj cmd))

(defn end-turn
  "End turn"
  []
  (let [commands (:commands @game)]
    (if (seq commands)
      (println (string/join " " commands))
      (exit 1 "no commands queued.. exiting"))))

(defn log-game
  "Debugging function to log the game data (without the game-map)"
  []
  (log (with-out-str (clojure.pprint/pprint (dissoc @game :game-map)))))
