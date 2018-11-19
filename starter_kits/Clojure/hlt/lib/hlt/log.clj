(ns hlt.log
  (:require [taoensso.timbre :as timbre]
            [hlt.state :refer [game]]))

(defonce config (atom nil))

(defn output-fn
  "Logging (fn [data]) -> string output fn."
  ([     data] (output-fn nil data))
  ([opts data] ;; For partials
   (let [{:keys [no-stacktrace? stacktrace-fonts]} opts
         {:keys [level ?err #_vargs msg_ ?ns-str ?file hostname_
                 timestamp_ ?line]} data]
     (str (force msg_)))))

(defn config-default
  "Generate a default Timbre config map"
  []
  (let [log-file (or (:log-file @game) "bot-0.log")]
    {:level :debug
     :timestamp-opts timbre/default-timestamp-opts
     :output-fn output-fn
     :appenders {:spit (timbre/spit-appender {:fname log-file})}}))

(defn set-config! [my-config]
  (timbre/set-config! (reset! config my-config)))

(defmacro log [& args]
  `(do
     (when-not @config (set-config! (config-default)))
     (timbre/info ~@args)))
