
(import ../build/abstract :as abstract)

               #(inc (length t))

(def tb @{})
(table/setproto
  tb
  @{
    :__get (fn [t k]
             (if (= k :length)
               (fn [t] 77)
               (string "value of key " k " is " (table/rawget t k))))
    :__put (fn [t k v] (put t k (string v "_withappendage")))
    :__tostring (fn [t buf] (buffer/push buf "mystringmystring"))
    :__compare (fn [a b] (compare a b))})
(def ab (abstract/new tb))

(assert (= (get ab "foo") "value of key foo is "))
(put ab "bar" "barval")
(assert (= (get ab "bar") "value of key bar is barval_withappendage"))
(assert (= (string ab) "mystringmystring"))
(assert (= ab ab))
(assert (= ab (abstract/new tb)))
(assert (not= ab (abstract/new @{})))
(assert (length ab) 77)

(def proto
    @{
      :__tostring (fn [t b] (buffer/push-string b "MyThing: ") (buffer/format b "%j" (in t :data)))
      :__compare (fn [a b] (compare (in a :data) (in b :data)))
      })

(def tb @{:data [1 2 3]})
(def ab (abstract/new (table/setproto tb proto)))

(string ab)  # => "MyThing: [1 2 3]"
(def ab2 (abstract/new (table/setproto @{:data [1 2 3]} proto)))
(compare ab ab2) # => 0



