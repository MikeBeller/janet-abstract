
(import ../build/abstract :as abstract)

               #(inc (length t))

(def tb @{"A" 1 "B" 2})
(table/setproto
  tb
  @{
    :__get (fn [t k]
             (if (= k :length)
               (fn [ab] (length (abstract/unwrap ab)))
               (table/rawget t k)))
    :__put (fn [t k v] (put t k (string v "x")))
    :__tostring (fn [t buf] (buffer/format buf "<abstract/new %j>" t))
    :__compare (fn [a b] (compare a b))})
(def ab (abstract/new tb))

(assert (= (get ab "foo") nil))
(put ab "bar" "barval")
(assert (= (get ab "bar") "barvalx"))
(assert (= (string ab) `<abstract/new @{"bar" "barvalx" "A" 1 "B" 2}>`))
(assert (= ab ab))
(assert (= ab (abstract/new tb)))
(assert (not= ab (abstract/new @{})))
(assert (= (length ab) 3))

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



