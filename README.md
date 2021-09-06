# data_access
Sample code to access model data.

## Background
Model data is stored as single scalar values for each field, collected in a class called `ElementData`.

Model calculations are performed by indiviual functions that read several model variables in a single element, perform calculations on these values and writ ethe results back.

Traditionally this would be achieved by writing functions that had long argument lists with several read-only variables (passed by value or declared `IN`) and several more written variables (passed by reference or declared `OUT`/`INOUT`).

To avoid this, it is my intention to pass objects as arguments to these calculation routines. The question is how to do this both efficiently and in a way that is easy to understand in code form.

### Model
The model is intended to act similarly to a grid-based fluid dynamics model. It defines a grid of values, and iterates over the chaotic Lorenz equations.

The class `ElementData` holds the variables that make up the model.

The function `level1Function()` calculates the two derivatives that use all three variables (dy/dt and dz/dt).

The function `level2Function()` calculates the x derivative which only need to know about the x and y values. This demonstrates how lower-level functions fit into each framework.

For reference, an equivalent to `level1Function()` might be the top level one-dimensional physics routine and an equivalent to level2Function might be the calculation of the open water matter, momentum and energy fluxes.

In frameworks that use them, the argument classes are classes that hold references to all of the variables that will be used by a particular function. The intent is that these allow definition of what variables are accessed and, by the `const`-ness of the accessing fucntion, whether that access is read-only or read-write.

## `FiniteElement` method
`infinitely_elementary.cpp`

This is not really a method like described above. This replicates how the current `FiniteElement` class manages data. All calculation routines are member functions of the main class, and the functions can access any data with read-write permissions.
### Advantages
* Current implementation
* No complex data structures
### Disadvantages
* No separation of concerns
* Adding a new function requires adding code to the central data-holding class

## No separation
`no_sep.cpp`

The `ElementData` class is the one which is passed as the argument to every calculation function. Data member variables are all declared public, so any calculation function can access any model variable with read-write permissions.
### Advantages
* No complex data structures
* Functions are encapsulated in so far as they are not members
### Disdavantages
* No separation of concerns
* Everything is declared public, no data encapsulation

## Public
`public.cpp`

The `ElementData` class exposes all of its accessible data members as public data members. The argument classes hold a reference to an `ElementData` instance, and can access any of the data at any time. The argument class defines what variables are accessed and with what permission. This is achieved by only accessing variables through functions that return references to the variable. Read-only values return `const double&`, whereas read-write values return `double&`.
### Advantages
* New functions can access the model data without needing to modify `ElementData`
* Separation of concerns
### Disadvantages
* Everything is declared public, no encapsulation
* Some complexity in the data structures

## Reference
`reference.cpp`

The `ElementData` class only exposes a read-only interface to the data, using `const double&` getters. The argument classes hold a reference to `ElementData`, and can access the data with read-write permissions due to each of them being declared a `friend` of `ElementData`. This only applies to top-level argument classes. Lower level argument classes only access the higher level ones, and do not need to be `friend`s of `ElementData`. Each argument class can signify what data it uses and what it accesses as read-write by defining `const double&` and `double&` getters.
### Advantages
* Separation of concerns
* Encapsulation
### Disadvantages
* Adding a new top-level function requires adding code to the central data-holding class
* More complex data structures

## Accessor
`accessor.cpp`

Similar to the above Reference method, except only one class is ever declared as a friend of `ElementData`. This class is `ElementDataAccessor`, which just provides explicit read-write access to the values of `ElementData` using pass-through `double&` accessors (getters/setters).
### Advantages
* Separation of concerns
* Encapsulation
* `ElementData` has a consistent interface
* New functions can access the model data without needing to modify `ElementData`
### Disadvantages
* Not clearly better than just declaring the respective variables as public
* More complex data structures

## No friends
`no_friends.cpp`

An adaptation of the Reference and Accessor methods which requires nothing to be declared as friend. Elements are all accessed through reference getters, but the read-only or read-write status define by the presence of `const` in the function declaration. The argument classes still advertise the variables they use and the permissions with which they provide them by the reference getter functions they define.
### Advantages
* Separation of concerns
* Some encapsulation
* `ElementData` has a clear interface
* New functions can access the model data without needing to modify `ElementData`
* No friends declared
### Disadvantages
* Should the `double&` access functions in fact just be public variables?
* Complex data structures

## Argument monsters
`monster.cpp`

Passing data as individual arguments. The old way to do it, for example in Fortran models. It looks simple with 3 variables, but much worse with twenty. Arguments can be missed but caught at run time or misplaced, which is much more insidious.
### Advantages
* Classic
* Conecptually simple
* Read/write status easily added
### Disadvantages
* So. Many. Arguments.

## Functor
`functor.cpp`

There are no direct functions. All of the functions are defined as the `operator()` member of a class. A new instance of the class id created from the parent data class, the members of which are all public. If a higher numbered level wants to declare the it will not write the data, then the member is a `const &` reference, rather than a plain `&` reference.

Once the instance is constructed from the more general data object, the calculation is performed by calling `object()`, using the `operator()()` function.
### Advantages
* Separation of concerns
* Read/write status clealry marked
* Clean code at the point of calling
### Disadvantages
* Only just fits inside the concept of a functor
* Many objects created
  * If the compiler cannot be persuaded to optimize that away

## Friend constructors
`close_friends.cpp`

A derived system based on the functor version above. Instead of having all members public and read-write, all data members are private. However, the constructor of each function is declared as a friend of the data class it uses on construction. Again, data members within the class are `const` references if the are intended to be used read-only.

Having to declare the `friend`ly constructors might not be such a disadvantage. It means that only a small, known set of functors are allowed to access the more basic level data as read-write. If read-write access were available to any class that used the accessor class (for example), then any new code would be able to access `ElementData`. Explicitly listing those that are retains more control.
### Advantages
* As functor
* Data members are not public read-writeable
* Other classes do not have general read-write access to `ElementData`
### Disadvantages
* Also as functor

## Derived classes
`friends_and_descendants.cpp`

Based on the Friend Constructors. A demonstration of including an alternative implementation of the functions, using protected data members and without having to declare the constructors of the derived funcotr classes as friends.
### Extra advantages
* Derived classes can provide alterantive implementations
