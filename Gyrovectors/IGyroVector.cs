using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Gyrovectors;

public interface IGyroVector<V, S> : IVector<V, S>
    where V : IGyroVector<V, S>
{
    public abstract static V Gyr(V a, V b, V c);
}