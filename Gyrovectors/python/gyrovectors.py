from collections.abc import Callable
from typing import TypeVar, Generic

Vector = TypeVar('Vector')
Scalar = TypeVar('Scalar')



class Gyrovectorspace(Generic[Vector, Scalar]):
    class Gyrovector:
        def __init__(self, value : Vector, parent):
            self.value = value
            self.parent = parent

        """Add another Gyrovector"""
        def __add__(self, other):
            return self.parent.add(self, other)
        
        def __neg__(self):
            return self.parent.negative(self)
        
        def __mul__(self, scalar):
            return self.parent.scale(scalar, self)
        
        def __str__(self):
            return str(self.value)
        
        def __repr__(self):
            return str(self.value)

    # class Gyroscalar: 
    #     def __init__(self, value : Scalar, parent):
    #         self.value = value
    #         self.parent = parent

    #     def __add__(self, other):
    #         return self.parent.addscalars(self, other)
        
    #     def __mul__(self, other):
    #         return self.parent.scale(self, other)
        
    #     def __str__(self):
    #         return str(self.value)

    def __init__(self, 
                 addition : Callable[[Vector, Vector], Vector], 
                 gyr : Callable[[Vector, Vector], Callable[[Vector], Vector]], 
                 identity : Vector,
                 inverse : Callable[[Vector], Vector],
                 scalar_multiplication : Callable[[Scalar, Vector], Vector]):
        self.value_level_addition = addition
        self.value_level_gyr = gyr
        self.value_level_identity = identity
        self.value_level_inverse = inverse
        self.value_level_scalar_multiplication = scalar_multiplication

        self.zero = self.newGyrovector(identity)

    def newGyrovector(self, value) -> Gyrovector:
        return self.Gyrovector(value, self)
    
    # def newGyroscalar(self, value) -> Scalar:
    #     return self.Scalar(value, self)
    
    def add(self, a : Gyrovector, b : Gyrovector) -> Gyrovector:
        return self.newGyrovector(self.value_level_addition(a.value, b.value))
    
    def negative(self, a : Gyrovector) -> Gyrovector:
        return self.newGyrovector(self.value_level_inverse(a.value))
    
    def subtract(self, a : Gyrovector, b : Gyrovector) -> Gyrovector:
        return self.add(a, self.negative(b))
    
    # def addscalars(self, a : S, b : Gyroscalar) -> Gyroscalar:
    #     return self.newGyroscalar(a.value + b.value) # type: ignore
    
    def gyr(self, a : Gyrovector, b : Gyrovector) -> Callable[[Gyrovector], Gyrovector]:
        value_level_function = self.value_level_gyr(a.value, b.value)
        return lambda z : self.newGyrovector(value_level_function(z.value))
            
    """A little synctactic sugar so that equations can look like `a + Gyr(a, b)(c)` where Gyr is the space itself"""
    def __call__(self, a : Gyrovector, b : Gyrovector) -> Callable[[Gyrovector], Gyrovector]:
        return self.gyr(a, b)
    
    def scale(self, r : Scalar, a : Gyrovector) -> Gyrovector:
        return self.newGyrovector(self.value_level_scalar_multiplication(r, a.value))
    

# GUESS: S=1. Mixing definitions given for the gyrogroup (D, +) and the gyrovector space (V_s, +, x)
S = 1


def complex_mobius_addition(a: complex, z : complex) -> complex:
    return (a + z) / (1 + (a.conjugate() * z))

def complex_mobius_gyr(a : complex, b : complex) -> Callable[[complex], complex]:
    return lambda z : (complex_mobius_addition(a, b) / complex_mobius_addition(b, a)) * z

def complex_mobius_scaling(r : float, v : complex) -> complex:
    V = abs(v)/S
    a = (1 + V)**r if 1 + V != 0.0 else 0.0
    b = (1 - V)**r if 1 - V != 0.0 else 0.0
    return (a - b)/(a + b) * v/V

def complex_mobius_additive_inverse(a : complex) -> complex:
    return -a


if __name__ == '__main__':
    gyr = Gyrovectorspace[complex, float](complex_mobius_addition, complex_mobius_gyr, 0 + 0j, complex_mobius_additive_inverse, complex_mobius_scaling)

    a = gyr.newGyrovector(1 + 0j)
    b = gyr.newGyrovector(0 + 1j)
    z = gyr(a, b)(b)
    print(z)


    ### further questions
    # ||-a + b|| = ||b-a||?
    # distances preserved under left gyrotranslation?
    # How to tile with pentagons
    #  - How angles work
    # How to render a sprite; how does it warp
    # What can we use box addition and cogyrolines for? What do cogyrolines look like
    # Make a toolkit for gyrolines and gyroline segments
