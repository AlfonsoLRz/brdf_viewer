from __future__ import annotations
from abc import ABC, abstractmethod
import glm


class BRDF:
    def __init__(self, strategy: Strategy) -> None:
        self._strategy = strategy

    @property
    def strategy(self) -> Strategy:
        return self._strategy

    @strategy.setter
    def strategy(self, strategy: Strategy) -> None:
        self._strategy = strategy

    def compute_intensity(self, L: glm.vec3, V: glm.vec3) -> None:
        result = self._strategy.compute_intensity(L, V)


class Strategy(ABC):
    @abstractmethod
    def compute_intensity(self, L: glm.vec3, V: glm.vec3):
        pass


class Lambertian(Strategy):
    def compute_intensity(self, L: glm.vec3, V: glm.vec3) -> float:
        return 1.0


class OrenNayar(Strategy):
    def compute_intensity(self, L: glm.vec3, V: glm.vec3) -> float:
        return 1.0