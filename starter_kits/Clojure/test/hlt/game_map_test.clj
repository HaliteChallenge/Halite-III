(ns hlt.game-map-test
  (:require [clojure.test :refer :all]
            [hlt.ship :as ship]
            [hlt.player :as player]
            [hlt.shipyard :as shipyard]
            [hlt.game-map :refer :all]))

(deftest test-naive-navigate
  (with-redefs [game-map  (fn [] {:width 32  :height 32})]
  (let [ship {::ship/id 1 ::player/id 0 :position [8 10] :halite 100}
        shipyard {::shipyard/id -1  ::player/id 0 :position [8 16]}]
    (is (= [0 1] (naive-navigate ship shipyard))))))
