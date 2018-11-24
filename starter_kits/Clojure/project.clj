(defproject mybot "0.2.0-SNAPSHOT"
  :dependencies [[org.clojure/clojure "1.9.0"]
                 [com.taoensso/timbre "4.10.0"]
                 [org.clojure/data.json "0.2.6"]]
  :source-paths ["hlt/lib" "hlt/app"]
  :test-paths ["test"]
  :main MyBot
  :aot [MyBot]
  :uberjar-name "MyBot.jar")
