(ns hlt.input
  (:require [clojure.string :as string]
            [hlt.log :refer [log]]))

(defn as-int
  "Coerce the argument to an int"
  [x]
  (cond
    (string? x) (Long. x)
    (nil? x) 0
    (integer? x) x
    (float? x) (int x)
    :else
    (throw (Exception. (str "as-int cannot coerce " (type x) " to int:" x)))))

(defn exit
  "Exit with status and a message"
  [status msg]
  (log msg)
  (shutdown-agents)
  (System/exit status))

(defn read-one-line
  "Read a line from STDIN. Exit when closed"
  []
  (let [line (read-line)]
    (when-not line
      (exit 0 "Input connection from server closed. Exiting..."))
    line))

(defn read-line-tokens
  "Read in a line from STDIN and break into token separated by whitespace.
  Exit when STDIN is closed"
  []
  (string/split (read-one-line) #"\s+"))

(defn read-line-ints
  "Read in a line from STDIN as a sequence of integers.
  Exit when STDIN is closed"
  []
  (map as-int (read-line-tokens)))
