(ns hlt.random
  (:import [java.util
            Random]))

(defonce state (atom nil))

(defn set-seed! [seed]
  (swap! state assoc :seed seed))

;; This function is required b/c (and to avoid a dep on data.genertors)
;; https://dev.clojure.org/jira/browse/CLJ-1452
(defn rrand-int
  "repeatable rand-int"
  [n]
  (let [{:keys [seed rnd]} @state
        seed (or seed (System/nanoTime))
        rnd (if (not rnd)
              (:rnd (swap! state assoc :rnd (Random. seed)))
              rnd)]
    (.nextInt rnd n)))
