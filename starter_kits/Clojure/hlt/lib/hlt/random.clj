(ns hlt.random
  (:import [java.util
            Random]))

(defonce state (atom nil))

(defn set-seed! [seed]
  (swap! state assoc :seed seed))

;; This function is required b/c
;; https://dev.clojure.org/jira/browse/CLJ-1452
;; is unresolved and the data.generators workaround is not sufficient
(defn rrand-int
  "repeatable rand-int"
  [n]
  (let [{:keys [seed rnd]} @state
        seed (or seed (System/nanoTime))
        rnd (if (not rnd)
              (:rnd (swap! state assoc :rnd (Random. seed)))
              rnd)]
    (.nextInt rnd n)))
