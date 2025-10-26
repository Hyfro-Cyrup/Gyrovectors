namespace Gyrovectors.Models;

public interface IGyroVector<V, S> : IVector<V, S>
    where V : IGyroVector<V, S>
{
    public abstract static V Gyr(V a, V b, V c);
}