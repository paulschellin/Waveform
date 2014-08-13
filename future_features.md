### Considerations for Future Features

#### Lazy Evaluation and Optimization Through [Expression Templates](http://en.wikipedia.org/wiki/Expression_templates)

Make use of expression templates similar to the [Blitz++ library](http://blitz.sourceforge.net/) and [Armadillo](http://arma.sourceforge.net/).

This could be used to minimize the number of transforms that occur when manipulating both domains in a loop, for example.

#### Better Guarded Iterator Support

This was a feature at first, but earlier rapid development of the rest of the library made it difficult to maintain, so it was removed.

The iterator class needs to be able to notify the Waveform class when it has been modified, and this can be done by having separate Input and Output iterator types which are chosen at compile time using argument-dependent name lookup.

#### Compound Assignment Operators

These were also removed due to uncertainty as to how to indicate whether or not an operation would be a linear transform or not.

Perhaps if Transform classes have transform_type typedefs added to them it might be possible to implement the compound assignment operators in a sane fashion.

#### Partially Invalidated Domains

In some cases, only a small section of one domain may be modified. As the library is currently written, that small section invalidates the other domain entirely, requiring the whole thing to be recomputed.

Some transforms, however, do not require transforming the whole thing if only a small section is modified. And if the transforms are particularly expensive then there is sufficient motivation to try to only invalidate the associated section in the other domain, not the whole domain.

Details of the implementation of this feature will take a while to work out.
