using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace Gyrovectors;

public interface IVector<V, S> :
    IEquatable<V>,
    IAdditionOperators<V, V, V>,
    IAdditiveIdentity<V, V>,
    IDivisionOperators<V, S, V>,
    IMultiplyOperators<V, S, V>,
    ISubtractionOperators<V, V, V>,
    IEqualityOperators<V, V, bool>,
    IUnaryNegationOperators<V, V>
    where V : IVector<V, S>
{}
