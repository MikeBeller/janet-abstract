# janet-abstract

Allow the creation, in pure Janet, of tables which act like
abstract types in Janet, by proxying the functions in the JanetAbstract
header (such as next, get, put..) and forwarding them to functions
in a table's prototype named `:__next`, `:__get`, `:__put`, etc. ).

The goal is to allow you use pure Janet code to develop
"object" like thingys that can have special iteration, stringification,
indexing, and other behaviors.

# Usage

```clojure
(def proto
    @{
      :__tostring (fn [t b] (buffer/push-string b "MyThing: ") (buffer/format b "%j" (in t :data)))
      :__compare (fn [a b] (compare (in a :data) (in b :data)))
      })

(def tb @{:data [1 2 3]})
(def ab (abstract/new (table/setproto tb proto)))

(tracev (string ab))  # => "MyThing: [1 2 3]"
(def ab2 (abstract/new (table/setproto @{:data [1 2 3]} proto)))
(tracev (compare ab ab2)) # => 0
```

See more examples in tests/ and in janet-set project which depends
on janet-abstract: https://github.com/MikeBeller/janet-set

# Status

Early early stage.  Only some of the functions are hooked as yet.

There is some question as to whether if someone messes up the implementation
of `:__compare` and `:__hash` perhaps they can crash Janet.  So far
I have not implemented the hooking of `:__hash`.

# Install

jpm install https://github.com/MikeBeller/janet-abstract.git

# AN OBSERVATION ABOUT JANET'S ABSTRACT TYPES

One of the use cases I have for this module is to allow the development
of new container types using pure Janet code with no C.  However developing
this caused me to observe that there is a limitation in Janet with regard
to developing containers as abstract types, regardless of whether it is
done in Janet or in C:

Abstract types in Janet allow the developer to "hook" the `get` function
in order to simulate indexing.  But the `get` function is also used for
retrieving methods on the abstract type.  If the key to `get` is a symbol,
the abstract type is supposed to look in its method table and return a
method if found.  This means that if you want to create a container type
using an abstract type, you probably should not allow storage of entries
where the keys are symbols.  Otherwise, if someone decides to store
a :length key in your data structure, the (length ...) function in Janet
will stop working for your data structure.   :-(

Is there some way to straighten out two things in the core:
(1) get function in the abstract type header is overloaded for method
retrieval and also available for "other uses", and (2) `length` is
implemented in the core so that it calls the method :length, rather than
having its own function pointer in the abstract type header.  Both
of these issues lead to difficulty implementing containers in Janet
outside the core, regardless of whether they are in C or Janet.

I suppose the answer could be "if you are going to create a container
outside the core you can not use symbols as keys".  If that be so, then
we should at least write that down somewhere.

