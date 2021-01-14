
(import ../build/abstract :as abstract)


(def t @{})
(table/setproto
  t
  @{
    :__get (fn [tb k]
             (if (= k :length)
               (inc (length tb))
               (string "value of key " k " is " (table/rawget tb k))))
    :__put (fn [tb k v] (put t k (string v "_withappendage")))
    :__tostring (fn [tb buf] (buffer/push buf "mystringmystring"))
    :__compare (fn [a b] (compare a b))})
(def ab (abstract/new t))

(assert (= (get ab "foo") "value of key foo is "))
(put ab "bar" "barval")
(assert (= (get ab "bar") "value of key bar is barval_withappendage"))
(assert (= (string ab) "mystringmystring"))
(assert (= ab ab))
(assert (= ab (abstract/new t)))
(assert (not= ab (abstract/new @{})))

